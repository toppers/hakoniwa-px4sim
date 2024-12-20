#include "mavlink_decoder.hpp"
#include <iostream>

bool mavlink_decode(uint8_t chan, const char* packet, int packet_len, mavlink_message_t *msg)
{
    mavlink_status_t status;
    bool messageReceived = false;

    for(int i = 0; i < packet_len; i++) {
        uint8_t result = mavlink_parse_char(chan, packet[i], msg, &status);
        if(result) {
            messageReceived = true;
        }
    }
    //std::cout << "mavlink_decode result = " << messageReceived << std::endl;
    return messageReceived;
}

bool mavlink_get_message(mavlink_message_t *msg, MavlinkDecodedMessage *message)
{
    switch (msg->msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT: 
        {
            message->type = MAVLINK_MSG_TYPE_HEARTBEAT;
            mavlink_msg_heartbeat_decode(msg, &message->data.heartbeat);
            return true;
        }
        case MAVLINK_MSG_ID_COMMAND_LONG:
        {
            message->type = MAVLINK_MSG_TYPE_LONG;
            mavlink_msg_command_long_decode(msg, &message->data.command_long);
            return true;
        }
        case MAVLINK_MSG_ID_HIL_ACTUATOR_CONTROLS:
        {
            message->type = MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS;
            mavlink_msg_hil_actuator_controls_decode(msg, &message->data.hil_actuator_controls);
            return true;
        }
        case MAVLINK_MSG_ID_HIL_SENSOR:
        {
            message->type = MAVLINK_MSG_TYPE_HIL_SENSOR;
            mavlink_msg_hil_sensor_decode(msg, &message->data.sensor);
            return true;
        }
        case MAVLINK_MSG_ID_SYSTEM_TIME:
        {
            message->type = MAVLINK_MSG_TYPE_SYSTEM_TIME;
            mavlink_msg_system_time_decode(msg, &message->data.system_time);
            return true;
        }
        case MAVLINK_MSG_ID_HIL_GPS:
        {
            message->type = MAVLINK_MSG_TYPE_HIL_GPS;
            mavlink_msg_hil_gps_decode(msg, &message->data.hil_gps);
            return true;
        }        
        default:
            message->type = MAVLINK_MSG_TYPE_UNKNOWN;
            return false;
    }
}
