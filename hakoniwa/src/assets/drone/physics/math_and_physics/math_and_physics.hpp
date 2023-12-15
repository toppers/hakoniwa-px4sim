#ifndef MATH_AND_PHYSICS_HPP_
#define MATH_AND_PHYSICS_HPP_
#include <cmath>
#include <tuple>

typedef std::tuple<double, double, double> 
PositionType,  VelocityType, AccelerationType,
AngleType, AngularVelocityType, AngularAccelerationType;

/* NOTE: for AnguleType, 
 * <0> phi(x-rotation or roll),      -PI <= phi   < PI
 * <1> theta(y-rotation or pitch), -PI/2 <= theta < PI/2
 * <2> psi(z-rotation or yaw),       -PI <= psi   < PI
 * and (0,0,0) is the initial state.
 * See https://www.sky-engin.jp/blog/eulerian-angles/
*/

/* maths for frame transformations */
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

/* physics for Force/Mass(F= ma) and Torque/Inertia(I dw/dt = T - w x Iw) */
AccelerationType acceleration_in_body_frame(
    const VelocityType& body,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag);

/* soon replaces the above */
AccelerationType acceleration_in_body_frame2(
    const VelocityType& body_velocity,
    const AngleType& angle,
    const AngularVelocityType& body_angular_velocity,
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
    double I_xx, /* in body frame, 0 is not allowed */
    double I_yy, /* in body frame, 0 is not allowed */
    double I_zz /* in body frame, 0 is not allowed */);

/* difficult not well-implemented yet !!! */
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
