#ifndef _HAKO_MODULE_CONTROLLER_H_
#define _HAKO_MODULE_CONTROLLER_H_

typedef struct {
    /*
     * Target
     */
    double target_pos_x;
    double target_pos_y;
    double target_pos_z;
    double target_velocity;

    /*
     * Drone's state
     */
    //position
    double pos_x;
    double pos_y;
    double pos_z;
    //angle
    double euler_x;
    double euler_y;
    double euler_z;
    // velocity
    double u; /* velocity x in body frame */
    double v; /* velocity y in body frame */
    double w; /* velocity z in body frame */
    // angular velocity
    double p; /* anglular velocity x in body frame */
    double q; /* anglular velocity y in body frame */
    double r; /* anglular velocity z in body frame */
} mi_drone_control_in_t;

typedef struct {
    double thrust;
    double torque_x;
    double torque_y;
    double torque_z;
} mi_drone_control_out_t;

#define HAKO_MODULE_DRONE_CONTROLLER_SYMBOLE_NAME   "hako_module_drone_controller"
typedef struct {
    int (*init) (void* context);
    mi_drone_control_out_t (*run) (mi_drone_control_in_t *in);
} HakoModuleDroneControllerType;

#endif /* _HAKO_MODULE_CONTROLLER_H_ */
