#ifndef _UDPCONNECTOR_HPP_
#define _UDPCONNECTOR_HPP_

#include "icomm_connector.hpp"

// プラットフォームごとのヘッダ
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib") // Winsockライブラリをリンク
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace hako::comm {

class UdpCommIO : public ICommIO {
private:
    int sockfd; // ソケットのディスクリプタ
#ifdef _WIN32
    struct sockaddr_in remote_addr; // リモートのアドレス情報
#else
    struct sockaddr_in remote_addr;
#endif

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
