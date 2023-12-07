#ifndef _SIMPLE_PID_HPP_
#define _SIMPLE_PID_HPP_

#include <iostream>
#define SIMPLE_PID_INUM 10

class PID {
private:
    double Kp; // 比例ゲイン
    double Ki; // 積分ゲイン
    double Kd; // 微分ゲイン

    double setpoint; // 目標値
    double integral; // 積分値
    double prev_error; // 前回の誤差
    bool first_time; // 初回フラグ

    int i_inx = 0;
    int i_num = 0;
    double i_values[SIMPLE_PID_INUM];
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
        double integral = 0;
        for (int i = 0; i < i_num; i++) {
            integral += i_values[i];
        }
        //integral = integral / i_num;
        return Kp * error + Ki * integral + Kd * derivative;
    }

    // インテグラル値のリセット
    void reset_integral() {
        integral = 0.0;
    }
};


#endif /* _SIMPLE_PID_HPP_ */