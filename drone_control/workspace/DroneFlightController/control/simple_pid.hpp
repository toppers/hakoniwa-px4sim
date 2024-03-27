#ifndef _SIMPLE_FLIGHT_CONTROL_PID_HPP_
#define _SIMPLE_FLIGHT_CONTROL_PID_HPP_
#include <vector>
#include <iostream>

class FlightControlPID {
private:
    double Kp;
    double Ki;
    double Kd;

    double target;
    double integral;
    double prev_error;
    bool first_time;

public:
    FlightControlPID(double Kp, double Ki, double Kd, double sp)
        : Kp(Kp), Ki(Ki), Kd(Kd), target(sp), integral(0.0), prev_error(0.0), first_time(true)
    {
    }

    void set_target(double sp) {
        target = sp;
    }

    double calculate(double input) {
        double error = target - input;
        integral += error * 0.003;

        double derivative = (first_time) ? 0.0 : error - prev_error;
        first_time = false;
        prev_error = error;

        return Kp * error + Ki * integral + Kd * derivative;
    }

    void reset_integral() {
        integral = 0.0;
    }
};

#endif /* _SIMPLE_FLIGHT_CONTROL_PID_HPP_ */