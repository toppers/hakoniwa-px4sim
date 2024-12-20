#ifndef _MAVLIN_MSG_TYPES_HPP_
#define _MAVLIN_MSG_TYPES_HPP_

#include "mavlink.h"
#include "mavlink_config.hpp"

typedef enum {
    MAVLINK_MSG_TYPE_UNKNOWN,
    MAVLINK_MSG_TYPE_HEARTBEAT,
    MAVLINK_MSG_TYPE_LONG,
    MAVLINK_MSG_TYPE_COMMAND_ACK,
    MAVLINK_MSG_TYPE_HIL_SENSOR,
    MAVLINK_MSG_TYPE_HIL_STATE_QUATERNION,
    MAVLINK_MSG_TYPE_SYSTEM_TIME,
    MAVLINK_MSG_TYPE_HIL_GPS,
    MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS,
    MAVLINK_MSG_TYPE_NUM,
} MavlinkMsgType;

typedef struct {
    MavlinkMsgType type;
    union {
        mavlink_heartbeat_t heartbeat;
        mavlink_command_long_t command_long;
        mavlink_command_ack_t command_ack;
        mavlink_hil_sensor_t sensor;
        mavlink_hil_state_quaternion_t hil_state_quaternion;
        mavlink_system_time_t system_time;
        mavlink_hil_gps_t hil_gps;
        mavlink_hil_actuator_controls_t hil_actuator_controls;
    } data;
} MavlinkDecodedMessage;


typedef struct {
    uint32_t dataLength;
    uint32_t owner;
    uint64_t relativeTimestamp;
    uint8_t  data[8];
} MavlinkCaptureDataType;

typedef struct {
    uint64_t start_time;
    uint64_t packet_num;
    uint64_t total_size;
    int save_file;
    uint64_t memsize;
    uint64_t offset;
    uint8_t *data;
    uint64_t packets_since_last_save;
    uint64_t last_save_offset;
} MavlinkCaptureControllerType;

#define MAVLINK_CAPTURE_INC_DATA_SIZE   8192

#endif /* _MAVLIN_MSG_TYPES_HPP_ */
