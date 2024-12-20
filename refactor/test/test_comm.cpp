#include <gtest/gtest.h>
#include <thread>
#include "comm/udp_connector.hpp"

namespace hako::comm {

TEST(UdpCommTest, SendReceiveData) {
    const char* server_ip = "127.0.0.1";
    const int server_port = 12345;
    const int client_port = 54321;
    const char* test_message = "Hello, UDP!";
    char buffer[1024] = {0};

    // サーバーの設定
    IcommEndpointType server_endpoint = {server_ip, server_port};
    UdpServer server;
    ICommIO* server_io = nullptr;

    // クライアントの設定
    IcommEndpointType client_src = {server_ip, client_port};
    IcommEndpointType client_dst = {server_ip, server_port};
    UdpClient client;

    // サーバースレッド
    std::thread server_thread([&]() {
        server_io = server.server_open(&server_endpoint);
        ASSERT_NE(server_io, nullptr) << "Failed to open server";

        int recv_len = 0;
        ASSERT_TRUE(server_io->recv(buffer, sizeof(buffer), &recv_len));
        ASSERT_EQ(recv_len, strlen(test_message));
        ASSERT_STREQ(buffer, test_message);

        // サーバー側でクライアントに返信
        const char* reply_message = "Hello, Client!";
        int sent_len = 0;
        ASSERT_TRUE(server_io->send(reply_message, static_cast<int>(strlen(reply_message)), &sent_len));
        ASSERT_EQ(sent_len, strlen(reply_message));

        server_io->close();
    });

    // クライアント処理
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // サーバーの準備待機
    ICommIO* client_io = client.client_open(&client_src, &client_dst);
    ASSERT_NE(client_io, nullptr) << "Failed to open client";

    // クライアントからデータ送信
    int sent_len = 0;
    ASSERT_TRUE(client_io->send(test_message, static_cast<int>(strlen(test_message)), &sent_len));
    ASSERT_EQ(sent_len, strlen(test_message));

    // クライアントで返信を受信
    int recv_len = 0;
    char reply_buffer[1024] = {0};
    ASSERT_TRUE(client_io->recv(reply_buffer, sizeof(reply_buffer), &recv_len));
    ASSERT_STREQ(reply_buffer, "Hello, Client!");

    client_io->close();
    server_thread.join();
}

} // namespace hako::comm
