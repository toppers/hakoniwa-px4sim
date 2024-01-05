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

typedef struct {
    double phi;   // rotation round x-axis
    double theta; // rotation round y-axis
    double psi;   // rotation round z-axis
} dp_angle_t;

typedef dp_vector_t
    dp_velocity_t, dp_acceleration_t;
typedef dp_angle_t
    dp_angular_velocity_t, dp_angular_acceleration_t;

/* maths for frame transformations */
dp_velocity_t dp_velocity_body_to_ground(
    const dp_velocity_t* body, /* non-null */
    const dp_angle_t* angle /* non-null */);

dp_velocity_t dp_velocity_ground_to_body(
    const dp_velocity_t* ground /* non-null*/,
    const dp_angle_t* angle);

dp_angular_velocity_t dp_angular_velocity_body_to_ground(
    const dp_vector_t* angular_velocity_body_frame,
    const dp_angle_t* angle);

dp_angular_velocity_t dp_angular_velocity_ground_to_body(
    const dp_vector_t* angular_velocity_ground_frame,
    const dp_angle_t* angle);

#ifdef __cplusplus
}
#endif

#endif /* _BODY_PHYSICS_C_H_ */