#include "hako_module_drone_controller_impl.h"
#include "move_position_controller.hpp"
#include <algorithm>
#include <iostream>

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "FlightController";
}


void* hako_module_drone_controller_impl_create_context(void*)
{
    return (void*)new MovePositionController();
}

int hako_module_drone_controller_impl_is_operation_doing(void *context)
{
    return true;
}

int hako_module_drone_controller_impl_init(void*)
{
    return 0;
}
mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    MovePositionController* ms = (MovePositionController*)in->context;
    RadioController *rc = ms->rc;
    mi_drone_control_out_t out = {};
    FlightControllerInputEulerType euler = {in->euler_x, in->euler_y, in->euler_z};
    FlightControllerInputPositionType pos = {in->pos_x, in->pos_y, in->pos_z};
    FlightControllerInputVelocityType velocity = {in->u, in->v, in->w};
    FlightControllerInputAngularRateType angular_rate = {in->p, in->q, in->r};

    double target_pos_x = in->target_pos_x;
    double target_pos_y = in->target_pos_y;
    double target_pos_z = -in->target_pos_z;
    double target_velocity = in->target_velocity;
    double target_angle = in->target_yaw_deg;

    //for radio control
    if (in->radio_control != 0) {
        ms->update_target_angle(in->target.direction_velocity.r);
        ms->update_target_pos(-in->target.attitude.pitch, in->target.attitude.roll);    
        rc->update_target_altitude(-in->target.throttle.power);
        target_pos_x = ms->r_pos_x;
        target_pos_y = ms->r_pos_y;
        target_pos_z = rc->r_altitude;
        target_velocity = ms->pos.max_spd;
        target_angle = ms->r_angle;
    }

    FlightControllerOutputType ret = ms->run(
        target_pos_x, target_pos_y, target_pos_z,
        target_angle, target_velocity,
        euler, pos, velocity, angular_rate
    );
    out.thrust = ret.thrust;
    out.torque_x = ret.torque_x;
    out.torque_y = ret.torque_y;
    out.torque_z = ret.torque_z;
    return out;
}


