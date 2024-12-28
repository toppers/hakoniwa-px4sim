#ifndef _IAIRCRAFT_SERVICE_CONTAINER_HPP_
#define _IAIRCRAFT_SERVICE_CONTAINER_HPP_

#include "service/iservice_container.hpp"

namespace hako::service {

class IAircraftServiceContainer : public IServiceContainer {
public:
    virtual ~IAircraftServiceContainer() = default;
    virtual bool startService(uint64_t deltaTimeUsec) = 0;
    virtual bool startService(bool lockStep, uint64_t deltaTimeUsec) = 0;
    virtual bool setRealTimeStepUsec(uint64_t deltaTimeUsec) = 0;
    virtual void advanceTimeStep(uint32_t index) = 0;
    virtual void stopService() = 0;
    virtual void resetService() = 0;
    virtual uint64_t getSimulationTimeUsec(uint32_t index) = 0;
    virtual uint64_t getSitlTimeUsec(uint32_t index) = 0;

    virtual bool write_pdu(uint32_t index, ServicePduDataType& pdu) = 0;
    virtual bool read_pdu(uint32_t index, ServicePduDataType& pdu) = 0;
    virtual void peek_pdu(uint32_t index, ServicePduDataType& pdu) = 0;

    virtual uint32_t getNumServices() = 0;
    virtual std::string getRobotName(uint32_t index) = 0;

    virtual void setPduSyncher(std::shared_ptr<IServicePduSyncher> pdu_syncher) = 0;
};

} // namespace hako::service::aircraft

#endif /* _IAIRCRAFT_SERVICE_CONTAINER_HPP_ */