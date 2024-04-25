#ifndef _FRAME_CONVERTOR_HPP_
#define _FRAME_CONVERTOR_HPP_

#include "frame_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*
 *  Maths for frame, coordinate/angle transformations.
 */
/* vector types works for angular vectors, and velocities, accelerations */
VectorType ground_vector_from_body(
    const VectorType& body,
    const EulerType& angle);
VectorType body_vector_from_ground(
    const VectorType& ground,
    const EulerType& angle);

/* translations between anguler vector and euler rate */
EulerRateType euler_rate_from_body_angular_velocity(
    const AngularVelocityType& angular_veleocy,
    const EulerType& euler);
AngularVelocityType body_angular_velocity_from_euler_rate(
    const EulerRateType& euler_rate,
    const EulerType& euler);

#if defined(__cplusplus)
}
#endif

#endif /* _FRAME_CONVERTOR_HPP_ */
