#ifndef _IDRONE_SERVICE_CONTAINER_HPP_
#define _IDRONE_SERVICE_CONTAINER_HPP_

#include "service/iservice_container.hpp"

namespace hako::service {

class IDroneServiceContainer : public IServiceContainer {
public:
    virtual ~IDroneServiceContainer() = default;
    virtual bool startService(uint64_t deltaTimeUsec) = 0;
    virtual void advanceTimeStep(uint32_t index) = 0;
    virtual void advanceTimeStep() = 0;
    virtual void stopService() = 0;
    virtual void resetService() = 0;
    virtual bool isServiceAvailable() = 0;
    virtual uint64_t getSimulationTimeUsec(uint32_t index) = 0;

    virtual bool write_pdu(uint32_t index, ServicePduDataType& pdu) = 0;
    virtual bool read_pdu(uint32_t index, ServicePduDataType& pdu) = 0;
    virtual void peek_pdu(uint32_t index, ServicePduDataType& pdu) = 0;

    virtual uint32_t getNumServices() = 0;
    virtual std::string getRobotName(uint32_t index) = 0;
};
}

#endif /* _IDRONE_SERVICE_CONTAINER_HPP_ */