#include "hako_module_drone_controller_impl.h"
#include "radio_controller.hpp"
#include <algorithm>
#include <iostream>

//TODO
#define SIMULATION_DELTA_TIME     0.003 // 333.3Hz
#define ANGULAR_RATE_CYCLE        SIMULATION_DELTA_TIME // 333.3Hz
#define ANGULAR_CYCLE             (SIMULATION_DELTA_TIME * 10.0) // 33.3Hz

#define THROTTLE_GAIN 0.3

#define PID_PARM_ROLL_Kp        100.0
#define PID_PARM_ROLL_Ki        0.1
#define PID_PARM_ROLL_Kd        100.0
#define PID_PARM_PITCH_Kp       100.0
#define PID_PARM_PITCH_Ki       0.1
#define PID_PARM_PITCH_Kd       100.0

#define PID_PARM_ROLL_RATE_Kp     0.001
#define PID_PARM_ROLL_RATE_Ki     0.0001
#define PID_PARM_ROLL_RATE_Kd     0.001
#define PID_PARM_PITCH_RATE_Kp    0.001
#define PID_PARM_PITCH_RATE_Ki    0.0001
#define PID_PARM_PITCH_RATE_Kd    0.001
#define PID_PARM_YAW_RATE_Kp      0.01
#define PID_PARM_YAW_RATE_Ki      0.001
#define PID_PARM_YAW_RATE_Kd      0.01

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "RadioController";
}


void* hako_module_drone_controller_impl_create_context(void*)
{
    RadioControllerParamType param;
    param.delta_time = SIMULATION_DELTA_TIME;
    param.throttle_gain = THROTTLE_GAIN;
    param.angular_cycle = ANGULAR_CYCLE;
    param.angular_rate_cycle = ANGULAR_RATE_CYCLE;
    param.roll.Kp = PID_PARM_ROLL_Kp;
    param.roll.Ki = PID_PARM_ROLL_Ki;
    param.roll.Kd = PID_PARM_ROLL_Kd;
    param.pitch.Kp = PID_PARM_PITCH_Kp;
    param.pitch.Ki = PID_PARM_PITCH_Ki;
    param.pitch.Kd = PID_PARM_PITCH_Kd;
    param.roll_rate.Kp = PID_PARM_ROLL_RATE_Kp;
    param.roll_rate.Ki = PID_PARM_ROLL_RATE_Ki;
    param.roll_rate.Kd = PID_PARM_ROLL_RATE_Kd;
    param.pitch_rate.Kp = PID_PARM_PITCH_RATE_Kp;
    param.pitch_rate.Ki = PID_PARM_PITCH_RATE_Ki;
    param.pitch_rate.Kd = PID_PARM_PITCH_RATE_Kd;
    param.yaw_rate.Kp = PID_PARM_YAW_RATE_Kp;
    param.yaw_rate.Ki = PID_PARM_YAW_RATE_Ki;
    param.yaw_rate.Kd = PID_PARM_YAW_RATE_Kd;
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

    //control
    out.thrust = rc->run_thrust_control(in->mass, 9.81, in->target.throttle.power);
    PidControlOutputType ret_angle = rc->run_angular_control({in->euler_x, in->target.attitude.roll}, {in->euler_y, in->target.attitude.pitch});
    PidRateControlOutputType ret = rc->run_angular_rate_control({in->p, ret_angle.roll_rate}, {in->q, ret_angle.pitch_rate}, {in->r, in->target.direction_velocity.r});
    out.torque_x = ret.p;
    out.torque_y = ret.q;
    out.torque_z = ret.r;
    return out;
}


