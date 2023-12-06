#include "drone_pid_control.hpp"
#include "hako_capi.h"
#include "drone_primitive_types.hpp"
#include "../../../hako/pdu/hako_pdu_data.hpp"
#include "hako_asset_runner.h"

static DronePidControl *pid_height;
static DronePidControl *pid_phi;
static DronePidControl *pid_theta;
static DronePidControl *pid_psi;
#define DELTA_TIME  0.001
#define HAKO_AVATOR_CHANNLE_ID_MOTOR    0
#define HAKO_AVATOR_CHANNLE_ID_POS      1
#define HAKO_AVATOR_CHANNLE_ID_CTRL     2
#define HAKO_ROBO_NAME "px4sim"

void drone_pid_control_init() 
{
    // PIDコントローラのパラメータ設定
    // ここでは例として固定値を使用していますが、実際には設定ファイルやコマンドライン引数から読み込むことを推奨します。
    double Kp_height = 1.0, Ki_height = 0.0, Kd_height = 0.01;
    double setpoint_height = 10.0;  // 目標高さ

    double Kp_phi = 0.5, Ki_phi = 0.01, Kd_phi = 0.01;
    double setpoint_phi = 0.0;  // 目標ロール角

    double Kp_theta = 0.5, Ki_theta = 0.01, Kd_theta = 0.01;
    double setpoint_theta = 0.0;  // 目標ピッチ角

    double Kp_psi = 0.5, Ki_psi = 0.01, Kd_psi = 0.01;
    double setpoint_psi = 0.0;  // 目標ヨー角

    // PIDコントローラのインスタンス化
    pid_height = new DronePidControl(Kp_height, Ki_height, Kd_height, setpoint_height,
                                     "python/results/height_data.csv", {"Time", "Height"});
    pid_phi = new DronePidControl(Kp_phi, Ki_phi, Kd_phi, setpoint_phi,
                                  "python/results/phi_data.csv", {"Time", "Phi"});
    pid_theta = new DronePidControl(Kp_theta, Ki_theta, Kd_theta, setpoint_theta,
                                    "python/results/theta_data.csv", {"Time", "Theta"});
    pid_psi = new DronePidControl(Kp_psi, Ki_psi, Kd_psi, setpoint_psi,
                                  "python/results/psi_data.csv", {"Time", "Psi"});
}
static void do_io_read(DronePositionType& dpos, DroneAngleType& dangle)
{
    Hako_Twist pos;

    if (!hako_asset_runner_pdu_read(HAKO_ROBO_NAME, HAKO_AVATOR_CHANNLE_ID_POS, (char*)&pos, sizeof(pos))) {
        std::cerr << "ERROR: can not read pdu data: pos" << std::endl;
    }
    dpos.data.x = pos.linear.x;
    dpos.data.y = pos.linear.y;
    dpos.data.z = pos.linear.z;
    dangle.data.x = pos.angular.x;
    dangle.data.y = pos.angular.y;
    dangle.data.z = pos.angular.z;
}

static double get_limit_value(double input_value, double base_value, double min_value, double max_value)
{
    double limited_value = base_value + std::max(min_value, std::min(max_value, input_value));
    return limited_value;
}
void drone_pid_control_run() 
{
    static double current_time = 0;
    static double last_time = 0;
    DronePositionType dpos;
    DroneAngleType dangle;

    // PDUからデータを読み取る
    do_io_read(dpos, dangle);

    // PIDコントローラを使用して制御計算を行う
    double height_input = -dpos.data.z; // 高さの入力値
    double phi_input = dangle.data.x;    // ロール角の入力値
    double theta_input = dangle.data.y;  // ピッチ角の入力値
    double psi_input = dangle.data.z;    // ヨー角の入力値

    double height_output = pid_height->calculate(height_input);
    height_output = get_limit_value(height_output, 9.81, -2, 2);
    double phi_output = pid_phi->calculate(phi_input);
    double theta_output = pid_theta->calculate(theta_input);
    double psi_output = pid_psi->calculate(psi_input);
    phi_output = get_limit_value(phi_output, 0, -2, 2);
    theta_output = get_limit_value(theta_output, 0, -2, 2);
    psi_output = get_limit_value(psi_output, 0, -2, 2);

    // CSVファイルに記録
    pid_height->write_to_csv({std::to_string(current_time), std::to_string(height_input)});
    pid_phi->write_to_csv({std::to_string(current_time), std::to_string(phi_input)});
    pid_theta->write_to_csv({std::to_string(current_time), std::to_string(theta_input)});
    pid_psi->write_to_csv({std::to_string(current_time), std::to_string(psi_input)});

    current_time += DELTA_TIME;
    if ((current_time - last_time) > 1) {
        std::cout << "T: " << current_time <<  "U: " << height_output << " H: " << height_input << std::endl;
        last_time = current_time;
    }

    DroneThrustType thrust;
    DroneTorqueType torque;
    thrust.data = height_output;
    torque.data.x = phi_output;
    torque.data.y = theta_output;
    torque.data.z = psi_output;
    {
        Hako_Twist control;
        control.linear.z = thrust.data;
        control.angular.x = torque.data.x;
        control.angular.y = torque.data.y;
        control.angular.z = torque.data.z;

        if (!hako_asset_runner_pdu_write(HAKO_ROBO_NAME, HAKO_AVATOR_CHANNLE_ID_CTRL, (const char*)&control, sizeof(control))) {
            std::cerr << "ERROR: can not write pdu data: control" << std::endl;
            return;
        }
    }


}
