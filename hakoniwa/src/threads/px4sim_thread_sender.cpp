#include "px4sim_thread_sender.hpp"
#include "mavlink.h"
#include "../mavlink/mavlink_encoder.hpp"
#include "../comm/tcp_connector.hpp"
#include "../hako/pdu/hako_pdu_data.hpp"
#include "../hako/runner/hako_px4_runner.hpp"
#include "../mavlink/mavlink_dump.hpp"
#include <iostream>
#include <unistd.h>
#include <random>
#include <chrono>

#include "../mavlink/mavlink_msg_types.hpp"
//#define DRONE_PX4_SEND_STATE_QUATERNION

static void px4sim_send_system_time(hako::px4::comm::ICommIO &clientConnector, uint64_t time_unix_usec, uint32_t time_boot_ms);
static void px4sim_send_hil_gps(hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec);
#ifdef DRONE_PX4_SEND_STATE_QUATERNION
static void px4sim_send_hil_state_quaternion(hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec);
#endif
static void px4sim_send_sensor(hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec);

static hako::px4::comm::ICommIO *px4_comm_io;
static uint64_t start_time_usec;
typedef struct {
    uint64_t last;
    uint64_t cycle;
} Px4SimSenderTimingType;
typedef struct {
    Px4SimSenderTimingType heartbeat;
    Px4SimSenderTimingType system_time;
    Px4SimSenderTimingType sensor;
    Px4SimSenderTimingType state_quaternion;
    Px4SimSenderTimingType gps;
} Px4SimSenderTimingConfigType;
static Px4SimSenderTimingConfigType px4sim_sender_timing_config;

void px4sim_sender_init(hako::px4::comm::ICommIO *comm_io)
{
    px4_comm_io = comm_io;
    auto now = std::chrono::system_clock::now();
    auto duration_since_epoch = now.time_since_epoch();
    start_time_usec = std::chrono::duration_cast<std::chrono::microseconds>(duration_since_epoch).count();

    memset(&px4sim_sender_timing_config, 0, sizeof(px4sim_sender_timing_config));
    px4sim_sender_timing_config.heartbeat.cycle = 1000000;
    px4sim_sender_timing_config.system_time.cycle = 3000;
    px4sim_sender_timing_config.sensor.cycle = 3000;
    //px4sim_sender_timing_config.state_quaternion.cycle = 8000;
    px4sim_sender_timing_config.gps.cycle = 21000;
    return;
}
static inline bool is_send_cycle(Px4SimSenderTimingType &timing, uint64_t boot_time_usec)
{
    if ((boot_time_usec - timing.last) >= timing.cycle) {
        timing.last = boot_time_usec;
        return true;
    }
    else {
        return false;
    }
}
void px4sim_sender_do_task(void)
{
    if (px4_comm_io == nullptr) {
        return;
    }
    //std::cout << "px4sim_sender_do_task: " << std::endl;
    uint64_t boot_time_usec = (uint64_t)hako_get_current_time_usec();
    uint64_t time_usec =  start_time_usec + boot_time_usec;
    //std::cout << "boot_time_usec: " << boot_time_usec  << std::endl;
    //std::cout << "time_usec: " << time_usec << std::endl;
#if 0
    if (is_send_cycle(px4sim_sender_timing_config.heartbeat, boot_time_usec)) {
        px4sim_send_dummy_heartbeat(*px4_comm_io);
    }
#endif
    if (is_send_cycle(px4sim_sender_timing_config.system_time, boot_time_usec)) {
        px4sim_send_system_time(*px4_comm_io, time_usec, boot_time_usec/1000);
    }
    if (is_send_cycle(px4sim_sender_timing_config.sensor, boot_time_usec)) {
        px4sim_send_sensor(*px4_comm_io, time_usec);
    }
#ifdef DRONE_PX4_SEND_STATE_QUATERNION
    if (is_send_cycle(px4sim_sender_timing_config.state_quaternion, boot_time_usec)) {
        px4sim_send_hil_state_quaternion(*px4_comm_io, time_usec);
    }
#endif
    if (is_send_cycle(px4sim_sender_timing_config.gps, boot_time_usec)) {
        px4sim_send_hil_gps(*px4_comm_io, time_usec);
    }
}


void px4sim_send_message(hako::px4::comm::ICommIO &clientConnector, MavlinkDecodedMessage &message)
{
    mavlink_message_t mavlinkMsg;
    if (mavlink_encode_message(&mavlinkMsg, &message)) 
    {
        int sentDataLen = 0;
        char packet[MAVLINK_MAX_PACKET_LEN];
        int packetLen = mavlink_get_packet(packet, sizeof(packet), &mavlinkMsg);
        if (packetLen > 0) 
        {
            if (clientConnector.send(packet, packetLen, &sentDataLen)) 
            {
                //std::cout << "Sent MAVLink message with length: " << sentDataLen << std::endl;
#ifdef DRONE_PX4_TX_DEBUG_ENABLE
                mavlink_message_dump(message);
#endif
            } 
            else 
            {
                std::cerr << "Failed to send MAVLink message" << std::endl;
            }
        }
    }
}

void px4sim_send_dummy_command_long(hako::px4::comm::ICommIO &clientConnector)
{
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_LONG;
    
    // Setting up the fields for COMMAND_LONG
    message.data.command_long.target_system = 0; // The system which should execute the command, for example, 1 for the first MAV
    message.data.command_long.target_component = 0; // The component which should execute the command, for example, 0 for a generic component
    message.data.command_long.command = 0x4246;
    message.data.command_long.confirmation = 0; // 0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command)
    message.data.command_long.param1 = 0x9c40; // Parameter 1, as defined by MAV_CMD enum
    message.data.command_long.param2 = 0x45; // Parameter 2, as defined by MAV_CMD enum
    message.data.command_long.param3 = 0; // Parameter 3, as defined by MAV_CMD enum
    message.data.command_long.param4 = 0; // Parameter 4, as defined by MAV_CMD enum
    message.data.command_long.param5 = 0; // Parameter 5, as defined by MAV_CMD enum
    message.data.command_long.param6 = 0; // Parameter 6, as defined by MAV_CMD enum
    message.data.command_long.param7 = 0; // Parameter 7, as defined by MAV_CMD enum
    
    px4sim_send_message(clientConnector, message);
}

void px4sim_send_dummy_command_long_ack(hako::px4::comm::ICommIO &clientConnector)
{
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_LONG;
    
    // Setting up the fields for COMMAND_LONG
    message.data.command_long.target_system = 1; // The system which should execute the command, for example, 1 for the first MAV
    message.data.command_long.target_component = 1; // The component which should execute the command, for example, 0 for a generic component
    message.data.command_long.command = 520;
    message.data.command_long.confirmation = 1; // 0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command)
    message.data.command_long.param1 = 0; // Parameter 1, as defined by MAV_CMD enum
    message.data.command_long.param2 = 0; // Parameter 2, as defined by MAV_CMD enum
    message.data.command_long.param3 = 0; // Parameter 3, as defined by MAV_CMD enum
    message.data.command_long.param4 = 0; // Parameter 4, as defined by MAV_CMD enum
    message.data.command_long.param5 = 0; // Parameter 5, as defined by MAV_CMD enum
    message.data.command_long.param6 = 0; // Parameter 6, as defined by MAV_CMD enum
    message.data.command_long.param7 = 0; // Parameter 7, as defined by MAV_CMD enum
    
    px4sim_send_message(clientConnector, message);
    std::cout << "INFO: COMMAND_LONG ack sended" << std::endl;
}

void px4sim_send_dummy_heartbeat(hako::px4::comm::ICommIO &clientConnector)
{
    // HEARTBEATメッセージの準備
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_HEARTBEAT;
    message.data.heartbeat.type = MAV_TYPE_GENERIC;
    message.data.heartbeat.autopilot = 0;
    message.data.heartbeat.base_mode = 	MAV_MODE_FLAG_HIL_ENABLED;
    message.data.heartbeat.custom_mode = 0;
    message.data.heartbeat.system_status = 0;

    px4sim_send_message(clientConnector, message);
}
static void px4sim_send_system_time(hako::px4::comm::ICommIO &clientConnector, uint64_t time_unix_usec, uint32_t time_boot_ms)
{
    // SYSTEM_TIMEメッセージの準備
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_SYSTEM_TIME;
    message.data.system_time.time_unix_usec = time_unix_usec;
    message.data.system_time.time_boot_ms = time_boot_ms;

    px4sim_send_message(clientConnector, message);
}
static void px4sim_send_hil_gps(hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec)
{
    static bool is_initialized = false;
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_HIL_GPS;

    if (hako_mavlink_read_hil_gps(message.data.hil_gps)) {
        is_initialized = true;
    }
    if (is_initialized) {
        message.data.hil_gps.time_usec = time_usec;
        px4sim_send_message(clientConnector, message);
    }
}
#ifdef DRONE_PX4_SEND_STATE_QUATERNION
static void px4sim_send_hil_state_quaternion(hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec)
{
    static std::default_random_engine generator;
    static std::normal_distribution<short> distribution(0.0, 10);
    static bool is_initialized = false;
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_HIL_STATE_QUATERNION;

    if (hako_mavlink_read_hil_state_quaternion(message.data.hil_state_quaternion)) {
        is_initialized = true;
    }
    if (is_initialized) {
        message.data.hil_state_quaternion.time_usec = time_usec;

        //noise
        //message.data.hil_state_quaternion.xacc += distribution(generator);
        //message.data.hil_state_quaternion.yacc += distribution(generator);
        //message.data.hil_state_quaternion.zacc += distribution(generator);

        px4sim_send_message(clientConnector, message);
    }
}
#endif
static void px4sim_send_sensor(hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec)
{
    // Random noise generator setup
    static std::default_random_engine generator;
    static std::normal_distribution<float> distribution(0.0, 0.0001);

    static bool is_initialized = false;
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_HIL_SENSOR;

    if (hako_mavlink_read_hil_sensor(message.data.sensor)) {
        is_initialized = true;
    }
    if (is_initialized) {
        //auto now = std::chrono::system_clock::now();
        //auto duration_since_epoch = now.time_since_epoch();
        //auto ctime = std::chrono::duration_cast<std::chrono::microseconds>(duration_since_epoch).count();
        //std::cout << "HIL_SENSOR: time_usec: " <<  ctime << std::endl;

        message.data.sensor.time_usec = time_usec;
        message.data.sensor.fields_updated = 7167; 
        message.data.sensor.id = 0;

        //noise
        //message.data.sensor.xacc += distribution(generator);
        //message.data.sensor.yacc += distribution(generator);
        //message.data.sensor.zacc += distribution(generator);
        //message.data.sensor.xgyro += distribution(generator);
        //message.data.sensor.ygyro += distribution(generator);
        //message.data.sensor.zgyro += distribution(generator);
        message.data.sensor.abs_pressure += distribution(generator);

        message.data.sensor.xmag += distribution(generator);
        message.data.sensor.ymag += distribution(generator);
        message.data.sensor.zmag += distribution(generator);

        //dummy
        //message.data.sensor.xmag = 0.217065 + distribution(generator);
        //message.data.sensor.ymag = 0.0063418 + distribution(generator);
        //message.data.sensor.zmag = 0.422639 + distribution(generator);

        px4sim_send_message(clientConnector, message);
    }
}

#if 0
static void send_hil_state_quaternion(hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec)
{
    // HIL_STATE_QUATERNIONメッセージの準備
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_HIL_STATE_QUATERNION;
    message.data.hil_state_quaternion.time_usec = time_usec;
    // 以下の値は固定値として設定
    message.data.hil_state_quaternion.attitude_quaternion[0] = 1.0;
    message.data.hil_state_quaternion.attitude_quaternion[1] = 0.0;
    message.data.hil_state_quaternion.attitude_quaternion[2] = 0.0;
    message.data.hil_state_quaternion.attitude_quaternion[3] = 0.0;
    message.data.hil_state_quaternion.rollspeed = 0.0;
    message.data.hil_state_quaternion.pitchspeed = 0.0;
    message.data.hil_state_quaternion.yawspeed = 0.0;
    message.data.hil_state_quaternion.lat = 463700; //4c	52	40	1c
    message.data.hil_state_quaternion.lon = 337732; //44	f4	17	5
    message.data.hil_state_quaternion.alt = 0;
    message.data.hil_state_quaternion.vx = 0;
    message.data.hil_state_quaternion.vy = 0;
    message.data.hil_state_quaternion.vz = 0;
    message.data.hil_state_quaternion.ind_airspeed = 0;
    message.data.hil_state_quaternion.true_airspeed = 0;
    message.data.hil_state_quaternion.xacc = 0;
    message.data.hil_state_quaternion.yacc = 0;
    message.data.hil_state_quaternion.zacc = 0;

    px4sim_send_message(clientConnector, message);
}
static void send_hil_gps(hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec)
{
    static std::default_random_engine generator;
    static std::normal_distribution<float> distribution(0.0, 0.01); // 平均: 0, 標準偏差: 0.01

    // HIL_GPSメッセージの準備
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_HIL_GPS;
    message.data.hil_gps.time_usec = time_usec;
    // 以下の値は固定値として設定
    message.data.hil_gps.fix_type = 0;  // GPS_FIX_TYPE_3D_FIX
    message.data.hil_gps.lat = 473977418 + distribution(generator);
    message.data.hil_gps.lon = 85455939 + distribution(generator);
    message.data.hil_gps.alt = 488008 + distribution(generator);
    message.data.hil_gps.eph = 9778;
    message.data.hil_gps.epv = 9778;
    message.data.hil_gps.vel = 0;
    message.data.hil_gps.vn = 0;
    message.data.hil_gps.ve = 0;
    message.data.hil_gps.vd = 0;
    message.data.hil_gps.cog = 0;
    message.data.hil_gps.satellites_visible = 0;
    message.data.hil_gps.id = 0;
    message.data.hil_gps.yaw = 0;

    px4sim_send_message(clientConnector, message);
}

static auto start_time = std::chrono::system_clock::now();

static void send_sensor(hako::px4::comm::ICommIO &clientConnector, uint64_t time_usec)
{
    // Random noise generator setup
    static std::default_random_engine generator;
    static std::normal_distribution<float> distribution(0.0, 0.01); // mean: 0, std_dev: 0.01    
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_HIL_SENSOR;
    message.data.sensor.time_usec = time_usec;
    message.data.sensor.xacc = -0.0785347 + distribution(generator);
    message.data.sensor.yacc = 0.00118181 + distribution(generator);
    message.data.sensor.zacc = -9.83317 + distribution(generator);
    message.data.sensor.xgyro = 0.00286057 + distribution(generator);
    message.data.sensor.ygyro = -0.00736865 + distribution(generator);
    message.data.sensor.zgyro = -0.00834229 + distribution(generator);
    message.data.sensor.xmag = 0.217065 + distribution(generator);
    message.data.sensor.ymag = 0.0063418 + distribution(generator);
    message.data.sensor.zmag = 0.422639 + distribution(generator);
    message.data.sensor.abs_pressure = 956.025 + distribution(generator);
    message.data.sensor.diff_pressure = 0.0f;
    message.data.sensor.pressure_alt = 0;
    message.data.sensor.temperature = 0.0f + distribution(generator);

    message.data.sensor.fields_updated = 7167; 
    message.data.sensor.id = 0;

    px4sim_send_message(clientConnector, message);
}
#endif