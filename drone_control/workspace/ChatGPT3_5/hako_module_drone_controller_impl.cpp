#include "hako_module_drone_controller_impl.h"

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "ChatGPT3_5";
}

#define KP 0.1
#define KI 0.01
#define KD 0.05

#define TARGET_ALTITUDE 10.0

static double integral_error = 0.0;
static double prev_error = 0.0;

void* hako_module_drone_controller_impl_create_context(void*)
{
    return nullptr;
}
int hako_module_drone_controller_impl_is_operation_doing(void*)
{
    return true;
}

int hako_module_drone_controller_impl_init(void* context)
{
    return 0;
}

mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    mi_drone_control_out_t out;

    double current_altitude = in->pos_z;
    //original code: bug
    //double error = TARGET_ALTITUDE - current_altitude;
    //fixed code
    double error = TARGET_ALTITUDE + current_altitude;
    double proportional = KP * error;
    integral_error += error;
    double integral = KI * integral_error;
    double derivative = KD * (error - prev_error);
    double control_signal = proportional + integral + derivative;

    out.thrust = control_signal;
    out.torque_x = 0.0;
    out.torque_y = 0.0;
    out.torque_z = 0.0;

    prev_error = error;

    return out;
}
