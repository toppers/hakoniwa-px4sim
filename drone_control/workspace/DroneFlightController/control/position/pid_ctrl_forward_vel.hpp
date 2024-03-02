#ifndef _PID_CTRL_FORWARD_VEL_HPP_
#define _PID_CTRL_FORWARD_VEL_HPP_

#include "control/simple_pid.hpp"
#include "pid_params.h"
#include "primitive_types.h"

class PidCtrlForwardVel {
private:
    PID pid;
public:
    PidCtrlForwardVel() : pid(FORWARD_VEL_Kp, FORWARD_VEL_Ki, FORWARD_VEL_Kd, 0)
    {
    }
    double run(double target_vel, double current_vel)
    {
        pid.set_target(target_vel);
        return pid.calculate(current_vel);
    }
};

#endif /* _PID_CTRL_FORWARD_VEL_HPP_ */