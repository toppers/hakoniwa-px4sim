#ifndef _BODY_PHYSICS_C_H_
#define _BODY_PHYSICS_C_H_

/**
 * C interface for body_physics.hpp
 * Identifiers are prefixed by "dp_" avoiding namespace pollution.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double x, y, z;
} dp_vector_t;

typedef dp_vector_t
    dp_velocity_t, dp_acceleration_t, dp_angle_t, dp_angular_velocity_t, dp_angular_acceleration_t;

/* maths for frame transformations */
dp_velocity_t dp_velocity_body_to_ground(
    const dp_velocity_t* body, /* non-null */
    const dp_angle_t* angle /* non-null */);

dp_velocity_t velocity_ground_to_body(
    const dp_velocity_t* ground /* non-null*/,
    const dp_angle_t* angle);

#ifdef __cplusplus
}
#endif

#endif /* _BODY_PHYSICS_C_H_ */