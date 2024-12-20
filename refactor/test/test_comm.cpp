#include <gtest/gtest.h>
#include "comm/udp_connector.hpp"

TEST(UdpConnectorTest, Initialization) {
    hako::comm::UdpClient client;
    EXPECT_TRUE(client.client_open(nullptr, nullptr) == nullptr);
}
