#include "hako_phys.hpp"
#include "hako_capi.h"
#include "assets/drone/aircraft/aircraft_factory.hpp"
#include "assets/drone/controller/drone_pid_control.hpp"
#include "hako_asset_runner.h"
#include "hako/pdu/hako_pdu_data.hpp"
#include "hako/runner/hako_px4_master.hpp"
#include "config/drone_config.hpp"
#include "utils/hako_params.hpp"
#include "utils/csv_logger.hpp"

#include "utils/hako_osdep.h"
#include <memory.h>
#include <iostream>
#include <thread>

#define HAKO_RUNNER_MASTER_MAX_DELAY_USEC       1000 /* usec*/
#define HAKO_AVATOR_CHANNLE_ID_MOTOR    0
#define HAKO_AVATOR_CHANNLE_ID_POS      1
#define HAKO_AVATOR_CHANNLE_ID_CTRL     2

#define HAKO_ROBO_NAME "px4sim"

static void asset_runner();

static IAirCraft *drone;

void hako_phys_main()
{
    std::string drone_config_path = hako_param_env_get_string(DRONE_CONFIG_PATH);
    if (drone_config_manager.loadConfigFromFile(drone_config_path) == 0)
    {
        std::cerr << "ERROR: can not find drone config file on " << drone_config_path << std::endl;
        return;
    }
    CsvLogger::enable();
    DroneConfig drone_config;
    if (drone_config_manager.getConfig(0, drone_config) == false) {
        std::cerr << "ERROR: " << "drone_config_manager.getConfig() error" << std::endl;
        return;
    }
    if (!hako_master_init()) {
        std::cerr << "ERROR: " << "hako_master_init() error" << std::endl;
        return;
    }
    else {
        std::cout << "INFO: hako_master_init() success" << std::endl;
    }
    hako_master_set_config_simtime((drone_config.getSimTimeStep()*1000000), (drone_config.getSimTimeStep()*1000000));
    try {
        std::thread thread(hako_px4_master_thread_run, (void*)nullptr);
        thread.detach();
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to create hako_px4_master_thread_run thread: " << e.what() << std::endl;
        return;
    }
    asset_runner();
    //not reached
    return;
}

static void my_setup()
{
    std::cout << "INFO: setup start" << std::endl;
    DroneConfig drone_config;
    if (drone_config_manager.getConfig(0, drone_config) == false) {
        std::cerr << "ERROR: " << "drone_config_manager.getConfig() error" << std::endl;
        return;
    }
    drone = hako::assets::drone::create_aircraft(0, drone_config);
    drone_pid_control_init();

    std::cout << "INFO: setup done" << std::endl;
    return;
}

static void do_io_write()
{
    Hako_Twist pos;

    DronePositionType dpos = drone->get_drone_dynamics().get_pos();
    DroneEulerType dangle = drone->get_drone_dynamics().get_angle();
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
    DroneThrustType thrust;
    DroneTorqueType torque;
    {
        Hako_Twist control;

        if (!hako_asset_runner_pdu_read(HAKO_ROBO_NAME, HAKO_AVATOR_CHANNLE_ID_CTRL, (char*)&control, sizeof(control))) {
            std::cerr << "ERROR: can not read pdu data: control" << std::endl;
            return;
        }
        thrust.data = control.linear.z;
        torque.data.x = control.angular.x;
        torque.data.y = control.angular.y;
        torque.data.z = control.angular.z;
    }
    hako::assets::drone::DroneDynamicsInputType input;
    input.no_use_actuator = true;
    input.thrust = thrust;
    input.torque = torque;
    drone->run(input);
    drone_pid_control_run();
    do_io_write();
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

static hako_time_t hako_asset_time = 0;
static void asset_runner()
{
    std::cout << "INFO: setup start" << std::endl;
    DroneConfig drone_config;
    if (drone_config_manager.getConfig(0, drone_config) == false) {
        std::cerr << "ERROR: " << "drone_config_manager.getConfig() error" << std::endl;
        return;
    }
    Hako_uint64 delta_time_usec = static_cast<Hako_uint64>(drone_config.getSimTimeStep() * 1000000.0);
    hako_asset_runner_register_callback(&my_callbacks);
    const char* config_path = hako_param_env_get_string(HAKO_CUSTOM_JSON_PATH);
    if (hako_asset_runner_init(HAKO_ROBO_NAME, config_path, delta_time_usec) == false) {
        std::cerr << "ERROR: " << "hako_asset_runner_init() error" << std::endl;
        return;
    }
    while (true) {
        hako_asset_time = 0;
        std::cout << "INFO: start simulation" << std::endl;
        while (true) {
            if (hako_asset_runner_step(1) == false) {
                std::cout << "INFO: stopped simulation" << std::endl;
                break;
            }
            else {
                hako_asset_time += delta_time_usec;
                CsvLogger::set_time_usec(hako_asset_time);
            }
        }
    }
    std::cout << "INFO: end simulation" << std::endl;
    hako_asset_runner_fin();

    return;
}

