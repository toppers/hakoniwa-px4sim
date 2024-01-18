#ifndef _BODY_PHYSICS_C_H_
#define _BODY_PHYSICS_C_H_

/**
 * C interface for body_physics.hpp
 * Identifiers are prefixed by "dp_" avoiding namespace pollution.
 * See body_physics.hpp for the C++ counter-part.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double x, y, z;
} dp_vector_t;

/* Euler Angle, NOT a VECTOR */
typedef struct {
    double phi;   // rotation round x-axis
    double theta; // rotation round y-axis
    double psi;   // rotation round z-axis
} dp_euler_t;

typedef dp_vector_t
    dp_velocity_t, dp_acceleration_t; // translation (movement) and its time derivative
typedef dp_vector_t
    dp_angular_velocity_t, dp_angular_acceleration_t; // rotation and its time derivative
typedef dp_euler_t
    dp_euler_rate_t; // time derivative of Euler Angles

/* maths for frame transformations */
dp_vector_t dp_ground_vector_from_body(
    const dp_vector_t* body, /* non-null */
    const dp_euler_t* angle /* non-null */);

dp_vector_t dp_body_vector_from_ground(
    const dp_vector_t* ground /* non-null*/,
    const dp_euler_t* angle);

dp_euler_rate_t dp_euler_rate_from_body_angular_velocity(
    const dp_angular_velocity_t* angular_rate_body_frame,
    const dp_euler_t* angle);

dp_angular_velocity_t dp_body_angular_velocity_from_euler_rate(
    const dp_vector_t* angular_rate_ground_frame,
    const dp_euler_t* angle);

dp_acceleration_t dp_acceleration_in_body_frame(
    const dp_vector_t* body_velocity,
    const dp_euler_t* angle,
    const dp_angular_velocity_t* body_angular_velocity,
    double thrust,
    double mass, /* 0 is not allowed */
    double gravity, double drag);

dp_angular_acceleration_t dp_angular_acceleration_in_body_frame(
    const dp_angular_velocity_t* body_angular_velocity,
    double torque_x, /* in body frame */
    double torque_y, /* in body frame */
    double torque_z, /* in body frame */
    double I_xx, /* in body frame, 0 is not allowed */
    double I_yy, /* in body frame, 0 is not allowed */
    double I_zz /* in body frame, 0 is not allowed */);

#ifdef __cplusplus
}
#endif

#endif /* _BODY_PHYSICS_C_H_ */