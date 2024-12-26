#ifndef _IDRONE_SERVICE_HPP_
#define _IDRONE_SERVICE_HPP_

#include "controller/iaircraft_controller.hpp"
#include "service/iservice_pdu_types.hpp"
#include <cstdint>
#include <atomic>

namespace hako::service {


extern bool drone_pdu_data_deep_copy(const ServicePduDataType& source, ServicePduDataType& dest);
class IDroneService {
public:
    virtual ~IDroneService() = default;
    virtual bool startService(uint64_t deltaTimeUsec) = 0;
    virtual void advanceTimeStep() = 0;
    virtual void stopService() = 0;
    virtual bool isServiceAvailable() = 0;
    virtual void resetService() = 0;
    virtual uint64_t getSimulationTimeUsec() = 0;

    virtual bool write_pdu(ServicePduDataType& pdu) = 0;
    virtual bool read_pdu(ServicePduDataType& pdu) = 0;
    virtual void peek_pdu(ServicePduDataType& pdu) = 0;
};

}

#endif /* _IDRONE_SERVICE_HPP_ */