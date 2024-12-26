#ifndef _ISERVICE_PDU_TYPES_HPP_
#define _ISERVICE_PDU_TYPES_HPP_

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
    HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_ACTUATOR_CONTROLS,
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