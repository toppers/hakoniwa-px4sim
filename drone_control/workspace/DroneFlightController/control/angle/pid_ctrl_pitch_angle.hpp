#ifndef _PID_CTRL_PITCH_ANGLE_HPP_
#define _PID_CTRL_PITCH_ANGLE_HPP_

#include "control/simple_pid.hpp"
#include "pid_params.h"
#include "utils/frame_convertor.hpp"
#include "primitive_types.h"

class PidCtrlPitchAngle {
private:
    FlightControlPID pid;
public:
    PidCtrlPitchAngle() : pid(PITCH_ANGLE_Kp, PITCH_ANGLE_Ki, PITCH_ANGLE_Kd, 0)
    {
    }
    double run(double target, const EulerType& angle)
    {
        EulerRateType euler_rate = {};
        pid.set_target(target);
        euler_rate.theta =  pid.calculate(angle.theta);
        AngularVelocityType angular_velocity = body_angular_velocity_from_euler_rate(euler_rate, angle);
        return angular_velocity.y;
    }
};

#endif /* _PID_CTRL_PITCH_ANGLE_HPP_ */