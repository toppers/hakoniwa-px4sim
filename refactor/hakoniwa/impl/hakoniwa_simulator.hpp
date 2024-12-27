#ifndef _HAKONIWA_SIMULATOR_HPP_
#define _HAKONIWA_SIMULATOR_HPP_

#include <stdint.h>
#include <memory>
#include <mutex>
#include <string>
#include <map>
#include "service/iservice_container.hpp"
#include "service/iservice_pdu_syncher.hpp"
#include "hakoniwa/impl/hakoniwa_pdu_accessor.hpp"

using namespace hako::service;

namespace hako::drone::impl {
class HakoniwaSimulator : public IServicePduSyncher{
private:
    static std::shared_ptr<HakoniwaSimulator> instance_;
public:
    static std::shared_ptr<HakoniwaSimulator> getInstance() {
        if (instance_ == nullptr) {
            instance_ = std::make_shared<HakoniwaSimulator>();
        }
        return instance_;
    }
    HakoniwaSimulator() {}
    virtual ~HakoniwaSimulator() {}

    bool registerService(std::string& asset_name, std::string& config_path, uint64_t delta_time_usec, std::shared_ptr<IServiceContainer> service_container);
    bool startService();
    bool stopService();
    bool isStarted() { return isStarted_; }


    void setPduIdMap(ServicePduDataIdType pdu_id, int channel_id) 
    {
        channel_id_map_[pdu_id] = channel_id;
    }


    void advanceTimeStep() {
        if (isStarted_) {
            service_container_->advanceTimeStep();
        }
    }
    bool flush(uint32_t index, ServicePduDataType& pdu) override {
        if (isStarted_) {
            std::string robot_name = service_container_->getRobotName(index);
            int channel_id = channel_id_map_[pdu.id];
            return HakoniwaPduAccessor::write(robot_name, channel_id, pdu);
        }
        else {
            return false;
        }
    }
    bool load(uint32_t index, ServicePduDataType& pdu) override {
        if (isStarted_) {
            std::string robot_name = service_container_->getRobotName(index);
            int channel_id = channel_id_map_[pdu.id];
            return HakoniwaPduAccessor::read(robot_name, channel_id, pdu);
        }
        else {
            return false;
        }
    }

    std::shared_ptr<IServiceContainer> service_container_;
    
private:
    std::mutex mtx_;
    bool isStarted_;
    std::map<ServicePduDataIdType, int> channel_id_map_;
};
} // namespace hako::drone

#endif /* _HAKONIWA_SIMULATOR_HPP_ */