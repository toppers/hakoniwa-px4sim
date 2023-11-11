#include "drone_control.hpp"
#include <cstring>

// PIDゲインの定数を定義
constexpr float DRONE_CONTROL_PID_POS_Z_KP = 3.0f;  // 比例ゲイン
constexpr float DRONE_CONTROL_PID_POS_Z_KI = 1.0f;  // 積分ゲイン
constexpr float DRONE_CONTROL_PID_POS_Z_KD = 0.50f;  // 微分ゲイン

constexpr float DRONE_CONTROL_PID_ROT_X_KP = 20.0f;  // 比例ゲイン
constexpr float DRONE_CONTROL_PID_ROT_X_KI = 1.0f;  // 積分ゲイン
constexpr float DRONE_CONTROL_PID_ROT_X_KD = 1.0f;  // 微分ゲイン

constexpr float DRONE_CONTROL_PID_ROT_Y_KP = 20.0f;  // 比例ゲイン
constexpr float DRONE_CONTROL_PID_ROT_Y_KI = 1.0f;  // 積分ゲイン
constexpr float DRONE_CONTROL_PID_ROT_Y_KD = 1.0f;  // 微分ゲイン

constexpr float DRONE_CONTROL_PID_ROT_Z_KP = 2.0f;  // 比例ゲイン
constexpr float DRONE_CONTROL_PID_ROT_Z_KI = 1.0f;  // 積分ゲイン
constexpr float DRONE_CONTROL_PID_ROT_Z_KD = 1.0f;  // 微分ゲイン

constexpr float DRONE_THRUST_MAX = 20.0f;
constexpr float DRONE_THRUST_MIN = 0.0f;
constexpr float DRONE_TORQUE_X_MAX_RATE = 1.0f;
constexpr float DRONE_TORQUE_Y_MAX_RATE = 1.0f;
constexpr float DRONE_TORQUE_Z_MAX_RATE = 1.0f;

static inline double get_value_with_limit(double value, double max, double min)
{
    if (value > max)
        return max;
    else if (value < min)
        return min;
    else
        return value;
}

void drone_control_init(DroneControlType& ctrl, double delta_t)
{
    std::memset(&ctrl, 0, sizeof(ctrl));  // 構造体の全メンバを0で初期化
    ctrl.delta_t = delta_t;
    ctrl.target_pos.target.z = 10.0f;   // 単位：m

    // Z軸用PIDコントローラの初期化
    initPID(ctrl.target_pos.pid_z, DRONE_CONTROL_PID_POS_Z_KP, DRONE_CONTROL_PID_POS_Z_KI, DRONE_CONTROL_PID_POS_Z_KD, ctrl.target_pos.target.z);

    //X軸の水平制御
    ctrl.target_rot.target.x = 0;
    initPID(ctrl.target_rot.pid_roll, DRONE_CONTROL_PID_ROT_X_KP, DRONE_CONTROL_PID_ROT_X_KI, DRONE_CONTROL_PID_ROT_X_KD, ctrl.target_rot.target.x);
    //Y軸の水平制御
    ctrl.target_rot.target.y = 0;
    initPID(ctrl.target_rot.pid_pitch, DRONE_CONTROL_PID_ROT_Y_KP, DRONE_CONTROL_PID_ROT_Y_KI, DRONE_CONTROL_PID_ROT_Y_KD, ctrl.target_rot.target.y);

    //Z軸平制御。初期値はゼロで良い。
    ctrl.target_rot.target.y = 0;
    initPID(ctrl.target_rot.pid_yaw, DRONE_CONTROL_PID_ROT_Z_KP, DRONE_CONTROL_PID_ROT_Z_KI, DRONE_CONTROL_PID_ROT_Z_KD, ctrl.target_rot.target.z);

}

void drone_control_run(DroneControlType& ctrl, Vector3Type& current_pos, double delta_t)
{
    ctrl.signal.thrust = updatePID(ctrl.target_pos.pid_z, current_pos.z, delta_t);
    //PX4_INFO("thrust: %f", ctrl.signal.thrust);
    ctrl.signal.thrust = get_value_with_limit(ctrl.signal.thrust, DRONE_THRUST_MAX, DRONE_THRUST_MIN);
}
