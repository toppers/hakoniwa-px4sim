#ifndef _MAVLINK_COMM_TCP_HPP_
#define _MAVLINK_COMM_TCP_HPP_

#include "imavlink_comm.hpp"
#include "mavlink_comm_message_parser.hpp"
#include <vector>

using namespace hako::comm;

namespace hako::mavlink {

class MavLinkCommTcp : public IMavLinkComm {
private:

public:
    MavLinkCommTcp() {
    }

    bool receiveMessage(ICommIO* io, char* data, int datalen, int* recv_datalen) override {
        if (!io || !data || datalen <= 0 || !recv_datalen) {
            std::cerr << "Invalid arguments to receiveMessage (TCP)" << std::endl;
            return false;
        }
        char header[MAVLINK_HEADER_LEN];
        int len = 0;
        if (!io->recv(header, MAVLINK_HEADER_LEN, &len)) {
            return false;
        }

        int packet_length = MavLinkCommMessageParser::getMessageLength(header);
        // Check if datalen is sufficient to hold header and packet data
        if (datalen < MAVLINK_HEADER_LEN + packet_length) {
            std::cerr << "Provided data buffer is too small to hold the MAVLink message." << std::endl;
            return false;
        }
        // Copy header data to output buffer
        memcpy(data, header, MAVLINK_HEADER_LEN);
        if (!io->recv(&data[MAVLINK_HEADER_LEN], packet_length, &len)) {
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

#endif /* _MAVLINK_COMM_TCP_HPP_ */
