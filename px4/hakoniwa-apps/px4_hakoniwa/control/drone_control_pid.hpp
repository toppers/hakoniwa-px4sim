#ifndef _DRONE_CONTROL_PID_HPP_
#define _DRONE_CONTROL_PID_HPP_
#include <px4_platform_common/log.h>

// PID制御パラメータの構造体
typedef struct {
    double Kp; // 比例ゲイン
    double Ki; // 積分ゲイン
    double Kd; // 微分ゲイン
    double previous_error; // 前回のエラー
    double integral; // エラーの積分値
    double setpoint; // 目標値
} PIDController;

// PIDコントローラの初期化
static inline void initPID(PIDController& pid, double Kp, double Ki, double Kd, double setpoint) 
{
    pid.Kp = Kp;
    pid.Ki = Ki;
    pid.Kd = Kd;
    pid.previous_error = 0.0;
    pid.integral = 0.0;
    pid.setpoint = setpoint;
}

// PID更新関数で制御出力を返す
static inline double updatePID(PIDController& pid, double current_value, double dt) 
{
    double error = pid.setpoint - current_value; // エラーの計算
    //PX4_INFO("error = %f", error);
    pid.integral += error * dt; // 積分値の更新
    double derivative = (error - pid.previous_error) / dt; // 微分値の計算
    double output = pid.Kp * error + pid.Ki * pid.integral + pid.Kd * derivative; // 制御出力の計算

    pid.previous_error = error; // エラーの更新

    return output; // 制御出力を返す
}

#endif /* _DRONE_CONTROL_PID_HPP_ */
