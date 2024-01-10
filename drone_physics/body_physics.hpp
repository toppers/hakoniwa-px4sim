#ifndef _BODY_PHYSICS_HPP_
#define _BODY_PHYSICS_HPP_

#ifdef BP_INCLUDE_IO /* for printint out */
#include <iostream>
#endif /* BP_INCLUDE_IO */


namespace hako::drone_physics {

/**
 * This type is used for "Vectors" in this library,
 * including relative position, velocity, acceleration, force, torque, etc.
*/
typedef struct vector_type_t {
    double x, y, z;
} VectorType;
typedef VectorType VelocityType;
typedef VectorType AccelerationType;
typedef VectorType TorqueType;
typedef VectorType ForceType;

/* angular vectors omega=(p,q,r) in x, y, z , not phi, theta, psi */
typedef VectorType AngularVectorType;
typedef AngularVectorType AngularVelocityType;
//typedef AngularVectorType AngularAccelerationType; TODO


/*
 * These Angle or Angular types are used for "Euler Angles" in this library,
 * including rotation, angular rate, angular acceleration, etc.
 * Note that the order of rotation is phi, theta, psi, and cannot be added or substracted
 * like vectors, and different from Angullar Velocity(which is a vector).
 * Angular rate/acceleration is derived from Euler angles and expressed in AngleType.
 *
 * - phi(x-rotation or roll),      -PI <= phi   < PI
 * - theta(y-rotation or pitch), -PI/2 <= theta < PI/2
 * - psi(z-rotation or yaw),       -PI <= psi   < PI,
 * 
 * but for psi, all range are possible(exceeding PI even 2PI) when traveling around circles,
 * 
 * From ground to body, Vectors are transformed in the order 
 * of psi, theta, phi. The coordinate system is right-handed, and
 * the rotation matrix is calculated as follows,
 * where v_e = (x_e, y_e, z_e)^t, v_b = (x_b, y_b, z_b)^t.
 * 
 *     v_e = R_z(psi)R_y(theta)R_x(phi) v_b
 * 
 * See https://www.sky-engin.jp/blog/eulerian-angles/
 * https://mtkbirdman.com/flight-dynamics-body-axes-system
*/

/* Euler Angles */
typedef struct angle_type_t {
    double phi;   // rotation round x-axis
    double theta; // rotation round y-axis
    double psi;   // rotation round z-axis
} AngleType;

/* angular rate is the time derivative of Euler Angles (phi', theta' psi') */
typedef AngleType AngularRateType;
typedef AngleType AngularAccelerationType;

/* basic operators */
VectorType cross(const VectorType& u, const VectorType& v);
double dot(const VectorType& u, const VectorType& v);
double length_squared(const VectorType& v);
double length(const VectorType& v);
VectorType& operator += (VectorType& u, const VectorType& v);
VectorType operator + (const VectorType& u, const VectorType& v);
VectorType& operator -= (VectorType& u, const VectorType& v);
VectorType operator - (const VectorType& u, const VectorType& v);
VectorType operator * (double s, const VectorType& v);
VectorType operator * (const VectorType& v, double s);
VectorType operator / (const VectorType& v, double s);

#ifdef BP_INCLUDE_IO /* for printint out */
std::ostream& operator << (std::ostream& os, const VectorType& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}
std::ostream& operator << (std::ostream& os, const AngleType& v) {
    os << "(" << v.phi << "r, " << v.theta << "r, " << v.psi << "r)";
    return os;
}
#endif /* BP_INCLUDE_IO */


/* maths for frame transformations */
VelocityType velocity_body_to_ground(
    const VelocityType& body,
    const AngleType& angle);
VelocityType velocity_ground_to_body(
    const VelocityType& ground,
    const AngleType& angle);

AngularRateType body_angular_velocity_to_euler_rate(
    const AngularVelocityType& angular_veleocy,
    const AngleType& euler);
AngularVelocityType euler_rate_to_body_angular_velocity(
    const AngularRateType& euler_rate,
    const AngleType& euler);

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
    const AngularRateType& body_angular_rate, /* for Coriolis */
    double thrust, double mass, /* 0 is not allowed */
    double gravity, double drag);

AccelerationType acceleration_in_ground_frame( /* no Coriolis needed */
    const VelocityType& body,
    const AngleType& angle,
    double thrust,
    double mass, /* 0 is not allowed */
    double gravity, double drag);

AngularAccelerationType angular_acceleration_in_body_frame(
    const AngularRateType& angular_rate_in_body_frame,
    double torque_x, double torque_y, double torque_z, /* in body frame */
    double I_xx, double I_yy, double I_zz /* in body frame, 0 is not allowed */);

AngularAccelerationType angular_acceleration_in_ground_frame(
    const AngularAccelerationType& angular_rate_in_ground_frame,
    const AngleType& angle,
    double torque_x, double torque_y, double torque_z, /* in BODY FRAME!! */
    double I_xx, double I_yy, double I_zz /* in BODY FRAME!! */);

/* physics for collision */

/**
 * Calculates the velocity after the collision.
 * Input vectors in the same frame, return vector in the same frame.
*/
VectorType velocity_after_contact_with_wall(
    const VectorType& velocity_before_contact,
    const VectorType& normal_vector,  /* of the wall, will be normalized internally */
    double restitution_coefficient /* 0.0 - 1.0 */);

VectorType velocity_after_contact_with_wall(
    const VectorType& velocity_before_contact,
    const VectorType& center_position,
    const VectorType& contact_position,
    double restitution_coefficient /* 0.0 - 1.0 */);

} /* namespace hako::drone_physics */

#endif /* _BODY_PHYSICS_HPP_ */
