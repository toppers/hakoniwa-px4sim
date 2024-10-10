#ifndef _PID_CONTROL_HPP_
#define _PID_CONTROL_HPP_

#include <vector>
#include <iostream>

class DronePidControl {
private:
    double Kp;
    double Ki;
    double Kd;

    double target;
    double integral;
    double prev_error;
    bool first_time;
    double delta_time;

public:
    DronePidControl(double Kp, double Ki, double Kd, double sp, double dt)
        : Kp(Kp), Ki(Ki), Kd(Kd), target(sp), integral(0.0), prev_error(0.0), first_time(true), delta_time(dt)
    {
    }
    ~DronePidControl() {}

    double calculate(double sp, double input) {
        target = sp;
        double error = target - input;
        integral += error * delta_time;

        double derivative = (first_time) ? 0.0 : (error - prev_error) / delta_time;
        first_time = false;
        prev_error = error;
        return Kp * error + Ki * integral + Kd * derivative;
    }

    void reset_integral() {
        integral = 0.0;
    }
};

#endif /* _PID_CONTROL_HPP_ */