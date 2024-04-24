#include "hako_module_drone_controller_impl.h"
#include "radio_controller.hpp"
#include <algorithm>
#include <iostream>

//TODO


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

    RadioControlInputType rin;
    rin.euler = {in->euler_x, in->euler_y, in->euler_z};                //STATE: euler
    rin.angular_rate = {in->p, in->q, in->r};                           //STATE: angular_rate
    rin.target_thrust = in->target.throttle.power;                      //TARGET: thrust
    rin.target_roll = in->target.attitude.roll;                         //TARGET: angular.roll
    rin.target_pitch = in->target.attitude.pitch;                       //TARGET: angular.pitch
    rin.target_angular_rate_r = in->target.direction_velocity.r;        //TARGET: angular_rate
    
    FlightControllerOutputType ret = rc->run(rin);
    out.thrust = ret.thrust;
    out.torque_x = ret.torque_x;
    out.torque_y = ret.torque_y;
    out.torque_z = ret.torque_z;
    return out;
}


