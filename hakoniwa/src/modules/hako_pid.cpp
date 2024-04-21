#include "hako_pid.hpp"
#include "hako_capi.h"
#include "assets/drone/aircraft/aircraft_factory.hpp"
#include "utils/hako_params.hpp"
#include "hako_asset_runner.h"
#include "hako/runner/hako_px4_master.hpp"
#include "config/drone_config.hpp"
#include "hako/pdu/hako_pdu_accessor.hpp"
#include "utils/csv_logger.hpp"
#include "assets/drone/controller/sample_controller.hpp"
#include "utils/hako_utils.hpp"
#include "utils/hako_control_utils.hpp"

#include "utils/hako_osdep.h"
#include <memory.h>
#include <iostream>
#include <thread>

#define HAKO_RUNNER_MASTER_MAX_DELAY_USEC       1000 /* usec*/
#define HAKO_ASSET_NAME "px4sim"
static void* asset_runner(void*);

void hako_pid_main(bool master)
{
    std::string drone_config_directory = hako_param_env_get_string(DRONE_CONFIG_PATH);
    if (drone_config_manager.loadConfigsFromDirectory(drone_config_directory) == 0)
    {
        std::cerr << "ERROR: can not find drone config file on " << drone_config_directory << std::endl;
        return;
    }

    CsvLogger::enable();
    DroneConfig drone_config;
    if (drone_config_manager.getConfig(0, drone_config) == false) {
        std::cerr << "ERROR: " << "drone_config_manager.getConfig() error" << std::endl;
        return;
    }
    if (master) {
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
    }
    asset_runner(nullptr);
    //not reached
    return;
}
static AirCraftModuleSimulator module_simulator;

static void my_setup()
{
    //nothing to do
}
static void my_task()
{
    for (auto& module : module_simulator.get_modules()) {
        hako::assets::drone::DroneDynamicsInputType drone_input = {};
        mi_drone_control_in_t in;
        mi_drone_control_out_t out;
        DronePositionType pos = module.drone->get_drone_dynamics().get_pos();
        DroneEulerType angle = module.drone->get_drone_dynamics().get_angle();
        hako::assets::drone::DroneVelocityBodyFrameType velocity = module.drone->get_drone_dynamics().get_vel_body_frame();
        //hako::assets::drone::DroneAngularVelocityBodyFrameType angular_velocity = container.drone->get_drone_dynamics().get_angular_vel_body_frame();
        hako::assets::drone::DroneAngularVelocityBodyFrameType angular_velocity = module.drone->get_gyro().sensor_value();
        in.context = module.get_context();
        in.mass = module.drone->get_drone_dynamics().get_mass();
        in.drag = module.drone->get_drone_dynamics().get_drag();
        in.pos_x = pos.data.x;
        in.pos_y = pos.data.y;
        in.pos_z = pos.data.z;
        in.euler_x = angle.data.x;
        in.euler_y = angle.data.y;
        in.euler_z = angle.data.z;
        in.u = velocity.data.x;
        in.v = velocity.data.y;
        in.w = velocity.data.z;
        in.p = angular_velocity.data.x;
        in.q = angular_velocity.data.y;
        in.r = angular_velocity.data.z;

        if (module.control_module.controller != nullptr) {
            out = module.control_module.controller->run(&in);
        }
        else {
            out = module.controller->run(in);
        }

        DroneThrustType thrust;
        DroneTorqueType torque;
        thrust.data = out.thrust;
        torque.data.x = out.torque_x;
        torque.data.y = out.torque_y;
        torque.data.z = out.torque_z;

        drone_input.no_use_actuator = true;
        drone_input.manual.control = false;
        drone_input.thrust = thrust;
        drone_input.torque = torque;
        if (module.drone->get_drone_dynamics().has_collision_detection()) {
            do_io_read_collision(module.drone, drone_input.collision);
        }
        if (module.drone->is_enabled_disturbance()) {
            do_io_read_disturb(module.drone, drone_input.disturbance);
        }
        module.drone->run(drone_input);
        calculate_simple_controls(module, thrust);
        do_io_write(module.drone, module.controls);
    }
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
static void* asset_runner(void*)
{
    std::cout << "INFO: setup start" << std::endl;
    DroneConfig drone_config;
    if (drone_config_manager.getConfig(0, drone_config) == false) {
        std::cerr << "ERROR: " << "drone_config_manager.getConfig() error" << std::endl;
        return nullptr;
    }
    Hako_uint64 delta_time_usec = static_cast<Hako_uint64>(drone_config.getSimTimeStep() * 1000000.0);
    module_simulator.init(0, delta_time_usec);
    hako_asset_runner_register_callback(&my_callbacks);
    const char* config_path = hako_param_env_get_string(HAKO_CUSTOM_JSON_PATH);
    if (hako_asset_runner_init(HAKO_ASSET_NAME, config_path, delta_time_usec) == false) {
        std::cerr << "ERROR: " << "hako_asset_runner_init() error" << std::endl;
        return nullptr;
    }
    while (true) {
        std::cout << "INFO: start simulation" << std::endl;
        module_simulator.do_task();
    }
    std::cout << "INFO: end simulation" << std::endl;
    hako_asset_runner_fin();

    return nullptr;
}

