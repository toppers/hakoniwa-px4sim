#ifndef _PID_CTRL_YAW_RATE_HPP_
#define _PID_CTRL_YAW_RATE_HPP_

#include "control/simple_pid.hpp"
#include "pid_params.h"
#include "primitive_types.h"

class PidCtrlYawRate {
private:
    FlightControlPID pid;
public:
    PidCtrlYawRate() : pid(YAW_VEL_Kp, YAW_VEL_Ki, YAW_VEL_Kd, 0)
    {
    }
    double run(double target_vel, double current_vel)
    {
        pid.set_target(target_vel);
        return pid.calculate(current_vel);
    }
};

#endif /* _PID_CTRL_YAW_RATE_HPP_ */