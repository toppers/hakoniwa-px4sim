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
#include "hako_mavlink_msgs/pdu_cpptype_HakoHilActuatorControls.hpp"
#include "geometry_msgs/pdu_cpptype_Twist.hpp"
#include "controller/iaircraft_controller.hpp"
#include <atomic>

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
        HakoCpp_HakoHilActuatorControls actuator_controls;
    } pdu;
} HakoniwaDronePduDataType;

bool drone_pdu_data_deep_copy(const HakoniwaDronePduDataType& source, HakoniwaDronePduDataType& dest) {
    dest.id = source.id;
    switch (source.id) {
        case HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_TAKEOFF:
            dest.pdu.takeoff = source.pdu.takeoff;
            break;
        case HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_LAND:
            dest.pdu.land = source.pdu.land;
            break;
        case HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_MOVE:
            dest.pdu.move = source.pdu.move;
            break;
        case HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_GAME_CTRL:
            dest.pdu.game_ctrl = source.pdu.game_ctrl;
            break;
        case HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_MAGNET:
            dest.pdu.magnet = source.pdu.magnet;
            break;
        case HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_COLLISION:
            dest.pdu.collision = source.pdu.collision;
            break;
        case HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_DISTURBANCE:
            dest.pdu.disturbance = source.pdu.disturbance;
            break;
        case HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_BATTERY_STATUS:
            dest.pdu.battery_status = source.pdu.battery_status;
            break;
        case HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_STATUS_MAGNET:
            dest.pdu.status_magnet = source.pdu.status_magnet;
            break;
        case HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_POSITION:
            dest.pdu.position = source.pdu.position;
            break;
        case HAKONIWA_DRONE_PDU_DATA_ID_TYPE_DRONE_ACTUATOR_CONTROLS:
            dest.pdu.actuator_controls = source.pdu.actuator_controls;
            break;
        default:
            return false;
    }
    return true;
}

class IDroneService {
public:
    virtual ~IDroneService() = default;
    virtual bool startService(uint64_t deltaTimeUsec) = 0;
    virtual void advanceTimeStep() = 0;
    virtual void stopService() = 0;
    virtual void resetService() = 0;
    virtual uint64_t getSimulationTimeUsec() = 0;

    virtual bool write_pdu(HakoniwaDronePduDataType& pdu) = 0;
    virtual bool read_pdu(HakoniwaDronePduDataType& pdu) = 0;
};

}

#endif /* _IDRONE_SERVICE_HPP_ */