#include "mavlink_dump.hpp"
#include <iostream>

using namespace hako::mavlink;

static void dump_heartbeat(const MavlinkDecodedMessage& message) {
    std::cout << "  Type: HEARTBEAT" << std::endl;
    std::cout << "  Type: " << message.data.heartbeat.type << std::endl;
    std::cout << "  Autopilot: " << message.data.heartbeat.autopilot << std::endl;
    std::cout << "  Base mode: " << static_cast<int>(message.data.heartbeat.base_mode) << std::endl;
    std::cout << "  Custom mode: " << message.data.heartbeat.custom_mode << std::endl;
    std::cout << "  System status: " << static_cast<int>(message.data.heartbeat.system_status) << std::endl;
    std::cout << "  MAVLink version: " << static_cast<int>(message.data.heartbeat.mavlink_version) << std::endl;
}
static void dump_long(const MavlinkDecodedMessage& message) {
    std::cout << "  Type: COMMAND_LONG" << std::endl;
    std::cout << "  Target system: " << static_cast<int>(message.data.command_long.target_system) << std::endl;
    std::cout << "  Target component: " << static_cast<int>(message.data.command_long.target_component) << std::endl;
    std::cout << "  Command ID: " << message.data.command_long.command << std::endl;
    std::cout << "  Confirmation: " << static_cast<int>(message.data.command_long.confirmation) << std::endl;
    std::cout << "  Param1: " << static_cast<int>(message.data.command_long.param1) << std::endl;
    std::cout << "  Param2: " << static_cast<int>(message.data.command_long.param2) << std::endl;
    std::cout << "  Param3: " << static_cast<int>(message.data.command_long.param3) << std::endl;
    std::cout << "  Param4: " << static_cast<int>(message.data.command_long.param4) << std::endl;
    std::cout << "  Param5: " << static_cast<int>(message.data.command_long.param5) << std::endl;
    std::cout << "  Param6: " << static_cast<int>(message.data.command_long.param6) << std::endl;
    std::cout << "  Param7: " << static_cast<int>(message.data.command_long.param7) << std::endl;
}
static void dump_command_ack(const MavlinkDecodedMessage& message) {
    std::cout << "  Type: COMMAND_ACK" << std::endl;
    std::cout << "  Command: " << message.data.command_ack.command << std::endl;
    std::cout << "  Result: " << static_cast<int>(message.data.command_ack.result) << std::endl;
}
static void dump_hil_sensor(const MavlinkDecodedMessage& message) {
    std::cout << "  Type: HIL_SENSOR" << std::endl;
    std::cout << "  Time stamp: " << message.data.hil_sensor.time_usec << std::endl;
    std::cout << "  Xacc: " << message.data.hil_sensor.xacc << std::endl;
    std::cout << "  Yacc: " << message.data.hil_sensor.yacc << std::endl;
    std::cout << "  Zacc: " << message.data.hil_sensor.zacc << std::endl;
    std::cout << "  Xgyro: " << message.data.hil_sensor.xgyro << std::endl;
    std::cout << "  Ygyro: " << message.data.hil_sensor.ygyro << std::endl;
    std::cout << "  Zgyro: " << message.data.hil_sensor.zgyro << std::endl;
    std::cout << "  Xmag: " << message.data.hil_sensor.xmag << std::endl;
    std::cout << "  Ymag: " << message.data.hil_sensor.ymag << std::endl;
    std::cout << "  Zmag: " << message.data.hil_sensor.zmag << std::endl;
    std::cout << "  Abs_pressure: " << message.data.hil_sensor.abs_pressure << std::endl;
    std::cout << "  Diff_pressure: " << message.data.hil_sensor.diff_pressure << std::endl;
    std::cout << "  Pressure_Alt: " << message.data.hil_sensor.pressure_alt << std::endl;
    std::cout << "  temparature: " << message.data.hil_sensor.temperature << std::endl;
    std::cout << "  fileds_updated: " << message.data.hil_sensor.fields_updated << std::endl;
    //std::cout << "  id: " << message.data.sensor.id << std::endl;
    printf(" id: 0x%x\n", message.data.hil_sensor.id);
}

static void dump_hil_state_quaternion(const MavlinkDecodedMessage& message) {
    std::cout << "  Type: HIL_STATE_QUATERNION" << std::endl;
    std::cout << "  time_usec: " << message.data.hil_state_quaternion.time_usec << std::endl;
    std::cout << "  Attitude quaternion (w,x,y,z): " 
            << message.data.hil_state_quaternion.attitude_quaternion[0] << ", "
            << message.data.hil_state_quaternion.attitude_quaternion[1] << ", "
            << message.data.hil_state_quaternion.attitude_quaternion[2] << ", "
            << message.data.hil_state_quaternion.attitude_quaternion[3] << std::endl;
    std::cout << "  Rollspeed: " << message.data.hil_state_quaternion.rollspeed << std::endl;
    std::cout << "  Pitchspeed: " << message.data.hil_state_quaternion.pitchspeed << std::endl;
    std::cout << "  Yawspeed: " << message.data.hil_state_quaternion.yawspeed << std::endl;
    std::cout << "  lat: " << message.data.hil_state_quaternion.lat << std::endl;
    std::cout << "  lon: " << message.data.hil_state_quaternion.lon << std::endl;
    std::cout << "  alt: " << message.data.hil_state_quaternion.alt << std::endl;
    std::cout << "  vx: " << message.data.hil_state_quaternion.vx << std::endl;
    std::cout << "  vy: " << message.data.hil_state_quaternion.vy << std::endl;
    std::cout << "  vz: " << message.data.hil_state_quaternion.vz << std::endl;
    std::cout << "  ind_airspeed: " << message.data.hil_state_quaternion.ind_airspeed << std::endl;
    std::cout << "  true_airspeed: " << message.data.hil_state_quaternion.true_airspeed << std::endl;
    std::cout << "  xacc: " << message.data.hil_state_quaternion.xacc << std::endl;
    std::cout << "  yacc: " << message.data.hil_state_quaternion.yacc << std::endl;
    std::cout << "  zacc: " << message.data.hil_state_quaternion.zacc << std::endl;
}
static void dump_system_time(const MavlinkDecodedMessage& message) {
    std::cout << "  Type: SYSTEM_TIME" << std::endl;
    std::cout << "  Unix time: " << message.data.system_time.time_unix_usec << std::endl;
    std::cout << "  Boot time: " << message.data.system_time.time_boot_ms << std::endl;
}
static void dump_hil_gps(const MavlinkDecodedMessage& message) {
    std::cout << "  Type: HIL_GPS" << std::endl;
    std::cout << "  Time stamp: " << message.data.hil_gps.time_usec << std::endl;
    printf("  fix_type: 0x%x\n", message.data.hil_gps.fix_type);
    std::cout << "  Latitude: " << message.data.hil_gps.lat << std::endl;
    std::cout << "  Longitude: " << message.data.hil_gps.lon << std::endl;
    std::cout << "  alt: " << message.data.hil_gps.alt << std::endl;
    std::cout << "  eph: " << message.data.hil_gps.eph << std::endl;
    std::cout << "  epv: " << message.data.hil_gps.epv << std::endl;
    std::cout << "  vn: " << message.data.hil_gps.vn << std::endl;
    std::cout << "  vel: " << message.data.hil_gps.vel << std::endl;
    std::cout << "  vd: " << message.data.hil_gps.vd << std::endl;
    std::cout << "  cog: " << message.data.hil_gps.cog << std::endl;
    std::cout << "  satelites_visible: " << message.data.hil_gps.satellites_visible << std::endl;
    //std::cout << "  id: " << message.data.hil_gps.id << std.endl;
    printf("  id: 0x%x\n", message.data.hil_gps.id);
    std::cout << "  yaw: " << message.data.hil_gps.yaw << std::endl;
}

static void dump_hil_actuator_controls(const MavlinkDecodedMessage& message) {
    std::cout << "  Type: HIL_ACTUATOR_CONTROLS" << std::endl;
    std::cout << "  Time_usec: " << message.data.hil_actuator_controls.time_usec << std::endl;
    std::cout << "  Mode: " << static_cast<int>(message.data.hil_actuator_controls.mode) << std::endl;
    std::cout << "  Flags: " << message.data.hil_actuator_controls.flags << std::endl;
    std::cout << "  Controls: ";
    for(int i = 0; i < 8; ++i) {  // Assuming there are 8 controls
        std::cout << message.data.hil_actuator_controls.controls[i] << " ";
    }
    std::cout << std::endl;
}
void MavlinkDump::mavlink_header_dump(mavlink_message_t &msg)
{
    std::cout << "Decoded MAVLink message:" << std::endl;
    std::cout << "  Message ID: " << msg.msgid << std::endl;
    std::cout << "  System ID: " << static_cast<int>(msg.sysid) << std::endl;
    std::cout << "  Component ID: " << static_cast<int>(msg.compid) << std::endl;
    std::cout << "  Sequence: " << static_cast<int>(msg.seq) << std::endl;
}

void MavlinkDump::mavlink_message_dump(MavlinkDecodedMessage &message)
{
    std::cout << "  Message.type: " << message.type << std::endl;
    switch (message.type) {
        case MAVLINK_MSG_TYPE_HEARTBEAT:
            dump_heartbeat(message);
            break;
        case MAVLINK_MSG_TYPE_COMMAND_ACK:
            dump_command_ack(message);
            break;
        case MAVLINK_MSG_TYPE_LONG:
            dump_long(message);
            break;
        case MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS:
            dump_hil_actuator_controls(message);
            break;
        case MAVLINK_MSG_TYPE_HIL_SENSOR:
            dump_hil_sensor(message);
            break;
        case MAVLINK_MSG_TYPE_SYSTEM_TIME:
            dump_system_time(message);
            break;
        case MAVLINK_MSG_TYPE_HIL_GPS:
            dump_hil_gps(message);
            break;
        case MAVLINK_MSG_TYPE_HIL_STATE_QUATERNION:
            dump_hil_state_quaternion(message);
            break;
        default:
            std::cout << "  Unknown or unsupported MAVLink message type received." << std::endl;
            break;
    }
}
