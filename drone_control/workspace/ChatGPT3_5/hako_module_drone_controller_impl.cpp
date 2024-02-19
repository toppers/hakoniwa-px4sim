#include "hako_module_drone_controller_impl.h"

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "ChatGPT3_5";
}

// PID制御パラメータ
#define KP 0.1
#define KI 0.01
#define KD 0.05

// 目標高度
#define TARGET_ALTITUDE 10.0

// 初期値
static double integral_error = 0.0;
static double prev_error = 0.0;

int hako_module_drone_controller_impl_init(void* context)
{
    // ここでは特に何もしない
    return 0; // 成功を示す値を返す
}

mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    mi_drone_control_out_t out;

    // 現在の高度を取得
    double current_altitude = in->pos_z;

    // 高度の誤差を計算
    double error = TARGET_ALTITUDE - current_altitude;

    // PID制御の計算
    double proportional = KP * error;
    integral_error += error;
    double integral = KI * integral_error;
    double derivative = KD * (error - prev_error);
    double control_signal = proportional + integral + derivative;

    // 制御出力を設定
    out.thrust = control_signal; // 推力に制御信号をそのまま使用
    out.torque_x = 0.0; // ロール方向のトルクはゼロ
    out.torque_y = 0.0; // ピッチ方向のトルクはゼロ
    out.torque_z = 0.0; // ヨー方向のトルクはゼロ

    // 前回の誤差を更新
    prev_error = error;

    return out;
}
