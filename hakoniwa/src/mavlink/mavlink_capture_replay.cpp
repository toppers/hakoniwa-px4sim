#include "mavlink_capture_replay.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <iostream>
#include <vector>

bool mavlink_capture_load_controller(MavlinkCaptureControllerType &controller, const char* filepath) {
    // Open the capture file for reading
    controller.save_file = open(filepath, O_RDONLY);
    if (controller.save_file == -1) {
        std::cerr << "Failed to open capture file for reading." << std::endl;
        return false;
    }
    lseek(controller.save_file, 0, SEEK_SET);
    controller.offset = 0;
    std::cout << "Open success: " << filepath << std::endl;

    // Read the metadata at the start of the file
    int read_result = read(controller.save_file, &controller.start_time, sizeof(controller.start_time));
    if (read_result != sizeof(controller.start_time)) {
        std::cerr << "Error reading start time." << std::endl;
        return false;
    }
    std::cout << "start_time: " << controller.start_time << std::endl;
    controller.start_time = 0;
    read_result = read(controller.save_file, &controller.packet_num, sizeof(controller.packet_num));
    if (read_result != sizeof(controller.packet_num)) {
        std::cerr << "Error reading packet number." << std::endl;
        return false;
    }

    read_result = read(controller.save_file, &controller.total_size, sizeof(controller.total_size));
    if (read_result != sizeof(controller.total_size)) {
        std::cerr << "Error reading total size." << std::endl;
        return false;
    }
    std::cout << "total_size: " << controller.total_size << std::endl;

    // Set the initial offset after metadata
    controller.last_save_offset = sizeof(controller.start_time) + sizeof(controller.packet_num) + sizeof(controller.total_size);
    std::cout << "last_save_offset: " << controller.last_save_offset << std::endl;

    // Allocate memory for data cache
    controller.data = (uint8_t*) malloc(controller.total_size);
    if (controller.data == nullptr) {
        std::cerr << "Memory allocation failed." << std::endl;
        return false;
    }

    // Read the data from the file
    int data_read = pread(controller.save_file, 
                        controller.data, 
                        controller.total_size,
                        controller.last_save_offset);
    std::cout << "data_read: " << data_read << std::endl;

    if (data_read != (int)(controller.total_size)) {
        std::cerr << "Error reading data." << std::endl;
        return false;
    }
    return true;
}
bool mavlink_capture_load_data(MavlinkCaptureControllerType &controller, uint32_t dataLength, uint8_t  *data, uint32_t *r_dataLength, uint32_t *r_owner, uint64_t *timestamp)
{
    if (controller.data == nullptr || data == nullptr || r_dataLength == nullptr || timestamp == nullptr) {
        std::cerr << "Invalid data or pointers." << std::endl;
        return false;
    }

    if (controller.offset >= controller.total_size) {
        std::cerr << "No more data to read." << std::endl;
        *r_dataLength = 0;
        return true;
    }

    // Read the dataLength value
    uint32_t packet_data_length;
    if (controller.offset + sizeof(uint32_t) > controller.total_size) {
        std::cerr << "Incomplete data in the cache." << std::endl;
        return false;
    }    
    memcpy(&packet_data_length, controller.data + controller.offset, sizeof(uint32_t));
    //std::cout << "data_length: controller.offset = " << controller.offset << std::endl;
    controller.offset += sizeof(uint32_t);

    // Check if there's enough space in the provided data buffer
    if (dataLength < packet_data_length) {
        std::cerr << "Data buffer too small." << std::endl;
        std::cerr << "dataLength = " << dataLength << std::endl;
        std::cerr << "packet_data_length = " << packet_data_length << std::endl;
        return false;
    }
    uint32_t packet_owner;
    if (controller.offset + sizeof(uint32_t) > controller.total_size) {
        std::cerr << "Incomplete data in the cache." << std::endl;
        return false;
    }    
    memcpy(&packet_owner, controller.data + controller.offset, sizeof(uint32_t));
    //std::cout << "data_length: controller.offset = " << controller.offset << std::endl;
    controller.offset += sizeof(uint32_t);

    // Check if there's enough space in the provided data buffer
    if (dataLength < packet_data_length) {
        std::cerr << "Data buffer too small." << std::endl;
        std::cerr << "dataLength = " << dataLength << std::endl;
        std::cerr << "packet_data_length = " << packet_data_length << std::endl;
        return false;
    }
    *r_owner = packet_owner;

    // Read the relative timestamp
    uint64_t packet_timestamp;
    if (controller.offset + sizeof(uint64_t) > controller.total_size) {
        std::cerr << "Incomplete data in the cache." << std::endl;
        return false;
    }    
    memcpy(&packet_timestamp, controller.data + controller.offset, sizeof(uint64_t));
    //std::cout << "timestamp: controller.offset = " << controller.offset << std::endl;
    controller.offset += sizeof(uint64_t);
    *timestamp = packet_timestamp;

    // Check if there's enough data left in the cache
    if (controller.offset + packet_data_length > controller.total_size) {
        std::cerr << "Incomplete data in the cache." << std::endl;
        return false;
    }

    // Read the packet data
    memcpy(data, controller.data + controller.offset, packet_data_length);
    //std::cout << "data: controller.offset = " << controller.offset << std::endl;
    controller.offset += packet_data_length;

    // Update the returned data length and relative timestamp
    *r_dataLength = packet_data_length;

    return true;
}

bool mavlink_set_timestamp_for_replay_data(MavlinkDecodedMessage &message, uint64_t time_usec)
{
    switch (message.type) {
        case MAVLINK_MSG_TYPE_HEARTBEAT:
            //nothing to do
            break;
        case MAVLINK_MSG_TYPE_HIL_SENSOR:
            message.data.sensor.time_usec = time_usec;
            break;
        case MAVLINK_MSG_TYPE_SYSTEM_TIME:
            message.data.system_time.time_unix_usec = time_usec;
            break;
        case MAVLINK_MSG_TYPE_HIL_GPS:
            message.data.hil_gps.time_usec = time_usec;
            break;
        case MAVLINK_MSG_TYPE_HIL_STATE_QUATERNION:
            message.data.hil_state_quaternion.time_usec = time_usec;
            break;
        case MAVLINK_MSG_TYPE_LONG:
        case MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS:
            break;
        default:
            std::cout << "  Unknown or unsupported MAVLink message type received. type = " << message.type << std::endl;
            break;
    }
    return true;
}