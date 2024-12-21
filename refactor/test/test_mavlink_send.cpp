#include <gtest/gtest.h>
#include <thread>
#include "mavlink/mavlink_comm_tcp.hpp"
#include "mavlink/mavlink_comm_udp.hpp"
#include "mavlink/mavlink_service.hpp"
#include "mavlink/mavlink_decoder.hpp"
#include "comm/udp_connector.hpp"
#include "comm/tcp_connector.hpp"

using namespace hako::mavlink;
using namespace hako::comm;



TEST(MavLinkSendTcpTest, SendSensor) {
    const char* server_ip = "127.0.0.1";
    const int server_port = 12347;

    // サーバーの設定
    IcommEndpointType server_endpoint = {server_ip, server_port};
    auto service = MavLinkService(0, MAVLINK_SERVICE_IO_TYPE_TCP, &server_endpoint, nullptr);
    // サーバースレッド
    std::thread server_thread([&]() {
        auto ret = service.startService();
        ASSERT_TRUE(ret) << "Failed to start service";

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

    // クライアントからメッセージ送信
    char buffer[1024] = {0};
    mavlink_message_t msg;
    MavlinkDecodedMessage message;
    MavLinkCommTcp mavlink_comm_tcp;
    auto ret = mavlink_comm_tcp.receiveMessage(client_io, buffer, sizeof(buffer), nullptr);
    ASSERT_TRUE(ret) << "Failed to receive message";
    
    ret = mavlink_decode(0, buffer, sizeof(buffer), &msg);
    ASSERT_TRUE(ret) << "Failed to decode message";
    ret = mavlink_get_message(&msg, &message);
    ASSERT_TRUE(ret) << "Failed to get message";

    ASSERT_EQ(message.type, MAVLINK_MSG_TYPE_HIL_SENSOR) << "Invalid message type";
    ASSERT_EQ(message.data.hil_sensor.time_usec, 123456789) << "Invalid time_usec";
    service.stopService();

    client_io->close();
    server_thread.join();
}

