#include "hako_replay.hpp"
#include "hako_capi.h"
#include "assets/drone/aircraft/aircraft_factory.hpp"
#include "utils/hako_params.hpp"
#include "hako_asset_runner.h"
#include "hako/runner/hako_px4_master.hpp"
#include "config/drone_config.hpp"
#include "hako/pdu/hako_pdu_accessor.hpp"
#include "utils/hako_replayer.hpp"
#include "hako/runner/hako_px4_master.hpp"

#include "utils/hako_osdep.h"
#include <memory.h>
#include <iostream>
#include <thread>

static HakoReplayer hako_replayer;

#define HAKO_ASSET_NAME "hako-replay"
static void* asset_runner(void*);

void hako_replay_main(bool master)
{
    std::string drone_config_directory = hako_param_env_get_string(DRONE_CONFIG_PATH);
    if (drone_config_manager.loadConfigsFromDirectory(drone_config_directory) == 0)
    {
        std::cerr << "ERROR: can not find drone config file on " << drone_config_directory << std::endl;
        return;
    }

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

static void my_setup()
{
    std::cout << "my_setup: enter" << std::endl;
    for (int i = 0; i < drone_config_manager.getConfigCount(); i++) {
        DroneConfig config;
        if (drone_config_manager.getConfig(i, config) == false) {
            std::cerr << "ERROR: " << "drone_config_manager.getConfig(" << i << ") error" << std::endl;
            return;
        }
        hako_replayer.set_mass(config.getRoboName(), config.getCompDroneDynamicsMass());
        hako_replayer.add_vehicle(config.getRoboName(), config.getSimLogFullPathFromIndex(i, "drone_dynamics.csv"));
    }
}


static inline void replay_for_controls(double hovering_force, const DroneThrustType& thrust, double controls[hako::assets::drone::ROTOR_NUM])
{
    double max_hovering_force = 2.0 * hovering_force;
    double control = 0;
    if (thrust.data > max_hovering_force) {
        control = 1.0;
    } else {
        control = thrust.data / max_hovering_force;
    }
    for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
        controls[i] = control;
    }
    return;
}
static void my_task()
{
    std::vector<std::string> vehicle_ids = hako_replayer.get_vehicle_ids();
    for (auto& id : vehicle_ids) {
        std::vector<std::string> data;
        if (hako_replayer.next(id, data)) {
            //X, Y, Z
            DronePositionType position;
            position.data.x = std::stod(data[1]);
            position.data.y = std::stod(data[2]);
            position.data.z = std::stod(data[3]);
            //std::cout << "position(" << position.data.x << ", " << position.data.y << ", " << position.data.z << " )" << std::endl;
            //Rx, Ry, Rz
            DroneEulerType angle;
            angle.data.x = std::stod(data[4]);
            angle.data.y = std::stod(data[5]);
            angle.data.z = std::stod(data[6]);
            //std::cout << "angle(" << angle.data.x << ", " << angle.data.y << ", " << angle.data.z << " )" << std::endl;
            DroneThrustType thrust;
            thrust.data = std::stod(data[13]);
            //std::cout << "thrust(" << thrust.data << " )" << std::endl;
            double controls[hako::assets::drone::ROTOR_NUM];
            double param = hako_replayer.get_mass(id) * hako::assets::drone::GRAVITY;
            replay_for_controls(param, thrust, controls);
            do_io_write_replay_data(id, position, angle, controls);
        }
    }    
    return;
}

static void my_reset()
{
    std::cout << "my_reset: enter" << std::endl;
    hako_replayer.close_all();
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

    hako_asset_runner_register_callback(&my_callbacks);
    const char* config_path = hako_param_env_get_string(HAKO_CUSTOM_JSON_PATH);
    if (hako_asset_runner_init(HAKO_ASSET_NAME, config_path, delta_time_usec) == false) {
        std::cerr << "ERROR: " << "hako_asset_runner_init() error" << std::endl;
        return nullptr;
    }
    while (true) {
        if (hako_asset_runner_step(1) == false) {
            std::cout << "INFO: stopped simulation" << std::endl;
            continue;
        }
    }
    std::cout << "INFO: end simulation" << std::endl;
    hako_asset_runner_fin();

    return nullptr;
}

