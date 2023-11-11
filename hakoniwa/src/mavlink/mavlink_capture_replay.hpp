#ifndef _MAVLINK_CAPTURE_REPLAY_HPP_
#define _MAVLINK_CAPTURE_REPLAY_HPP_

#include "mavlink_msg_types.hpp"

extern bool mavlink_capture_load_controller(MavlinkCaptureControllerType &controller, const char* filepath);
extern bool mavlink_capture_load_data(MavlinkCaptureControllerType &controller, uint32_t dataLength, uint8_t  *data, uint32_t *r_dataLength, uint64_t *timestamp);
extern bool mavlink_set_timestamp_for_replay_data(MavlinkDecodedMessage &message, uint64_t time_usec);

#endif /* _MAVLINK_CAPTURE_REPLAY_HPP_ */