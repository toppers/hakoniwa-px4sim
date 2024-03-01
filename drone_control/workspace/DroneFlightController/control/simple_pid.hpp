#ifndef _SIMPLE_PID_HPP_
#define _SIMPLE_PID_HPP_

#include <iostream>
#define SIMPLE_PID_INUM 10

class PID {
private:
    double Kp;
    double Ki;
    double Kd;

    double setpoint;
    double integral;
    double prev_error;
    bool first_time;

    int i_inx = 0;
    int i_num = 0;
    double i_values[SIMPLE_PID_INUM];
public:
    PID(double Kp, double Ki, double Kd, double setpoint)
        : Kp(Kp), Ki(Ki), Kd(Kd), setpoint(setpoint), integral(0.0), prev_error(0.0), first_time(true) 
    {
        for (int i = 0; i < SIMPLE_PID_INUM; i++) {
            i_values[i] = 0;
        }
    }

    void set_setpoint(double sp) {
        setpoint = sp;
    }

    double calculate(double input) {
        double error = setpoint - input;
        i_values[i_inx++] = error;

        double derivative = (first_time) ? 0.0 : error - prev_error;
        first_time = false;
        prev_error = error;

        if (i_num < SIMPLE_PID_INUM) {
            i_num++;
        }
        if (i_inx >= SIMPLE_PID_INUM) {
            i_inx = 0;
        }
        double _integral = 0;
        for (int i = 0; i < i_num; i++) {
            _integral += i_values[i];
        }
        return Kp * error + Ki * _integral + Kd * derivative;
    }

    void reset_integral() {
        integral = 0.0;
    }
};

#endif /* _SIMPLE_PID_HPP_ */