#ifndef _HAKO_MODULE_CONTROLLER_H_
#define _HAKO_MODULE_CONTROLLER_H_

typedef struct {
    double pos_x;
    double pos_y;
    double pos_z;
    double euler_x;
    double euler_y;
    double euler_z;
} mi_drone_control_in_t;

typedef struct {
    double thrust;
    double torque_x;
    double torque_y;
    double torque_z;
} mi_drone_control_out_t;

#define HAKO_MODULE_DRONE_CONTROLLER_SYMBOLE_NAME   "hako_module_drone_controller"
typedef struct {
    bool (*init) (void* context);
    mi_drone_control_out_t (*run) (mi_drone_control_in_t *in);
} HakoModuleDroneControllerType;

#endif /* _HAKO_MODULE_CONTROLLER_H_ */
