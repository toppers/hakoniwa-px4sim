#include "hako_module_drone_controller_impl.h"
#include "move_position_controller.hpp"
#include <algorithm>
#include <iostream>

//TODO


const char* hako_module_drone_controller_impl_get_name(void)
{
    return "RadioController";
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

    //position control
    double pos_x = -in->target.attitude.pitch;
    double pos_y = in->target.attitude.roll;
    ms->update_target_pos(pos_x, pos_y);
    PositionControlInputType p_in;
    p_in.euler = {in->euler_x, in->euler_y, in->euler_z};
    p_in.pos = {in->pos_x, in->pos_y, in->pos_z};
    p_in.target_spd = PID_PARAM_MAX_SPD;
    p_in.target_x = ms->r_pos_x;
    p_in.target_y = ms->r_pos_y;
    PositionControlPidControlOutputType p_out = ms->pos.run(p_in);
    std::cout << "TARGET POS ( " << p_in.target_x << ", " << p_in.target_y << " )" <<std::endl;
    std::cout << "CURRENT POS( " << p_in.pos.x << ", " << p_in.pos.y << " )" <<std::endl;
    std::cout << "TARGET  VEL( " << p_out.target_vx << ", " << p_out.target_vy << " )" <<std::endl;
    std::cout << "CURRENT VEL( " << in->u << ", " << in->v << " )" <<std::endl;

    //altitude control
    double throttle_value = -in->target.throttle.power;
    rc->update_target_altitude(throttle_value);
    AltitudeControlInputType a_in;
    a_in.pos = { in->pos_x, in->pos_y, in->pos_z };
    a_in.target_altitude = rc->r_altitude;
    AltitudeControlPidControlOutputType a_out = rc->alt.run(a_in);

    //speed control
    SpeedControlInputType s_in;
    //s_in.target_vx = in->target.attitude.pitch * -PID_PARAM_MAX_SPD; /* -20m/s to 20m/s */
    //s_in.target_vy = in->target.attitude.roll * PID_PARAM_MAX_SPD;  /* -20m/s to 20m/s */
    s_in.target_vx = p_out.target_vx;
    s_in.target_vy = p_out.target_vy;
    s_in.velocity = { in->u, in->v, in->w };
    SpeedControlPidControlOutputType s_out = rc->spd.run(s_in);

    //radio control
    RadioControlInputType rin;
    rin.euler = {in->euler_x, in->euler_y, in->euler_z};                //STATE: euler
    rin.angular_rate = {in->p, in->q, in->r};                           //STATE: angular_rate
    rin.target_thrust = a_out.throttle_power;                           //TARGET: thrust
    rin.target_roll = s_out.roll;
    rin.target_pitch = s_out.pitch;
    rin.target_angular_rate_r = in->target.direction_velocity.r;        //TARGET: angular_rate

    FlightControllerOutputType ret = rc->run(rin);
    out.thrust = ret.thrust;
    out.torque_x = ret.torque_x;
    out.torque_y = ret.torque_y;
    out.torque_z = ret.torque_z;
    return out;
}


