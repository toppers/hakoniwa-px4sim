#include "hako_module_drone_controller_impl.h"

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "ChatGPT4";
}

static double Kp = 0.5;
static double Ki = 0.01;
static double Kd = 0.05;

static double integral = 0.0;
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
    integral = 0.0;
    prev_error = 0.0;
    return 0;
}

mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    const double target_altitude = 10.0;
    
    double error = target_altitude + in->pos_z;
    
    integral += error * 0.003;
    
    double derivative = (error - prev_error) / 0.003;
    
    double output = Kp*error + Ki*integral + Kd*derivative;
    
    prev_error = error;
    
    mi_drone_control_out_t control_output;
    control_output.thrust = output;
    control_output.torque_x = 0.0;
    control_output.torque_y = 0.0;
    control_output.torque_z = 0.0;
    return control_output;
}
