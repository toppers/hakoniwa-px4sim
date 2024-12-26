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
    HakoniwaConductor::startService(delta_time_usec, max_delay_usec);
    std::this_thread::sleep_for(std::chrono::microseconds(500));
    if (HakoniwaSimulator::registerService(asset_name, config_path, delta_time_usec, service_container)) {
        return true;
    }
    else {
        std::cerr << "ERROR: " << "HakoniwaSimulator::registerService() error" << std::endl;
        return false;
    }
}

bool HakoniwaDroneService::startService()
{
    return HakoniwaSimulator::startService();
}

bool HakoniwaDroneService::stopService()
{
    bool ret = HakoniwaSimulator::stopService();
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

bool HakoniwaDroneService::setTransferPduFromHakoniwaToService(ServicePduDataIdType pdu_id, int channel_id)
{
    return HakoniwaSimulator::setTransferPduFromHakoniwaToService(pdu_id, channel_id);
}

bool HakoniwaDroneService::setTransferPduFromServiceToHakoniwa(ServicePduDataIdType pdu_id, int channel_id)
{
    return HakoniwaSimulator::setTransferPduFromServiceToHakoniwa(pdu_id, channel_id);
}