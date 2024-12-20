#include <gtest/gtest.h>
#include <thread>
#include "mavlink/mavlink_comm_tcp.hpp"
#include "mavlink/mavlink_comm_udp.hpp"
#include "comm/udp_connector.hpp"
#include "comm/tcp_connector.hpp"

using namespace hako::mavlink;
using namespace hako::comm;

#include <cstring>
#include <iostream>
#include <vector>


std::vector<char> createMavlinkMessage(const char* payload, int payload_len) {
    std::vector<char> message(MAVLINK_TOTAL_HEADER_LEN + payload_len);

    // ヘッダー作成 (ダミーデータ)
    message[0] = 0xFE; // スタートフレーム
    message[1] = static_cast<char>(payload_len); // ペイロード長
    message[2] = 0x01; // シーケンス
    message[3] = 0x01; // システムID
    message[4] = 0x01; // コンポーネントID
    message[5] = 0x01; // メッセージID (ダミー)

    // CRC（ダミーで設定）
    message[MAVLINK_TOTAL_HEADER_LEN - 2] = 0x00; // CRC1
    message[MAVLINK_TOTAL_HEADER_LEN - 1] = 0x00; // CRC2

    // ペイロードコピー
    memcpy(message.data() + MAVLINK_HEADER_LEN, payload, payload_len);

    return message;
}

TEST(MavLinkCommUdpTest, SendReceiveDataWithMavlinkHeader) {
    const char* server_ip = "127.0.0.1";
    const int server_port = 12346;
    const int client_port = 54322;
    const char* payload = "Hello, MAVLink!";
    const int payload_len = strlen(payload);

    // サーバーの設定
    IcommEndpointType server_endpoint = {server_ip, server_port};
    UdpServer udp_server;
    ICommIO* server_io = nullptr;

    // クライアントの設定
    UdpClient udp_client;
    IcommEndpointType client_src = {server_ip, client_port};
    IcommEndpointType client_endpoint = {server_ip, server_port};
    MavLinkCommUdp mavlink_comm_udp;

    // サーバースレッド
    std::thread server_thread([&]() {
        server_io = udp_server.server_open(&server_endpoint);
        ASSERT_NE(server_io, nullptr) << "Failed to open UDP server";
        char recv_buffer[1024] = {0};
        int recv_len = 0;
        ASSERT_TRUE(mavlink_comm_udp.receiveMessage(server_io, recv_buffer, sizeof(recv_buffer), &recv_len));
        ASSERT_GE(recv_len, MAVLINK_HEADER_LEN);

        // ヘッダーとペイロードを検証
        ASSERT_EQ(recv_buffer[1], payload_len);
        ASSERT_STREQ(recv_buffer + MAVLINK_HEADER_LEN, payload);

        // サーバーから返信
        const char* reply_payload = "Reply from Server";
        auto reply_message = createMavlinkMessage(reply_payload, strlen(reply_payload));
        ASSERT_TRUE(mavlink_comm_udp.sendMessage(server_io, reply_message.data(), reply_message.size()));
        server_io->close();
    });

    // クライアント処理
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // サーバーの準備待機
    ICommIO* client_io = udp_client.client_open(&client_src, &client_endpoint);
    ASSERT_NE(client_io, nullptr) << "Failed to open UDP client";

    // クライアントからメッセージ送信
    auto message = createMavlinkMessage(payload, payload_len);
    ASSERT_TRUE(mavlink_comm_udp.sendMessage(client_io, message.data(), message.size()));
    // クライアントで返信を受信
    char reply_buffer[1024] = {0};
    int recv_len = 0;
    ASSERT_TRUE(mavlink_comm_udp.receiveMessage(client_io, reply_buffer, sizeof(reply_buffer), &recv_len));
    ASSERT_GE(recv_len, MAVLINK_HEADER_LEN);
    ASSERT_STREQ(reply_buffer + MAVLINK_HEADER_LEN, "Reply from Server");
    client_io->close();
    server_thread.join();
}

#if 1
TEST(MavLinkCommTcpTest, SendReceiveDataWithMavlinkHeader) {
    const char* server_ip = "127.0.0.1";
    const int server_port = 12346;
    const int client_port = 54322;
    const char* payload = "Hello, MAVLink!";
    const int payload_len = strlen(payload);

    // サーバーの設定
    IcommEndpointType server_endpoint = {server_ip, server_port};
    TcpServer tcp_server;
    ICommIO* server_io = nullptr;

    // クライアントの設定
    TcpClient tcp_client;
    IcommEndpointType client_src = {server_ip, client_port};
    IcommEndpointType client_endpoint = {server_ip, server_port};
    MavLinkCommTcp mavlink_comm_tcp;

    // サーバースレッド
    std::thread server_thread([&]() {
        server_io = tcp_server.server_open(&server_endpoint);
        ASSERT_NE(server_io, nullptr) << "Failed to open TCP server";

        char recv_buffer[1024] = {0};
        int recv_len = 0;
        ASSERT_TRUE(mavlink_comm_tcp.receiveMessage(server_io, recv_buffer, sizeof(recv_buffer), &recv_len));
        ASSERT_GE(recv_len, MAVLINK_HEADER_LEN);

        // ヘッダーとペイロードを検証
        ASSERT_EQ(recv_buffer[1], payload_len);
        ASSERT_STREQ(recv_buffer + MAVLINK_HEADER_LEN, payload);

        // サーバーから返信
        const char* reply_payload = "Reply from Server";
        auto reply_message = createMavlinkMessage(reply_payload, strlen(reply_payload));
        ASSERT_TRUE(mavlink_comm_tcp.sendMessage(server_io, reply_message.data(), reply_message.size()));
        server_io->close();
    });

    // クライアント処理
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // サーバーの準備待機
    ICommIO* client_io = tcp_client.client_open(&client_src, &client_endpoint);
    ASSERT_NE(client_io, nullptr) << "Failed to open TCP client";

    // クライアントからメッセージ送信
    auto message = createMavlinkMessage(payload, payload_len);
    ASSERT_TRUE(mavlink_comm_tcp.sendMessage(client_io, message.data(), message.size()));

    // クライアントで返信を受信
    char reply_buffer[1024] = {0};
    int recv_len = 0;
    ASSERT_TRUE(mavlink_comm_tcp.receiveMessage(client_io, reply_buffer, sizeof(reply_buffer), &recv_len));
    ASSERT_GE(recv_len, MAVLINK_HEADER_LEN);
    ASSERT_STREQ(reply_buffer + MAVLINK_HEADER_LEN, "Reply from Server");

    client_io->close();
    server_thread.join();
}
#endif