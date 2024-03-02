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
        EulerRateType euler_rate = {};
        pid.set_target(target);
        euler_rate.phi =  pid.calculate(angle.phi);
        AngularVelocityType angular_velocity = body_angular_velocity_from_euler_rate(euler_rate, angle);
        return angular_velocity.x;

    }
};

#endif /* _PID_CTRL_YAW_ANGLE_HPP_ */