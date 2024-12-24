#ifndef _AIRCRAFT_CONTROLLER_TYPES_H_
#define _AIRCRAFT_CONTROLLER_TYPES_H_

typedef struct {
    double roll;
    double pitch;
} mi_aircraft_control_in_attitude_t;
typedef struct {
    double power;
} mi_aircraft_control_in_throttle_t;
typedef struct {
    double r;
} mi_aircraft_control_in_direction_velocity_t;

typedef struct {
    double x;
    double y;
} mi_aircraft_control_in_position_t;
typedef struct {
    double height;
} mi_aircraft_control_in_altitude_t;

#define HAKO_AIRCRAFT_MAX_ROTOR_NUM    16
typedef struct {
    int num;
    double controls[HAKO_AIRCRAFT_MAX_ROTOR_NUM];
} mi_aircraft_control_rotor_t;

typedef struct {
    int radio_control; /* 0: off, 1: on */
    /*
     * Radio control
     */
    mi_aircraft_control_in_attitude_t attitude;
    mi_aircraft_control_in_throttle_t throttle;
    mi_aircraft_control_in_direction_velocity_t direction_velocity;
    /*
     * Position control
     */
    mi_aircraft_control_in_position_t position;
    mi_aircraft_control_in_altitude_t altitude;
} mi_aircraft_control_in_target_t;

typedef struct {
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
    mi_aircraft_control_in_target_t target;

    /*
     * Drone's state
     */
    double mass;
    double drag;
    double max_rpm;
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
} mi_aircraft_control_in_t;

typedef struct {
    double thrust;
    double torque_x;
    double torque_y;
    double torque_z;
    /*
     * Rotor control
     */
    mi_aircraft_control_rotor_t rotor;
} mi_aircraft_control_out_t;


#endif /* _AIRCRAFT_CONTROLLER_TYPES_H_ */
