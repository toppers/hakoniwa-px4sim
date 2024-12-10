#ifndef _BODY_PHYSICS_HPP_
#define _BODY_PHYSICS_HPP_
#include <cmath>

#ifdef BP_INCLUDE_IO /* for printint out */
#include <iostream>
#endif /* BP_INCLUDE_IO */


namespace hako::drone_physics {

const double eps = 1.0e-30; // for double values are ZERO for assertion. almost MIN_FLT.
inline bool is_zero(double a){return std::abs(a) < eps;}


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

struct QuaternionType {
    double w, x, y, z;
};
typedef QuaternionType QuaternionVelocityType;

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
inline VectorType cross(const VectorType& u, const VectorType& v)
{ return { u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x };}
inline double dot(const VectorType& u, const VectorType& v) { return u.x * v.x + u.y * v.y + u.z * v.z;}
inline double length_squared(const VectorType& v) { return dot(v, v);}
inline double length_squared(const QuaternionType& q) { return q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z;}
inline double length(const VectorType& v) { return std::sqrt(length_squared(v));}
inline double length(const QuaternionType& q) { return std::sqrt(length_squared(q));}
inline VectorType& operator += (VectorType& u, const VectorType& v) { u.x += v.x; u.y += v.y; u.z += v.z; return u;}
inline VectorType operator + (const VectorType& u, const VectorType& v) { VectorType result = u; return result += v;}
inline VectorType& operator -= (VectorType& u, const VectorType& v) { u.x -= v.x;    u.y -= v.y;    u.z -= v.z; return u;}
inline VectorType operator - (const VectorType& u, const VectorType& v) { VectorType result = u; return result -= v;}
inline VectorType operator * (double s, const VectorType& v) { return {s*v.x, s*v.y, s*v.z};}
inline VectorType operator * (const VectorType& v, double s) { return s*v;}
inline VectorType& operator /= (VectorType& v, const double s) { v.x /= s; v.y /= s; v.z /= s; return v;}
inline VectorType operator / (const VectorType& v, double s) { return {v.x/s, v.y/s, v.z/s};}
inline VectorType operator - (const VectorType& v) { return {-v.x, -v.y, -v.z};}
inline QuaternionType& operator += (QuaternionType& q1, const QuaternionType& q2) { q1.w += q2.w; q1.x += q2.x; q1.y += q2.y; q1.z += q2.z; return q1;}
inline QuaternionType operator + (const QuaternionType& q1, const QuaternionType& q2) { QuaternionType result = q1; return result += q2;}
inline QuaternionType& operator -= (QuaternionType& q1, const QuaternionType& q2) { q1.w -= q2.w; q1.x -= q2.x; q1.y -= q2.y; q1.z -= q2.z; return q1;}
inline QuaternionType operator - (const QuaternionType& q1, const QuaternionType& q2) { QuaternionType result = q1; return result -= q2;}
inline QuaternionType& operator *= (QuaternionType& q, double s) { q.w *= s; q.x *= s; q.y *= s; q.z *= s; return q;}
inline QuaternionType operator * (const QuaternionType& q, double s) { QuaternionType result = q; return result *= s;}
inline QuaternionType operator * (double s, const QuaternionType& q) { return q * s;}
inline QuaternionType& operator /= (QuaternionType& q, double s) { q.w /= s; q.x /= s; q.y /= s; q.z /= s; return q;}
inline QuaternionType operator / (const QuaternionType& q, double s) { QuaternionType result = q; return result /= s;}
inline QuaternionType operator - (const QuaternionType& q) { return {-q.w, -q.x, -q.y, -q.z};}
void normalize(QuaternionType& quaternion);

#ifdef BP_INCLUDE_IO /* for printint out */
std::ostream& operator << (std::ostream& os, const VectorType& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}
std::ostream& operator << (std::ostream& os, const EulerType& v) {
    os << "(" << (v.phi)*180/M_PI << "d, " << (v.theta)*180/M_PI << "d, " << (v.psi)*180/M_PI << "d)";
    return os;
}
std::ostream& operator << (std::ostream& os, const QuaternionType& v) {
    os << "(" << v.w << ", " << v.x << ", " << v.y << ", " << v.z << ")";
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

/* The translation dynamics. drags are vectors in the three directions */
AccelerationType acceleration_in_body_frame(
    const VelocityType& body_velocity,
    const EulerType& angle,
    const AngularVelocityType& body_angular_velocity, /* for Coriolis */
    double thrust, double mass, /* 0 is not allowed */
    double gravity, /* usually 9.8 > 0*/
    const VectorType& wind, /* wind vector in frame */
    const VectorType& drag1,   /* air friction of 1-st order(-d1*v) counter to velocity */
    const VectorType& drag2  /* air friction of 2-nd order(-d2*v*v) counter to velocity */);

/* simplified version of the above */
AccelerationType acceleration_in_body_frame(
    const VelocityType& body_velocity,
    const EulerType& angle,
    const AngularVelocityType& body_angular_velocity,
    double thrust, double mass /* 0 is not allowed */,
    double gravity, /* usually 9.8 > 0*/
    double drag1,  /* air friction of 1-st order(-d1*v) counter to velocity */
    double drag2 = 0 /* air friction of 2-nd order(-d2*v*v) counter to velocity */);


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

/* Quaternion velocity(dq/dt) from body angllar velocity */
QuaternionVelocityType quaternion_velocity_from_body_angular_velocity(
    const AngularVelocityType& body_angular_velocity,
    const QuaternionType& quaternion);

/* Quaternion from euler angle */
QuaternionType quaternion_from_euler(const EulerType& euler);

/* Euler angle from Quaternion */
EulerType euler_from_quaternion(const QuaternionType& quaternion);

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
