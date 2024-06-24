#include "hako_module_drone_controller_impl.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <string.h>

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "DroneAvator";
}


typedef struct {
    double Kp;
    double Ki;
    double Kd;
    double integral;
    double prev_error;
    char name[256];
} MyContextType;

void* hako_module_drone_controller_impl_create_context(void* argp)
{
    auto* context = new MyContextType();
    const char* ptr = (const char*)argp;
    memset(context->name, 0, sizeof(context->name));
    strcpy(context->name, ptr);
    printf("my name is %s\n", context->name);
    return (void*)context;
}
int hako_module_drone_controller_impl_is_operation_doing(void*)
{
    return true;
}

int hako_module_drone_controller_impl_init(void* argp) {
    MyContextType* context = (MyContextType*)argp;
    char cwd[1024];

#ifdef _WIN32
    if (GetCurrentDirectory(sizeof(cwd), cwd) != 0) {
        printf("Current working dir: %s\n", cwd);
    }
    else {
        perror("GetCurrentDirectory() error");
        return -1;
    }
#else
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    }
    else {
        perror("getcwd() error");
        return -1;
    }
#endif

    FILE* file = fopen("drone_control_params.txt", "r");
    if (file == NULL) {
        perror("Failed to open params.txt");
        return -1;
    }

    char line[256];
    char param_name[256];
    char name[256];
    char param_type[3];
    double param_value;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%[^_]_%2s=%lf", name, param_type, &param_value) == 3) {
            if (strcmp(name, context->name) == 0) {
                if (strcmp(param_type, "Kp") == 0) {
                    context->Kp = param_value;
                }
                else if (strcmp(param_type, "Ki") == 0) {
                    context->Ki = param_value;
                }
                else if (strcmp(param_type, "Kd") == 0) {
                    context->Kd = param_value;
                }
            }
        }
    }

    printf("DroneController Name: %s Kp=%lf Kd=%lf Ki=%lf\n",
        context->name,
        context->Kp, context->Kd, context->Ki);

    fclose(file);

    context->integral = 0.0;
    context->prev_error = 0.0;
    return 0;
}
#define THRUST_MAX (9.81 * 5.0)
#define THRUST_MIN (0.0)
mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    MyContextType* context = (MyContextType*)in->context;
    const double target_altitude = 10.0;
    
    double error = target_altitude + in->pos_z;
    
    context->integral += error * 0.003;
    
    double derivative = (error - context->prev_error) / 0.003;
    
    double output = context->Kp * error + context->Ki * context->integral + context->Kd * derivative;
    
    context->prev_error = error;

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
