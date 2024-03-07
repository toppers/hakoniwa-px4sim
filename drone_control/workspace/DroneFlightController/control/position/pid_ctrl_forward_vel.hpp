#ifndef _PID_CTRL_FORWARD_VEL_HPP_
#define _PID_CTRL_FORWARD_VEL_HPP_

#include "control/simple_pid.hpp"
#include "pid_params.h"
#include "primitive_types.h"

class PidCtrlForwardVel {
private:
    FlightControlPID pidF;
    FlightControlPID pidH;
public:
    PidCtrlForwardVel() : pidF(FORWARD_VEL_Kp, FORWARD_VEL_Ki, FORWARD_VEL_Kd, 0), pidH(FORWARD_VEL_Kp, FORWARD_VEL_Ki, FORWARD_VEL_Kd, 0)
    {
    }
    double runF(double target_vel, double current_vel)
    {
        pidF.set_target(target_vel);
        return pidF.calculate(current_vel);
    }
    double runH(double target_vel, double current_vel)
    {
        pidH.set_target(target_vel);
        return pidH.calculate(current_vel);
    }
};

#endif /* _PID_CTRL_FORWARD_VEL_HPP_ */