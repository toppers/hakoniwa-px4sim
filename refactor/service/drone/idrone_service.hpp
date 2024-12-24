#ifndef _IDRONE_SERVICE_HPP_
#define _IDRONE_SERVICE_HPP_

#include <cstdint>
#include "hako_msgs/pdu_cpptype_HakoDroneCmdTakeoff.hpp"
#include "hako_msgs/pdu_cpptype_HakoDroneCmdLand.hpp"
#include "hako_msgs/pdu_cpptype_HakoDroneCmdMove.hpp"
#include "hako_msgs/pdu_cpptype_GameControllerOperation.hpp"
#include "hako_msgs/pdu_cpptype_HakoCmdMagnetHolder.hpp"
#include "hako_msgs/pdu_cpptype_Collision.hpp"
#include "hako_msgs/pdu_cpptype_Disturbance.hpp"
#include "hako_msgs/pdu_cpptype_HakoBatteryStatus.hpp"
#include "hako_msgs/pdu_cpptype_HakoStatusMagnetHolder.hpp"
#include "geometry_msgs/pdu_cpptype_Twist.hpp"

namespace hako::service {

typedef enum {
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_TAKEOFF,
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_LAND,
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_MOVE,
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_GAME_CTRL,
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_MAGNET,
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_COLLISION,
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_DISTURBANCE,
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_BATTERY_STATUS,
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_STATUS_MAGNET,
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_POSITION,
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM,
} HakoniwaDronePduDataIdType;

typedef struct {
    HakoniwaDronePduDataIdType id;
    union {
        HakoCpp_HakoDroneCmdTakeoff takeoff;
        HakoCpp_HakoDroneCmdLand land;
        HakoCpp_HakoDroneCmdMove move;
        HakoCpp_GameControllerOperation game_ctrl;
        HakoCpp_HakoCmdMagnetHolder magnet;
        HakoCpp_Collision collision;
        HakoCpp_Disturbance disturbance;
        HakoCpp_HakoBatteryStatus battery_status;
        HakoCpp_HakoStatusMagnetHolder status_magnet;
        HakoCpp_Twist position;
    } pdu;
} HakoniwaDronePduDataType;

class IDroneService {
public:
    virtual ~IDroneService() = default;
    virtual bool startService(uint64_t deltaTimeUsec) = 0;
    virtual void advanceTimeStep(uint32_t index) = 0;
    virtual void stopService() = 0;
    virtual void resetService() = 0;
    virtual uint64_t getSimulationTimeUsec(uint32_t index) = 0;

    virtual bool write_pdu(uint32_t index, HakoniwaDronePduDataType& pdu) = 0;
    virtual bool read_pdu(uint32_t index, HakoniwaDronePduDataType& pdu) = 0;
};
}

#endif /* _IDRONE_SERVICE_HPP_ */