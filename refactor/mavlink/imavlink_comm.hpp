#ifndef _IMAVLINK_COMM_HPP_
#define _IMAVLINK_COMM_HPP_

#include "icomm_connector.hpp"

using namespace hako::comm;
namespace hako::mavlink {

class IMavLinkComm {
public:
    virtual ~IMavLinkComm() = default;

    // MAVLinkメッセージを受信
    virtual bool receiveMessage(ICommIO* io, char* data, int datalen, int* recv_datalen) = 0;

    // MAVLinkメッセージを送信
    virtual bool sendMessage(ICommIO* io, const char* data, int datalen) = 0;
};

} // namespace hako::comm

#endif /* _IMAVLINK_COMM_HPP_ */
