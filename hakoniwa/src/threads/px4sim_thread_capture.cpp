
#include "px4sim_thread_capture.hpp"
#include "mavlink.h"
#include "../comm/tcp_connector.hpp"
#include "../hako/pdu/hako_pdu_data.hpp"

#include <iostream>

#include "../mavlink/mavlink_msg_types.hpp"
#include "../mavlink/mavlink_capture.hpp"

void *px4sim_thread_capture(void *arg)
{
    MavlinkCaptureControllerType controller;
    bool ret = mavlink_capture_create_controller(controller, MAVLINK_CAPTURE_SAVE_FILEPATH);
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
