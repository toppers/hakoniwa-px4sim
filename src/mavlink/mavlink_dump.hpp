#ifndef _MAVLINK_DUMP_HPP_
#define _MAVLINK_DUMP_HPP_

#include "mavlink.h"
#include "mavlink_msg_types.hpp"


namespace hako::mavlink {
class MavlinkDump {
public:
    static void mavlink_header_dump(mavlink_message_t &msg);
    static void mavlink_message_dump(MavlinkDecodedMessage &message);
};
} // namespace hako::mavlink

#endif /* _MAVLINK_DUMP_HPP_ */