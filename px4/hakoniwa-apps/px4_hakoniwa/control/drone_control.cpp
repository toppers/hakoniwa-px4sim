#include "drone_control.hpp"
#include <cstring>

// PIDゲインの定数を定義
constexpr float DRONE_CONTROL_PID_POS_Z_KP = 1.5f;  // 比例ゲイン
constexpr float DRONE_CONTROL_PID_POS_Z_KI = 1.0f;  // 積分ゲイン
constexpr float DRONE_CONTROL_PID_POS_Z_KD = 0.50f;  // 微分ゲイン

constexpr float DRONE_CONTROL_PID_ROT_X_KP = 20.0f;  // 比例ゲイン
constexpr float DRONE_CONTROL_PID_ROT_X_KI = 1.0f;  // 積分ゲイン
constexpr float DRONE_CONTROL_PID_ROT_X_KD = 1.0f;  // 微分ゲイン

constexpr float DRONE_CONTROL_PID_ROT_Y_KP = 0.1f;  // 比例ゲイン
constexpr float DRONE_CONTROL_PID_ROT_Y_KI = 0.1f;  // 積分ゲイン
constexpr float DRONE_CONTROL_PID_ROT_Y_KD = 0.1f;  // 微分ゲイン

constexpr float DRONE_CONTROL_PID_ROT_Z_KP = 2.0f;  // 比例ゲイン
constexpr float DRONE_CONTROL_PID_ROT_Z_KI = 1.0f;  // 積分ゲイン
constexpr float DRONE_CONTROL_PID_ROT_Z_KD = 1.0f;  // 微分ゲイン

constexpr float DRONE_THRUST_MAX = 1.0f;
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
    ctrl.target_pos.target.z = 2.5f;   // 単位：m

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

void drone_control_run(DroneControlType& ctrl, Vector3Type& current_pos, Vector3Type& current_rot, double delta_t)
{
    static Vector3Type roll_power_for_move = { 2, 0, 0 };
    static int antei_count = 0;
    static bool antei = false;
    ctrl.signal.thrust = updatePID(ctrl.target_pos.pid_z, current_pos.z, delta_t);
    //PX4_INFO("thrust: %f", ctrl.signal.thrust);
    ctrl.signal.thrust = get_value_with_limit(ctrl.signal.thrust, DRONE_THRUST_MAX, DRONE_THRUST_MIN);

    if (antei == false) {
        double diff = ctrl.target_pos.target.z - current_pos.z;
        if (abs(diff) <= 0.1) {
            antei_count++;
            if (antei_count > 500) {
                PX4_INFO("START MOVE RIGHT");
                antei = true;
                antei_count = 0;
            }
        }
        else {
            antei_count = 0;
        }
    }
    else {
        // roll power
        Vector3Type roll_power_for_same;

        roll_power_for_same.x = updatePID(ctrl.target_rot.pid_roll, current_rot.x, ctrl.delta_t);
        double torque_x = roll_power_for_move.x + roll_power_for_same.x;
        double torque_x_max = ctrl.signal.thrust * DRONE_PARAM_L / (2.0 * (double)DRONE_TORQUE_X_MAX_RATE);
        ctrl.signal.torque.x = get_value_with_limit(torque_x, torque_x_max, -torque_x_max);

#if 0
        roll_power_for_same.y = updatePID(ctrl.target_rot.pid_pitch, current_rot.y, ctrl.delta_t);
        double torque_y = roll_power_for_move.y + roll_power_for_same.y;
        double torque_y_max = ctrl.signal.thrust * DRONE_PARAM_L / (2.0 * (double)DRONE_TORQUE_Y_MAX_RATE);
        ctrl.signal.torque.y = get_value_with_limit(torque_y, torque_y_max, -torque_y_max);

        antei_count++;
        if (roll_power_for_move.x > 0 && antei_count >= 100) {
            antei_count = 0;
            roll_power_for_move.x = 0;
            PX4_INFO("END MOVE RIGHT");
        }
#endif
    }
}
