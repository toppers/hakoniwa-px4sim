#include <gtest/gtest.h>
#include <thread>
#include "mavlink/mavlink_comm_tcp.hpp"
#include "mavlink/mavlink_comm_udp.hpp"
#include "mavlink/mavlink_service.hpp"
#include "mavlink/mavlink_decoder.hpp"
#include "mavlink/mavlink_encoder.hpp"
#include "comm/udp_connector.hpp"
#include "comm/tcp_connector.hpp"

using namespace hako::mavlink;
using namespace hako::comm;



TEST(MavLinkSendTcpTest, SendReceive) {
    const char* server_ip = "127.0.0.1";
    const int server_port = 12347;

    // サーバーの設定
    IcommEndpointType server_endpoint = {server_ip, server_port};
    auto service = MavLinkService(0, MAVLINK_SERVICE_IO_TYPE_TCP, &server_endpoint, nullptr);
    // サーバースレッド
    std::thread server_thread([&]() {
        auto ret = service.startService();
        ASSERT_TRUE(ret) << "Failed to start service";

        MavlinkHakoMessage actuator_message;
        actuator_message.type = MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS;
        while (true) {
            bool is_dirty;
            ret = service.readMessage(actuator_message, is_dirty);
            if (ret) {
                std::cout << "Received message: " << actuator_message.type << std::endl;
                break;
            }
            else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        //check actuator message
        ASSERT_EQ(actuator_message.type, MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS) << "Invalid message type";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.time_usec, 123456789) << "Invalid time_usec";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[0], 1.0f) << "Invalid controls[0]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[1], 2.0f) << "Invalid controls[1]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[2], 3.0f) << "Invalid controls[2]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[3], 4.0f) << "Invalid controls[3]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[4], 5.0f) << "Invalid controls[4]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[5], 6.0f) << "Invalid controls[5]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[6], 7.0f) << "Invalid controls[6]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[7], 8.0f) << "Invalid controls[7]";

        MavlinkHakoMessage message;
        message.type = MAVLINK_MSG_TYPE_HIL_SENSOR;
        message.data.hil_sensor.time_usec = 123456789;
        message.data.hil_sensor.xacc = 1.0f;
        message.data.hil_sensor.yacc = 2.0f;
        message.data.hil_sensor.zacc = 3.0f;
        message.data.hil_sensor.xgyro = 4.0f;
        message.data.hil_sensor.ygyro = 5.0f;
        message.data.hil_sensor.zgyro = 6.0f;
        message.data.hil_sensor.xmag = 7.0f;
        message.data.hil_sensor.ymag = 8.0f;
        message.data.hil_sensor.zmag = 9.0f;
        ret = service.sendMessage(message);
        ASSERT_TRUE(ret) << "Failed to send message";
        //thread sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    });
    // クライアントの設定
    TcpClient tcp_client;
    IcommEndpointType client_endpoint = {server_ip, server_port};

    // クライアント処理
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // サーバーの準備待機
    ICommIO* client_io = tcp_client.client_open(nullptr, &client_endpoint);
    ASSERT_NE(client_io, nullptr) << "Failed to open TCP client";

    MavLinkCommTcp mavlink_comm_tcp;
    char buffer[1024] = {0};
    mavlink_message_t msg;
    // クライアントからメッセージ送信
    //spec 
    // uint16_t mavlink_msg_hil_actuator_controls_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
    //                           uint64_t time_usec, const float *controls, uint8_t mode, uint64_t flags)
    float controls[16] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    mavlink_msg_hil_actuator_controls_pack(0, 0, &msg, 123456789, controls, 0, 0);
    auto len = mavlink_get_packet(buffer, sizeof(buffer), &msg);
    ASSERT_TRUE(len) << "Failed to get packet";
    //std::cout << "tcp send message" << std::endl;
    auto ret = mavlink_comm_tcp.sendMessage(client_io, buffer, len);
    ASSERT_TRUE(ret) << "Failed to send message";
    //std::cout << "tcp send done" << std::endl;

    // クライアントで返信を受信
    MavlinkDecodedMessage message;
    ret = mavlink_comm_tcp.receiveMessage(client_io, buffer, sizeof(buffer), nullptr);
    ASSERT_TRUE(ret) << "Failed to receive message";
    ret = mavlink_decode(0, buffer, sizeof(buffer), &msg);
    ASSERT_TRUE(ret) << "Failed to decode message";
    ret = mavlink_get_message(&msg, &message);
    ASSERT_TRUE(ret) << "Failed to get message";
    ASSERT_EQ(message.type, MAVLINK_MSG_TYPE_HIL_SENSOR) << "Invalid message type";
    ASSERT_EQ(message.data.hil_sensor.time_usec, 123456789) << "Invalid time_usec";
    ASSERT_EQ(message.data.hil_sensor.xacc, 1.0f) << "Invalid xacc";
    ASSERT_EQ(message.data.hil_sensor.yacc, 2.0f) << "Invalid yacc";
    ASSERT_EQ(message.data.hil_sensor.zacc, 3.0f) << "Invalid zacc";
    ASSERT_EQ(message.data.hil_sensor.xgyro, 4.0f) << "Invalid xgyro";
    ASSERT_EQ(message.data.hil_sensor.ygyro, 5.0f) << "Invalid ygyro";
    ASSERT_EQ(message.data.hil_sensor.zgyro, 6.0f) << "Invalid zgyro";
    ASSERT_EQ(message.data.hil_sensor.xmag, 7.0f) << "Invalid xmag";
    ASSERT_EQ(message.data.hil_sensor.ymag, 8.0f) << "Invalid ymag";
    ASSERT_EQ(message.data.hil_sensor.zmag, 9.0f) << "Invalid zmag";

    service.stopService();

    client_io->close();
    server_thread.join();
}

#if 1
TEST(MavLinkSendUdpTest, SendSensor) {
    const char* server_ip = "127.0.0.1";
    const int server_port = 12347;

    // サーバーの設定
    IcommEndpointType server_endpoint = {server_ip, server_port};
    auto service = MavLinkService(0, MAVLINK_SERVICE_IO_TYPE_UDP, &server_endpoint, nullptr);
    // サーバースレッド
    std::thread server_thread([&]() {
        auto ret = service.startService();
        ASSERT_TRUE(ret) << "Failed to start service";

        MavlinkHakoMessage actuator_message;
        actuator_message.type = MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS;
        while (true) {
            bool is_dirty;
            ret = service.readMessage(actuator_message, is_dirty);
            if (ret) {
                std::cout << "Received message: " << actuator_message.type << std::endl;
                break;
            }
            else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        //check actuator message
        ASSERT_EQ(actuator_message.type, MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS) << "Invalid message type";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.time_usec, 123456789) << "Invalid time_usec";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[0], 1.0f) << "Invalid controls[0]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[1], 2.0f) << "Invalid controls[1]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[2], 3.0f) << "Invalid controls[2]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[3], 4.0f) << "Invalid controls[3]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[4], 5.0f) << "Invalid controls[4]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[5], 6.0f) << "Invalid controls[5]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[6], 7.0f) << "Invalid controls[6]";
        ASSERT_EQ(actuator_message.data.hil_actuator_controls.controls[7], 8.0f) << "Invalid controls[7]";

        MavlinkHakoMessage message;
        message.type = MAVLINK_MSG_TYPE_HIL_SENSOR;
        message.data.hil_sensor.time_usec = 123456789;
        message.data.hil_sensor.xacc = 1.0f;
        message.data.hil_sensor.yacc = 2.0f;
        message.data.hil_sensor.zacc = 3.0f;
        message.data.hil_sensor.xgyro = 4.0f;
        message.data.hil_sensor.ygyro = 5.0f;
        message.data.hil_sensor.zgyro = 6.0f;
        message.data.hil_sensor.xmag = 7.0f;
        message.data.hil_sensor.ymag = 8.0f;
        message.data.hil_sensor.zmag = 9.0f;
        ret = service.sendMessage(message);
        ASSERT_TRUE(ret) << "Failed to send message";
        //thread sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    });
    // クライアントの設定
    UdpClient udp_client;
    IcommEndpointType client_endpoint = {server_ip, server_port};

    // クライアント処理
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // サーバーの準備待機
    ICommIO* client_io = udp_client.client_open(nullptr, &client_endpoint);
    ASSERT_NE(client_io, nullptr) << "Failed to open UDP client";

    MavLinkCommUdp mavlink_comm_udp;
    char buffer[1024] = {0};
    mavlink_message_t msg;
    // クライアントからメッセージ送信
    //spec 
    // uint16_t mavlink_msg_hil_actuator_controls_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
    //                           uint64_t time_usec, const float *controls, uint8_t mode, uint64_t flags)
    float controls[16] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    mavlink_msg_hil_actuator_controls_pack(0, 0, &msg, 123456789, controls, 0, 0);
    auto len = mavlink_get_packet(buffer, sizeof(buffer), &msg);
    ASSERT_TRUE(len) << "Failed to get packet";
    //std::cout << "tcp send message" << std::endl;
    auto ret = mavlink_comm_udp.sendMessage(client_io, buffer, len);
    ASSERT_TRUE(ret) << "Failed to send message";
    //std::cout << "tcp send done" << std::endl;

    // クライアントで返信を受信
    MavlinkDecodedMessage message;
    ret = mavlink_comm_udp.receiveMessage(client_io, buffer, sizeof(buffer), nullptr);
    ASSERT_TRUE(ret) << "Failed to receive message";
    ret = mavlink_decode(0, buffer, sizeof(buffer), &msg);
    ASSERT_TRUE(ret) << "Failed to decode message";
    ret = mavlink_get_message(&msg, &message);
    ASSERT_TRUE(ret) << "Failed to get message";
    ASSERT_EQ(message.type, MAVLINK_MSG_TYPE_HIL_SENSOR) << "Invalid message type";
    ASSERT_EQ(message.data.hil_sensor.time_usec, 123456789) << "Invalid time_usec";
    ASSERT_EQ(message.data.hil_sensor.xacc, 1.0f) << "Invalid xacc";
    ASSERT_EQ(message.data.hil_sensor.yacc, 2.0f) << "Invalid yacc";
    ASSERT_EQ(message.data.hil_sensor.zacc, 3.0f) << "Invalid zacc";
    ASSERT_EQ(message.data.hil_sensor.xgyro, 4.0f) << "Invalid xgyro";
    ASSERT_EQ(message.data.hil_sensor.ygyro, 5.0f) << "Invalid ygyro";
    ASSERT_EQ(message.data.hil_sensor.zgyro, 6.0f) << "Invalid zgyro";
    ASSERT_EQ(message.data.hil_sensor.xmag, 7.0f) << "Invalid xmag";
    ASSERT_EQ(message.data.hil_sensor.ymag, 8.0f) << "Invalid ymag";
    ASSERT_EQ(message.data.hil_sensor.zmag, 9.0f) << "Invalid zmag";
    
    service.stopService();

    client_io->close();
    server_thread.join();
}

#endif