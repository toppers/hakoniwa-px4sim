#include "hako_module_drone_controller_impl.h"
#include "heading_controller.hpp"
#include "radio_controller.hpp"
#include <algorithm>
#include <iostream>

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "RadioController";
}


void* hako_module_drone_controller_impl_create_context(void*)
{
    RadioControllerParamType param = get_radio_control_default_parameters();
    auto rc = create_radio_controller(param);
    if (rc == nullptr) {
        exit(1);
    }
    return (void*)rc;
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
    RadioController *rc = (RadioController*)in->context;
    mi_drone_control_out_t out = {};
    if (rc->r_altitude_initialized == false) {
        rc->r_altitude = -in->pos_z;
        rc->r_altitude_initialized = true;
        std::cout << "r_altitude: " << rc->r_altitude << std::endl;
    }

    //altitude control
    double throttle_value = -in->target.throttle.power;
    rc->update_target_altitude(throttle_value);
    AltitudeControlInputType a_in;
    a_in.pos = { in->pos_x, in->pos_y, in->pos_z };
    a_in.target_altitude = rc->r_altitude;
    AltitudeControlPidControlOutputType a_out = rc->alt.run(a_in);
    //std::cout << "TARGET ALTITUDE : " << a_in.target_altitude <<std::endl;
    //std::cout << "CURRENT ALTITUDE: " << -a_in.pos.z <<std::endl;

    SpeedControlInputType s_in = {};
    s_in.velocity = { in->u, in->v, in->w };
    SpeedControlPidControlOutputType s_out = {};
    if (rc->use_spd_ctrl) {
        //speed control
        s_in.target_vx = in->target.attitude.pitch * -rc->max_spd; /* -20m/s to 20m/s */
        s_in.target_vy = in->target.attitude.roll * rc->max_spd;  /* -20m/s to 20m/s */
        s_out = rc->spd.run(s_in);
        //std::cout << "TARGET VELOCITY ( " << s_in.target_vx << ", " << s_in.target_vy << " )" <<std::endl;
        //std::cout << "CURRENT VELOCITY( " << s_in.velocity.u << ", " << s_in.velocity.v << " )" <<std::endl;
        //std::cout << "TARGET  ANGLE   ( " << s_out.pitch << ", " << s_out.roll << " )" <<std::endl;
        //std::cout << "CURRENT ANGLE   ( " << in->euler_y << ", " << in->euler_x << " )" <<std::endl;
    }

    //radio control
    RadioControlInputType rin;
    rin.euler = {in->euler_x, in->euler_y, in->euler_z};                //STATE: euler
    rin.angular_rate = {in->p, in->q, in->r};                           //STATE: angular_rate
    //rin.target_thrust = -in->target.throttle.power;                   //TARGET: thrust
    rin.target_thrust = a_out.throttle_power;                           //TARGET: thrust

    if (rc->use_spd_ctrl) {
        rin.target_roll = s_out.roll;
        rin.target_pitch = s_out.pitch;

        //Heading control;
        HeadingControlInputType h_in;
        h_in.euler = rin.euler;
        rc->update_target_yaw(in->target.direction_velocity.r);
        h_in.target_angle_deg = rc->r_yaw;
        HeadingControlPidControlOutputType h_out = rc->heading.run(h_in);
        rin.target_angular_rate_r = h_out.angular_rate_r;        //TARGET: angular_rate
    }
    else {
        rin.target_roll = in->target.attitude.roll * rc->pid_param_max_roll;   //TARGET: angular.roll
        rin.target_pitch = in->target.attitude.pitch * rc->pid_param_max_pitch; //TARGET: angular.pitch
        rin.target_angular_rate_r = in->target.direction_velocity.r;        //TARGET: angular_rate
    }

    FlightControllerOutputType ret = rc->run(rin);
    out.thrust = ret.thrust;
    out.torque_x = ret.torque_x;
    out.torque_y = ret.torque_y;
    out.torque_z = ret.torque_z;
    return out;
}


