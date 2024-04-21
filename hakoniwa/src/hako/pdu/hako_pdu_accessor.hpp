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
#define HAKO_AVATOR_CHANNLE_ID_CTRL         8 /* for pid control program only */

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

    if (!hako_asset_runner_pdu_read(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_CTRL, (char*)&control, sizeof(control))) {
        std::cerr << "ERROR: can not read pdu data: control" << std::endl;
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
    memset(&drone_collision, 0, sizeof(drone_collision));
    if (!hako_asset_runner_pdu_read(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_COLLISION, (char*)&hako_collision, sizeof(hako_collision))) {
        std::cerr << "ERROR: can not read pdu data: Hako_Collision" << std::endl;
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
        if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_COLLISION, (const char*)&hako_collision, sizeof(hako_collision))) {
            std::cerr << "ERROR: can not write pdu data: Hako_Collision" << std::endl;
        }
    }
}
static inline void do_io_read_manual(hako::assets::drone::IAirCraft *drone, hako::assets::drone::DroneDynamicsManualControlType& drone_manual)
{
    Hako_ManualPosAttControl hako_manual;
    memset(&hako_manual, 0, sizeof(hako_manual));
    if (!hako_asset_runner_pdu_read(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_MANUAL, (char*)&hako_manual, sizeof(hako_manual))) {
        std::cerr << "ERROR: can not read pdu data: Hako_ManualPosAttControl" << std::endl;
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
        if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_MANUAL, (const char*)&hako_manual, sizeof(hako_manual))) {
            std::cerr << "ERROR: can not write pdu data: Hako_ManualPosAttControl" << std::endl;
        }
    }
}

static inline void do_io_read_disturb(hako::assets::drone::IAirCraft *drone, hako::assets::drone::DroneDynamicsDisturbanceType& drone_disturb)
{
    Hako_Disturbance hako_disturb;
    memset(&hako_disturb, 0, sizeof(hako_disturb));
    if (!hako_asset_runner_pdu_read(drone->get_name().c_str(), HAKO_AVATOR_CHANNEL_ID_DISTURB, (char*)&hako_disturb, sizeof(hako_disturb))) {
        std::cerr << "ERROR: can not read pdu data: Hako_Disturbance" << std::endl;
    }
    //temperature
    drone_disturb.values.d_temp.value = hako_disturb.d_temp.value;
}

template<typename PacketType>
static inline bool do_io_read_cmd(hako::assets::drone::IAirCraft *drone, int channel_id, PacketType& packet) {
    if (!hako_asset_runner_pdu_read(drone->get_name().c_str(), channel_id, reinterpret_cast<char*>(&packet), sizeof(packet))) {
        std::cerr << "ERROR: can not read pdu data: packet channel_id: " << channel_id  << std::endl;
        return false;
    }
    return true;
}

template<typename PacketType>
static inline bool do_io_write_cmd(hako::assets::drone::IAirCraft *drone, int channel_id, const PacketType& packet) {
    if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), channel_id, reinterpret_cast<const char*>(&packet), sizeof(packet))) {
        std::cerr << "ERROR: can not write pdu data: packet channel_id: " << channel_id  << std::endl;
        return false;
    }
    return true;
}

static inline void do_io_write(hako::assets::drone::IAirCraft *drone, const double controls[hako::assets::drone::ROTOR_NUM])
{
    Hako_HakoHilActuatorControls hil_actuator_controls;
    Hako_Twist pos;

    memset(&hil_actuator_controls, 0, sizeof(hil_actuator_controls));
    for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
        hil_actuator_controls.controls[i] = controls[i];
    }
    if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_MOTOR, (const char*)&hil_actuator_controls, sizeof(hil_actuator_controls))) {
        std::cerr << "ERROR: can not write pdu data: hil_actuator_controls" << std::endl;
    }

    DronePositionType dpos = drone->get_drone_dynamics().get_pos();
    DroneEulerType dangle = drone->get_drone_dynamics().get_angle();
    pos.linear.x = dpos.data.x;
    pos.linear.y = -dpos.data.y;
    pos.linear.z = -dpos.data.z;
    pos.angular.x = dangle.data.x;
    pos.angular.y = -dangle.data.y;
    pos.angular.z = -dangle.data.z;
    if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_POS, (const char*)&pos, sizeof(pos))) {
        std::cerr << "ERROR: can not write pdu data: pos" << std::endl;
    }
}

#endif /* _HAKO_PDU_ACCESSOR_HPP_ */