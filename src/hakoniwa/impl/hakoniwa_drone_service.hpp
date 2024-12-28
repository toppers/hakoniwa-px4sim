#ifndef _HAKONIW_DRONE_SERVICE_HPP_
#define _HAKONIW_DRONE_SERVICE_HPP_

#include "hakoniwa/ihakoniwa_drone_service.hpp"
#include "hakoniwa/impl/hakoniwa_simulator.hpp"

using namespace hako::service;

namespace hako::drone::impl {
class HakoniwaDroneService : public IHakoniwaDroneService {
public:
    static std::shared_ptr<IHakoniwaDroneService> getInstance() {
        if (instance_ == nullptr) {
            instance_ = std::make_shared<HakoniwaDroneService>();
        }
        return instance_;
    }
    HakoniwaDroneService() 
    {
        simulator_ = HakoniwaSimulator::getInstance();
    }
    virtual ~HakoniwaDroneService() {}

    bool registerService(std::string& asset_name, std::string& config_path, uint64_t delta_time_usec, uint64_t max_delay_usec, std::shared_ptr<IServiceContainer> service_container) override;
    bool startService() override;
    bool stopService() override;
    bool isStarted() override;

    void setPduIdMap(ServicePduDataIdType pdu_id, int channel_id) override
    {
        simulator_->setPduIdMap(pdu_id, channel_id);
    }
private:
    static std::shared_ptr<HakoniwaDroneService> instance_;
    std::shared_ptr<HakoniwaSimulator> simulator_;
};
} // namespace hako::drone::impl

#endif /* _HAKONIW_DRONE_SERVICE_HPP_ */
