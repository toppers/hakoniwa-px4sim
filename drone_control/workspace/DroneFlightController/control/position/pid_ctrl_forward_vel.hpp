#ifndef _PID_CTRL_FORWARD_VEL_HPP_
#define _PID_CTRL_FORWARD_VEL_HPP_

#include "control/simple_pid.hpp"
#include "pid_params.h"
#include "primitive_types.h"

class PidCtrlForwardVel {
private:
    FlightControlPID pidVel;
    FlightControlPID pidAngleF;
    FlightControlPID pidAngleH;
public:
    PidCtrlForwardVel() : 
        pidVel(FORWARD_VEL_Kp, FORWARD_VEL_Ki, FORWARD_VEL_Kd, 0), 
        pidAngleF(FORWARD_ANGLE_Kp, FORWARD_ANGLE_Ki, FORWARD_ANGLE_Kd, 0), 
        pidAngleH(FORWARD_ANGLE_Kp, FORWARD_ANGLE_Ki, FORWARD_ANGLE_Kd, 0)
    {
    }
    double runV(double target_vel, double current_vel)
    {
        pidVel.set_target(target_vel);
        return pidVel.calculate(current_vel);
    }
    double runF(double target_vel, double current_vel)
    {
        pidAngleF.set_target(target_vel);
        return pidAngleF.calculate(current_vel);
    }
    double runH(double target_vel, double current_vel)
    {
        pidAngleH.set_target(target_vel);
        return pidAngleH.calculate(current_vel);
    }
};

#endif /* _PID_CTRL_FORWARD_VEL_HPP_ */