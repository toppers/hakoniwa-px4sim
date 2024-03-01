#include "hako_module_drone_controller_impl.h"
#include "control/position/pid_ctrl_vertical_pos.hpp"
#include "control/position/pid_ctrl_vertical_vel.hpp"

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "DroneFlightController";
}


static PidCtrlVerticalPos *pid_ctrl_vertical_pos;
static PidCtrlVerticalVel *pid_ctrl_vertical_vel;

int hako_module_drone_controller_impl_init(void* context)
{
    pid_ctrl_vertical_pos = new PidCtrlVerticalPos();
    pid_ctrl_vertical_vel = new PidCtrlVerticalVel();
    return 0;
}

mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    in->target_pos_z = -10;
    in->target_velocity = 3.5;
    mi_drone_control_out_t control_output;
    EulerType euler = {in->euler_x, in->euler_y, in->euler_z};
    double target_velocity_z = pid_ctrl_vertical_pos->run(in->target_pos_z, in->pos_z, euler);
    if (target_velocity_z > 0) {
        target_velocity_z = 0;
    }
    else if (target_velocity_z < -in->target_velocity) {
        target_velocity_z = -in->target_velocity;
    }
    control_output.thrust = -pid_ctrl_vertical_vel->run(target_velocity_z, in->w) + (in->mass * GRAVITY);
    //std::cout << "pos_z: " << in->pos_z << std::endl;
    //std::cout << "in->w: " << in->w << std::endl;
    //std::cout << "target_velocity_z: " << target_velocity_z << std::endl;
    //std::cout << "control_output.thrust: " << control_output.thrust << std::endl;
    control_output.torque_x = 0.0;
    control_output.torque_y = 0.0;
    control_output.torque_z = 0.0;
    return control_output;
}
