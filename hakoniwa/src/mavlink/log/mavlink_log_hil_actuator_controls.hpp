#ifndef _MAVLINK_LOG_HIL_ACTUATOR_CONTROLS_HPP_
#define _MAVLINK_LOG_HIL_ACTUATOR_CONTROLS_HPP_


#include "utils/icsv_log.hpp"
#include "mavlink.h"
#include <iostream>
#include "utils/csv_logger.hpp"

namespace hako::assets::drone::mavlink::log {

class MavlinkLogHilActuatorControls: public ICsvLog {
private:
    mavlink_hil_actuator_controls_t msg;
public:
    MavlinkLogHilActuatorControls(){}
    virtual ~MavlinkLogHilActuatorControls() {}
    void set_data(const mavlink_hil_actuator_controls_t& message)
    {
        msg = message;
    }
    const std::vector<std::string> log_head() override
    {
        return { "TIME", 
                 "mode", "flags", 
                 "controls[0]", "controls[1]", "controls[2]", "controls[3]", 
                 "controls[4]", "controls[5]", "controls[6]", "controls[7]",
                 "controls[8]", "controls[9]", "controls[10]", "controls[11]",
                 "controls[12]", "controls[13]", "controls[14]", "controls[15]", 
        };
    }
    const std::vector<std::string> log_data() override
    {
        return {
            std::to_string(CsvLogger::get_time_usec()), 
            std::to_string(msg.mode), std::to_string(msg.flags), 
            std::to_string(msg.controls[0]), std::to_string(msg.controls[1]), std::to_string(msg.controls[2]), std::to_string(msg.controls[3]),
            std::to_string(msg.controls[4]), std::to_string(msg.controls[5]), std::to_string(msg.controls[6]), std::to_string(msg.controls[7]),
            std::to_string(msg.controls[8]), std::to_string(msg.controls[9]), std::to_string(msg.controls[10]), std::to_string(msg.controls[11]),
            std::to_string(msg.controls[12]), std::to_string(msg.controls[13]), std::to_string(msg.controls[14]), std::to_string(msg.controls[15])
        };
    }

};
}


#endif /* _MAVLINK_LOG_HIL_ACTUATOR_CONTROLS_HPP_ */