#ifndef _MAVLINK_LOG_HIL_GPS_HPP_
#define _MAVLINK_LOG_HIL_GPS_HPP_


#include "utils/icsv_log.hpp"
#include "utils/csv_logger.hpp"
#include "mavlink.h"
#include <iostream>

namespace hako::assets::drone::mavlink::log {

class MavlinkLogHilGps: public ICsvLog {
private:
    mavlink_hil_gps_t msg;
public:
    MavlinkLogHilGps(){}
    virtual ~MavlinkLogHilGps() {}
    void set_data(const mavlink_hil_gps_t& message)
    {
        msg = message;
    }
    const std::vector<std::string> log_head() override
    {
        return { "TIME", 
                 "lat", "lon", "alt", 
                 "eph", "epv", "vel", 
                 "vn", "ve", "vd", 
                 "cog", "satellites_visible", "id", 
                 "yaw"
        };
    }
    const std::vector<std::string> log_data() override
    {
        return {
            std::to_string(CsvLogger::get_time_usec()), 
            std::to_string(msg.lat), std::to_string(msg.lon), std::to_string(msg.alt),
            std::to_string(msg.eph), std::to_string(msg.epv), std::to_string(msg.vel),
            std::to_string(msg.vn), std::to_string(msg.ve), std::to_string(msg.vd),
            std::to_string(msg.cog), std::to_string(msg.satellites_visible), std::to_string(msg.id),
            std::to_string(msg.yaw)
        };
    }

};
}


#endif /* _MAVLINK_LOG_HIL_GPS_HPP_ */