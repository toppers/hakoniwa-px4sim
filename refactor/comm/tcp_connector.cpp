#include "tcp_connector.hpp"
#include <iostream>
#include <cstring>
#include <errno.h>
#include <string.h>

#define MAX_ATTEMPTS 600
#define RETRY_INTERVAL 3

namespace hako::comm {


#ifdef WIN32
int comm_init()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return -1;
    }
    return 0;
}
#else
int comm_init()
{
    return 0;
}
#endif

TcpCommIO::TcpCommIO(ICOMM_SOCKET sockfd) : sockfd(sockfd) {}

TcpCommIO::~TcpCommIO() {
    close();
}

TcpClient::TcpClient() {}

TcpClient::~TcpClient() {}

#ifdef WIN32
ICommIO* TcpClient::client_open(IcommEndpointType*, IcommEndpointType* dst) {
    ICOMM_SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return nullptr;
    }

    struct sockaddr_in remote_addr;
    ZeroMemory(&remote_addr, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    inet_pton(AF_INET, dst->ipaddr, &remote_addr.sin_addr);
    remote_addr.sin_port = (u_short)htons(dst->portno);

    int attempt = 0;
    while (connect(sockfd, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) == SOCKET_ERROR) {
        if (++attempt >= MAX_ATTEMPTS) {
            std::cerr << "Failed to connect after " << MAX_ATTEMPTS << " attempts: " << WSAGetLastError() << std::endl;
            closesocket(sockfd);
            WSACleanup();
            return nullptr;
        }

        std::cout << "Connection attempt " << attempt << " failed, retrying..." << std::endl;
        Sleep(RETRY_INTERVAL * 1000);
    }
    return new TcpCommIO(sockfd);
}
#else
ICommIO* TcpClient::client_open(IcommEndpointType *src, IcommEndpointType *dst) {
    ICOMM_SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
#endif


TcpServer::TcpServer() {}

TcpServer::~TcpServer() {}

#ifdef WIN32
ICommIO* TcpServer::server_open(IcommEndpointType* endpoint) {

    ICOMM_SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return nullptr;
    }

    char optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == SOCKET_ERROR) {
        std::cerr << "Failed to set SO_REUSEADDR: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return nullptr;
    }

    struct sockaddr_in local_addr;
    ZeroMemory(&local_addr, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    inet_pton(AF_INET, endpoint->ipaddr, &local_addr.sin_addr);
    local_addr.sin_port = htons(endpoint->portno);

    if (bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return nullptr;
    }

    if (listen(sockfd, 1) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on socket: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return nullptr;
    }

    struct sockaddr_in remote_addr;
    int addr_len = sizeof(remote_addr);
    SOCKET client_sockfd = accept(sockfd, (struct sockaddr*)&remote_addr, &addr_len);
    if (client_sockfd == INVALID_SOCKET) {
        std::cerr << "Failed to accept connection: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return nullptr;
    }
    return new TcpCommIO(client_sockfd);
}
#else
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
#endif

#define MAVLINK_HEADER_LEN  9
#ifdef WIN32
bool TcpCommIO::recv(char* data, int datalen, int* recv_datalen) {
    char header[MAVLINK_HEADER_LEN];
    int received = 0;

    // Receive header
    while (received < MAVLINK_HEADER_LEN) {
        int len = ::recv(sockfd, header + received, MAVLINK_HEADER_LEN - received, 0);
        if (len > 0) {
            received += len;
        }
        else if (len == 0 || (WSAGetLastError() != WSAEWOULDBLOCK)) {
            std::cerr << "Failed to receive MAVLink header." << std::endl;
            return false;
        }
    }
    // Parse header to get packet length (assuming packet length is at offset 1)
    int packetlen = static_cast<unsigned char>(header[1]) + 2 /* CRC */ + 1 /* Signature */;

    // Check if datalen is sufficient to hold header and packet data
    if (datalen < MAVLINK_HEADER_LEN + packetlen) {
        std::cerr << "Provided data buffer is too small to hold the MAVLink message." << std::endl;
        return false;
    }

    // Copy header data to output buffer
    memcpy(data, header, MAVLINK_HEADER_LEN);

    // Receive packet data
    received = 0;
    while (received < packetlen) {
        int len = ::recv(sockfd, data + MAVLINK_HEADER_LEN + received, packetlen - received, 0);
        if (len > 0) {
            received += len;
        }
        else if (len == 0 || (WSAGetLastError() != WSAEWOULDBLOCK)) {
            std::cerr << "Failed to receive MAVLink data." << std::endl;
            return false;
        }
    }

    *recv_datalen = MAVLINK_HEADER_LEN + packetlen;
    return true;
}

#else
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
#endif

#ifdef WIN32
bool TcpCommIO::send(const char* data, int datalen, int* send_datalen) {
    int total_sent = 0;
    while (total_sent < datalen) {
        int sent = ::send(sockfd, data + total_sent, datalen - total_sent, 0);
        if (sent > 0) {
            total_sent += sent;
        }
        else if (sent == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
            std::cerr << "Failed to send data: " << WSAGetLastError() << std::endl;
            return false;
        }
    }
    *send_datalen = total_sent;
    return total_sent == datalen;
}

#else
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
#endif

#ifdef WIN32
bool TcpCommIO::close() {
    if (closesocket(sockfd) == SOCKET_ERROR) {
        std::cerr << "Failed to close socket: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

#else
bool TcpCommIO::close() {
    if (::close(sockfd) < 0) {
        std::cout << "Failed to close socket: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}
#endif

}  // namespace hako::px4::comm
