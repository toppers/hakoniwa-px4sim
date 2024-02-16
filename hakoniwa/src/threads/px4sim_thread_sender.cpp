#include "px4sim_thread_sender.hpp"
#include "mavlink.h"
#include "../mavlink/mavlink_encoder.hpp"
#include "../comm/tcp_connector.hpp"
#include "../hako/pdu/hako_pdu_data.hpp"
#include "../mavlink/mavlink_dump.hpp"
#include <iostream>
#include "utils/csv_logger.hpp"
#include "mavlink/log/mavlink_log_hil_sensor.hpp"
#include "mavlink/log/mavlink_log_hil_gps.hpp"
#include "config/drone_config.hpp"
#include "../mavlink/mavlink_msg_types.hpp"
#include "hako/runner/hako_px4_master.hpp"

static void px4sim_send_hil_gps(int index, hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec);
static void px4sim_send_sensor(int index, hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec);

static std::vector<std::unique_ptr<hako::px4::comm::ICommIO>> px4_comm_ios_unique;

using hako::assets::drone::mavlink::log::MavlinkLogHilSensor;
using hako::assets::drone::mavlink::log::MavlinkLogHilGps;
static std::vector<CsvLogger> logger_hil_sensors;
static std::vector<CsvLogger> logger_hil_gpss;
static std::vector<MavlinkLogHilSensor> log_hil_sensors;
static std::vector<MavlinkLogHilGps> log_hil_gpss;

void px4sim_sender_init(hako::px4::comm::ICommIO *comm_io)
{
    DroneConfig drone_config;
    int index = px4_comm_ios_unique.size();
    if (drone_config_manager.getConfig(index, drone_config) == false) {
        std::cerr << "ERROR: " << "drone_config_manager.getConfig() error" << std::endl;
        return;
    }
    px4_comm_ios_unique.push_back(std::unique_ptr<hako::px4::comm::ICommIO>(comm_io));
    MavlinkLogHilSensor log_hil_sensor;
    MavlinkLogHilGps log_hil_gps;
    log_hil_sensors.push_back(log_hil_sensor);
    log_hil_gpss.push_back(log_hil_gps);
    CsvLogger logger_hil_sensor;
    CsvLogger logger_hil_gps;
    logger_hil_sensors.push_back(logger_hil_sensor);
    logger_hil_gpss.push_back(logger_hil_gps);

    logger_hil_sensors[index].add_entry(log_hil_sensors[index], drone_config.getSimLogFullPath("log_comm_hil_sensor.csv"));
    logger_hil_gpss[index].add_entry(log_hil_gpss[index], drone_config.getSimLogFullPath("log_comm_hil_gps.csv"));
    std::cout << "INFO: px4sim_sender_init(): register comm_io: " << index << std::endl;
    return;
}

void px4sim_send_sensor_data(int index, Hako_uint64 time_usec, Hako_uint64 boot_time_usec)
{
    if (index < 0 || index >= (int)px4_comm_ios_unique.size()) {
        //std::cerr << "ERROR: Index out of range: " << index << std::endl;
        return;
    }    
    static int count = 0;
    (void)boot_time_usec;
    auto* px4_comm_io = px4_comm_ios_unique[index].get();
    if (px4_comm_io == nullptr) {
        return;
    }
    px4sim_send_sensor(index, *px4_comm_io, time_usec);
    if ((count % 10) == 0) {
        px4sim_send_hil_gps(index, *px4_comm_io, time_usec);
    }
    count++;
    return;
}


void px4sim_send_message(hako::px4::comm::ICommIO &clientConnector, MavlinkDecodedMessage &message)
{
    mavlink_message_t mavlinkMsg;
    if (mavlink_encode_message(&mavlinkMsg, &message)) 
    {
        int sentDataLen = 0;
        char packet[MAVLINK_MAX_PACKET_LEN];
        int packetLen = mavlink_get_packet(packet, sizeof(packet), &mavlinkMsg);
        if (packetLen > 0) 
        {
            if (clientConnector.send(packet, packetLen, &sentDataLen)) 
            {
                //std::cout << "Sent MAVLink message with length: " << sentDataLen << std::endl;
#ifdef DRONE_PX4_TX_DEBUG_ENABLE
                mavlink_message_dump(message);
#endif
            } 
            else 
            {
                std::cerr << "Failed to send MAVLink message" << std::endl;
            }
        }
    }
}

void px4sim_send_dummy_command_long(hako::px4::comm::ICommIO &clientConnector)
{
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_LONG;
    
    // Setting up the fields for COMMAND_LONG
    message.data.command_long.target_system = 0; // The system which should execute the command, for example, 1 for the first MAV
    message.data.command_long.target_component = 0; // The component which should execute the command, for example, 0 for a generic component
    message.data.command_long.command = 0x4246;
    message.data.command_long.confirmation = 0; // 0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command)
    message.data.command_long.param1 = 0x9c40; // Parameter 1, as defined by MAV_CMD enum
    message.data.command_long.param2 = 0x45; // Parameter 2, as defined by MAV_CMD enum
    message.data.command_long.param3 = 0; // Parameter 3, as defined by MAV_CMD enum
    message.data.command_long.param4 = 0; // Parameter 4, as defined by MAV_CMD enum
    message.data.command_long.param5 = 0; // Parameter 5, as defined by MAV_CMD enum
    message.data.command_long.param6 = 0; // Parameter 6, as defined by MAV_CMD enum
    message.data.command_long.param7 = 0; // Parameter 7, as defined by MAV_CMD enum
    
    px4sim_send_message(clientConnector, message);
}

void px4sim_send_dummy_command_long_ack(hako::px4::comm::ICommIO &clientConnector)
{
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_LONG;
    
    // Setting up the fields for COMMAND_LONG
    message.data.command_long.target_system = 1; // The system which should execute the command, for example, 1 for the first MAV
    message.data.command_long.target_component = 1; // The component which should execute the command, for example, 0 for a generic component
    message.data.command_long.command = 520;
    message.data.command_long.confirmation = 1; // 0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command)
    message.data.command_long.param1 = 0; // Parameter 1, as defined by MAV_CMD enum
    message.data.command_long.param2 = 0; // Parameter 2, as defined by MAV_CMD enum
    message.data.command_long.param3 = 0; // Parameter 3, as defined by MAV_CMD enum
    message.data.command_long.param4 = 0; // Parameter 4, as defined by MAV_CMD enum
    message.data.command_long.param5 = 0; // Parameter 5, as defined by MAV_CMD enum
    message.data.command_long.param6 = 0; // Parameter 6, as defined by MAV_CMD enum
    message.data.command_long.param7 = 0; // Parameter 7, as defined by MAV_CMD enum
    
    px4sim_send_message(clientConnector, message);
    std::cout << "INFO: COMMAND_LONG ack sended" << std::endl;
}

void px4sim_send_dummy_heartbeat(hako::px4::comm::ICommIO &clientConnector)
{
    // HEARTBEATメッセージの準備
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_HEARTBEAT;
    message.data.heartbeat.type = MAV_TYPE_GENERIC;
    message.data.heartbeat.autopilot = 0;
    message.data.heartbeat.base_mode = 	MAV_MODE_FLAG_HIL_ENABLED;
    message.data.heartbeat.custom_mode = 0;
    message.data.heartbeat.system_status = 0;

    px4sim_send_message(clientConnector, message);
}


static void px4sim_send_hil_gps(int index, hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec)
{
    static bool is_initialized = false;
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_HIL_GPS;

    if (hako_mavlink_read_hil_gps(index, message.data.hil_gps)) {
        is_initialized = true;
    }
    if (is_initialized) {
        message.data.hil_gps.time_usec = time_usec;
        log_hil_gpss[index].set_data(message.data.hil_gps);
        logger_hil_gpss[index].run();
        px4sim_send_message(clientConnector, message);
    }
}

static void px4sim_send_sensor(int index, hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec)
{
    static bool is_initialized = false;
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_HIL_SENSOR;

    if (hako_mavlink_read_hil_sensor(index, message.data.sensor)) {
        is_initialized = true;
    }
    if (is_initialized) {
        message.data.sensor.time_usec = time_usec;
        log_hil_sensors[index].set_data(message.data.sensor);
        logger_hil_sensors[index].run();
        px4sim_send_message(clientConnector, message);
    }
}
