#include "drone_control.hpp"
#include "../../pdu/hako_pdu_data.hpp"
#include <iostream>
#include <cstring> // memsetの代わりにC++スタイルのヘッダを使用

// PIDゲインの定数を定義
constexpr float DRONE_CONTROL_PID_POS_Z_KP = 4.3f;  // 比例ゲイン
constexpr float DRONE_CONTROL_PID_POS_Z_KI = 1.0f;  // 積分ゲイン
constexpr float DRONE_CONTROL_PID_POS_Z_KD = 1.5f;  // 微分ゲイン

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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

static void key_init(void)
{
    struct termios oldt, newt;
    int oldf;

    // 端末設定を取得
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // カノニカルモードとエコーをオフに設定
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // ファイル記述子の設定を非ブロッキングに
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);    
}

void drone_control_init(DroneControlType& ctrl, double delta_t)
{
    key_init();
    std::memset(&ctrl, 0, sizeof(ctrl));  // 構造体の全メンバを0で初期化
    ctrl.delta_t = delta_t;
    ctrl.target_pos.target.z = 7500.0f;   // 単位：mm

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

    // initPID関数がPIDControllerのインスタンスを適切に初期化することを想定しています。
}


void drone_control_run(DroneControlType& ctrl)
{
    Hako_HakoHilStateQuaternion hil_state_quaternion;
    if (hako_read_hil_state_quaternion(hil_state_quaternion) == false) {
        return;
    }
    // PID制御の計算
    ctrl.signal.thrust = updatePID(ctrl.target_pos.pid_z, -hil_state_quaternion.alt, ctrl.delta_t) / 1000.0f;
#ifdef ENABLE_DRONE_PHYS_DEBUG
    std::cout << "ctrl.target_pos.pid_z.previous_error= " << ctrl.target_pos.pid_z.previous_error << std::endl;
#endif
    ctrl.signal.thrust = get_value_with_limit(ctrl.signal.thrust, DRONE_THRUST_MAX, DRONE_THRUST_MIN);
    QuaternionType q;
    Vector3Type angle;
    q.w = hil_state_quaternion.attitude_quaternion[0];
    q.x = hil_state_quaternion.attitude_quaternion[1];
    q.y = hil_state_quaternion.attitude_quaternion[2];
    q.z = hil_state_quaternion.attitude_quaternion[3];
    quaternion2Euler(q, angle);
#ifdef ENABLE_DRONE_PHYS_DEBUG
    std::cout << "angle.x = " << angle.x << std::endl;
    std::cout << "angle.y = " << angle.y << std::endl;
    std::cout << "angle.z = " << angle.z << std::endl;
    std::cout << "hil_state_quaternion.lon= " << hil_state_quaternion.lon << std::endl;
#endif
    // roll power
    static Vector3Type roll_power_for_move = { 0, 0, 0 };
#ifdef ENABLE_DRONE_PHYS_DEBUG
    std::cout << "ctrl.target_rot.pid_roll.previous_error= " << ctrl.target_rot.pid_roll.previous_error << std::endl;
#endif
    int c = getc(stdin);
    if (c == 'j') { // move left
        std::cout << "MOVE LEFT: key=" << c << std::endl;
        roll_power_for_move.x = -1.0;
    }
    else if (c == 'l') { //move right
        std::cout << "MOVE RIGHT: key=" << c << std::endl;
        roll_power_for_move.x= 1.0;
    }
    else if (c == 'i') { // move forward
        std::cout << "MOVE FORWARD: key=" << c << std::endl;
        roll_power_for_move.y= 1.0;
    }
    else if (c == 'm') { // move back
        std::cout << "MOVE BACK: key=" << c << std::endl;
        roll_power_for_move.y= -1.0;
    }
    else if (c == 'g') { //turn right
        std::cout << "TURN RIGHT: key=" << c << std::endl;
        roll_power_for_move.z -= 90.0 * (M_PI / 180.0);
    }
    else if (c == 'f') { //turn left
        std::cout << "TURN LEFT: key=" << c << std::endl;
        roll_power_for_move.z += 90.0 * (M_PI / 180.0);
    }
    else if (c == ' ') {
        std::cout << "RESET: key=" << c << std::endl;
        roll_power_for_move.x = 0;
        roll_power_for_move.y = 0;
    }
    Vector3Type roll_power_for_same;
    roll_power_for_same.x = updatePID(ctrl.target_rot.pid_roll, angle.x, ctrl.delta_t);
    roll_power_for_same.y = updatePID(ctrl.target_rot.pid_pitch, -angle.y, ctrl.delta_t);
    ctrl.target_rot.pid_yaw.setpoint = roll_power_for_move.z;
    roll_power_for_same.z = updatePID(ctrl.target_rot.pid_yaw, -angle.z, ctrl.delta_t);

#ifdef ENABLE_DRONE_PHYS_DEBUG
    std::cout << "roll_power_for_move.x= " << roll_power_for_move.x << std::endl;
    std::cout << "roll_power_for_same.x= " << roll_power_for_same.x << std::endl;
    std::cout << "roll_power_for_move.y= " << roll_power_for_move.y << std::endl;
    std::cout << "roll_power_for_same.y= " << roll_power_for_same.y << std::endl;
    std::cout << "roll_power_for_move.z= " << roll_power_for_move.z << std::endl;
    std::cout << "roll_power_for_same.z= " << roll_power_for_same.z << std::endl;
#endif
    double torque_x = roll_power_for_move.x + roll_power_for_same.x;
    double torque_x_max = ctrl.signal.thrust * DRONE_PARAM_L / (2.0 * DRONE_TORQUE_X_MAX_RATE);
    ctrl.signal.torque.x = get_value_with_limit(torque_x, torque_x_max, -torque_x_max);

    double torque_y = roll_power_for_move.y + roll_power_for_same.y;
    double torque_y_max = ctrl.signal.thrust * DRONE_PARAM_L / (2.0 * DRONE_TORQUE_Y_MAX_RATE);
    ctrl.signal.torque.y = get_value_with_limit(torque_y, torque_y_max, -torque_y_max);

    double torque_z = roll_power_for_same.z;
    double torque_z_max = ctrl.signal.thrust * DRONE_PARAM_K / (DRONE_PARAM_P * DRONE_TORQUE_Z_MAX_RATE);
    ctrl.signal.torque.z = get_value_with_limit(torque_z, torque_z_max, -torque_z_max);

}
