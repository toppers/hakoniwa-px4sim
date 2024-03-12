#include "hako_module_drone_controller_impl.h"
#include "drone_flight_controller.hpp"

static double get_limit_value(double input_value, double base_value, double min_value, double max_value)
{
    double limited_value = base_value + std::max(min_value, std::min(max_value, input_value));
    return limited_value;
}

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "DroneFlightController";
}

#define ALMOST_EQUAL(target, current, range) ( ( (current) >= ((target) - (range)) ) &&  ( (current) <= ((target) + (range)) ) )


int hako_module_drone_controller_impl_init(void* context)
{
    if (context == nullptr) {
        return -1;
    }
    DroneFlightControllerContextType *drone_context = (DroneFlightControllerContextType*)context;
    drone_context->drone_control_mode = DRONE_CONTROL_MODE_NONE;
    drone_context->target_yaw = 0.0;
    for (int i = 0; i < DRONE_CONTROL_MODE_NUM; i++) {
        drone_context->count[i] = 0;
    }
    if (drone_context->plan_filepath == nullptr) {
        return 0;
    }
    std::cout << "context:" << drone_context->plan_filepath << std::endl;
    std::string filepath(drone_context->plan_filepath);
    if (drone_context->move_plan.load_plan_from_file(filepath) == false) {
        std::cout << "ERROR: can not load file: " << drone_context->plan_filepath << std::endl;
        return -1;
    }
    return 0;
}

/*
 * power: 
 *  forward: 0..100
 *  back   : -100..0
 */
#define MAX_FORWARD_DEGREE 50.0
static double move_forward(DroneFlightControllerContextType *drone_context, double power, double q, EulerType &euler)
{
    double target_angle_degree = 0;
    power = get_limit_value(power, 0, -100, 100);
    if (power > 0) {
        target_angle_degree = -get_limit_value(power/2.0, 0, 0, MAX_FORWARD_DEGREE);
    }
    else {
        target_angle_degree = -get_limit_value(power/2.0, 0, -MAX_FORWARD_DEGREE, 0);
    }
    //std::cout << "target_degree: " << target_angle_degree << std::endl;
    double target_pitch = DEGREE2RADIAN(target_angle_degree);
    double target_pitch_rate_max = RPM2EULER_RATE(100);
    double target_pitch_rate = drone_context->pid_ctrl_pitch_angle.run(target_pitch, euler);
    target_pitch_rate = get_limit_value(target_pitch_rate, 0, -target_pitch_rate_max, target_pitch_rate_max);
    double torque_y = drone_context->pid_ctrl_pitch_rate.run(target_pitch_rate, NORMALIZE_RADIAN(q));
    torque_y = get_limit_value(torque_y, 0, -M_PI/8.0, M_PI/8.0);
    return torque_y;
}
/*
 * power: 
 *  right: 100..0
 *  left : 0..-100
 */
#define MAX_HORIZONTAL_DEGREE 10.0
static double move_horizontal(DroneFlightControllerContextType *drone_context, double power, double p, EulerType &euler)
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
    double target_roll_rate = drone_context->pid_ctrl_roll_angle.run(target_roll, euler);
    target_roll_rate = get_limit_value(target_roll_rate, 0, -target_roll_rate_max, target_roll_rate_max);
    double torque_x = drone_context->pid_ctrl_roll_rate.run(target_roll_rate, NORMALIZE_RADIAN(p));
    torque_x = get_limit_value(torque_x, 0, -M_PI/10.0, M_PI/10.0);
    return torque_x;
}

static double rotate_yaw(DroneFlightControllerContextType *drone_context, double target_yaw, double r, EulerType &euler)
{
    double target_yaw_rate_max = RPM2EULER_RATE(20);
    double target_yaw_rate = drone_context->pid_ctrl_yaw_angle.run(target_yaw, euler);
    target_yaw_rate = get_limit_value(target_yaw_rate, 0, -target_yaw_rate_max, target_yaw_rate_max);
    double torque_z = drone_context->pid_ctrl_yaw_rate.run(target_yaw_rate, NORMALIZE_RADIAN(r));
    torque_z = get_limit_value(torque_z, 0, -M_PI/4.0, M_PI/4.0);
    return torque_z;
}

static mi_drone_control_out_t do_landing(mi_drone_control_in_t *in)
{
    DroneFlightControllerContextType *drone_context = (DroneFlightControllerContextType*)in->context;
    mi_drone_control_out_t control_output = {};
    EulerType euler = {NORMALIZE_RADIAN(in->euler_x), NORMALIZE_RADIAN(in->euler_y), NORMALIZE_RADIAN(in->euler_z)};
    
    double target_velocity_z = drone_context->pid_ctrl_vertical_pos.run(in->target_pos_z, in->pos_z, euler);
    target_velocity_z = get_limit_value(target_velocity_z, 0, -in->target_velocity, 0);
    control_output.thrust = -drone_context->pid_ctrl_vertical_vel.run(target_velocity_z, in->w) + (in->mass * PID_PARAM_GRAVITY / 1.3);
    control_output.torque_x = move_horizontal(drone_context, 0, in->p, euler);
    control_output.torque_y = move_forward(drone_context, 0, in->q, euler);
    control_output.torque_z = rotate_yaw(drone_context, 0, in->r, euler);
    //std::cout << "yaw: " << euler.psi << " torque_z: " << control_output.torque_z << std::endl;
    if (ALMOST_EQUAL(in->target_pos_z, in->pos_z, 0.1)) {
        drone_context->count[DRONE_CONTROL_MODE_LAND]++;
    }

    if (drone_context->count[DRONE_CONTROL_MODE_LAND] >= 10) {
        drone_context->drone_control_mode = DRONE_CONTROL_MODE_NONE;
        //std::cout << "INFO: Landing Operation is done" << std::endl;
        drone_context->count[DRONE_CONTROL_MODE_LAND] = 0;
    }

    return control_output;
}
static mi_drone_control_out_t drone_controller_impl_run(mi_drone_control_in_t *in)
{
    /*
     * Target
     */
    DroneFlightControllerContextType *drone_context = (DroneFlightControllerContextType*)in->context;
    mi_drone_control_out_t control_output = {};
    EulerType euler = {NORMALIZE_RADIAN(in->euler_x), NORMALIZE_RADIAN(in->euler_y), NORMALIZE_RADIAN(in->euler_z)};
    if (drone_context->drone_control_mode == DRONE_CONTROL_MODE_NONE) {
        drone_context->target_pos = { in->target_pos_x, in->target_pos_y, in->target_pos_z };
        PositionType relative_vector = { in->target_pos_x - in->pos_x, in->target_pos_y - in->pos_y, 0};
        drone_context->target_yaw = atan2(relative_vector.y, relative_vector.x);

        drone_context->drone_control_mode = DRONE_CONTROL_MODE_TAKEOFF;
        std::cout << "TARGET: pos_x= " << in->target_pos_x << std::endl;
        std::cout << "TARGET: pos_y= " << in->target_pos_y << std::endl;
        std::cout << "TARGET: pos_z= " << in->target_pos_z << std::endl;
        std::cout << "TARGET: vel = " << in->target_velocity << std::endl;
        std::cout << "TARGET: yaw = " << drone_context->target_yaw << std::endl;

        std::cout << "INFO: start takeoff" << std::endl;
    }
    //PositionType target_pos = { in->target_pos_x, in->target_pos_y, in->target_pos_z };
    PositionType current_pos = { in->pos_x, in->pos_y, in->pos_z };

    /*
     * Vertical control
     */
    if (drone_context->drone_control_mode >= DRONE_CONTROL_MODE_TAKEOFF) {        
        double target_velocity_z = drone_context->pid_ctrl_vertical_pos.run(in->target_pos_z, in->pos_z, euler);
        target_velocity_z = get_limit_value(target_velocity_z, 0, -in->target_velocity, 0);
        control_output.thrust = -drone_context->pid_ctrl_vertical_vel.run(target_velocity_z, in->w) + (in->mass * PID_PARAM_GRAVITY);
        //std::cout << "pos_z: " << in->pos_z << std::endl;
        //std::cout << "in->w: " << in->w << std::endl;
        //std::cout << "target_velocity_z: " << target_velocity_z << std::endl;
        //std::cout << "control_output.thrust: " << control_output.thrust << std::endl;
        if (drone_context->drone_control_mode == DRONE_CONTROL_MODE_TAKEOFF) {
            if (ALMOST_EQUAL(in->target_pos_z, in->pos_z, 0.1)) {
                drone_context->count[DRONE_CONTROL_MODE_TAKEOFF]++;
            }

            if (drone_context->count[DRONE_CONTROL_MODE_TAKEOFF] >= 10) {
                drone_context->drone_control_mode = DRONE_CONTROL_MODE_YAW;
                std::cout << "INFO: start yaw mode" << std::endl;
                drone_context->count[DRONE_CONTROL_MODE_TAKEOFF] = 0;
            }
        }
    }
    // Yaw control
    if (drone_context->drone_control_mode >= DRONE_CONTROL_MODE_YAW) {
        //std::cout << "current yaw: " << euler.psi << std::endl;
        control_output.torque_z = rotate_yaw(drone_context, drone_context->target_yaw, in->r, euler);
        if (drone_context->drone_control_mode == DRONE_CONTROL_MODE_YAW) {
            if (ALMOST_EQUAL(drone_context->target_yaw, euler.psi, DEGREE2RADIAN(5))) {
                drone_context->count[DRONE_CONTROL_MODE_YAW]++;
            }
            if (drone_context->count[DRONE_CONTROL_MODE_YAW] >= 2) {
                drone_context->drone_control_mode = DRONE_CONTROL_MODE_MOVE;
                std::cout << "INFO: start move mode" << std::endl;
                drone_context->count[DRONE_CONTROL_MODE_YAW] = 0;
            }
        }
    }
    if (drone_context->drone_control_mode >= DRONE_CONTROL_MODE_MOVE) {
        std::pair<double, double> target_vel = drone_context->pid_ctrl_forward_pos.run(drone_context->target_pos, current_pos, euler);
        double target_vel_f = target_vel.first;
        double target_vel_h = target_vel.second;
        target_vel_f = get_limit_value(target_vel_f, 0, -in->target_velocity, in->target_velocity);
        target_vel_h = get_limit_value(target_vel_h, 0, -in->target_velocity, in->target_velocity);
        double power_f = drone_context->pid_ctrl_forward_vel.runF(target_vel_f, in->q);
        double power_h = drone_context->pid_ctrl_forward_vel.runH(target_vel_h, in->p);
        power_f = get_limit_value(power_f, 0, -100, 100);
        power_h = get_limit_value(power_h, 0, -100, 100);
        control_output.torque_x = move_horizontal(drone_context, power_h, in->p, euler);
        control_output.torque_y = move_forward(drone_context, power_f, in->q, euler);

        if (ALMOST_EQUAL(in->target_pos_x, in->pos_x, 0.1) & ALMOST_EQUAL(in->target_pos_y, in->pos_y, 0.1)) {
            //std::cout << "INFO: pos( " << in->pos_x << " , " << in->pos_y << " )" << std::endl;
            drone_context->count[DRONE_CONTROL_MODE_MOVE]++;
        }
        if (drone_context->count[DRONE_CONTROL_MODE_MOVE] >= 10) {
            drone_context->drone_control_mode = DRONE_CONTROL_MODE_NONE;
            std::cout << "INFO: ALL OPERATIONS are DONE" << std::endl;
            drone_context->count[DRONE_CONTROL_MODE_MOVE] = 0;
        }
    }

    return control_output;
}

mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    DroneFlightControllerContextType *drone_context = (DroneFlightControllerContextType*)in->context;
    if (drone_context->plan_filepath == nullptr) {
        if ((drone_context->drone_control_mode == DRONE_CONTROL_MODE_NONE) && (in->target_pos_z > (in->pos_z + 1.0))) 
        {
            drone_context->drone_control_mode = DRONE_CONTROL_MODE_LAND;
        }
        if (drone_context->drone_control_mode == DRONE_CONTROL_MODE_LAND)
        {
            return do_landing(in);
        }
        else {
            return drone_controller_impl_run(in);
        }
    }
    in->target_pos_z = -10;
    in->target_velocity = 20;
    int count = drone_context->count[DRONE_CONTROL_MODE_NONE];

    if (count < drone_context->move_plan.positions.size())
    {
        in->target_pos_x = drone_context->move_plan.positions[count].x;
        in->target_pos_y = drone_context->move_plan.positions[count].y;
        mi_drone_control_out_t out = drone_controller_impl_run(in);
        if (drone_context->drone_control_mode == DRONE_CONTROL_MODE_NONE) {
            drone_context->count[DRONE_CONTROL_MODE_NONE]++;
        }
        return out;
    }
    else {
        in->target_pos_z = -1;
        mi_drone_control_out_t out = do_landing(in);
        return out;
    }
}


