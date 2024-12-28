#include "hakoniwa/impl/hakoniwa_drone_service.hpp"
#include "hakoniwa/impl/hakoniwa_pdu_accessor.hpp"
#include "hakoniwa/impl/hakoniwa_simulator.hpp"
#include "hakoniwa/impl/hakoniwa_conductor.hpp"

#include <thread>

using namespace hako::drone::impl;

std::shared_ptr<HakoniwaDroneService> HakoniwaDroneService::instance_ = nullptr;

bool HakoniwaDroneService::registerService(std::string& asset_name, std::string& config_path, uint64_t delta_time_usec, uint64_t max_delay_usec, std::shared_ptr<IServiceContainer> service_container)
{
    if (HakoniwaConductor::isStarted()) {
        return false;
    }
    service_container->setPduSyncher(simulator_);
    std::cout << "conductor start" << std::endl;
    HakoniwaConductor::startService(delta_time_usec, max_delay_usec);
    std::cout << "conductor start done" << std::endl;

    //std::this_thread::sleep_for(std::chrono::microseconds(500));
    std::cout << "simulator register" << std::endl;
    if (simulator_->registerService(asset_name, config_path, delta_time_usec, service_container)) {
        std::cout << "simulator register done" << std::endl;
        return true;
    }
    else {
        std::cerr << "ERROR: " << "HakoniwaSimulator::registerService() error" << std::endl;
        return false;
    }
}

bool HakoniwaDroneService::startService()
{
    return simulator_->startService();
}

bool HakoniwaDroneService::stopService()
{
    bool ret = simulator_->stopService();
    if (ret == false) {
        std::cerr << "ERROR: " << "HakoniwaSimulator::stopService() error" << std::endl;
    }
    ret = HakoniwaConductor::stopService();
    if (ret == false) {
        std::cerr << "ERROR: " << "HakoniwaConductor::stopService() error" << std::endl;
    }
    return true;
}

bool HakoniwaDroneService::isStarted()
{
    return HakoniwaConductor::isStarted();
}

