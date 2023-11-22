#ifndef _MAVLINK_CAPTURE_HPP_
#define _MAVLINK_CAPTURE_HPP_

#include "mavlink_msg_types.hpp"


extern bool mavlink_capture_create_controller(MavlinkCaptureControllerType &controller, const char* filepath);
typedef enum {
    MAVLINK_CAPTURE_DATA_OWNER_CONTROL = 0,
    MAVLINK_CAPTURE_DATA_OWNER_PHYSICS
} MavlinkCaptureDataOwnerType;
extern bool mavlink_capture_append_data(MavlinkCaptureControllerType &controller, MavlinkCaptureDataOwnerType owner, uint32_t dataLength, const uint8_t  *data);
extern bool mavlink_capture_save(MavlinkCaptureControllerType &controller);

#endif /* _MAVLINK_CAPTURE_HPP_ */