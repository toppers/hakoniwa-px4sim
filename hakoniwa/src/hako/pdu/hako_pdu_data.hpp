#ifndef _HAKO_PDU_DATA_HPP_
#define _HAKO_PDU_DATA_HPP_

#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilSensor.hpp"
#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilGps.hpp"
#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilStateQuaternion.hpp"
#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilActuatorControls.hpp"
#include "geometry_msgs/pdu_ctype_Twist.h"
#include "hako_msgs/pdu_ctype_Collision.h"
#include "hako_msgs/pdu_ctype_ManualPosAttControl.h"

extern bool hako_read_hil_sensor(Hako_HakoHilSensor &hil_sensor);
extern bool hako_read_hil_gps(Hako_HakoHilGps &hil_gps);
extern bool hako_read_hil_state_quaternion(Hako_HakoHilStateQuaternion &hil_state_quaternion);
extern bool hako_read_hil_actuator_controls(Hako_HakoHilActuatorControls &hil_actuator_controls);

extern void hako_write_hil_sensor(const Hako_HakoHilSensor &hil_sensor);
extern void hako_write_hil_gps(const Hako_HakoHilGps &hil_gps);
extern void hako_write_hil_state_quaternion(const Hako_HakoHilStateQuaternion &hil_state_quaternion);
extern void hako_write_hil_actuator_controls(const Hako_HakoHilActuatorControls &hil_actuator_controls);


static inline bool hako_mavlink_read_hil_sensor(mavlink_hil_sensor_t &dst)
{
    Hako_HakoHilSensor src;
    if (hako_read_hil_sensor(src)) {
        hako_convert_pdu2mavlink_HakoHilSensor(src, dst);
        return true;
    }
    else {
        return false;
    }
}
static inline bool hako_mavlink_read_hil_gps(mavlink_hil_gps_t &dst)
{
    Hako_HakoHilGps src;
    if (hako_read_hil_gps(src)) {
        hako_convert_pdu2mavlink_HakoHilGps(src, dst);
        return true;
    }
    else {
        return false;
    }
}

static inline bool hako_mavlink_read_hil_state_quaternion(mavlink_hil_state_quaternion_t &dst)
{
    Hako_HakoHilStateQuaternion src;
    if (hako_read_hil_state_quaternion(src)) {
        hako_convert_pdu2mavlink_HakoHilStateQuaternion(src, dst);
        return true;
    }
    else {
        return false;
    }
}

static inline bool hako_mavlink_read_hil_actuator_controls(mavlink_hil_actuator_controls_t &dst)
{
    Hako_HakoHilActuatorControls src;
    if (hako_read_hil_actuator_controls(src)) {
        hako_convert_pdu2mavlink_HakoHilActuatorControls(src, dst);
        return true;
    }
    else {
        return false;
    }
}
static inline void hako_mavlink_write_hil_sensor(mavlink_hil_sensor_t &src)
{
    Hako_HakoHilSensor dst;
    hako_convert_mavlink2pdu_HakoHilSensor(src, dst);
    hako_write_hil_sensor(dst);
}
static inline void hako_mavlink_write_hil_gps(mavlink_hil_gps_t &src)
{
    Hako_HakoHilGps dst;
    hako_convert_mavlink2pdu_HakoHilGps(src, dst);
    hako_write_hil_gps(dst);
}

static inline void hako_mavlink_write_hil_state_quaternion(mavlink_hil_state_quaternion_t &src)
{
    Hako_HakoHilStateQuaternion dst;
    hako_convert_mavlink2pdu_HakoHilStateQuaternion(src, dst);
    hako_write_hil_state_quaternion(dst);
}

static inline void hako_mavlink_write_hil_actuator_controls(mavlink_hil_actuator_controls_t &src)
{
    Hako_HakoHilActuatorControls dst;
    hako_convert_mavlink2pdu_HakoHilActuatorControls(src, dst);
    hako_write_hil_actuator_controls(dst);
}

#endif /* _HAKO_PDU_DATA_HPP_ */