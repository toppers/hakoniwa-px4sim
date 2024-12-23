#ifndef _IAIRCRAFT_SERVICE_HPP_
#define _IAIRCRAFT_SERVICE_HPP_

#include <cstdint>
#include "mavlink/mavlink_service.hpp"
#include "hako_mavlink_msgs/pdu_cpptype_HakoHilActuatorControls.hpp"
#include "hako_msgs/pdu_cpptype_Disturbance.hpp"
#include "hako_msgs/pdu_cpptype_Collision.hpp"
#include "geometry_msgs/pdu_cpptype_Twist.hpp"

namespace hako::service {

typedef enum {
    HAKONIWA_PDU_DATA_ID_TYPE_AIRCRAFT_DISTURBANCE,
    HAKONIWA_PDU_DATA_ID_TYPE_AIRCRAFT_COLLISION,
    HAKONIWA_PDU_DATA_ID_TYPE_AIRCRAFT_POSITION,
    HAKONIWA_PDU_DATA_ID_TYPE_AIRCRAFT_CONTROLS,
    HAKONIWA_PDU_DATA_ID_TYPE_NUM,
} HakoniwaPduDataIdType;

typedef struct {
    HakoniwaPduDataIdType id;
    union {
        HakoCpp_Disturbance disturbance;
        HakoCpp_Collision collision;
        HakoCpp_Twist aircraft_position;
        HakoCpp_HakoHilActuatorControls aircraft_controls;
    } data;
} HakoniwaPduDataType;

class IAircraftService {
public:
    virtual ~IAircraftService() = default;
    virtual bool startService(bool lockStep, uint64_t deltaTimeUsec) = 0;
    virtual void advanceTimeStep(int index) = 0;
    virtual void stopService() = 0;
    virtual void resetService() = 0;
    virtual uint64_t getSimulationTimeUsec(int index) = 0;

    virtual bool write_pdu(uint32_t index, HakoniwaPduDataType& pdu) = 0;
    virtual bool read_pdu(uint32_t index, HakoniwaPduDataType& pdu) = 0;
};

} // namespace hako::service::aircraft

#endif /* _IAIRCRAFT_SERVICE_HPP_ */