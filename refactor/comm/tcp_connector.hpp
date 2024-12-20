#ifndef _TCPCONNECTOR_HPP_
#define _TCPCONNECTOR_HPP_

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

class TcpCommIO : public ICommIO {
private:
#ifdef _WIN32
    SOCKET sockfd; // WindowsではSOCKETを使用
#else
    int sockfd;    // 他のプラットフォームではintを使用
#endif
public:
#ifdef _WIN32
    TcpCommIO(SOCKET sockfd);
#else
    TcpCommIO(int sockfd);
#endif
    ~TcpCommIO() override;

    bool send(const char* data, int datalen, int* send_datalen) override;
    bool recv(char* data, int datalen, int* recv_datalen) override;
    bool close() override;
};

class TcpClient : public ICommClient {
private:

public:
    TcpClient();
    ~TcpClient() override;

    ICommIO* client_open(IcommEndpointType *src, IcommEndpointType *dst) override;
};

class TcpServer : public ICommServer {
private:

public:
    TcpServer();
    ~TcpServer() override;

    ICommIO* server_open(IcommEndpointType *endpoint) override;
};

extern int comm_init();

} // namespace hako::px4::comm

#endif /* _TCPCONNECTOR_HPP_ */
