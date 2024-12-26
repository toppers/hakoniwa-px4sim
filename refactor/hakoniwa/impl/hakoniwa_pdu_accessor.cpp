#include "hakoniwa/impl/hakoniwa_pdu_accessor.hpp"

#include "hako_msgs/pdu_cpptype_conv_HakoDroneCmdTakeoff.hpp"
#include "hako_msgs/pdu_cpptype_conv_HakoDroneCmdLand.hpp"
#include "hako_msgs/pdu_cpptype_conv_HakoDroneCmdMove.hpp"
#include "hako_msgs/pdu_cpptype_conv_GameControllerOperation.hpp"
#include "hako_msgs/pdu_cpptype_conv_HakoCmdMagnetHolder.hpp"
#include "hako_msgs/pdu_cpptype_conv_Collision.hpp"
#include "hako_msgs/pdu_cpptype_conv_Disturbance.hpp"
#include "hako_msgs/pdu_cpptype_conv_HakoBatteryStatus.hpp"
#include "hako_msgs/pdu_cpptype_conv_HakoStatusMagnetHolder.hpp"
#include "hako_mavlink_msgs/pdu_cpptype_conv_HakoHilActuatorControls.hpp"
#include "geometry_msgs/pdu_cpptype_conv_Twist.hpp"

#include "include/hako_asset.h"
#include "include/hako_asset_pdu.hpp"

using namespace hako::drone::impl;

std::map<CompositeKey, int> HakoniwaPduAccessor::pdu_map_;
std::vector<hako::asset::Robot> HakoniwaPduAccessor::robots_;

bool HakoniwaPduAccessor::init()
{
    bool success = hako_asset_get_pdus(robots_);
    if (success) {
        create_map();
    }
    return success;
}

void HakoniwaPduAccessor::create_map()
{
    for (auto robot : robots_) {
        for (auto reader : robot.pdu_readers) {
            pdu_map_[{robot.name, reader.channel_id}] = reader.pdu_size;
        }
        for (auto writer : robot.pdu_writers) {
            pdu_map_[{robot.name, writer.channel_id}] = writer.pdu_size;
        }
    }
    return;
}

#define HAKO_CPP2PDU(_pdu_data, pdu_field, pdu_type, _ret, _ptr)                                \
do {                                                                                \
    Hako_##pdu_type* raw_data;                                                      \
    _ret = hako_convert_cpp2pdu_##pdu_type(_pdu_data.pdu.pdu_field, &raw_data);       \
    if (ret >= 0) {                                                                 \
        _ptr = static_cast<void*>(raw_data);                                        \
    } else {                                                                        \
        throw std::runtime_error("Failed to convert cpp to pdu data: " #pdu_field); \
    }                                                                               \
} while (0)

bool HakoniwaPduAccessor::write(std::string& robot_name, int channel_id, ServicePduDataType& pdu_data)
{
    CompositeKey key{robot_name, channel_id};
    if (pdu_map_.find(key) == pdu_map_.end()) {
        throw std::runtime_error("Failed to access PDU: robot_name = " + robot_name + 
                                 ", channel_id = " + std::to_string(channel_id));
    }    
    int ret = -1;
    int pdu_data_size = pdu_map_[{robot_name, channel_id}];
    void *ptr = nullptr;
    switch (pdu_data.id) {
        case SERVICE_PDU_DATA_ID_TYPE_TAKEOFF:
            HAKO_CPP2PDU(pdu_data, takeoff, HakoDroneCmdTakeoff, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_LAND:
            HAKO_CPP2PDU(pdu_data, land, HakoDroneCmdLand, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_MOVE:
            HAKO_CPP2PDU(pdu_data, move, HakoDroneCmdMove, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_GAME_CTRL:
            HAKO_CPP2PDU(pdu_data, game_ctrl, GameControllerOperation, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_MAGNET:
            HAKO_CPP2PDU(pdu_data, magnet, HakoCmdMagnetHolder, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_COLLISION:
            HAKO_CPP2PDU(pdu_data, collision, Collision, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_DISTURBANCE:
            HAKO_CPP2PDU(pdu_data, disturbance, Disturbance, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_BATTERY_STATUS:
            HAKO_CPP2PDU(pdu_data, battery_status, HakoBatteryStatus, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_STATUS_MAGNET:
            HAKO_CPP2PDU(pdu_data, status_magnet, HakoStatusMagnetHolder, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_POSITION:
            HAKO_CPP2PDU(pdu_data, position, Twist, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_ACTUATOR_CONTROLS:
            HAKO_CPP2PDU(pdu_data, actuator_controls, HakoHilActuatorControls, ret, ptr);
            break;
        default:
            throw std::runtime_error("Failed to write PDU: robot_name = " + robot_name + 
                                    ", channel_id = " + std::to_string(channel_id));
    }
    if (hako_asset_pdu_write(robot_name.c_str(), channel_id, (const char*)ptr, static_cast<size_t>(pdu_data_size)) != 0) {
        hako_destroy_pdu(ptr);
        throw std::runtime_error("Failed to write PDU: robot_name = " + robot_name + 
                                    ", channel_id = " + std::to_string(channel_id));
    }
    hako_destroy_pdu(ptr);
    return true;
}

#define HAKO_PDU2CPP(_pdu_data, pdu_field, pdu_type, _ret, _ptr)                                \
do {                                                                                \
    void* base_ptr = hako_get_base_ptr_pdu(_ptr);                                                    \
    _ret = hako_convert_pdu2cpp_##pdu_type(*static_cast<Hako_##pdu_type*>(base_ptr), _pdu_data.pdu.pdu_field); \
    if (_ret < 0) {                                                                 \
        throw std::runtime_error("Failed to convert pdu to cpp data: " #pdu_field); \
    }                                                                               \
} while (0)

bool HakoniwaPduAccessor::read(std::string& robot_name, int channel_id, ServicePduDataType& pdu_data)
{
    CompositeKey key{robot_name, channel_id};
    if (pdu_map_.find(key) == pdu_map_.end()) {
        throw std::runtime_error("Failed to access PDU: robot_name = " + robot_name + 
                                 ", channel_id = " + std::to_string(channel_id));
    }    
    int ret = -1;
    int pdu_data_size = pdu_map_[{robot_name, channel_id}];
    void *ptr = malloc(pdu_data_size);
    if (ptr == nullptr) {
        throw std::runtime_error("Failed to allocate memory for PDU: robot_name = " + robot_name + 
                                 ", channel_id = " + std::to_string(channel_id));
    }
    if (hako_asset_pdu_read(robot_name.c_str(), channel_id, (char*)ptr, static_cast<size_t>(pdu_data_size)) != 0) {
        free(ptr);
        throw std::runtime_error("Failed to read PDU: robot_name = " + robot_name + 
                                    ", channel_id = " + std::to_string(channel_id));
    }
    switch (pdu_data.id) {
        case SERVICE_PDU_DATA_ID_TYPE_TAKEOFF:
            HAKO_PDU2CPP(pdu_data, takeoff, HakoDroneCmdTakeoff, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_LAND:
            HAKO_PDU2CPP(pdu_data, land, HakoDroneCmdLand, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_MOVE:
            HAKO_PDU2CPP(pdu_data, move, HakoDroneCmdMove, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_GAME_CTRL:
            HAKO_PDU2CPP(pdu_data, game_ctrl, GameControllerOperation, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_MAGNET:
            HAKO_PDU2CPP(pdu_data, magnet, HakoCmdMagnetHolder, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_COLLISION:
            HAKO_PDU2CPP(pdu_data, collision, Collision, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_DISTURBANCE:
            HAKO_PDU2CPP(pdu_data, disturbance, Disturbance, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_BATTERY_STATUS:
            HAKO_PDU2CPP(pdu_data, battery_status, HakoBatteryStatus, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_STATUS_MAGNET:
            HAKO_PDU2CPP(pdu_data, status_magnet, HakoStatusMagnetHolder, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_POSITION:
            HAKO_PDU2CPP(pdu_data, position, Twist, ret, ptr);
            break;
        case SERVICE_PDU_DATA_ID_TYPE_ACTUATOR_CONTROLS:
            HAKO_PDU2CPP(pdu_data, actuator_controls, HakoHilActuatorControls, ret, ptr);
            break;
        default:
            free(ptr);
            throw std::runtime_error("Failed to read PDU: robot_name = " + robot_name + 
                                    ", channel_id = " + std::to_string(channel_id));
    }
    free(ptr);
    return true;
}
