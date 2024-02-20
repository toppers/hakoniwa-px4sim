#include "hako_module_drone_controller_impl.h"

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "DroneAvator";
}

// PID制御パラメータ
static double Kp = (1/4.0)*1.0e+0; // 比例ゲイン
static double Ki = 0.0; // 積分ゲイン
static double Kd = 0.0; // 微分ゲイン

// PID制御に必要な変数
static double integral = 0.0; // 積分項
static double prev_error = 0.0; // 前回の偏差

int hako_module_drone_controller_impl_init(void* context)
{
    // PID制御パラメータの初期化など、必要な初期化処理をここに記述
    integral = 0.0;
    prev_error = 0.0;
    return 0; // 初期化成功
}

mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    const double mass = 0.1;
    const double gravity = 9.81;
    const double target_altitude = 10.0; // 目標高度 (m)
    double error = target_altitude + in->pos_z; // 目標との偏差計算(NED座標系なので、+にする)
    integral += error * 0.003; // 積分項の更新 (3msec = 0.003sec)
    double derivative = (error - prev_error) / 0.003; // 微分項の計算

    double output = Kp*error + (mass * gravity) + Ki*integral + Kd*derivative; // PID制御出力

    prev_error = error; // 前回の偏差を更新

    mi_drone_control_out_t control_output;
    control_output.thrust = output; // 推力の指示値
    control_output.torque_x = 0.0; // ロール方向のトルクは0
    control_output.torque_y = 0.0; // ピッチ方向のトルクは0
    control_output.torque_z = 0.0; // ヨー方向のトルクは0
    return control_output;
}
