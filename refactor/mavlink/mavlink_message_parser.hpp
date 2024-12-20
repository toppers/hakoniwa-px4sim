#ifndef _MAVLINK_MESSAGE_PARSER_HPP_
#define _MAVLINK_MESSAGE_PARSER_HPP_

#include <cstring>
#include <iostream>

namespace hako::mavlink {
#define MAVLINK_HEADER_LEN  9
#define MAVLINK_CRC_LEN  2
#define MAVLINK_SIGNATURE_LEN  1
#define MAVLINK_TOTAL_HEADER_LEN  (MAVLINK_HEADER_LEN + MAVLINK_CRC_LEN + MAVLINK_SIGNATURE_LEN)

class MavLinkMessageParser {
public:
    static int getMessageLength(const char* header) {
        return static_cast<unsigned char>(header[1]) 
                + MAVLINK_CRC_LEN /* CRC */ 
                + MAVLINK_SIGNATURE_LEN /* Signature */;
    }

    static bool verifyCRC(const char* data, int length) {
        //not supported
        (void)data;
        (void)length;
        return true;
    }
    static bool parseMessage(const char* data, int length) {
        if (length < MAVLINK_HEADER_LEN) {
            std::cerr << "Data too short to contain MAVLink header (received " << length << " bytes)" << std::endl;
            return false;
        }

        int packet_length = getMessageLength(data);
        if (length < MAVLINK_HEADER_LEN + packet_length) {
            std::cerr << "Data too short for complete MAVLink message (expected " 
                    << MAVLINK_HEADER_LEN + packet_length 
                    << " bytes, received " << length << " bytes)" << std::endl;
            return false;
        }

        if (!verifyCRC(data, MAVLINK_HEADER_LEN + packet_length)) {
            std::cerr << "CRC verification failed for MAVLink message" << std::endl;
            return false;
        }

        return true;
    }

};

} // namespace hako::comm

#endif /* _MAVLINK_MESSAGE_PARSER_HPP_ */
