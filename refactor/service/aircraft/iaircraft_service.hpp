#ifndef _IAIRCRAFT_SERVICE_HPP_
#define _IAIRCRAFT_SERVICE_HPP_

#include <cstdint>
#include "mavlink/mavlink_service.hpp"
#include "hako_mavlink_msgs/pdu_cpptype_HakoHilActuatorControls.hpp"
#include "hako_msgs/pdu_cpptype_Disturbance.hpp"
#include "hako_msgs/pdu_cpptype_Collision.hpp"
#include "geometry_msgs/pdu_cpptype_Twist.hpp"
#include "service/iservice_pdu_types.hpp"

namespace hako::service {

class IAircraftService {
public:
    virtual ~IAircraftService() = default;
    virtual bool startService(bool lockStep, uint64_t deltaTimeUsec) = 0;
    virtual void advanceTimeStep(uint32_t index) = 0;
    virtual void stopService() = 0;
    virtual void resetService() = 0;
    virtual uint64_t getSimulationTimeUsec(uint32_t index) = 0;
    virtual uint64_t getSitlTimeUsec(uint32_t index) = 0;

    virtual bool write_pdu(uint32_t index, ServicePduDataType& pdu) = 0;
    virtual bool read_pdu(uint32_t index, ServicePduDataType& pdu) = 0;
};

} // namespace hako::service::aircraft

#endif /* _IAIRCRAFT_SERVICE_HPP_ */