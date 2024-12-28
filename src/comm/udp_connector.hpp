#ifndef _UDPCONNECTOR_HPP_
#define _UDPCONNECTOR_HPP_

#include "icomm_connector.hpp"
#include <mutex>
namespace hako::comm {

class UdpCommIO : public ICommIO {
private:
    ICOMM_SOCKET sockfd;
    std::mutex mtx_;
    struct sockaddr_in remote_addr_ = {};

public:
    UdpCommIO(ICOMM_SOCKET sockfd) : sockfd(sockfd) {
    }
    UdpCommIO(ICOMM_SOCKET sockfd, const sockaddr_in& remote_addr);
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

}

#endif /* _UDPCONNECTOR_HPP_ */
