#ifndef _MAVLINK_DUMP_HPP_
#define _MAVLINK_DUMP_HPP_

#include "mavlink.h"
#include "mavlink_msg_types.hpp"

extern bool px4_data_hb_received;
extern bool px4_data_long_received;

extern void mavlink_msg_dump(mavlink_message_t &msg);
extern void mavlink_message_dump(MavlinkDecodedMessage &message);


#endif /* _MAVLINK_DUMP_HPP_ */