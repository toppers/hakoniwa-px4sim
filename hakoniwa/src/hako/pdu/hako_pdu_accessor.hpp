#ifndef _HAKO_PDU_ACCESSOR_HPP_
#define _HAKO_PDU_ACCESSOR_HPP_

#include "hako_pdu_data.hpp"
#define HAKO_AVATOR_CHANNLE_ID_MOTOR        0
#define HAKO_AVATOR_CHANNLE_ID_POS          1
#define HAKO_AVATOR_CHANNLE_ID_COLLISION    2
#define HAKO_AVATOR_CHANNLE_ID_MANUAL       3
#define HAKO_AVATOR_CHANNEL_ID_DISTURB      4
#define HAKO_AVATOR_CHANNEL_ID_CMD_TAKEOFF  5
#define HAKO_AVATOR_CHANNEL_ID_CMD_MOVE     6
#define HAKO_AVATOR_CHANNEL_ID_CMD_LAND     7
#define HAKO_AVATOR_CHANNEL_ID_GAME_CTRL    8
#define HAKO_AVATOR_CHANNEL_ID_BATTERY_STAT 9
#define HAKO_AVATOR_CHANNEL_ID_CMD_CAMERA   10
#define HAKO_AVATOR_CHANNEL_ID_CAMERA_DATA  11
#define HAKO_AVATOR_CHANNEL_ID_CAMERA_MOVE  12
#define HAKO_AVATOR_CHANNEL_ID_CAMERA_INFO  13
#define HAKO_AVATOR_CHANNEL_ID_CMD_MAG      14
#define HAKO_AVATOR_CHANNEL_ID_STAT_MAG     15
#define HAKO_AVATOR_CHANNEL_ID_LIDAR_DATA   16
#define HAKO_AVATOR_CHANNEL_ID_LIDAR_POS    17
#define HAKO_AVATOR_CHANNLE_ID_CTRL         18 /* for pid control program only */
/*

        {
          "type": "hako_msgs/GameControllerOperation",
          "org_name": "hako_cmd_game",
          "name": "DroneTransporter_hako_cmd_game",
          "class_name": "Hakoniwa.PluggableAsset.Communication.Pdu.Raw.RawPduWriter",
          "conv_class_name": "Hakoniwa.PluggableAsset.Communication.Pdu.Raw.RawPduWriterConverter",
          "channel_id": 14,
          "pdu_size": 52,
          "write_cycle": 1,
          "method_type": "SHM"
        }
*/
static inline void debug_print_drone_collision(hako::assets::drone::DroneDynamicsCollisionType& drone_collision)
{
    std::cout << "Collision: " << (drone_collision.collision ? "Yes" : "No") << std::endl;
    std::cout << "Contact Number: " << drone_collision.contact_num << std::endl;
    std::cout << "Relative Velocity: (" 
                    << drone_collision.relative_velocity.x << ", " 
                    << drone_collision.relative_velocity.y << "," 
                    << drone_collision.relative_velocity.z << ")"
                    << std::endl;

    for (int i = 0; i < drone_collision.contact_num; ++i) {
        std::cout << "Contact Positions[" << i << "]: (" 
                    << drone_collision.contact_position[i].x << ", " 
                    << drone_collision.contact_position[i].y << "," 
                    << drone_collision.contact_position[i].z << ")"
                    << std::endl;
    }
    std::cout << "Restitution Coefficient: " << drone_collision.restitution_coefficient << std::endl;
}
static inline void do_io_read_control(hako::assets::drone::IAirCraft *drone, DroneThrustType& thrust, DroneTorqueType& torque)
{
    Hako_Twist control;
    char buffer[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(Hako_Twist)];

    if (!hako_asset_runner_pdu_read(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_CTRL, buffer, sizeof(buffer))) {
        std::cerr << "ERROR: can not read pdu data: control" << std::endl;
        return;
    }
    if (hako_pdu_get_fixed_data(buffer, (char*)&control, sizeof(Hako_Twist), sizeof(buffer)) < 0) {
        std::cerr << "ERROR: can not get pdu data: control" << std::endl;
        return;
    }
    thrust.data = control.linear.z;
    torque.data.x = control.angular.x;
    torque.data.y = control.angular.y;
    torque.data.z = control.angular.z;
    return;
}
static inline void do_io_read_collision(hako::assets::drone::IAirCraft *drone, hako::assets::drone::DroneDynamicsCollisionType& drone_collision)
{
    Hako_Collision hako_collision;
    char buffer[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(Hako_Collision)];
    memset(&drone_collision, 0, sizeof(drone_collision));
    if (!hako_asset_runner_pdu_read(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_COLLISION, buffer, sizeof(buffer))) {
        std::cerr << "ERROR: can not read pdu data: Hako_Collision" << std::endl;
        return;
    }
    if (hako_pdu_get_fixed_data(buffer, (char*)&hako_collision, sizeof(Hako_Collision), sizeof(buffer)) < 0) {
        std::cerr << "ERROR: can not get pdu data: Hako_Collision" << std::endl;
        return;
    }
    drone_collision.collision = (bool)hako_collision.collision;
    if (drone_collision.collision) {
        drone_collision.contact_num = hako_collision.contact_num;
        drone_collision.relative_velocity.x = hako_collision.relative_velocity.x;
        drone_collision.relative_velocity.y = -hako_collision.relative_velocity.y;
        drone_collision.relative_velocity.z = -hako_collision.relative_velocity.z;
        drone_collision.restitution_coefficient = hako_collision.restitution_coefficient;
        for (int i = 0; i < drone_collision.contact_num; i++) {
            drone_collision.contact_position[i].x = hako_collision.contact_position[i].x;
            drone_collision.contact_position[i].y = -hako_collision.contact_position[i].y;
            drone_collision.contact_position[i].z = -hako_collision.contact_position[i].z;
        }
        debug_print_drone_collision(drone_collision);
        hako_collision.collision = false;

        // 書き込み用バッファを準備する
        char write_buffer[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(Hako_Collision)];
        if (hako_pdu_put_fixed_data(write_buffer, reinterpret_cast<const char*>(&hako_collision), sizeof(Hako_Collision), sizeof(write_buffer)) < 0) {
            std::cerr << "ERROR: can not put pdu data: Hako_Collision" << std::endl;
            return;
        }
        if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_COLLISION, write_buffer, sizeof(write_buffer))) {
            std::cerr << "ERROR: can not write pdu data: Hako_Collision" << std::endl;
        }
    }
}

static inline void do_io_read_manual(hako::assets::drone::IAirCraft *drone, hako::assets::drone::DroneDynamicsManualControlType& drone_manual)
{
    Hako_ManualPosAttControl hako_manual;
    char buffer[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(Hako_ManualPosAttControl)];
    memset(&hako_manual, 0, sizeof(hako_manual));
    if (!hako_asset_runner_pdu_read(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_MANUAL, buffer, sizeof(buffer))) {
        std::cerr << "ERROR: can not read pdu data: Hako_ManualPosAttControl" << std::endl;
        return;
    }
    if (hako_pdu_get_fixed_data(buffer, (char*)&hako_manual, sizeof(Hako_ManualPosAttControl), sizeof(buffer)) < 0) {
        std::cerr << "ERROR: can not get pdu data: Hako_ManualPosAttControl" << std::endl;
        return;
    }
    drone_manual.control = hako_manual.do_operation;
    if (drone_manual.control) {
        std::cout << "manual set angle( " << hako_manual.posatt.angular.x << ", " << hako_manual.posatt.angular.y << ", " << hako_manual.posatt.angular.z << " )" << std::endl;
        hako_manual.do_operation = false;
        drone_manual.angle.data.x = hako_manual.posatt.angular.x;
        drone_manual.angle.data.y = hako_manual.posatt.angular.y;
        drone_manual.angle.data.z = hako_manual.posatt.angular.z;
        drone_manual.pos.data.x = hako_manual.posatt.linear.x;
        drone_manual.pos.data.y = hako_manual.posatt.linear.y;
        drone_manual.pos.data.z = hako_manual.posatt.linear.z;

        // 書き込み用バッファを準備する
        char write_buffer[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(Hako_ManualPosAttControl)];
        if (hako_pdu_put_fixed_data(write_buffer, reinterpret_cast<const char*>(&hako_manual), sizeof(Hako_ManualPosAttControl), sizeof(write_buffer)) < 0) {
            std::cerr << "ERROR: can not put pdu data: Hako_ManualPosAttControl" << std::endl;
            return;
        }
        if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_MANUAL, write_buffer, sizeof(write_buffer))) {
            std::cerr << "ERROR: can not write pdu data: Hako_ManualPosAttControl" << std::endl;
        }
    }
}

static inline void do_io_read_disturb(hako::assets::drone::IAirCraft *drone, hako::assets::drone::DroneDynamicsDisturbanceType& drone_disturb)
{
    Hako_Disturbance hako_disturb;
    char buffer[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(Hako_Disturbance)];
    memset(&hako_disturb, 0, sizeof(hako_disturb));
    if (!hako_asset_runner_pdu_read(drone->get_name().c_str(), HAKO_AVATOR_CHANNEL_ID_DISTURB, buffer, sizeof(buffer))) {
        std::cerr << "ERROR: can not read pdu data: Hako_Disturbance" << std::endl;
        return;
    }
    if (hako_pdu_get_fixed_data(buffer, (char*)&hako_disturb, sizeof(Hako_Disturbance), sizeof(buffer)) < 0) {
        std::cerr << "ERROR: can not get pdu data: Hako_Disturbance" << std::endl;
        return;
    }
    //temperature
    drone_disturb.values.d_temp.value = hako_disturb.d_temp.value;
    //wind
    drone_disturb.values.d_wind.x = hako_disturb.d_wind.value.x;
    drone_disturb.values.d_wind.y = hako_disturb.d_wind.value.y;
    drone_disturb.values.d_wind.z = hako_disturb.d_wind.value.z;
}
#include <iomanip>  // for std::hex and std::setfill

template<typename PacketType>
static inline bool do_io_read_cmd(hako::assets::drone::IAirCraft *drone, int channel_id, PacketType& packet)
{
    char buffer[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(PacketType)];
    if (!hako_asset_runner_pdu_read(drone->get_name().c_str(), channel_id, buffer, sizeof(buffer))) {
        std::cerr << "ERROR: can not read pdu data: packet channel_id: " << channel_id << std::endl;
        return false;
    }
    if (hako_pdu_get_fixed_data(buffer, (char*)&packet, sizeof(PacketType), sizeof(buffer)) < 0) {
        //std::cerr << "ERROR: can not get pdu data: PacketType" << std::endl;
        return false;
    }
    return true;
}

template<typename PacketType>
static inline bool do_io_write_cmd(hako::assets::drone::IAirCraft *drone, int channel_id, const PacketType& packet) {
    char buffer[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(PacketType)];
    if (hako_pdu_put_fixed_data(buffer, reinterpret_cast<const char*>(&packet), sizeof(PacketType), sizeof(buffer)) < 0) {
        std::cerr << "ERROR: can not put pdu data: PacketType" << std::endl;
        return false;
    }
    if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), channel_id, buffer, sizeof(buffer))) {
        std::cerr << "ERROR: can not write pdu data: packet channel_id: " << channel_id << " pdu_size: " << sizeof(buffer) << std::endl;
        return false;
    }
    return true;
}
template<typename PacketType>
static inline bool do_io_write_data(hako::assets::drone::IAirCraft *drone, int channel_id, const PacketType& packet) {
    char buffer[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(PacketType)];
    if (hako_pdu_put_fixed_data(buffer, reinterpret_cast<const char*>(&packet), sizeof(PacketType), sizeof(buffer)) < 0) {
        std::cerr << "ERROR: can not put pdu data: PacketType" << std::endl;
        return false;
    }
    if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), channel_id, buffer, sizeof(buffer))) {
        std::cerr << "ERROR: can not write pdu data: packet channel_id: " << channel_id << " pdu_size: " << sizeof(buffer) << std::endl;
        return false;
    }
    return true;
}
static inline bool do_io_write_battery_status(hako::assets::drone::IAirCraft *drone)
{
    Hako_HakoBatteryStatus packet = {};
    auto battery = drone->get_battery_dynamics();
    if (battery != nullptr) {
        auto status = battery->get_status();
        packet.full_voltage = status.full_voltage;
        packet.curr_voltage = status.curr_voltage;
        packet.curr_temp = status.temperature;
        packet.cycles = status.cycles;
        packet.status = status.status;
        return do_io_write_data(drone, HAKO_AVATOR_CHANNEL_ID_BATTERY_STAT, packet);
    }
    return false;
}

static inline void do_io_write(hako::assets::drone::IAirCraft *drone, const double controls[hako::assets::drone::ROTOR_NUM])
{
    Hako_HakoHilActuatorControls hil_actuator_controls;
    Hako_Twist pos;
    char buffer_hil_actuator[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(Hako_HakoHilActuatorControls)];
    char buffer_pos[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(Hako_Twist)];

    memset(&hil_actuator_controls, 0, sizeof(hil_actuator_controls));
    for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
        hil_actuator_controls.controls[i] = controls[i];
    }
    if (hako_pdu_put_fixed_data(buffer_hil_actuator, reinterpret_cast<const char*>(&hil_actuator_controls), sizeof(Hako_HakoHilActuatorControls), sizeof(buffer_hil_actuator)) < 0) {
        std::cerr << "ERROR: can not put pdu data: hil_actuator_controls" << std::endl;
        return;
    }
    if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_MOTOR, buffer_hil_actuator, sizeof(buffer_hil_actuator))) {
        std::cerr << "ERROR: can not write pdu data: hil_actuator_controls" << std::endl;
        return;
    }

    DronePositionType dpos = drone->get_drone_dynamics().get_pos();
    DroneEulerType dangle = drone->get_drone_dynamics().get_angle();
    pos.linear.x = dpos.data.x;
    pos.linear.y = -dpos.data.y;
    pos.linear.z = -dpos.data.z;
    pos.angular.x = dangle.data.x;
    pos.angular.y = -dangle.data.y;
    pos.angular.z = -dangle.data.z;

    if (hako_pdu_put_fixed_data(buffer_pos, reinterpret_cast<const char*>(&pos), sizeof(Hako_Twist), sizeof(buffer_pos)) < 0) {
        std::cerr << "ERROR: can not put pdu data: pos" << std::endl;
        return;
    }
    if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_POS, buffer_pos, sizeof(buffer_pos))) {
        std::cerr << "ERROR: can not write pdu data: pos" << std::endl;
        return;
    }
}

static inline void do_io_write_replay_data(std::string &name, DronePositionType &dpos, DroneEulerType &dangle, double controls[hako::assets::drone::ROTOR_NUM])
{
    Hako_HakoHilActuatorControls hil_actuator_controls;
    Hako_Twist pos;
    char buffer_hil_actuator[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(Hako_HakoHilActuatorControls)];
    char buffer_pos[HAKO_PDU_FIXED_DATA_SIZE_BY_TYPE(Hako_Twist)];

    memset(&hil_actuator_controls, 0, sizeof(hil_actuator_controls));
    for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
        hil_actuator_controls.controls[i] = controls[i];
    }
    if (hako_pdu_put_fixed_data(buffer_hil_actuator, reinterpret_cast<const char*>(&hil_actuator_controls), sizeof(Hako_HakoHilActuatorControls), sizeof(buffer_hil_actuator)) < 0) {
        std::cerr << "ERROR: can not put pdu data: hil_actuator_controls" << std::endl;
        return;
    }
    if (!hako_asset_runner_pdu_write(name.c_str(), HAKO_AVATOR_CHANNLE_ID_MOTOR, buffer_hil_actuator, sizeof(buffer_hil_actuator))) {
        std::cerr << "ERROR: can not write pdu data: hil_actuator_controls" << std::endl;
        return;
    }

    pos.linear.x = dpos.data.x;
    pos.linear.y = -dpos.data.y;
    pos.linear.z = -dpos.data.z;
    pos.angular.x = dangle.data.x;
    pos.angular.y = -dangle.data.y;
    pos.angular.z = -dangle.data.z;

    if (hako_pdu_put_fixed_data(buffer_pos, reinterpret_cast<const char*>(&pos), sizeof(Hako_Twist), sizeof(buffer_pos)) < 0) {
        std::cerr << "ERROR: can not put pdu data: pos" << std::endl;
        return;
    }
    if (!hako_asset_runner_pdu_write(name.c_str(), HAKO_AVATOR_CHANNLE_ID_POS, buffer_pos, sizeof(buffer_pos))) {
        std::cerr << "ERROR: can not write pdu data: pos" << std::endl;
        return;
    }
}

#endif /* _HAKO_PDU_ACCESSOR_HPP_ */