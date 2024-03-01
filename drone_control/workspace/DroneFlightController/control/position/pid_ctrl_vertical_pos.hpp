#ifndef _PID_CTRL_VERTICAL_POS_HPP_
#define _PID_CTRL_VERTICAL_POS_HPP_

#include "control/simple_pid.hpp"
#include "pid_params.h"
#include "utils/frame_convertor.hpp"
#include "primitive_types.h"

class PidCtrlVerticalPos {
private:
    PID pid;
public:
    PidCtrlVerticalPos() : pid(VERTICAL_POS_Kp, VERTICAL_POS_Ki, VERTICAL_POS_Kd, 0)
    {
    }
    double run(double target, const double current_pos, const EulerType& angle)
    {
        VelocityType target_velocity_ground_frame = {};
        VelocityType target_velocity_body_frame;
        pid.set_target(target);
        target_velocity_ground_frame.z =  pid.calculate(current_pos);
        target_velocity_body_frame = body_vector_from_ground(target_velocity_ground_frame, angle);
        return target_velocity_body_frame.z;
    }
};

#endif /* _PID_CTRL_VERTICAL_POS_HPP_ */