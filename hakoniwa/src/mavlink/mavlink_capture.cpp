#include "mavlink_capture.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <iostream>
#include <vector>
#include <chrono>
#include <errno.h>

bool mavlink_capture_create_controller(MavlinkCaptureControllerType &controller, const char* filepath) {
    controller.start_time = 0;
    controller.packet_num = 0;
    controller.total_size = 0;
    controller.packets_since_last_save = 0;
    controller.memsize = MAVLINK_CAPTURE_INC_DATA_SIZE;
    controller.offset = 0;

    // Set the initial offset after metadata
    controller.last_save_offset = sizeof(controller.start_time) + sizeof(controller.packet_num) + sizeof(controller.total_size);

    // Allocate initial memory for data cache
    controller.data = (uint8_t*) malloc(controller.memsize);
    if (controller.data == nullptr) {
        std::cerr << "Initial memory allocation failed." << std::endl;
        return false;
    }

    // Open the file for writing
    controller.save_file = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (controller.save_file == -1) {
        std::cerr << "Failed to open capture file for writing." << std::endl;
        free(controller.data);
        controller.data = nullptr;
        return false;
    }

    return true;
}

bool mavlink_capture_append_data(MavlinkCaptureControllerType &controller, uint32_t owner, uint32_t dataLength, const uint8_t *data) {
    auto now = std::chrono::system_clock::now();
    auto duration_since_epoch = now.time_since_epoch();
    uint64_t time_usec = std::chrono::duration_cast<std::chrono::microseconds>(duration_since_epoch).count();
    if (controller.packet_num == 0) {
        controller.start_time = time_usec;
    }
    MavlinkCaptureDataType packet;
    packet.dataLength = dataLength;
    packet.owner = owner;
    packet.relativeTimestamp = time_usec - controller.start_time;
    uint64_t packet_size = sizeof(packet.dataLength) + sizeof(packet.owner) + (packet.relativeTimestamp) + dataLength;

    if (controller.offset + packet_size > controller.memsize) {
        controller.memsize += MAVLINK_CAPTURE_INC_DATA_SIZE;
        uint8_t* newData = (uint8_t*) realloc(controller.data, controller.memsize);
        if (newData == nullptr) {
            std::cerr << "Memory allocation failed." << std::endl;
            return false;
        }
        controller.data = newData;
    }

    // Copy dataLength and data to cache
    //std::cout << "off: " << controller.offset << " datalen=" << packet.dataLength << std::endl;
    memcpy(&controller.data[controller.offset], &packet.dataLength, sizeof(packet.dataLength));
    controller.offset += sizeof(packet.dataLength);
    controller.total_size += sizeof(packet.dataLength);

    memcpy(&controller.data[controller.offset], &packet.owner, sizeof(packet.owner));
    controller.offset += sizeof(packet.owner);
    controller.total_size += sizeof(packet.owner);

    //std::cout << "off: " << controller.offset << " relativeTimestamp=" << packet.relativeTimestamp << std::endl;
    memcpy(&controller.data[controller.offset], &packet.relativeTimestamp, sizeof(packet.relativeTimestamp));
    controller.offset += sizeof(packet.relativeTimestamp);
    controller.total_size += sizeof(packet.relativeTimestamp);
    //std::cout << "off: " << controller.offset << " data" << std::endl;
    memcpy(&controller.data[controller.offset], data, dataLength);
    controller.offset += dataLength;
    controller.total_size += dataLength;

    controller.packet_num++;

    controller.packets_since_last_save++;
    if (controller.packets_since_last_save >= MAVLINK_SAVE_PACKET_NUM) {
        mavlink_capture_save(controller);
        controller.packets_since_last_save = 0;
    }
    return true;
}

#define HEADER_SIZE  (sizeof(controller.start_time) + sizeof(controller.packet_num) + sizeof(controller.total_size))
bool mavlink_capture_save(MavlinkCaptureControllerType &controller) {
    if (controller.save_file == -1 || controller.data == nullptr) {
        std::cerr << "Invalid file descriptor or data." << std::endl;
        return false;
    }

    // Update the metadata at the start of the file
    ssize_t ret = pwrite(controller.save_file, &controller.start_time, sizeof(controller.start_time), 0);
    if (ret != sizeof(controller.start_time)) {
        std::cerr << "failed to write data start_time: ret = " << ret << std::endl;
        return false;
    }
    ret = pwrite(controller.save_file, &controller.packet_num, sizeof(controller.packet_num), 8);
    if (ret != sizeof(controller.packet_num)) {
        std::cerr << "failed to write data packet_num: ret = " << ret << std::endl;
        return false;
    }
    ret = pwrite(controller.save_file, &controller.total_size, sizeof(controller.total_size), 16);
    if (ret != sizeof(controller.total_size)) {
        std::cerr << "failed to write data total_size: ret = " << ret << std::endl;
        return false;
    }

    //std::cout << "packet_num: " << controller.packet_num << std::endl;
    //std::cout << "offset: " << controller.offset << std::endl;
    //std::cout << "total_size: " << controller.total_size << std::endl;
    //std::cout << "last_save_offset: " << controller.last_save_offset << std::endl;
    off_t loff = controller.last_save_offset - HEADER_SIZE;
    off_t woff = controller.last_save_offset;
    size_t wsize = controller.offset - loff;
    //std::cout << "loff: " << loff << std::endl;
    //std::cout << "woff: " << woff << std::endl;
    //std::cout << "wsize: " << wsize << std::endl;
    ret = pwrite(controller.save_file, 
        &controller.data[loff], wsize, woff);
    if (ret != (ssize_t)wsize) {
        std::cerr << "Can not write file. errno= " << errno << std::endl;
        std::cerr << "woff= " << woff << std::endl;
        std::cerr << "data_size= " << wsize << std::endl;
        exit(1);
        return false;
    }

    // Update the last save offset
    controller.last_save_offset += ret;
    fsync(controller.save_file);
    return true;
}
