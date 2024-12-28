#ifndef _MAVLINK_DECODER_HPP_
#define _MAVLINK_DECODER_HPP_

#include <mavlink.h>
#include "mavlink_msg_types.hpp"
#include "mavlink_config.hpp"

extern bool mavlink_decode(uint8_t chan, const char* packet, int packet_len, mavlink_message_t *msg);
extern bool mavlink_get_message(mavlink_message_t *msg, MavlinkDecodedMessage *message);

#endif /* _MAVLINK_DECODER_HPP_ */
