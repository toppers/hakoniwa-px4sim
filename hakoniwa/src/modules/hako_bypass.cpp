#include "hako_bypass.hpp"
#include "../comm/tcp_connector.hpp"
#include "../utils/hako_params.hpp"
#include "../mavlink/mavlink_capture.hpp"
#include "../utils/hako_utils.hpp"
#include <stdlib.h>
#include <iostream>
#include "utils/csv_logger.hpp"
#include "mavlink/log/mavlink_log_hil_sensor.hpp"
#include "mavlink/log/mavlink_log_hil_gps.hpp"
#include "mavlink/log/mavlink_log_hil_actuator_controls.hpp"
#include "mavlink/mavlink_decoder.hpp"

using hako::assets::drone::mavlink::log::MavlinkLogHilSensor;
using hako::assets::drone::mavlink::log::MavlinkLogHilGps;
using hako::assets::drone::mavlink::log::MavlinkLogHilActuatorControls;

static CsvLogger logger_hil_sensor;
static CsvLogger logger_hil_gps;
static CsvLogger logger_hil_actuator_controls;

static MavlinkLogHilSensor log_hil_sensor;
static MavlinkLogHilGps log_hil_gps;
static MavlinkLogHilActuatorControls log_hil_actuator_controls;

typedef struct {
    const char* name;
    uint32_t owner;
    hako::px4::comm::ICommIO *src_comm;
    hako::px4::comm::ICommIO *dst_comm;
    MavlinkCaptureControllerType *capture;
    pthread_mutex_t *mutex;
} HakoBypassCommType;

static void hako_bypass_logging(const char* recvBuffer, int recvDataLen)
{
    mavlink_message_t msg;
    bool ret = mavlink_decode(MAVLINK_CONFIG_CHAN_0, recvBuffer, recvDataLen, &msg);
    if (ret)
    {
        MavlinkDecodedMessage message;
        ret = mavlink_get_message(&msg, &message);
        if (ret) {
            switch (message.type) {
            case MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS:
                log_hil_actuator_controls.set_data(message.data.hil_actuator_controls);
                logger_hil_actuator_controls.run();
                break;
            case MAVLINK_MSG_TYPE_HIL_SENSOR:
                log_hil_sensor.set_data(message.data.sensor);
                logger_hil_sensor.run();
                break;
            case MAVLINK_MSG_TYPE_HIL_GPS:
                log_hil_gps.set_data(message.data.hil_gps);
                logger_hil_gps.run();
                break;
            default:
                break;
            }
        }
    }    
}

static void *hako_bypass_thread(void *argp)
{
    HakoBypassCommType *bypass_ctrl = (HakoBypassCommType*)argp;
    std::cout << "INFO: start " << bypass_ctrl->name << " : " << bypass_ctrl->owner << std::endl;

    if (bypass_ctrl->owner == MAVLINK_CAPTURE_DATA_OWNER_CONTROL) {
        //px4
        logger_hil_actuator_controls.add_entry(log_hil_actuator_controls, "./log_comm_hil_actuator_controls.csv");
    }
    else {
        //airsim
        logger_hil_sensor.add_entry(log_hil_sensor, "./log_comm_hil_sensor.csv");
        logger_hil_gps.add_entry(log_hil_gps, "./log_comm_hil_gps.csv");
    }

    while (true) {
        char recvBuffer[1024];
        int recvDataLen;
        if (bypass_ctrl->src_comm->recv(recvBuffer, sizeof(recvBuffer), &recvDataLen)) 
        {
            //std::cout << "Capture data with length: " << recvDataLen << std::endl;
            pthread_mutex_lock(bypass_ctrl->mutex);
            bool ret = mavlink_capture_append_data(*bypass_ctrl->capture, bypass_ctrl->owner, recvDataLen, (const uint8_t*) recvBuffer);
            pthread_mutex_unlock(bypass_ctrl->mutex);
            if (ret == false) {
                std::cerr << "ERROR: " << bypass_ctrl->name << " Failed to capture data" << std::endl;
            }
            hako_bypass_logging(recvBuffer, recvDataLen);

            int sndLen = 0;
            ret = bypass_ctrl->dst_comm->send(recvBuffer, recvDataLen, &sndLen);
            if (ret == false) {
                //std::cerr << "ERROR: " << bypass_ctrl->name << " Failed to send data" << std::endl;
            }
        } else {
            //std::cerr << "ERROR: " << bypass_ctrl->name << " Failed to receive data" << std::endl;
        }
    }
    return nullptr;
}

void hako_bypass_main(const char* sever_ipaddr, int server_portno)
{
    hako::px4::comm::ICommIO *phys_comm  = nullptr;
    hako::px4::comm::ICommIO *ctrl_comm  = nullptr;
    MavlinkCaptureControllerType capture;
    {
        const char* filepath = hako_param_env_get_string(HAKO_CAPTURE_SAVE_FILEPATH);
        if (filepath == nullptr) {
            HAKO_ABORT("Failed to get HAKO_CAPTURE_SAVE_FILEPATH");
        }
        if (mavlink_capture_create_controller(capture, filepath) == false) {
            HAKO_ABORT("Failed to create capture controller");
        }
    }
    {
        hako::px4::comm::TcpClient client;
        std::cout << "INFO: connecting phys server" << std::endl;
        const char* srv_ip = hako_param_env_get_string(HAKO_BYPASS_IPADDR);
        if (srv_ip == NULL) {
            HAKO_ABORT("Failed to get HAKO_BYPASS_IPADDR");
        }
        int portno;
        if (hako_param_env_get_integer(HAKO_BYPASS_PORTNO, &portno) == false) {
            HAKO_ABORT("Failed to get HAKO_BYPASS_PORTNO");
        }
        hako::px4::comm::IcommEndpointType physEndpoint = { srv_ip, portno };
        std::cout << "client ipaddr: " << physEndpoint.ipaddr << std::endl;
        std::cout << "client portno: " << physEndpoint.portno << std::endl;
        phys_comm = client.client_open(nullptr, &physEndpoint);
        if (phys_comm == nullptr) 
        {
            HAKO_ABORT("Failed to connect phys");
        }
        std::cout << "INFO: connected phys server" << std::endl;
    }
    {
        hako::px4::comm::TcpServer srv_comm;
        std::cout << "INFO: waiting for controller connection" << std::endl;
        hako::px4::comm::IcommEndpointType ctrlEndpoint = { sever_ipaddr, server_portno };
        ctrl_comm = srv_comm.server_open(&ctrlEndpoint);
        if (ctrl_comm == nullptr) 
        {
            HAKO_ABORT("Failed to connect controller");
        }
        std::cout << "INFO: connected to controller" << std::endl;
    }
    pthread_mutex_t capture_mutex = PTHREAD_MUTEX_INITIALIZER;
    static HakoBypassCommType phys2ctrl_arg;
    {
        pthread_t thread;
        phys2ctrl_arg.name = "phys2ctrl";
        phys2ctrl_arg.src_comm = phys_comm;
        phys2ctrl_arg.dst_comm = ctrl_comm;
        phys2ctrl_arg.capture = &capture;
        phys2ctrl_arg.mutex = &capture_mutex;
        phys2ctrl_arg.owner = (uint32_t)MAVLINK_CAPTURE_DATA_OWNER_PHYSICS;
        if (pthread_create(&thread, NULL, hako_bypass_thread, &phys2ctrl_arg) != 0) {
            HAKO_ABORT("Failed to create thread phys2ctrl");
        }
    }
    static HakoBypassCommType ctrl2phys_arg;
    {
        ctrl2phys_arg.name = "ctrl2phys";
        ctrl2phys_arg.src_comm = ctrl_comm;
        ctrl2phys_arg.dst_comm = phys_comm;
        ctrl2phys_arg.capture = &capture;
        ctrl2phys_arg.mutex = &capture_mutex;
        ctrl2phys_arg.owner = (uint32_t)MAVLINK_CAPTURE_DATA_OWNER_CONTROL;
        hako_bypass_thread(&ctrl2phys_arg);
    }
    HAKO_ABORT("Not reached: hako_bypass");
    return;
}