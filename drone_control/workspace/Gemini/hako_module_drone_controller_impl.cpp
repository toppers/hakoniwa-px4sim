#include "hako_module_drone_controller_impl.h"

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "Gemini";
}

double _kp_z = 1.0, _ki_z = 0.0, _kd_z = 0.0, _integral_error_z = 0.0, _prev_z = 0.0;
double _dt; // Assuming _dt is defined somewhere

int hako_module_drone_controller_impl_init(void* context)
{
  // PIDゲインの設定
  _kp_z = 1.0;
  _ki_z = 0.0;
  _kd_z = 0.0;

  // 積分誤差の初期化
  _integral_error_z = 0.0;

  // 状態変数の初期化
  _prev_z = 0.0;

  return 0;
}

mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
  // 高度偏差の計算
  double error_z = 10.0 - in->pos_z;

  // PID制御
  double u_z = _kp_z * error_z + _ki_z * _integral_error_z + _kd_z * (error_z - _prev_z);

  // 積分誤差の更新
  _integral_error_z += error_z * _dt;

  // 状態変数の更新
  _prev_z = error_z;

  // 制御出力の設定
  mi_drone_control_out_t out;
  out.thrust = u_z;
  out.torque_x = 0.0;
  out.torque_y = 0.0;
  out.torque_z = 0.0;

  return out;
}
