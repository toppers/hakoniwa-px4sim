#include "mavlink_encoder.hpp"
#include <iostream>

int mavlink_get_packet(char* packet, int packet_len, const mavlink_message_t *msg) 
{
    if (!packet || !msg) {
        return -1;
    }

    // この関数は、エンコードされたMAVLinkメッセージのバイト数を返します。
    int len = mavlink_msg_to_send_buffer((uint8_t*)packet, msg);

    if (len <= packet_len) {
        return len;
    } else {
        return -1;
    }
}

bool mavlink_encode_message(mavlink_message_t *msg, const MavlinkDecodedMessage *message) 
{
    if (!msg || !message) {
        return false;
    }

    switch (message->type) {
        case MAVLINK_MSG_TYPE_HEARTBEAT:
            mavlink_msg_heartbeat_pack(
                MAVLINK_CONFIG_SYSTEM_ID, 
                MAVLINK_CONFIG_COMPONENT_ID, 
                msg, 
                message->data.heartbeat.type,
                message->data.heartbeat.autopilot, 
                message->data.heartbeat.base_mode,
                message->data.heartbeat.custom_mode, 
                message->data.heartbeat.system_status
            );
            return true;
        
        case MAVLINK_MSG_TYPE_LONG:
            mavlink_msg_command_long_pack(
                MAVLINK_CONFIG_SYSTEM_ID, 
                MAVLINK_CONFIG_COMPONENT_ID, 
                msg, 
                message->data.command_long.target_system,
                message->data.command_long.target_component, 
                message->data.command_long.command, 
                message->data.command_long.confirmation, 
                message->data.command_long.param1, 
                message->data.command_long.param2,
                message->data.command_long.param3, 
                message->data.command_long.param4, 
                message->data.command_long.param5, 
                message->data.command_long.param6, 
                message->data.command_long.param7
            );
            return true;
        case MAVLINK_MSG_TYPE_ACK:
            mavlink_msg_command_ack_pack(
                MAVLINK_CONFIG_SYSTEM_ID, 
                MAVLINK_CONFIG_COMPONENT_ID, 
                msg, 
                message->data.ack.command,
                message->data.ack.result,
                message->data.ack.progress,
                message->data.ack.target_system,
                message->data.ack.target_component,
                message->data.ack.result_param2
            );
            return true;
        case MAVLINK_MSG_TYPE_HIL_SENSOR:
            mavlink_msg_hil_sensor_pack(
                MAVLINK_CONFIG_SYSTEM_ID, 
                MAVLINK_CONFIG_COMPONENT_ID, 
                msg, 
                message->data.sensor.time_usec,
                message->data.sensor.xacc,
                message->data.sensor.yacc,
                message->data.sensor.zacc,
                message->data.sensor.xgyro,
                message->data.sensor.ygyro,
                message->data.sensor.zgyro,
                message->data.sensor.xmag,
                message->data.sensor.ymag,
                message->data.sensor.zmag,
                message->data.sensor.abs_pressure,
                message->data.sensor.diff_pressure,
                message->data.sensor.pressure_alt,
                message->data.sensor.temperature,
                message->data.sensor.fields_updated,
                message->data.sensor.id
            );
            return true;
        case MAVLINK_MSG_TYPE_HIL_STATE_QUATERNION:
            mavlink_msg_hil_state_quaternion_pack(
                MAVLINK_CONFIG_SYSTEM_ID, 
                MAVLINK_CONFIG_COMPONENT_ID, 
                msg, 
                message->data.hil_state_quaternion.time_usec,
                message->data.hil_state_quaternion.attitude_quaternion,
                message->data.hil_state_quaternion.rollspeed,
                message->data.hil_state_quaternion.pitchspeed,
                message->data.hil_state_quaternion.yawspeed,
                message->data.hil_state_quaternion.lat,
                message->data.hil_state_quaternion.lon,
                message->data.hil_state_quaternion.alt,
                message->data.hil_state_quaternion.vx,
                message->data.hil_state_quaternion.vy,
                message->data.hil_state_quaternion.vz,
                message->data.hil_state_quaternion.ind_airspeed,
                message->data.hil_state_quaternion.true_airspeed,
                message->data.hil_state_quaternion.xacc,
                message->data.hil_state_quaternion.yacc,
                message->data.hil_state_quaternion.zacc
            );
            return true;
        case MAVLINK_MSG_TYPE_SYSTEM_TIME:
            mavlink_msg_system_time_pack(
                MAVLINK_CONFIG_SYSTEM_ID,
                MAVLINK_CONFIG_COMPONENT_ID,
                msg,
                message->data.system_time.time_unix_usec,
                message->data.system_time.time_boot_ms
            );
            return true;
        case MAVLINK_MSG_TYPE_HIL_GPS:
            mavlink_msg_hil_gps_pack(
                MAVLINK_CONFIG_SYSTEM_ID,
                MAVLINK_CONFIG_COMPONENT_ID,
                msg,
                message->data.hil_gps.time_usec,
                message->data.hil_gps.fix_type,
                message->data.hil_gps.lat,
                message->data.hil_gps.lon,
                message->data.hil_gps.alt,
                message->data.hil_gps.eph,
                message->data.hil_gps.epv,
                message->data.hil_gps.vel,
                message->data.hil_gps.vn,
                message->data.hil_gps.ve,
                message->data.hil_gps.vd,
                message->data.hil_gps.cog,
                message->data.hil_gps.satellites_visible,
                message->data.hil_gps.id,  // Assuming id field is present in your struct
                message->data.hil_gps.yaw  // Assuming yaw field is present in your struct
            );
            return true;
        default:
            std::cerr << "Unsupported message type for encoding: " << message->type << std::endl;
            return false;
    }
}
