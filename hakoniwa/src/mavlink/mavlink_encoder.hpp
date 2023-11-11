#ifndef _MAVLINK_ENCODER_HPP_
#define _MAVLINK_ENCODER_HPP_

#include <mavlink.h>
#include "mavlink_msg_types.hpp"
#include "mavlink_config.hpp"

extern int mavlink_get_packet(char* packet, int packet_len, const mavlink_message_t *msg);
extern bool mavlink_encode_message(mavlink_message_t *msg, const MavlinkDecodedMessage *message);

#endif /* _MAVLINK_ENCODER_HPP_ */
