#ifndef _ISERVICE_PDU_TYPES_HPP_
#define _ISERVICE_PDU_TYPES_HPP_

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
#include "hako_mavlink_msgs/pdu_cpptype_HakoHilActuatorControls.hpp"
#include "geometry_msgs/pdu_cpptype_Twist.hpp"

namespace hako::service {

typedef enum {
    SERVICE_PDU_DATA_ID_TYPE_TAKEOFF,
    SERVICE_PDU_DATA_ID_TYPE_MOVE,
    SERVICE_PDU_DATA_ID_TYPE_LAND,
    SERVICE_PDU_DATA_ID_TYPE_GAME_CTRL,
    SERVICE_PDU_DATA_ID_TYPE_MAGNET,
    SERVICE_PDU_DATA_ID_TYPE_COLLISION,
    SERVICE_PDU_DATA_ID_TYPE_DISTURBANCE,
    SERVICE_PDU_DATA_ID_TYPE_BATTERY_STATUS,
    SERVICE_PDU_DATA_ID_TYPE_STATUS_MAGNET,
    SERVICE_PDU_DATA_ID_TYPE_POSITION,
    SERVICE_PDU_DATA_ID_TYPE_ACTUATOR_CONTROLS,
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM,
} ServicePduDataIdType;

typedef struct {
    ServicePduDataIdType id;
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
        HakoCpp_HakoHilActuatorControls actuator_controls;
    } pdu;
} ServicePduDataType;

} // namespace hako::service

#endif /* _ISERVICE_PDU_TYPES_HPP_ */