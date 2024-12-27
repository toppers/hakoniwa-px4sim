#include "hakoniwa/impl/hakoniwa_simulator.hpp"
#include "hakoniwa/impl/hakoniwa_pdu_accessor.hpp"
#include "include/hako_asset.h"
#include <thread>

using namespace hako::drone::impl;
std::shared_ptr<IServiceContainer> HakoniwaSimulator::service_container_ = nullptr;
std::mutex HakoniwaSimulator::mtx_;
bool HakoniwaSimulator::isStarted_ = false;
std::map<ServicePduDataIdType, int> HakoniwaSimulator::h2s_pdu_id_map_;
std::map<ServicePduDataIdType, int> HakoniwaSimulator::s2h_pdu_id_map_;

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
    if (HakoniwaSimulator::service_container_ != nullptr) {
        HakoniwaSimulator::transferPduFromHakoniwaToservice();
        HakoniwaSimulator::service_container_->advanceTimeStep();
        HakoniwaSimulator::transferPduFromServiceToHakoniwa();
    }
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
    HakoniwaSimulator::service_container_ = service_container;
    std::cout << "asset_name = " << asset_name << std::endl;
    std::cout << "config_path = " << config_path << std::endl;
    int ret = hako_asset_register(asset_name.c_str(), config_path.c_str(), &my_callback, delta_time_usec, HAKO_ASSET_MODEL_CONTROLLER);
    if (ret == 0) {
        std::cout << "before init" << std::endl;
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

void HakoniwaSimulator::transferPduFromHakoniwaToservice()
{
    ServicePduDataType pdu_data;
    for (uint32_t i = 0; i < service_container_->getNumServices(); i++) {
        std::string robot_name = service_container_->getRobotName(i);
        //h2s_pdu_id_map_ から pdu_id と channel_id を取得
        for (auto it = h2s_pdu_id_map_.begin(); it != h2s_pdu_id_map_.end(); it++) {
            ServicePduDataIdType pdu_id = it->first;
            pdu_data.id = pdu_id;
            int channel_id = it->second;
            if (HakoniwaPduAccessor::read(robot_name, channel_id, pdu_data)) {
                service_container_->write_pdu(i, pdu_data);
                //std::cout << "success read pdu: robot_name = " << robot_name << ", channel_id = " << channel_id << std::endl;
            }
            else {
                //nothing to do
            }
        }
    }
}
void HakoniwaSimulator::transferPduFromServiceToHakoniwa()
{
    ServicePduDataType pdu_data;
    for (uint32_t i = 0; i < service_container_->getNumServices(); i++) {
        std::string robot_name = service_container_->getRobotName(i);
        //s2h_pdu_id_map_ から pdu_id と channel_id を取得
        for (auto it = s2h_pdu_id_map_.begin(); it != s2h_pdu_id_map_.end(); it++) {
            ServicePduDataIdType pdu_id = it->first;
            pdu_data.id = pdu_id;
            int channel_id = it->second;
            if (service_container_->read_pdu(i, pdu_data)) {
                bool ret = HakoniwaPduAccessor::write(robot_name, channel_id, pdu_data);
                if (!ret) {
                    throw std::runtime_error("Failed to write pdu: robot_name = " + robot_name + ", channel_id = " + std::to_string(channel_id));
                }
                else {
                    //std::cout << "success write pdu: robot_name = " << robot_name << ", channel_id = " << channel_id << std::endl;
                }
            }
            else {
                //nothing to do
            }
        }
    }
}

bool HakoniwaSimulator::setTransferPduFromHakoniwaToService(ServicePduDataIdType pdu_id, int channel_id)
{
    h2s_pdu_id_map_[pdu_id] = channel_id;
    return true;
}
bool HakoniwaSimulator::setTransferPduFromServiceToHakoniwa(ServicePduDataIdType pdu_id, int channel_id)
{
    s2h_pdu_id_map_[pdu_id] = channel_id;
    return true;
}
