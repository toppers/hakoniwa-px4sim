#include "hako_module_drone_controller_impl.h"
#include "control/position/pid_ctrl_vertical_pos.hpp"
#include "control/position/pid_ctrl_vertical_vel.hpp"
#include "control/angle/pid_ctrl_yaw_angle.hpp"
#include "control/angle/pid_ctrl_yaw_rate.hpp"
#include "control/angle/pid_ctrl_roll_angle.hpp"
#include "control/angle/pid_ctrl_roll_rate.hpp"
#include "control/angle/pid_ctrl_pitch_angle.hpp"
#include "control/angle/pid_ctrl_pitch_rate.hpp"

static double get_limit_value(double input_value, double base_value, double min_value, double max_value)
{
    double limited_value = base_value + std::max(min_value, std::min(max_value, input_value));
    return limited_value;
}

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "DroneFlightController";
}


static PidCtrlVerticalPos *pid_ctrl_vertical_pos;
static PidCtrlVerticalVel *pid_ctrl_vertical_vel;
static PidCtrlYawAngle    *pid_ctrl_yaw_angle;
static PidCtrlYawRate     *pid_ctrl_yaw_rate;
static PidCtrlRollAngle   *pid_ctrl_roll_angle;
static PidCtrlRollRate    *pid_ctrl_roll_rate;
static PidCtrlPitchAngle  *pid_ctrl_pitch_angle;
static PidCtrlPitchRate   *pid_ctrl_pitch_rate;

#define ALMOST_EQUAL(target, current, range) ( ( (current) >= ((target) - (range)) ) &&  ( (current) <= ((target) + (range)) ) )

int hako_module_drone_controller_impl_init(void* context)
{
    pid_ctrl_vertical_pos = new PidCtrlVerticalPos();
    if (pid_ctrl_vertical_pos == nullptr) {
        return -1;
    }
    pid_ctrl_vertical_vel = new PidCtrlVerticalVel();
    if (pid_ctrl_vertical_vel == nullptr) {
        return -1;
    }
    pid_ctrl_yaw_angle = new PidCtrlYawAngle();
    if (pid_ctrl_yaw_angle == nullptr) {
        return -1;
    }
    pid_ctrl_yaw_rate = new PidCtrlYawRate();
    if (pid_ctrl_yaw_rate == nullptr) {
        return -1;
    }
    pid_ctrl_roll_angle = new PidCtrlRollAngle();
    if (pid_ctrl_roll_angle == nullptr) {
        return -1;
    }
    pid_ctrl_roll_rate = new PidCtrlRollRate();
    if (pid_ctrl_roll_rate == nullptr) {
        return -1;
    }
    pid_ctrl_pitch_angle = new PidCtrlPitchAngle();
    if (pid_ctrl_pitch_angle == nullptr) {
        return -1;
    }
    pid_ctrl_pitch_rate = new PidCtrlPitchRate();
    if (pid_ctrl_pitch_rate == nullptr) {
        return -1;
    }
    return 0;
}
typedef enum {
    DRONE_CONTROL_MODE_NONE = 0,
    DRONE_CONTROL_MODE_TAKEOFF,
    DRONE_CONTROL_MODE_YAW,
    DRONE_CONTROL_MODE_MOVE,
} DroneControlModeType;
static DroneControlModeType drone_control_mode = DRONE_CONTROL_MODE_NONE;

/*
 * power: 
 *  forward: 0..100
 *  back   : -100..0
 */
#define MAX_FORWARD_DEGREE 10.0
static double move_forward(double power, double q, EulerType &euler)
{
    double target_angle_degree = 0;
    power = get_limit_value(power, 0, -100, 100);
    if (power > 0) {
        target_angle_degree = -get_limit_value(power/10.0, 0, 0, MAX_FORWARD_DEGREE);
    }
    else {
        target_angle_degree = -get_limit_value(power/10.0, 0, -MAX_FORWARD_DEGREE, 0);
    }
    double target_pitch = DEGREE2RADIAN(target_angle_degree);
    double target_pitch_rate_max = RPM2EULER_RATE(2);
    double target_pitch_rate = pid_ctrl_pitch_angle->run(target_pitch, euler);
    target_pitch_rate = get_limit_value(target_pitch_rate, 0, -target_pitch_rate_max, target_pitch_rate_max);
    double torque_y = pid_ctrl_pitch_rate->run(target_pitch_rate, NORMALIZE_RADIAN(q));
    torque_y = get_limit_value(torque_y, 0, -M_PI/10.0, M_PI/10.0);
    return torque_y;
}
/*
 * power: 
 *  right: 100..0
 *  left : 0..-100
 */
#define MAX_HORIZONTAL_DEGREE 10.0
static double move_horizontal(double power, double p, EulerType &euler)
{
    double target_roll_rate_max = RPM2EULER_RATE(10);
    double target_angle_degree = 0;
    power = get_limit_value(power, 0, -100, 100);
    if (power > 0) {
        target_angle_degree = -get_limit_value(power/10.0, 0, 0, MAX_HORIZONTAL_DEGREE);
    }
    else {
        target_angle_degree = -get_limit_value(power/10.0, 0, -MAX_HORIZONTAL_DEGREE, 0);
    }
    double target_roll = DEGREE2RADIAN(target_angle_degree);
    double target_roll_rate = pid_ctrl_roll_angle->run(target_roll, euler);
    target_roll_rate = get_limit_value(target_roll_rate, 0, -target_roll_rate_max, target_roll_rate_max);
    double torque_x = pid_ctrl_roll_rate->run(target_roll_rate, NORMALIZE_RADIAN(p));
    torque_x = get_limit_value(torque_x, 0, -M_PI/10.0, M_PI/10.0);
    return torque_x;
}

static double rotate_yaw(double target_yaw, double r, EulerType &euler)
{
    double target_yaw_rate_max = RPM2EULER_RATE(20);
    double target_yaw_rate = pid_ctrl_yaw_angle->run(target_yaw, euler);
    target_yaw_rate = get_limit_value(target_yaw_rate, 0, -target_yaw_rate_max, target_yaw_rate_max);
    double torque_z = pid_ctrl_yaw_rate->run(target_yaw_rate, NORMALIZE_RADIAN(r));
    torque_z = get_limit_value(torque_z, 0, -M_PI/4.0, M_PI/4.0);
    return torque_z;
}

mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    /*
     * Target
     */
    mi_drone_control_out_t control_output;
    double target_yaw = DEGREE2RADIAN(45);
    EulerType euler = {NORMALIZE_RADIAN(in->euler_x), NORMALIZE_RADIAN(in->euler_y), NORMALIZE_RADIAN(in->euler_z)};
    if (drone_control_mode == DRONE_CONTROL_MODE_NONE) {
        in->target_pos_z = -10;
        in->target_velocity = 3.5;
        drone_control_mode = DRONE_CONTROL_MODE_TAKEOFF;
        std::cout << "TARGET: pos = " << in->target_pos_z << std::endl;
        std::cout << "TARGET: vel = " << in->target_velocity << std::endl;
        std::cout << "TARGET: yaw = " << target_yaw << std::endl;

        std::cout << "INFO: start takeoff" << std::endl;
    }
    /*
     * Vertical control
     */
    if (drone_control_mode >= DRONE_CONTROL_MODE_TAKEOFF) {
        static int target_done_count = 0;
        
        double target_velocity_z = pid_ctrl_vertical_pos->run(in->target_pos_z, in->pos_z, euler);
        target_velocity_z = get_limit_value(target_velocity_z, 0, -in->target_velocity, 0);
        control_output.thrust = -pid_ctrl_vertical_vel->run(target_velocity_z, in->w) + (in->mass * GRAVITY);
        //std::cout << "pos_z: " << in->pos_z << std::endl;
        //std::cout << "in->w: " << in->w << std::endl;
        //std::cout << "target_velocity_z: " << target_velocity_z << std::endl;
        //std::cout << "control_output.thrust: " << control_output.thrust << std::endl;
        if (drone_control_mode == DRONE_CONTROL_MODE_TAKEOFF) {
            if (ALMOST_EQUAL(in->target_pos_z, in->pos_z, 0.1)) {
                target_done_count++;
            }

            if (target_done_count >= 10) {
                drone_control_mode = DRONE_CONTROL_MODE_YAW;
                std::cout << "INFO: start yaw mode" << std::endl;
            }
        }
    }
    // Yaw control
    if (drone_control_mode >= DRONE_CONTROL_MODE_YAW) {
        static int target_done_count = 0;
        control_output.torque_z = rotate_yaw(target_yaw, in->r, euler);
        if (drone_control_mode == DRONE_CONTROL_MODE_YAW) {
            if (ALMOST_EQUAL(target_yaw, euler.psi, DEGREE2RADIAN(1))) {
                target_done_count++;
            }
            if (target_done_count >= 10) {
                drone_control_mode = DRONE_CONTROL_MODE_MOVE;
                std::cout << "INFO: start move mode" << std::endl;
            }
        }
    }
    if (drone_control_mode >= DRONE_CONTROL_MODE_MOVE) {
        control_output.torque_y = move_forward(100, in->q, euler);
    }
    //roll control
    {
        control_output.torque_x = move_horizontal(0, in->p, euler);
    }

    return control_output;
}
