#ifndef _HAKONIWA_SIMULATOR_HPP_
#define _HAKONIWA_SIMULATOR_HPP_

#include <stdint.h>
#include <memory>
#include <mutex>
#include <string>
#include <map>
#include "service/iservice_container.hpp"

using namespace hako::service;

namespace hako::drone::impl {
class HakoniwaSimulator {
public:
    virtual ~HakoniwaSimulator() {}

    static bool registerService(std::string& asset_name, std::string& config_path, uint64_t delta_time_usec, std::shared_ptr<IServiceContainer> service_container);
    static bool startService();
    static bool stopService();
    static bool isStarted() { return isStarted_; }

    static bool setTransferPduFromHakoniwaToService(ServicePduDataIdType pdu_id, int channel_id);
    static bool setTransferPduFromServiceToHakoniwa(ServicePduDataIdType pdu_id, int channel_id);

    static void transferPduFromHakoniwaToservice();
    static void transferPduFromServiceToHakoniwa();
    static std::shared_ptr<IServiceContainer> service_container_;
    
private:
    HakoniwaSimulator() {}
    static std::mutex mtx_;
    static bool isStarted_;
    static std::map<ServicePduDataIdType, int> h2s_pdu_id_map_;
    static std::map<ServicePduDataIdType, int> s2h_pdu_id_map_;
};
} // namespace hako::drone

#endif /* _HAKONIWA_SIMULATOR_HPP_ */