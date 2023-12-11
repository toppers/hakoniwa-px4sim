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
AccelerationType Acceleration_in_body_frame(
    const VelocityType& body,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag);

AccelerationType Acceleration_in_ground_frame(
    const VelocityType& body,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag);

AngularAccelerationType Angular_acceleration_in_body_frame(
    const AngularVelocityType& angular_velocity,
    const AngleType& angle,
    double trust, double mass, double inertia_x, double inertia_y, double inertia_z);

#endif /* MATH_AND_PHYSICS_HPP_ */
