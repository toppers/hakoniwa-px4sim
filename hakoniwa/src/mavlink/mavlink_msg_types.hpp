#ifndef _MAVLIN_MSG_TYPES_HPP_
#define _MAVLIN_MSG_TYPES_HPP_

#include "mavlink.h"
#include "mavlink_config.hpp"

typedef enum {
    MAVLINK_MSG_TYPE_UNKNOWN,
    MAVLINK_MSG_TYPE_HEARTBEAT,
    MAVLINK_MSG_TYPE_LONG,
    MAVLINK_MSG_TYPE_ACK,
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
        mavlink_command_ack_t ack;
        mavlink_hil_sensor_t sensor;
        mavlink_hil_state_quaternion_t hil_state_quaternion;
        mavlink_system_time_t system_time;
        mavlink_hil_gps_t hil_gps;
        mavlink_hil_actuator_controls_t hil_actuator_controls;
    } data;
} MavlinkDecodedMessage;


typedef struct {
    uint32_t dataLength;         // 受信データ長: 4バイト
    uint32_t owner;
    uint64_t relativeTimestamp;  // 受信相対時間: 8バイト
    uint8_t  data[8];           // 受信データ（可変：受信データ長）
} MavlinkCaptureDataType;

/*
 * キャプチャデータのデータ構造
 */
typedef struct {
    /*
     * キャプチャ開始した時間がセットされる。
     * セットされる値は相対時間であり。１個目のデータの場合は０となる。
     * 単位：usec
     */
    uint64_t start_time;
    /*
     * 格納されているパケット数
     */
    uint64_t packet_num;
    /*
     * 格納されているパケットの層データサイズ（単位：バイト）
     */
    uint64_t total_size;
    /*
     * 保存用ファイルディスクリプタ
     */
    int save_file;
    uint64_t memsize;
    uint64_t offset;
    uint8_t *data;
    uint64_t packets_since_last_save;
    uint64_t last_save_offset;
} MavlinkCaptureControllerType;

#define MAVLINK_CAPTURE_INC_DATA_SIZE   8192 /* メモリ拡張サイズ（単位：バイト） */
#define MAVLINK_CAPTURE_SAVE_FILEPATH "./capture.bin"

#endif /* _MAVLIN_MSG_TYPES_HPP_ */
