#ifndef _PID_CTRL_PITCH_RATE_HPP_
#define _PID_CTRL_PITCH_RATE_HPP_

#include "control/simple_pid.hpp"
#include "pid_params.h"
#include "frame_types.h"

class PidCtrlPitchRate {
private:
    FlightControlPID pid;
public:
    PidCtrlPitchRate() : pid(PITCH_VEL_Kp, PITCH_VEL_Ki, PITCH_VEL_Kd, 0)
    {
    }
    double run(double target_vel, double current_vel)
    {
        pid.set_target(target_vel);
        return pid.calculate(current_vel);
    }
};

#endif /* _PID_CTRL_PITCH_RATE_HPP_ */