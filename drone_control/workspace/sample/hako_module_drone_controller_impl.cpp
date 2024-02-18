#include "hako_module_drone_controller_impl.h"

#include <iostream>
#define SIMPLE_PID_INUM 10
class SamplePID {
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
    SamplePID(double Kp, double Ki, double Kd, double setpoint)
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
        return Kp * error + Ki * integral + Kd * derivative;
    }

    // インテグラル値のリセット
    void reset_integral() {
        integral = 0.0;
    }
};
static double get_limit_value(double input_value, double base_value, double min_value, double max_value)
{
    double limited_value = base_value + std::max(min_value, std::min(max_value, input_value));
    return limited_value;
}

static SamplePID *pid_pos_z;

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "sample";
}
static double hovering_thrust;
static double hovering_thrust_range;
int hako_module_drone_controller_impl_init(void* context)
{
    hovering_thrust = HAKO_MODULE_DRONE_MASS * 9.81;
    hovering_thrust_range = hovering_thrust / 2;
    pid_pos_z = new SamplePID(
        1.0e+0, /* Kp */
        1.0e-1, /* Ki */
        0.5e+2, /* Kd */
        10      /* setpoint */
        );
    return pid_pos_z != nullptr;
}

mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    mi_drone_control_out_t out;
    double z = pid_pos_z->calculate(-in->pos_z);
    out.thrust = get_limit_value(z, hovering_thrust, -hovering_thrust_range, hovering_thrust_range);
    out.torque_x = 0;
    out.torque_y = 0;
    out.torque_z = 0;
    return out;
}
