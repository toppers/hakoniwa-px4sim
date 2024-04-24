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
    PidControl *roll = new PidControl(PID_PARM_ROLL_Kp, PID_PARM_ROLL_Ki, PID_PARM_ROLL_Kd, 0, SIMULATION_DELTA_TIME);
    if (roll == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        exit(1);
    }
    PidControl *pitch = new PidControl(PID_PARM_PITCH_Kp, PID_PARM_PITCH_Ki, PID_PARM_PITCH_Kd, 0, SIMULATION_DELTA_TIME);
    if (pitch == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        exit(1);
    }
    PidControl *roll_rate = new PidControl(PID_PARM_ROLL_RATE_Kp, PID_PARM_ROLL_RATE_Ki, PID_PARM_ROLL_RATE_Kd, 0, SIMULATION_DELTA_TIME);
    if (roll_rate == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        exit(1);
    }
    PidControl *pitch_rate = new PidControl(PID_PARM_PITCH_RATE_Kp, PID_PARM_PITCH_RATE_Ki, PID_PARM_PITCH_RATE_Kd, 0, SIMULATION_DELTA_TIME);
    if (pitch_rate == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        exit(1);
    }
    PidControl *yaw_rate = new PidControl(PID_PARM_YAW_RATE_Kp, PID_PARM_YAW_RATE_Ki, PID_PARM_YAW_RATE_Kd, 0, SIMULATION_DELTA_TIME);
    if (yaw_rate == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        exit(1);
    }
    RadioController *rc = new RadioController(roll, pitch, roll_rate, pitch_rate, yaw_rate, SIMULATION_DELTA_TIME);
    if (rc == nullptr) {
        std::cerr << "ERROR: can not allocate memory..." << std::endl;
        exit(1);
    }
    rc->set_angular_cycle(ANGULAR_CYCLE);
    rc->set_angular_rate_cycle(ANGULAR_RATE_CYCLE);
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
    out.thrust = (in->mass * 9.81) - (THROTTLE_GAIN * in->target.throttle.power);

    // angle control
    PidControlInputType roll_angle;
    roll_angle.current = in->euler_x;
    roll_angle.target = in->target.attitude.roll;
    PidControlInputType pitch_angle;
    pitch_angle.current = in->euler_y;
    pitch_angle.target = in->target.attitude.pitch;
    PidControlOutputType ret_angle = rc->run_angular_control(roll_angle, pitch_angle);

    // rate control
    PidControlInputType roll;
    roll.current = in->p;
    roll.target = ret_angle.roll_rate;
    PidControlInputType pitch;
    pitch.current = in->q;
    pitch.target = ret_angle.pitch_rate;
    PidControlInputType yaw;
    yaw.current = in->r;
    yaw.target = in->target.direction_velocity.r;
    PidRateControlOutputType ret = rc->run_angular_rate_control(roll, pitch, yaw);
    out.torque_x = ret.p;
    out.torque_y = ret.q;
    out.torque_z = ret.r;
    return out;
}


