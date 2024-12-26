#include "hakoniwa/hakoniwa_pdu_accessor.hpp"

#include "pdu/types/hako_msgs/pdu_cpptype_conv_HakoDroneCmdTakeoff.hpp"
#include "pdu/types/hako_msgs/pdu_cpptype_conv_HakoDroneCmdLand.hpp"
#include "pdu/types/hako_msgs/pdu_cpptype_conv_HakoDroneCmdMove.hpp"
#include "pdu/types/hako_msgs/pdu_cpptype_conv_GameControllerOperation.hpp"
#include "pdu/types/hako_msgs/pdu_cpptype_conv_HakoCmdMagnetHolder.hpp"
#include "pdu/types/hako_msgs/pdu_cpptype_conv_Collision.hpp"
#include "pdu/types/hako_msgs/pdu_cpptype_conv_Disturbance.hpp"
#include "pdu/types/hako_msgs/pdu_cpptype_conv_HakoBatteryStatus.hpp"
#include "pdu/types/hako_msgs/pdu_cpptype_conv_HakoStatusMagnetHolder.hpp"
#include "pdu/types/hako_mavlink_msgs/pdu_cpptype_conv_HakoHilActuatorControls.hpp"
#include "pdu/types/geometry_msgs/pdu_cpptype_conv_Twist.hpp"


using namespace hako::pdu;

#define HAKO_CPP2PDU(pdu_field, pdu_type, ret, _ptr)                                \
do {                                                                                \
    Hako_##pdu_type* raw_data;                                                      \
    ret = hako_convert_cpp2pdu_##pdu_type(pdu_data.pdu.pdu_field, &raw_data);       \
    if (ret >= 0) {                                                                 \
        _ptr = static_cast<void*>(raw_data);                                        \
        pdu_data_size = HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(Hako_##pdu_type);          \
    } else {                                                                        \
        throw std::runtime_error("Failed to convert cpp to pdu data: " #pdu_field); \
    }                                                                               \
} while (0)

#define HAKO_PDU2CPP(pdu_field, pdu_type, ret, _ptr)                                \
do {                                                                                \
    HakoCpp_##pdu_type cpp_data;                                                    \
    ret = hako_convert_pdu2cpp_##pdu_type(*static_cast<Hako_##pdu_type*>(_ptr), cpp_data); \
    if (ret >= 0) {                                                                 \
        pdu_data.pdu.pdu_field = cpp_data;                                          \
    } else {                                                                        \
        throw std::runtime_error("Failed to convert pdu to cpp data: " #pdu_field); \
    }                                                                               \
} while (0)

bool HakoniwaPduAssessor::write(std::string& robot_name, int channel_id, ServicePduDataType& pdu_data)
{
    int ret = -1;
    int pdu_data_size = -1;
    void *ptr = nullptr;
    switch (pdu_data.id) {
        case SERVICE_PDU_DATA_ID_TYPE_TAKEOFF:
            HAKO_CPP2PDU(takeoff, HakoDroneCmdTakeoff, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_LAND:
            HAKO_CPP2PDU(land, HakoDroneCmdLand, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_MOVE:
            HAKO_CPP2PDU(move, HakoDroneCmdMove, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_GAME_CTRL:
            HAKO_CPP2PDU(game_ctrl, GameControllerOperation, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_MAGNET:
            HAKO_CPP2PDU(magnet, HakoCmdMagnetHolder, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_COLLISION:
            HAKO_CPP2PDU(collision, Collision, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_DISTURBANCE:
            HAKO_CPP2PDU(disturbance, Disturbance, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_BATTERY_STATUS:
            HAKO_CPP2PDU(battery_status, HakoBatteryStatus, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_STATUS_MAGNET:
            HAKO_CPP2PDU(status_magnet, HakoStatusMagnetHolder, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_POSITION:
            HAKO_CPP2PDU(position, Twist, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_ACTUATOR_CONTROLS:
            HAKO_CPP2PDU(actuator_controls, HakoHilActuatorControls, pdu_data_size, ptr);
            break;
        default:
            return false;
    }
    if ((ret == 0) && (ptr != nullptr)) {
        //TODO write pdu
        hako_destroy_pdu(ptr);
    }
    return true;
}

bool HakoniwaPduAssessor::read(std::string& robot_name, int channel_id, ServicePduDataType& pdu_data)
{
    int ret = -1;
    int pdu_data_size = -1;
    void *ptr = nullptr;

    //TODO read pdu
    //PDUサイズはhako_asset_get_pdus()使って、探索できる
    //事前にマップを作るんだろうな。。

    switch (pdu_data.id) {
        case SERVICE_PDU_DATA_ID_TYPE_TAKEOFF:
            HAKO_PDU2CPP(takeoff, HakoDroneCmdTakeoff, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_LAND:
            HAKO_PDU2CPP(land, HakoDroneCmdLand, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_MOVE:
            HAKO_PDU2CPP(move, HakoDroneCmdMove, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_GAME_CTRL:
            HAKO_PDU2CPP(game_ctrl, GameControllerOperation, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_MAGNET:
            HAKO_PDU2CPP(magnet, HakoCmdMagnetHolder, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_COLLISION:
            HAKO_PDU2CPP(collision, Collision, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_DISTURBANCE:
            HAKO_PDU2CPP(disturbance, Disturbance, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_BATTERY_STATUS:
            HAKO_PDU2CPP(battery_status, HakoBatteryStatus, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_STATUS_MAGNET:
            HAKO_PDU2CPP(status_magnet, HakoStatusMagnetHolder, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_POSITION:
            HAKO_PDU2CPP(position, Twist, pdu_data_size, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_ACTUATOR_CONTROLS:
            HAKO_PDU2CPP(actuator_controls, HakoHilActuatorControls, pdu_data_size, ptr);
            break;
        default:
            return false;
    }
    if ((ret == 0) && (ptr != nullptr)) {
        //TODO read pdu
        hako_destroy_pdu(ptr);
    }
    return true;
}
