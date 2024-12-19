#include "udp_connector.hpp"
#include <cstring>

#ifdef _WIN32
#define close_socket closesocket
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#define close_socket ::close
#include <unistd.h>
#include <arpa/inet.h>
#endif

namespace hako::comm {

// ===== UdpCommIOクラス =====
UdpCommIO::UdpCommIO(int sockfd, const sockaddr_in& remote_addr) : sockfd(sockfd), remote_addr(remote_addr) {}

UdpCommIO::~UdpCommIO() {
    close();
}

bool UdpCommIO::recv(char* data, int datalen, int* recv_datalen) {
    if(sockfd < 0 || !data || datalen <= 0) return false;

    socklen_t addr_len = sizeof(remote_addr);
    int bytes_received = recvfrom(sockfd, data, datalen, 0, (struct sockaddr*)&remote_addr, &addr_len);
    if(bytes_received < 0) {
        return false;
    }

    if(recv_datalen) {
        *recv_datalen = bytes_received;
    }

    return true;
}

bool UdpCommIO::send(const char* data, int datalen, int* send_datalen) {
    if(sockfd < 0 || !data || datalen <= 0) return false;

    int bytes_sent = sendto(sockfd, data, datalen, 0, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if(bytes_sent < 0) {
        return false;
    }

    if(send_datalen) {
        *send_datalen = bytes_sent;
    }

    return true;
}

bool UdpCommIO::close() {
    if(sockfd >= 0) {
        close_socket(sockfd); // プラットフォームごとに正しい関数を使用
        sockfd = -1;
    }
    return true;
}

// ===== UdpClientクラス =====
UdpClient::UdpClient() {
    #ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    #endif
    std::memset(&local_addr, 0, sizeof(local_addr));
}

UdpClient::~UdpClient() {
    #ifdef _WIN32
    WSACleanup();
    #endif
}

ICommIO* UdpClient::client_open(IcommEndpointType *src, IcommEndpointType *dst) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        return nullptr;
    }

    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(src->portno);
    inet_pton(AF_INET, src->ipaddr, &(local_addr.sin_addr));

    struct sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(dst->portno);
    inet_pton(AF_INET, dst->ipaddr, &(remote_addr.sin_addr));

    if(bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        close_socket(sockfd);
        return nullptr;
    }

    return new UdpCommIO(sockfd, remote_addr);
}

// ===== UdpServerクラス =====
UdpServer::UdpServer() {
    #ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    #endif
    std::memset(&local_addr, 0, sizeof(local_addr));
}

UdpServer::~UdpServer() {
    #ifdef _WIN32
    WSACleanup();
    #endif
}

ICommIO* UdpServer::server_open(IcommEndpointType *endpoint) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        return nullptr;
    }

    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(endpoint->portno);
    inet_pton(AF_INET, endpoint->ipaddr, &(local_addr.sin_addr));

    if(bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        close_socket(sockfd);
        return nullptr;
    }

    struct sockaddr_in remote_addr;  // このアドレスは、recvfromで設定される
    return new UdpCommIO(sockfd, remote_addr);
}

} // namespace hako::comm
