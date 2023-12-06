#ifndef _SIMPLE_PID_HPP_
#define _SIMPLE_PID_HPP_

#include <iostream>

class PID {
private:
    double Kp; // 比例ゲイン
    double Ki; // 積分ゲイン
    double Kd; // 微分ゲイン

    double setpoint; // 目標値
    double integral; // 積分値
    double prev_error; // 前回の誤差
    bool first_time; // 初回フラグ

public:
    // コンストラクタ
    PID(double Kp, double Ki, double Kd, double setpoint)
        : Kp(Kp), Ki(Ki), Kd(Kd), setpoint(setpoint), integral(0.0), prev_error(0.0), first_time(true) {}

    // 目標値を設定
    void set_setpoint(double sp) {
        setpoint = sp;
    }

    // PID制御を行うメソッド
    double calculate(double input) {
        double error = setpoint - input;
        integral += error;
        double derivative = (first_time) ? 0.0 : error - prev_error;
        first_time = false;
        prev_error = error;

        return Kp * error + Ki * integral + Kd * derivative;
    }

    // インテグラル値のリセット
    void reset_integral() {
        integral = 0.0;
    }
};


#endif /* _SIMPLE_PID_HPP_ */