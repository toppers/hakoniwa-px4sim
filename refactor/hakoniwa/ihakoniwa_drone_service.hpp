#ifndef _IHAKONIWA_DRONE_SERVICE_HPP_
#define _IHAKONIWA_DRONE_SERVICE_HPP_

#include <stdint.h>
#include <string>
#include "service/iservice_container.hpp"

using namespace hako::service;

namespace hako::drone {
class IHakoniwaDroneService {
public:
    virtual ~IHakoniwaDroneService() {}

    virtual bool registerService(std::string& asset_name, std::string& config_path, uint64_t delta_time_usec, uint64_t max_delay_usec, std::shared_ptr<IServiceContainer> service_container) = 0;
    virtual bool startService() = 0;
    virtual bool stopService() = 0;

    virtual bool isStarted() = 0;

    virtual bool setTransferPduFromHakoniwaToService(ServicePduDataIdType pdu_id, int channel_id) = 0;
    virtual bool setTransferPduFromServiceToHakoniwa(ServicePduDataIdType pdu_id, int channel_id) = 0;
};
} // namespace hako::drone

#endif /* _IHAKONIWA_DRONE_SERVICE_HPP_ */