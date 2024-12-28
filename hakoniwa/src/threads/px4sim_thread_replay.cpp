#include "px4sim_thread_replay.hpp"
#include "mavlink.h"
#include "../mavlink/mavlink_encoder.hpp"
#include "../comm/tcp_connector.hpp"
#include "../hako/pdu/hako_pdu_data.hpp"
#include "../mavlink/mavlink_capture_replay.hpp"
#include "../mavlink/mavlink_dump.hpp"
#include "../mavlink/mavlink_decoder.hpp"
#include "../threads/px4sim_thread_sender.hpp"
#include "../utils/hako_params.hpp"
#include "../utils/hako_utils.hpp"

#include <iostream>
#include <unistd.h>
#include <chrono>


void *px4sim_thread_replay(void *arg)
{
    hako::px4::comm::ICommIO *clientConnector = static_cast<hako::px4::comm::ICommIO *>(arg);
    MavlinkCaptureControllerType controller;
    const char* filepath = hako_param_env_get_string(HAKO_CAPTURE_SAVE_FILEPATH);
    if (filepath == nullptr) {
        HAKO_ABORT("Failed to get HAKO_CAPTURE_SAVE_FILEPATH");
    }
    bool ret = mavlink_capture_load_controller(controller, filepath);
    if (ret == false) {
        std::cout << "ERROR: can not create replay thread " << std::endl;
        exit(1);
    }
    //wait replay trigger
    std::cout << "WAIT REPLAY TRIGGER" << std::endl;
    while (true) {
        if (px4_data_hb_received == false) {
            usleep(1000);
            continue;
        }
        if (px4_data_long_received == false) {
            usleep(1000);
            continue;
        }
        break;
    }
    uint64_t prev_timestamp = 0;
    std::cout << "START REPLAYING " << std::endl;
    auto now = std::chrono::system_clock::now();
    auto duration_since_epoch = now.time_since_epoch();
    uint64_t start_time_usec = std::chrono::duration_cast<std::chrono::microseconds>(duration_since_epoch).count();
    while (true) {
        uint8_t recvBuffer[1024];
        uint32_t owner;
        uint32_t recvDataLen;
        uint64_t timestamp;

        ret = mavlink_capture_load_data(controller, 1024, &recvBuffer[0], &recvDataLen, &owner, &timestamp);
        if (ret && recvDataLen > 0) 
        {
            //sleep for send timing
            if (prev_timestamp != 0) {
                usleep(timestamp - prev_timestamp);
            }
            prev_timestamp = timestamp;
            //decode and send
            mavlink_message_t msg;
            ret = mavlink_decode(MAVLINK_CONFIG_CHAN_1, (const char*)recvBuffer, recvDataLen, &msg);
            if (ret) {
                MavlinkDecodedMessage message;
                ret = mavlink_get_message(&msg, &message);
                if (ret == false) {
                    std::cerr << "Failed to get message data" << std::endl;
                    exit(1);
                }
                mavlink_set_timestamp_for_replay_data(message, start_time_usec + timestamp);
                px4sim_send_message(*clientConnector, message);
            }
            else {
                std::cerr << "REPLAY THREAD: Failed to decode data" << std::endl;
                exit(1);
            }
        } else {
            std::cerr << "Failed to load data" << std::endl;
            break;
        }
    }
    std::cout << "END REPLAYING " << std::endl;
    return NULL;
}

void *px4sim_thread_replay_dump(void *arg)
{
    if (arg == nullptr) {
        //OK
    }
    MavlinkCaptureControllerType controller;
    const char* filepath = hako_param_env_get_string(HAKO_CAPTURE_SAVE_FILEPATH);
    if (filepath == nullptr) {
        HAKO_ABORT("Failed to get HAKO_CAPTURE_SAVE_FILEPATH");
    }
    bool ret = mavlink_capture_load_controller(controller, filepath);
    if (ret == false) {
        std::cout << "ERROR: can not create replay thread " << std::endl;
        exit(1);
    }
    uint64_t prev_timestamp = 0;
    std::cout << "START REPLAYING " << std::endl;
    auto now = std::chrono::system_clock::now();
    auto duration_since_epoch = now.time_since_epoch();
    uint64_t start_time_usec = std::chrono::duration_cast<std::chrono::microseconds>(duration_since_epoch).count();
    while (true) {
        uint8_t recvBuffer[1024];
        uint32_t owner;
        uint32_t recvDataLen;
        uint64_t timestamp;

        ret = mavlink_capture_load_data(controller, 1024, &recvBuffer[0], &recvDataLen, &owner, &timestamp);
        if (ret && recvDataLen > 0) 
        {
            //sleep for send timing
            if (prev_timestamp != 0) {
                usleep(timestamp - prev_timestamp);
            }
            prev_timestamp = timestamp;
            //decode and send
            mavlink_message_t msg;
            ret = mavlink_decode(MAVLINK_CONFIG_CHAN_0, (const char*)recvBuffer, recvDataLen, &msg);
            if (ret) {
                MavlinkDecodedMessage message;
                ret = mavlink_get_message(&msg, &message);
                if (ret == false) {
                    std::cerr << "Failed to get message data" << std::endl;
                    exit(1);
                }
                if (owner == 0) {
                    std::cout << "Message Owner: Controller: " << owner << std::endl;
                }
                else {
                    std::cout << "Message Owner: Physics: " << owner << std::endl;
                }
                mavlink_set_timestamp_for_replay_data(message, start_time_usec + timestamp);
                mavlink_msg_dump(msg);
                mavlink_message_dump(message);
            }
            else {
                std::cerr << "RECV THREAD: Failed to decode data" << std::endl;
                exit(1);
            }
        } else {
            std::cout << "END REPLAYING " << std::endl;
            exit(1);
        }
    }
    std::cout << "END REPLAYING " << std::endl;
    return nullptr;
}
