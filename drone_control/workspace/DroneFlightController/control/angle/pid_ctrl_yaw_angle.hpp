#ifndef _PID_CTRL_YAW_ANGLE_HPP_
#define _PID_CTRL_YAW_ANGLE_HPP_

#include "control/simple_pid.hpp"
#include "pid_params.h"
#include "utils/frame_convertor.hpp"
#include "primitive_types.h"

class PidCtrlYawAngle {
private:
    PID pid;
public:
    PidCtrlYawAngle() : pid(YAW_ANGLE_Kp, YAW_ANGLE_Ki, YAW_ANGLE_Kd, 0)
    {
    }
    double run(double target, const EulerType& angle)
    {
        EulerRateType euler_rate = {};
        pid.set_target(target);
        euler_rate.psi =  pid.calculate(angle.psi);
        AngularVelocityType angular_velocity = body_angular_velocity_from_euler_rate(euler_rate, angle);
        return angular_velocity.z;
    }
};

#endif /* _PID_CTRL_YAW_ANGLE_HPP_ */