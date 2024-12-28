#include "hakoniwa/impl/hakoniwa_simulator.hpp"
#include "hakoniwa/impl/hakoniwa_pdu_accessor.hpp"
#include "include/hako_asset.h"
#include <thread>

using namespace hako::drone::impl;
std::shared_ptr<HakoniwaSimulator> HakoniwaSimulator::instance_;

static int my_on_initialize(hako_asset_context_t*)
{
    return 0;
}
static int my_on_reset(hako_asset_context_t*)
{
    return 0;
}
static int my_on_simulation_step(hako_asset_context_t*)
{
    auto simulator = HakoniwaSimulator::getInstance();
    simulator->advanceTimeStep();
    return 0;
}
static hako_asset_callbacks_t my_callback = {
    .on_initialize = my_on_initialize,
    .on_simulation_step = my_on_simulation_step,
    .on_manual_timing_control = NULL,
    .on_reset = my_on_reset
};

bool HakoniwaSimulator::registerService(std::string& asset_name, std::string& config_path, uint64_t delta_time_usec, std::shared_ptr<IServiceContainer> service_container)
{
    delta_time_usec_ = delta_time_usec;
    HakoniwaSimulator::service_container_ = service_container;
    std::cout << "asset_name = " << asset_name << std::endl;
    std::cout << "config_path = " << config_path << std::endl;
    int ret = hako_asset_register(asset_name.c_str(), config_path.c_str(), &my_callback, delta_time_usec, HAKO_ASSET_MODEL_CONTROLLER);
    if (ret == 0) {
        ret = HakoniwaPduAccessor::init();
        if (!ret) {
            throw std::runtime_error("Failed to initialize HakoniwaPduAccessor");
        }
        return true;
    }
    else {
        std::cerr << "ERROR: " << "hako_asset_register() error: " << std::endl;
        return false;
    }
}

bool HakoniwaSimulator::startService()
{
    bool do_task = false;
    mtx_.lock();
    if (isStarted_) {
        mtx_.unlock();
        std::cerr << "ERROR: " << "HakoniwaSimulator is already started" << std::endl;
        return false;
    }
    isStarted_ = true;
    do_task = true;
    mtx_.unlock();
    service_container_->startService(delta_time_usec_);
    do {
        (void)hako_asset_start();
        mtx_.lock();
        do_task = isStarted_;
        mtx_.unlock();
    } while (do_task);    
    return true;
}

bool HakoniwaSimulator::stopService()
{
    mtx_.lock();
    isStarted_ = false;
    mtx_.unlock();
    return false;
}
