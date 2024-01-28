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
struct VectorType {
    double x, y, z;
};
typedef VectorType VelocityType;
typedef VectorType AccelerationType;
typedef VectorType TorqueType;
typedef VectorType ForceType;

/* angular VECTORS omega=(p,q,r) in x, y, z , NOT PHI, THTEA, PSI */
typedef VectorType AngularVectorType;
typedef AngularVectorType AngularVelocityType;
typedef AngularVectorType AngularAccelerationType;

/*
 * The Euler types below are used for "Euler Angles" in this library,
 * including rotation, rotation rate, rotation acceleration, etc.
 * Note that they CANNOT be added or substracted like vectors, 
 * and different from Angullar Velocity above (which is a vector).
 * 
 * - phi(x-rotation or roll),      -PI <= phi   < PI
 * - theta(y-rotation or pitch), -PI/2 <= theta < PI/2
 * - psi(z-rotation or yaw),       -PI <= psi   < PI,
 * 
 * but for psi, all range are possible(exceeding PI even 2PI) when
 * traveling around circles,
 * 
 * From ground to body, Vectors are transformed in the order 
 * of psi, theta, phi. The coordinate system is right-handed, and
 * the rotation matrix is calculated as follows,
 * where
 * 
 * - v_e = (x_e, y_e, z_e)^t  .. ground coordinate values
 * - v_b = (x_b, y_b, z_b)^t  .. body coordinate values
 * 
 *     v_e = R_z(psi)R_y(theta)R_x(phi) v_b
 * 
 * See https://www.sky-engin.jp/blog/eulerian-angles/
 * https://mtkbirdman.com/flight-dynamics-body-axes-system
*/

/* Euler Angles (NOT A VECTOR) */
struct EulerType {
    double phi;   // rotation round x-axis
    double theta; // rotation round y-axis
    double psi;   // rotation round z-axis
};
/* angular rate is the time derivative of Euler Angles (phi', theta' psi') */
typedef EulerType EulerRateType;
typedef EulerType EulerAccelerationType;

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
std::ostream& operator << (std::ostream& os, const EulerType& v) {
    os << "(" << v.phi << "r, " << v.theta << "r, " << v.psi << "r)";
    return os;
}
#endif /* BP_INCLUDE_IO */


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

/*
 *  Dynamics(differential quuations) for accelertion from force and torque.
 */
AccelerationType acceleration_in_ground_frame(
    const VelocityType& ground,
    const EulerType& angle,
    double thrust, double mass /* 0 is not allowed */,
    double gravity, /* usually 9.8 > 0*/
    double drag1,   /* air friction of 1-st order(-d1*v) counter to velocity */
    double drag2 = 0.0 /* air friction of 2-nd order(-d2*v*v) counter to velocity */);


/* physics for Force/Mass(F= ma) and Torque/Inertia(I dw/dt = T - w x Iw) */
AccelerationType acceleration_in_body_frame_without_Coriolis_for_testing_only(
    const VelocityType& body,
    const EulerType& angle,
    double thrust, double mass, /* 0 is not allowed */
    double gravity, double drag);

/* The right dynamics including Coriolis's Force */
AccelerationType acceleration_in_body_frame(
    const VelocityType& body_velocity,
    const EulerType& angle,
    const AngularVelocityType& body_angular_velocity, /* for Coriolis */
    double thrust, double mass, /* 0 is not allowed */
    double gravity, /* usually 9.8 > 0*/
    double drag1,   /* air friction of 1-st order(-d1*v) counter to velocity */
    double drag2 = 0.0 /* air friction of 2-nd order(-d2*v*v) counter to velocity */);

/* angular acceleration in body frame based on JW' = W x JW =Tb ...eq.(1.137),(2.31) */
AngularAccelerationType angular_acceleration_in_body_frame(
    const AngularVelocityType& body_angular_velocity,
    const TorqueType& torque, /* in body frame */
    double I_xx, /* in body frame, 0 is not allowed */
    double I_yy, /* in body frame, 0 is not allowed */
    double I_zz /* in body frame, 0 is not allowed */);

/* euler angle acceleration(dd phi, dd theta, dd psi) */
EulerAccelerationType euler_acceleration_in_ground_frame(
    const EulerRateType& current_euler_rate,
    const EulerType& current_euler,
    const TorqueType& torque, /* in BODY FRAME!! */
    double I_xx, double I_yy, double I_zz /* in BODY FRAME!! */);

/**
 * Physics for collision with wall(walls don't move).
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
