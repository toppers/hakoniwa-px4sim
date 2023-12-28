#include "hako_sim.hpp"
#include "hako_capi.h"
#include "assets/drone/mavlink/mavlink_io.hpp"
#include "assets/drone/aircraft/aircraft_factory.hpp"
#include "utils/hako_params.hpp"
#include "hako_asset_runner.h"
#include "hako/pdu/hako_pdu_data.hpp"
#include "hako/runner/hako_px4_master.hpp"
#include "threads/px4sim_thread_sender.hpp"
#include "threads/px4sim_thread_receiver.hpp"
#include "config/drone_config.hpp"

#include <unistd.h>
#include <memory.h>
#include <iostream>

#define HAKO_RUNNER_MASTER_MAX_DELAY_USEC       1000 /* usec*/
#define HAKO_AVATOR_CHANNLE_ID_MOTOR    0
#define HAKO_AVATOR_CHANNLE_ID_POS      1
#define HAKO_AVATOR_CHANNLE_ID_CTRL     2

#define HAKO_ROBO_NAME "px4sim"

static void* asset_runner(void*);

static IAirCraft *drone;

void hako_sim_main(bool master, hako::px4::comm::IcommEndpointType serverEndpoint)
{
    hako::px4::comm::TcpServer server;
    pthread_t thread;
    if (master) {
        if (!hako_master_init()) {
            std::cerr << "ERROR: " << "hako_master_init() error" << std::endl;
            return;
        }
        else {
            std::cout << "INFO: hako_master_init() success" << std::endl;
        }
        hako_master_set_config_simtime((drone_config.getSimTimeStep()*1000000), (drone_config.getSimTimeStep()*1000000));
        
        if (pthread_create(&thread, NULL, hako_px4_master_thread_run, nullptr) != 0) {
            std::cerr << "Failed to create hako_px4_master_thread_run thread!" << std::endl;
            return;
        }
    }
    if (pthread_create(&thread, NULL, asset_runner, nullptr) != 0) {
        std::cerr << "Failed to create asset_runner thread!" << std::endl;
        return;
    }

    auto comm_io = server.server_open(&serverEndpoint);
    if (comm_io == nullptr) 
    {
        std::cerr << "Failed to open TCP server" << std::endl;
        return;
    }
    px4sim_sender_init(comm_io);
    px4sim_thread_receiver(comm_io);
    //not reached
    return;
}


static void my_setup()
{
    std::cout << "INFO: setup start" << std::endl;
    drone = hako::assets::drone::create_aircraft("default");

    std::cout << "INFO: setup done" << std::endl;
    return;
}

static void do_io_write(double controls[hako::assets::drone::ROTOR_NUM])
{
    Hako_HakoHilActuatorControls hil_actuator_controls;
    Hako_Twist pos;

    memset(&hil_actuator_controls, 0, sizeof(hil_actuator_controls));
    for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
        hil_actuator_controls.controls[i] = controls[i];
    }
    if (!hako_asset_runner_pdu_write(HAKO_ROBO_NAME, HAKO_AVATOR_CHANNLE_ID_MOTOR, (const char*)&hil_actuator_controls, sizeof(hil_actuator_controls))) {
        std::cerr << "ERROR: can not write pdu data: hil_actuator_controls" << std::endl;
    }

    DronePositionType dpos = drone->get_drone_dynamics().get_pos();
    DroneAngleType dangle = drone->get_drone_dynamics().get_angle();
    pos.linear.x = dpos.data.x;
    pos.linear.y = -dpos.data.y;
    pos.linear.z = -dpos.data.z;
    pos.angular.x = dangle.data.x;
    pos.angular.y = -dangle.data.y;
    pos.angular.z = -dangle.data.z;
    if (!hako_asset_runner_pdu_write(HAKO_ROBO_NAME, HAKO_AVATOR_CHANNLE_ID_POS, (const char*)&pos, sizeof(pos))) {
        std::cerr << "ERROR: can not write pdu data: pos" << std::endl;
    }
}


static double controls[hako::assets::drone::ROTOR_NUM] = { 0, 0, 0, 0};
static hako::assets::drone::MavlinkIO mavlink_io;
static void my_task()
{
    //std::cout << "drone run()" << std::endl;
    drone->run(controls);
    do_io_write(controls);
    return;
}

static void my_reset()
{
    //nothing to do
}

static hako_asset_runner_callback_t my_callbacks = {
    my_setup,   // setup
    NULL,   // write_initial_pdu_data
    my_task,   // do_task
    my_reset    // reset
};
#include <chrono>
#include "utils/csv_logger.hpp"
bool CsvLogger::enable_flag = false;
uint64_t CsvLogger::time_usec = 0; 
static hako_time_t hako_sim_asset_time = 0;
static void* asset_runner(void*)
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    //microseconds = 0;
    Hako_uint64 delta_time_usec = static_cast<Hako_uint64>(drone_config.getSimTimeStep() * 1000000.0);
    bool lockstep = drone_config.getSimLockStep();
    hako_asset_runner_register_callback(&my_callbacks);
    const char* config_path = hako_param_env_get_string(HAKO_CUSTOM_JSON_PATH);
    if (hako_asset_runner_init(HAKO_ROBO_NAME, config_path, delta_time_usec) == false) {
        std::cerr << "ERROR: " << "hako_asset_runner_init() error" << std::endl;
        return nullptr;
    }
    while (true) {
        Hako_uint64 hako_asset_time_usec = microseconds;
        Hako_uint64 px4_time_usec;
        hako_sim_asset_time = 0;
        bool isRecvControl = false;
        std::cout << "INFO: start simulation" << std::endl;
        while (true) {
            //read Mavlink Message
            //std::cout << "lockstep: " << lockstep << " isRecvControl: " << isRecvControl << std::endl;
            if (mavlink_io.read_actuator_data(controls, px4_time_usec) == false) {
                if (lockstep && isRecvControl) {
                    //std::cout << "waiting .... " << std::endl;
                    usleep(delta_time_usec); //1msec sleep
                    continue;
                }
                else {
                    //std::cout << "go!" << std::endl;
                    //case1. lockstep = false
                    //          ==> do not sync with px4 sim timing
                    //case2. lockstep = true && isRecvControl = false
                    //          ==> does not recv HIL_ACTUATOR_CONTROLS yet, so send HIL_SENSOR..
                }
            }
            else {
                isRecvControl = true;
                CsvLogger::enable();
                CsvLogger::set_time_usec(px4_time_usec);
                //std::cout << "recv HIL_ACTUATOR_CONTROLS: " << px4_time_usec << std::endl;
            }

            if (hako_asset_runner_step(1) == false) {
                std::cout << "INFO: stopped simulation" << std::endl;
                break;
            }
            else {
                hako_asset_time_usec += delta_time_usec;
                //write Mavlink Message
                mavlink_io.write_sensor_data(*drone);
                px4sim_send_sensor_data(hako_asset_time_usec, microseconds);
                hako_sim_asset_time += delta_time_usec;
            }
        }
    }
    std::cout << "INFO: end simulation" << std::endl;
    hako_asset_runner_fin();

    return nullptr;
}

