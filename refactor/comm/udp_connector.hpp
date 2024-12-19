#ifndef _UDPCONNECTOR_HPP_
#define _UDPCONNECTOR_HPP_

#include "icomm_connector.hpp"
#include <sys/socket.h>
#include <netinet/in.h>

namespace hako::comm {

class UdpCommIO : public ICommIO {
private:
    int sockfd; // ソケットのディスクリプタ
    struct sockaddr_in remote_addr; // リモートのアドレス情報

public:
    UdpCommIO(int sockfd, const sockaddr_in& remote_addr);
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
