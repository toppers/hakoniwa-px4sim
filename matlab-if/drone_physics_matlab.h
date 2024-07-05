#ifndef _DRONE_PHYSICS_MATLAB_H_
#define _DRONE_PHYSICS_MATLAB_H_

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * These types are used for matlab interface of drone_phiysics.
 * currently, only acceleration and anguler acceleration are supported.
 * But other types can be added in the future, including sensors, actuators, etc.
 * in order to support simulation of the drone parts and the whole drone.
 * 
 * mi .. prefix for matlab interface
 */
typedef struct mi_drone_acceleration_in_t {
    double phi;   /* euler angle x */
    double theta; /* euler angle y */
    double psi;   /* euler angle z */

    double u; /* velocity x */
    double v; /* velocity y */
    double w; /* velocity z */

    double p; /* anglular velocity x */
    double q; /* anglular velocity y */
    double r; /* anglular velocity z */

    /* force and torque */
    double thrust;
    double torque_x;
    double torque_y;
    double torque_z;

    /* other constants */
    double mass;
    double Ixx;
    double Iyy;
    double Izz;
    double gravity;
    double drag;
} mi_drone_acceleration_in_t;

typedef struct mi_drone_acceleration_out_t {
    double du; /* dot u = dotdot x = x - acceleration */
    double dv; /* dot v = dotdot y = y - acceleration */
    double dw; /* dot w = dotdot z = z - acceleration */

    double dp; /* dot p  = x coordinate of angular vector acceleration */
    double dq; /* dot q  = y coordinate of angular vector acceleration */
    double dr; /* dot r  = z coordinate of angular vector acceleration */
} mi_drone_acceleration_out_t;

/* Matlab function entry point */
mi_drone_acceleration_out_t mi_drone_acceleration(
    const mi_drone_acceleration_in_t* in);
void mi_drone_acceleration_initialize(void);

mi_drone_acceleration_out_t drone_acceleration_by_linear_at_hover(
    const mi_drone_acceleration_in_t* in);

/***
The Original function signatures are:

// acceleration in ground frame based on m a = F + m g ...eq.(1.136)
AccelerationType acceleration_in_body_frame(
    const VelocityType& body_velocity,
    const EulerType& angle,
    const AngularVelocityType& body_angular_velocity, // for Coriolis
    double thrust, double mass, // 0 is not allowed
    double gravity, double drag);

// angular acceleration in body frame based on JW' = W x JW =Tb ...eq.(1.137),(2.31) 
AngularAccelerationType angular_acceleration_in_body_frame(
    const AngularVelocityType& body_angular_velocity,
    double torque_x, // in body frame 
    double torque_y, // in body frame 
    double torque_z, // in body frame 
    double I_xx, // in body frame, 0 is not allowed 
    double I_yy, // in body frame, 0 is not allowed 
    double I_zz // in body frame, 0 is not allowed );
*/

#if defined(__cplusplus)
}
#endif

#endif /* _DRONE_PHYSICS_MATLAB_H_ */