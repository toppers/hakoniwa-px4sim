#ifndef _UDPCONNECTOR_HPP_
#define _UDPCONNECTOR_HPP_

#include "icomm_connector.hpp"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace hako::comm {

class UdpCommIO : public ICommIO {
private:
#ifdef _WIN32
    SOCKET sockfd; // WindowsではSOCKETを使用
#else
    int sockfd;    // 他のプラットフォームではintを使用
#endif
#ifdef _WIN32
    struct sockaddr_in remote_addr; // リモートのアドレス情報
#else
    struct sockaddr_in remote_addr;
#endif

public:
#ifdef _WIN32
    UdpCommIO(SOCKET sockfd, const sockaddr_in& remote_addr);
#else
    UdpCommIO(int sockfd, const sockaddr_in& remote_addr);
#endif
    ~UdpCommIO() override;

    bool send(const char* data, int datalen, int* send_datalen) override;
    bool recv(char* data, int datalen, int* recv_datalen) override;
    bool close() override;
};

class UdpClient : public ICommClient {
private:
    struct sockaddr_in local_addr; // ローカルのアドレス情報

public:
    UdpClient();
    ~UdpClient() override;

    ICommIO* client_open(IcommEndpointType *src, IcommEndpointType *dst) override;
};

class UdpServer : public ICommServer {
private:
    struct sockaddr_in local_addr; // ローカルのアドレス情報

public:
    UdpServer();
    ~UdpServer() override;

    ICommIO* server_open(IcommEndpointType *endpoint) override;
};

} // namespace hako::px4::comm

#endif /* _UDPCONNECTOR_HPP_ */
