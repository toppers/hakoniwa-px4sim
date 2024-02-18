#ifndef _SAMPLE_CONTROLLER_HPP_
#define _SAMPLE_CONTROLLER_HPP_

#include "icontroller.hpp"
#include "utils/simple_pid.hpp"

namespace hako::assets::drone {

class SampleController: public IController {
private:
    PID *pid_pos_x;
    PID *pid_pos_y;
    PID *pid_pos_z;
    PID *pid_euler_x;
    PID *pid_euler_y;
    PID *pid_euler_z;
    double hovering_thrust;
    double hovering_thrust_range;
    double get_limit_value(double input_value, double base_value, double min_value, double max_value)
    {
        double limited_value = base_value + std::max(min_value, std::min(max_value, input_value));
        return limited_value;
    }

public:
    SampleController(int index);
    virtual ~SampleController() {}
    virtual mi_drone_control_out_t run(mi_drone_control_in_t &in) override;
};
}


#endif /* _SAMPLE_CONTROLLER_HPP_ */