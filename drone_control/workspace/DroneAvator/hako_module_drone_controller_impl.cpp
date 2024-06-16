#include "hako_module_drone_controller_impl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "DroneAvator";
}

static double Kp = 10;
static double Ki = 0.0;
static double Kd = 0.0;

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
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return -1;
    }

    FILE* file = fopen("drone_control_params.txt", "r");
    if (file == nullptr) {
        perror("Failed to open params.txt");
        return -1;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "DroneAvator_Kp=%lf", &Kp) == 1) continue;
        if (sscanf(line, "DroneAvator_Ki=%lf", &Ki) == 1) continue;
        if (sscanf(line, "DroneAvator_Kd=%lf", &Kd) == 1) continue;
    }
    
    fclose(file);

    integral = 0.0;
    prev_error = 0.0;
    return 0;
}
#define THRUST_MAX (9.81 * 1.1)
#define THRUST_MIN (0.0)
mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    const double target_altitude = 10.0;
    
    double error = target_altitude + in->pos_z;
    
    integral += error * 0.003;
    
    double derivative = (error - prev_error) / 0.003;
    
    double output = Kp * error + Ki * integral + Kd * derivative;
    
    prev_error = error;

    if (output > THRUST_MAX) {
        output = THRUST_MAX;
    }
    else if (output < THRUST_MIN) {
        output = THRUST_MIN;
    }
    
    mi_drone_control_out_t control_output;
    control_output.thrust = output;
    control_output.torque_x = 0.0;
    control_output.torque_y = 0.0;
    control_output.torque_z = 0.0;
    return control_output;
}
