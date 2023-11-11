#ifndef _TCPCONNECTOR_HPP_
#define _TCPCONNECTOR_HPP_

#include "icomm_connector.hpp"
#include <sys/socket.h>
#include <netinet/in.h>

namespace hako::px4::comm {

class TcpCommIO : public ICommIO {
private:
    int sockfd; // ソケットのディスクリプタ

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

} // namespace hako::px4::comm

#endif /* _TCPCONNECTOR_HPP_ */
