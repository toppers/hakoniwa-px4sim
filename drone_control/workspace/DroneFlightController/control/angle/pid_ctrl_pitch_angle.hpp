#ifndef _PID_CTRL_PITCH_ANGLE_HPP_
#define _PID_CTRL_PITCH_ANGLE_HPP_

#include "control/simple_pid.hpp"
#include "pid_params.h"
#include "utils/frame_convertor.hpp"
#include "primitive_types.h"

class PidCtrlRollAngle {
private:
    PID pid;
public:
    PidCtrlPitchAngle() : pid(PITCH_ANGLE_Kp, PITCH_ANGLE_Ki, PITCH_ANGLE_Kd, 0)
    {
    }
    double run(double target, const EulerType& angle)
    {
        pid.set_target(target);
        return pid.calculate(angle.theta);
    }
};

#endif /* _PID_CTRL_PITCH_ANGLE_HPP_ */