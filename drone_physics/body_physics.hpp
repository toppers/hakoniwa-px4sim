#ifndef _BODY_PHYSICS_HPP_
#define _BODY_PHYSICS_HPP_
#include <tuple>

namespace hako::drone_physics {

typedef std::tuple<double, double, double> VectorType;
typedef VectorType
    VelocityType, AccelerationType, AngleType, AngularVelocityType, AngularAccelerationType;

/* basic operators */
VectorType cross(const VectorType& u, const VectorType& v);
VectorType& operator += (VectorType& u, const VectorType& v);
VectorType operator + (const VectorType& u, const VectorType& v);


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
AccelerationType acceleration_in_body_frame_without_Coriolis_for_testing_only(
    const VelocityType& body,
    const AngleType& angle,
    double thrust, double mass, /* 0 is not allowed */
    double gravity, double drag);

/* replaces the above with Coriolis's Force */
AccelerationType acceleration_in_body_frame(
    const VelocityType& body_velocity,
    const AngleType& angle,
    const AngularVelocityType& body_angular_velocity,
    double thrust, double mass, /* 0 is not allowed */
    double gravity, double drag);

AccelerationType acceleration_in_ground_frame(
    const VelocityType& body,
    const AngleType& angle,
    double thrust,
    double mass, /* 0 is not allowed */
    double gravity, double drag);

AngularAccelerationType angular_acceleration_in_body_frame(
    const AngularVelocityType& angular_velocity_in_body_frame,
    double torque_x, double torque_y, double torque_z, /* in body frame */
    double I_xx, double I_yy, double I_zz /* in body frame, 0 is not allowed */);

#endif /* _BODY_PHYSICS_HPP_ */

} /* namespace hako::drone_physics */
