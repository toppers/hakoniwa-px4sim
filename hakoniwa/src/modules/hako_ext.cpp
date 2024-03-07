#include "hako_ext.hpp"
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
#include "assets/drone/controller/drone_control_proxy.hpp"

#include "utils/hako_osdep.h"
#include <memory.h>
#include <iostream>
#include <thread>

#define HAKO_RUNNER_MASTER_MAX_DELAY_USEC       1000 /* usec*/
#define HAKO_ASSET_NAME "px4sim"
static void* asset_runner(void*);

void hako_ext_main(bool master)
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
static hako::assets::drone::DroneControlProxyManager drone_control_proxy_manager;

static void my_setup()
{
    //nothing to do
}

static void my_task()
{

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
    drone_control_proxy_manager.init(0, delta_time_usec);
    hako_asset_runner_register_callback(&my_callbacks);
    const char* config_path = hako_param_env_get_string(HAKO_CUSTOM_JSON_PATH);
    if (hako_asset_runner_init(HAKO_ASSET_NAME, config_path, delta_time_usec) == false) {
        std::cerr << "ERROR: " << "hako_asset_runner_init() error" << std::endl;
        return nullptr;
    }
    while (true) {
        std::cout << "INFO: start simulation" << std::endl;
        drone_control_proxy_manager.run();
    }
    std::cout << "INFO: end simulation" << std::endl;
    hako_asset_runner_fin();

    return nullptr;
}

