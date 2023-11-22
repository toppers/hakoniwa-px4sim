
#include "px4sim_thread_capture.hpp"
#include "mavlink.h"
#include "../comm/tcp_connector.hpp"
#include "../hako/pdu/hako_pdu_data.hpp"

#include <iostream>

#include "../mavlink/mavlink_msg_types.hpp"
#include "../mavlink/mavlink_capture.hpp"
#include "../utils/hako_params.hpp"
#include "../utils/hako_utils.hpp"

void *px4sim_thread_capture(void *arg)
{
    MavlinkCaptureControllerType controller;
    const char* filepath = hako_param_env_get_string(HAKO_CAPTURE_SAVE_FILEPATH);
    if (filepath == nullptr) {
        HAKO_ABORT("Failed to get HAKO_CAPTURE_SAVE_FILEPATH");
    }
    bool ret = mavlink_capture_create_controller(controller, filepath);
    if (ret == false) {
        std::cout << "ERROR: can not create capture thread " << std::endl;
        exit(1);
    }
    std::cout << "START CAPTURING " << std::endl;
    hako::px4::comm::ICommIO *clientConnector = static_cast<hako::px4::comm::ICommIO *>(arg);
    while (true) {
        char recvBuffer[1024];
        int recvDataLen;
        if (clientConnector->recv(recvBuffer, sizeof(recvBuffer), &recvDataLen)) 
        {
            std::cout << "Capture data with length: " << recvDataLen << std::endl;
            ret = mavlink_capture_append_data(controller, MAVLINK_CAPTURE_DATA_OWNER_PHYSICS, recvDataLen, (const uint8_t*) recvBuffer);
            if (ret == false) {
                std::cerr << "Failed to capture data" << std::endl;
            }
        } else {
            std::cerr << "Failed to receive data" << std::endl;
        }
    }
    return NULL;
}
