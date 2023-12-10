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

#include <unistd.h>
#include <memory.h>
#include <iostream>

#define HAKO_RUNNER_MASTER_MAX_DELAY_USEC       1000 /* usec*/
#define HAKO_RUNNER_MASTER_DELTA_USEC           1000 /* usec*/
#define HAKO_AVATOR_CHANNLE_ID_MOTOR    0
#define HAKO_AVATOR_CHANNLE_ID_POS      1
#define HAKO_AVATOR_CHANNLE_ID_CTRL     2

#define HAKO_ROBO_NAME "px4sim"

static void asset_runner();

static IAirCraft *drone;

void hako_sim_main(hako::px4::comm::IcommEndpointType serverEndpoint)
{
    hako::px4::comm::TcpServer server;
    if (!hako_master_init()) {
        std::cerr << "ERROR: " << "hako_master_init() error" << std::endl;
        return;
    }
    else {
        std::cout << "INFO: hako_master_init() success" << std::endl;
    }
    hako_master_set_config_simtime(HAKO_RUNNER_MASTER_MAX_DELAY_USEC, HAKO_RUNNER_MASTER_DELTA_USEC);
    pthread_t thread;
    if (pthread_create(&thread, NULL, hako_px4_master_thread_run, nullptr) != 0) {
        std::cerr << "Failed to create hako_px4_master_thread_run thread!" << std::endl;
        return;
    }
    auto comm_io = server.server_open(&serverEndpoint);
    if (comm_io == nullptr) 
    {
        std::cerr << "Failed to open TCP server" << std::endl;
        return;
    }
    px4sim_sender_init(comm_io);
    if (pthread_create(&thread, NULL, px4sim_thread_receiver, comm_io) != 0) {
        std::cerr << "Failed to create px4sim_thread_receiver thread!" << std::endl;
        return;
    }
    asset_runner();
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

static void do_io_write()
{
    Hako_Twist pos;

    DronePositionType dpos = drone->get_drone_dynamics().get_pos();
    DroneAngleType dangle = drone->get_drone_dynamics().get_angle();
    pos.linear.x = dpos.data.x;
    pos.linear.y = dpos.data.y;
    pos.linear.z = dpos.data.z;
    pos.angular.x = dangle.data.x;
    pos.angular.y = dangle.data.y;
    pos.angular.z = dangle.data.z;
    if (!hako_asset_runner_pdu_write(HAKO_ROBO_NAME, HAKO_AVATOR_CHANNLE_ID_POS, (const char*)&pos, sizeof(pos))) {
        std::cerr << "ERROR: can not write pdu data: pos" << std::endl;
    }
}


static void my_task()
{
    static hako::assets::drone::MavlinkIO mavlink_io;
    static double controls[hako::assets::drone::ROTOR_NUM] = { 0, 0, 0, 0};

    //read Mavlink Message
    mavlink_io.read_actuator_data(controls);

    drone->run(controls);

    //write Mavlink Message
    mavlink_io.write_sensor_data(*drone);

    do_io_write();
    px4sim_sender_do_task();
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

static hako_time_t hako_sim_asset_time = 0;
static void asset_runner()
{
    hako_asset_runner_register_callback(&my_callbacks);
    const char* config_path = hako_param_env_get_string(HAKO_CUSTOM_JSON_PATH);
    if (hako_asset_runner_init(HAKO_ROBO_NAME, config_path, HAKO_RUNNER_MASTER_DELTA_USEC) == false) {
        std::cerr << "ERROR: " << "hako_asset_runner_init() error" << std::endl;
        return;
    }
    while (true) {
        hako_sim_asset_time = 0;
        std::cout << "INFO: start simulation" << std::endl;
        while (true) {
            if (hako_asset_runner_step(1) == false) {
                std::cout << "INFO: stopped simulation" << std::endl;
                break;
            }
            else {
                hako_sim_asset_time += HAKO_RUNNER_MASTER_DELTA_USEC;
            }
        }
    }
    std::cout << "INFO: end simulation" << std::endl;
    hako_asset_runner_fin();

    return;
}

