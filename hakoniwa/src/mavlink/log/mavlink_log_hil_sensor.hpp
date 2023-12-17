#ifndef _MAVLINK_LOG_HIL_SENSOR_HPP_
#define _MAVLINK_LOG_HIL_SENSOR_HPP_


#include "utils/icsv_log.hpp"
#include "utils/csv_logger.hpp"
#include "mavlink.h"
#include <iostream>

namespace hako::assets::drone::mavlink::log {

class MavlinkLogHilSensor: public ICsvLog {
private:
    mavlink_hil_sensor_t msg;
public:
    MavlinkLogHilSensor(){}
    virtual ~MavlinkLogHilSensor() {}
    void set_data(const mavlink_hil_sensor_t& message)
    {
        msg = message;
    }
    const std::vector<std::string> log_head() override
    {
        return { "TIME", 
                 "xacc", "yacc", "zacc", 
                 "xgyro", "ygyro", "zgyro", 
                 "xmag", "ymag", "zmag", 
                 "abs_pressure", "diff_pressure", "pressure_alt", 
                 "temperature"
        };
    }
    const std::vector<std::string> log_data() override
    {
        return {
            std::to_string(CsvLogger::get_time_usec()), 
            std::to_string(msg.xacc), std::to_string(msg.yacc), std::to_string(msg.zacc),
            std::to_string(msg.xgyro), std::to_string(msg.ygyro), std::to_string(msg.zgyro),
            std::to_string(msg.xmag), std::to_string(msg.ymag), std::to_string(msg.zmag),
            std::to_string(msg.abs_pressure), std::to_string(msg.diff_pressure), std::to_string(msg.pressure_alt),
            std::to_string(msg.temperature)
        };
    }

};
}


#endif /* _MAVLINK_LOG_HIL_SENSOR_HPP_ */