#include "px4sim_thread_receiver.hpp"
#include "mavlink.h"
#include "../mavlink/mavlink_decoder.hpp"
#include "../mavlink/mavlink_dump.hpp"
#include "../comm/tcp_connector.hpp"
#include "../threads/px4sim_thread_sender.hpp"
#include "../hako/pdu/hako_pdu_data.hpp"
#include "config/drone_config.hpp"
#include <iostream>

#include "../mavlink/mavlink_msg_types.hpp"
#include "utils/csv_logger.hpp"
#include "mavlink/log/mavlink_log_hil_actuator_controls.hpp"

using hako::assets::drone::mavlink::log::MavlinkLogHilActuatorControls;
static CsvLogger logger_recv;
static MavlinkLogHilActuatorControls log_hil_actuator_controls;

hako_time_t hako_px4_asset_time = 0;
static uint64_t px4_boot_time = 0;
static void hako_mavlink_write_data(int index, MavlinkDecodedMessage &message)
{
    switch (message.type) {
        case MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS:
            log_hil_actuator_controls.set_data(message.data.hil_actuator_controls);
            logger_recv.run();
            hako_mavlink_write_hil_actuator_controls(index, message.data.hil_actuator_controls);
            if (px4_boot_time == 0) {
                px4_boot_time = message.data.hil_actuator_controls.time_usec;
            }
            else {
                hako_px4_asset_time = message.data.hil_actuator_controls.time_usec - px4_boot_time;
                //std::cout << "px4_asset_time : " << hako_px4_asset_time << std::endl;
                //std::cout << "hako_asset_time: " << hako_asset_time << std::endl;
                //std::cout << "diff_time      : " << (long long)(hako_asset_time - hako_px4_asset_time) << std::endl;
            }
            break;
        case MAVLINK_MSG_TYPE_HEARTBEAT:
            break;
        case MAVLINK_MSG_TYPE_LONG:
            break;
        case MAVLINK_MSG_TYPE_HIL_SENSOR:
            break;
        case MAVLINK_MSG_TYPE_SYSTEM_TIME:
            break;
        case MAVLINK_MSG_TYPE_HIL_GPS:
            break;
        default:
            std::cout << "  Unknown or unsupported MAVLink message type received." << std::endl;
            break;
    }    
}

void *px4sim_thread_receiver(void *arg)
{
    Px4simRcvArgType *rcv_argp = static_cast<Px4simRcvArgType*>(arg);
    std::cout << "INFO: px4 reciver[" << rcv_argp->index << "] start" << std::endl;
    DroneConfig drone_config;
    if (drone_config_manager.getConfig(rcv_argp->index, drone_config) == false) {
        std::cerr << "ERROR: " << "drone_config_manager.getConfig() error" << std::endl;
        return nullptr;
    }
    logger_recv.add_entry(log_hil_actuator_controls, drone_config.getSimLogFullPath("log_comm_hil_actuator_controls.csv"));
    hako::px4::comm::ICommIO *clientConnector = static_cast<hako::px4::comm::ICommIO *>(rcv_argp->comm_io);
    while (true) {
        char recvBuffer[1024];
        int recvDataLen;
        if (clientConnector->recv(recvBuffer, sizeof(recvBuffer), &recvDataLen)) 
        {
            //std::cout << "Received data with length: " << recvDataLen << std::endl;
            mavlink_message_t msg;
            bool ret = mavlink_decode(rcv_argp->index, recvBuffer, recvDataLen, &msg);
            if (ret)
            {
                MavlinkDecodedMessage message;
                ret = mavlink_get_message(&msg, &message);
                if (ret) {
#ifdef DRONE_PX4_RX_DEBUG_ENABLE
                    mavlink_msg_dump(msg);
                    mavlink_message_dump(message);
#endif
                    if (message.type == MAVLINK_MSG_TYPE_LONG) {
                        px4sim_send_dummy_command_long_ack(*clientConnector);
                    }
                    hako_mavlink_write_data(rcv_argp->index, message);
                }
            }
        } else {
            //std::cerr << "Failed to receive data" << std::endl;
        }
    }
    return NULL;
}