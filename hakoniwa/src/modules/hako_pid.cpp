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

#include "utils/hako_osdep.h"
#include <memory.h>
#include <iostream>
#include <thread>

#define HAKO_RUNNER_MASTER_MAX_DELAY_USEC       1000 /* usec*/
#define HAKO_ASSET_NAME "px4sim"
static void* asset_runner(void*);
static hako::assets::drone::AirCraftManager drone_manager;

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
#include "utils/hako_module_loader.hpp"
#include "hako_module_drone_controller.h"

typedef struct {
    void *handle;
    HakoModuleHeaderType *header;
    HakoModuleDroneControllerType *controller;
} AircraftControlModuleType;

class AircraftSystemContainer
{
public:
    AircraftControlModuleType control_module;
    hako::assets::drone::IController *controller;
    IAirCraft *drone;
    double controls[hako::assets::drone::ROTOR_NUM] = { 0, 0, 0, 0};

    bool load_controller(const char* filepath) 
    {
        control_module.header = nullptr;
        control_module.controller = nullptr;
        control_module.handle = hako_module_handle(filepath, &control_module.header);
        if (control_module.handle == nullptr) {
            return false;
        }
        control_module.controller = (HakoModuleDroneControllerType*)hako_module_load_symbol(control_module.handle, HAKO_MODULE_DRONE_CONTROLLER_SYMBOLE_NAME);
        if (control_module.controller == nullptr) {
            return false;
        }
        std::cout << "SUCCESS: Loaded module name: " << control_module.header->get_name() << std::endl;
        return (control_module.controller->init(nullptr) == 0);
    }

};

class AircraftSystemTaskManager
{
private:
    bool do_asset_task()
    {
        CsvLogger::set_time_usec(hako_asset_time_usec);
        if (hako_asset_runner_step(1) == false) {
            std::cout << "INFO: stopped simulation" << std::endl;
            return false;
        }
        hako_asset_time_usec += delta_time_usec;
        return true;
    }

public:
    std::vector<AircraftSystemContainer> aircraft_system_container;
    Hako_uint64 hako_asset_time_usec;
    Hako_uint64 delta_time_usec;
    void init(Hako_uint64 microseconds, Hako_uint64 dt_usec)
    {
        hako_asset_time_usec = microseconds;
        delta_time_usec = dt_usec;
        drone_manager.createAirCrafts(drone_config_manager);
        for (auto* drone : drone_manager.getAllAirCrafts()) {
            std::cout << "INFO: loading drone & controller: " << drone->get_index() << std::endl;
            AircraftSystemContainer arg;
            arg.drone = drone;
            DroneConfig drone_config;
            drone_config_manager.getConfig(drone->get_index(), drone_config);
            arg.control_module.controller = nullptr;
            std::string filepath = drone_config.getControllerModuleFilePath();
            if (!filepath.empty()) {
                arg.load_controller(filepath.c_str());
            }
            else {
                std::cerr << "WARNING: can not find module for " << drone->get_name() << std::endl;
            }
            if (arg.control_module.controller == nullptr) {
                HAKO_ASSERT(drone_config.isExistController("pid"));
                arg.controller = new hako::assets::drone::SampleController(drone->get_index());
                if (arg.controller == nullptr) {
                    std::cerr << "ERROR: can not create Controller: " << drone->get_index() << std::endl;
                    HAKO_ASSERT(arg.controller != nullptr);
                    return;
                }
            }
            aircraft_system_container.push_back(arg);
        }
    }
    void do_task()
    {
        while (do_asset_task() == true){};
    }
};
static AircraftSystemTaskManager task_manager;

static void my_setup()
{
    //nothing to do
}
static void calculate_simple_controls(AircraftSystemContainer& container, const DroneThrustType& thrust)
{
    double hovering_force = container.drone->get_drone_dynamics().get_mass() * hako::assets::drone::GRAVITY;
    double max_hovering_force = 2.0 * hovering_force;
    double control = 0;
    if (thrust.data > max_hovering_force) {
        control = 1.0;
    }
    else {
        control = thrust.data / max_hovering_force;
    }
    for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
        container.controls[i] = control;
    }
    return;
}
static void my_task()
{
    for (auto& container : task_manager.aircraft_system_container) {
        hako::assets::drone::DroneDynamicsInputType drone_input = {};
        mi_drone_control_in_t in;
        mi_drone_control_out_t out;
        DronePositionType pos = container.drone->get_drone_dynamics().get_pos();
        DroneEulerType angle = container.drone->get_drone_dynamics().get_angle();
        hako::assets::drone::DroneVelocityBodyFrameType velocity = container.drone->get_drone_dynamics().get_vel_body_frame();
        //hako::assets::drone::DroneAngularVelocityBodyFrameType angular_velocity = container.drone->get_drone_dynamics().get_angular_vel_body_frame();
        hako::assets::drone::DroneAngularVelocityBodyFrameType angular_velocity = container.drone->get_gyro().sensor_value();
        in.mass = container.drone->get_drone_dynamics().get_mass();
        in.drag = container.drone->get_drone_dynamics().get_drag();
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

        if (container.control_module.controller != nullptr) {
            out = container.control_module.controller->run(&in);
        }
        else {
            out = container.controller->run(in);
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
        if (container.drone->get_drone_dynamics().has_collision_detection()) {
            do_io_read_collision(container.drone, drone_input.collision);
        }
        if (container.drone->is_enabled_disturbance()) {
            do_io_read_disturb(container.drone, drone_input.disturbance);
        }
        container.drone->run(drone_input);
        calculate_simple_controls(container, thrust);
        do_io_write(container.drone, container.controls);
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
    task_manager.init(0, delta_time_usec);
    hako_asset_runner_register_callback(&my_callbacks);
    const char* config_path = hako_param_env_get_string(HAKO_CUSTOM_JSON_PATH);
    if (hako_asset_runner_init(HAKO_ASSET_NAME, config_path, delta_time_usec) == false) {
        std::cerr << "ERROR: " << "hako_asset_runner_init() error" << std::endl;
        return nullptr;
    }
    while (true) {
        std::cout << "INFO: start simulation" << std::endl;
        task_manager.do_task();
    }
    std::cout << "INFO: end simulation" << std::endl;
    hako_asset_runner_fin();

    return nullptr;
}

