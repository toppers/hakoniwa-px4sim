#include "tcp_connector.hpp"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <iostream>  // Added for error output
#include <arpa/inet.h>  // for inet_pton
#include <unistd.h>

#define MAX_ATTEMPTS 600
#define RETRY_INTERVAL 3

namespace hako::px4::comm {

TcpCommIO::TcpCommIO(int sockfd) : sockfd(sockfd) {}

TcpCommIO::~TcpCommIO() {
    close();
}

TcpClient::TcpClient() {}

TcpClient::~TcpClient() {}

ICommIO* TcpClient::client_open(IcommEndpointType *src, IcommEndpointType *dst) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cout << "Failed to create socket: " << strerror(errno) << std::endl;
        return nullptr;
    }
    if (src != nullptr) {
        //nothing to do
    }

    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr(dst->ipaddr);
    remote_addr.sin_port = htons(dst->portno);
    int attempt = 0;
    while (connect(sockfd, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) < 0) {
        if (++attempt >= MAX_ATTEMPTS) {
            std::cout << "Failed to connect after " << MAX_ATTEMPTS << " attempts: " << strerror(errno) << std::endl;
            ::close(sockfd);
            return nullptr;
        }

        std::cout << "Connection attempt " << attempt << " failed, retrying..." << std::endl;
        sleep(RETRY_INTERVAL); // リトライ間隔（秒単位）
    }
    return new TcpCommIO(sockfd);
}

TcpServer::TcpServer() {}

TcpServer::~TcpServer() {}

ICommIO* TcpServer::server_open(IcommEndpointType *endpoint) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cout << "Failed to create socket: " << strerror(errno) << std::endl;
        return nullptr;
    }
    // SO_REUSEADDR オプションを設定
    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        std::cout << "Failed to set SO_REUSEADDR: " << strerror(errno) << std::endl;
        ::close(sockfd);
        return nullptr;
    }
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr(endpoint->ipaddr);
    local_addr.sin_port = htons(endpoint->portno);
    if (bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        std::cout << "Failed to bind socket: " << strerror(errno) << std::endl;
        ::close(sockfd);
        return nullptr;
    }

    if (listen(sockfd, 1) < 0) {
        std::cout << "Failed to listen on socket: " << strerror(errno) << std::endl;
        ::close(sockfd);
        return nullptr;
    }

    struct sockaddr_in remote_addr;
    socklen_t addr_len = sizeof(remote_addr);
    int client_sockfd = accept(sockfd, (struct sockaddr*)&remote_addr, &addr_len);
    if (client_sockfd < 0) {
        std::cout << "Failed to accept connection: " << strerror(errno) << std::endl;
        ::close(sockfd);
        return nullptr;
    }

    return new TcpCommIO(client_sockfd);
}


#define MAVLINK_HEADER_LEN  9
bool TcpCommIO::recv(char* data, int datalen, int* recv_datalen) {
    // see: http://mavlink.io/en/guide/serialization.html

    char header[MAVLINK_HEADER_LEN];
    int received = 0;

    // Receive header
    while (received < MAVLINK_HEADER_LEN) {
        int len = read(sockfd, header + received, MAVLINK_HEADER_LEN - received);
        if (len > 0) {
            received += len;
        } else if (len == 0 || (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)) {
            //std::cout << "Failed to receive MAVLink header: " << strerror(errno) << std::endl;
            return false;
        }
    }
#if 0
    for (int i = 0; i < 10; i++) {
        printf("header[%d] = 0x%x\n", i, header[i]);
    }
#endif
    // Parse header to get packet length (assuming packet length is at offset 1)
    int packetlen = static_cast<unsigned char>(header[1]) + 2 /* CRC */ + 1 /* Signature */;

    // Check if datalen is sufficient to hold header and packet data
    if (datalen < MAVLINK_HEADER_LEN + packetlen) {
        std::cout << "Provided data buffer is too small to hold the MAVLink message." << std::endl;
        return false;
    }

    // Copy header data to output buffer
    memcpy(data, header, MAVLINK_HEADER_LEN);

    // Receive packet data
    received = 0;
    while (received < packetlen) {
        int len = read(sockfd, data + MAVLINK_HEADER_LEN + received, packetlen - received);
        if (len > 0) {
            received += len;
        } else if (len == 0 || (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)) {
            std::cout << "Failed to receive MAVLink data: " << strerror(errno) << std::endl;
            return false;
        }
    }

    *recv_datalen = MAVLINK_HEADER_LEN + packetlen;
    return true;
}


bool TcpCommIO::send(const char* data, int datalen, int* send_datalen) {
    int total_sent = 0;
    while (total_sent < datalen) {
        int sent = write(sockfd, data + total_sent, datalen - total_sent);
        if (sent > 0) {
            total_sent += sent;
        } else if (sent == 0 || (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)) {
            std::cout << "Failed to send data: " << strerror(errno) << std::endl;
            break;
        }
    }
    *send_datalen = total_sent;
    return total_sent == datalen;
}

bool TcpCommIO::close() {
    if (::close(sockfd) < 0) {
        std::cout << "Failed to close socket: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

}  // namespace hako::px4::comm
