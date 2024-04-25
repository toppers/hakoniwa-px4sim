#ifndef _PID_CTRL_VERTICAL_VEL_HPP_
#define _PID_CTRL_VERTICAL_VEL_HPP_

#include "control/simple_pid.hpp"
#include "pid_params.h"
#include "frame_types.h"

class PidCtrlVerticalVel {
private:
    FlightControlPID pid;
public:
    PidCtrlVerticalVel() : pid(VERTICAL_VEL_Kp, VERTICAL_VEL_Ki, VERTICAL_VEL_Kd, 0)
    {
    }
    double run(double target_vel, double current_vel)
    {
        pid.set_target(target_vel);
        return pid.calculate(current_vel);
    }
};

#endif /* _PID_CTRL_VERTICAL_VEL_HPP_ */