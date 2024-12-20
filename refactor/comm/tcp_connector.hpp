#ifndef _TCPCONNECTOR_HPP_
#define _TCPCONNECTOR_HPP_

#include "icomm_connector.hpp"

namespace hako::comm {

class TcpCommIO : public ICommIO {
private:
#ifdef _WIN32
    SOCKET sockfd; // WindowsではSOCKETを使用
#else
    int sockfd;    // 他のプラットフォームではintを使用
#endif
public:
    TcpCommIO(int sockfd);
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
