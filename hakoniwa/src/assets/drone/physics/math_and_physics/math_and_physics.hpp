#ifndef MATH_AND_PHYSICS_HPP_
#define MATH_AND_PHYSICS_HPP_
#include <cmath>
#include <tuple>

typedef std::tuple<double, double, double> 
PositionType,  VelocityType, AccelerationType,
AngleType, AngularVelocityType, AngularAccelerationType;

/* maths */
/* think about the sin/cos cache later... check working right first */

VelocityType velocity_body_to_ground(
    const VelocityType& body,
    const AngleType& angle);
VelocityType velocity_ground_to_body(
    const VelocityType& ground,
    const AngleType& angle);

AngularVelocityType angular_velocity_body_to_ground(
    const AngularVelocityType& angular_veleocy,
    const AngleType& angle);
AngularVelocityType angular_velocity_ground_to_body(
    const AngularVelocityType& angular_velocity_ground_frame,
    const AngleType& angle);

/* physics */
AccelerationType acceleration_in_body_frame(
    const VelocityType& body,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag);

AccelerationType acceleration_in_ground_frame(
    const VelocityType& body,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag);

AngularAccelerationType angular_acceleration_in_body_frame(
    const AngularVelocityType& angular_velocity_in_body_frame,
    const AngleType& angle,
    double torque_x, /* in body frame */
    double torque_y, /* in body frame */
    double torque_z, /* in body frame */
    double inertia_x, /* in body frame */
    double inertia_y, /* in body frame */
    double inertia_z /* in body frame */);

/* difficult not well-implemented yet !!!*/
#if 0
AngularAccelerationType angular_acceleration_in_ground_frame(
    const AngularVelocityType& angular_velocity_in_ground_frame,
    const AngleType& angle,
    double torque_x, /* in ground frame */
    double torque_y, /* in ground frame */
    double torque_z, /* in ground frame */
    double inertia_x, /* in body frame */
    double inertia_y, /* in body frame */
    double inertia_z /* in body frame */);

#endif
#endif /* MATH_AND_PHYSICS_HPP_ */
