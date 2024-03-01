#ifndef _PID_CTRL_ROLL_ANGLE_HPP_
#define _PID_CTRL_ROLL_ANGLE_HPP_

#include "control/simple_pid.hpp"
#include "pid_params.h"
#include "utils/frame_convertor.hpp"
#include "primitive_types.h"

class PidCtrlRollAngle {
private:
    PID pid;
public:
    PidCtrlRollAngle() : pid(ROLL_ANGLE_Kp, ROLL_ANGLE_Ki, ROLL_ANGLE_Kd, 0)
    {
    }
    double run(double target, const EulerType& angle)
    {
        pid.set_target(target);
        return pid.calculate(angle.phi);
    }
};

#endif /* _PID_CTRL_YAW_ANGLE_HPP_ */