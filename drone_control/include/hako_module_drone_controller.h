#ifndef _HAKO_MODULE_CONTROLLER_H_
#define _HAKO_MODULE_CONTROLLER_H_

typedef struct {
    double roll;
    double pitch;
} mi_drone_control_in_attitude_t;
typedef struct {
    double power;
} mi_drone_control_in_throttle_t;
typedef struct {
    double r;
} mi_drone_control_in_direction_velocity_t;

typedef struct {
    double x;
    double y;
} mi_drone_control_in_position_t;
typedef struct {
    double height;
} mi_drone_control_in_altitude_t;

typedef struct {
    int radio_control; /* 0: off, 1: on */
    /*
     * Radio control
     */
    mi_drone_control_in_attitude_t attitude;
    mi_drone_control_in_throttle_t throttle;
    mi_drone_control_in_direction_velocity_t direction_velocity;
    /*
     * Position control
     */
    mi_drone_control_in_position_t position;
    mi_drone_control_in_altitude_t altitude;
} mi_drone_control_in_target_t;

typedef struct {
    void *context;
    int radio_control;
    double target_yaw_deg;
    /* deprecated : start */
    /*
     * Target
     */
    double target_pos_x;
    double target_pos_y;
    double target_pos_z;
    double target_velocity;
    /* deprecated : end */

    /*
     * Control information
     */
    mi_drone_control_in_target_t target;

    /*
     * Drone's state
     */
    double mass;
    double drag;
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
    void* (*create_context) (void* arguments);
    int (*is_operation_doing) (void* context);
    int (*init) (void* context);
    mi_drone_control_out_t (*run) (mi_drone_control_in_t *in);
} HakoModuleDroneControllerType;

#endif /* _HAKO_MODULE_CONTROLLER_H_ */
