#ifndef _MAVLINK_COMM_UDP_HPP_
#define _MAVLINK_COMM_UDP_HPP_

#include "imavlink_comm.hpp"
#include "mavlink_message_parser.hpp"

using namespace hako::comm;
namespace hako::mavlink {

class MavLinkCommUdp : public IMavLinkComm {
public:
    bool receiveMessage(ICommIO* io, char* data, int datalen, int* recv_datalen) override {
        if (!io || !data || datalen <= 0 || !recv_datalen) {
            std::cerr << "Invalid arguments to receiveMessage (UDP)" << std::endl;
            return false;
        }

        int len = 0;
        if (!io->recv(data, datalen, &len)) {
            return false;
        }
        int packet_length = MavLinkMessageParser::getMessageLength(header);

        if (!MavLinkMessageParser::parseMessage(data, len)) {
            return false;
        }

        *recv_datalen = MAVLINK_HEADER_LEN + packet_length;
        return true;
    }


    bool sendMessage(ICommIO* io, const char* data, int datalen) override {
        return io && io->send(data, datalen, nullptr);
    }
};

} // namespace hako::comm

#endif /* _MAVLINK_COMM_UDP_HPP_ */
