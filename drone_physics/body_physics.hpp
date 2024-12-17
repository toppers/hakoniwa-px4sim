#ifndef _BODY_PHYSICS_HPP_
#define _BODY_PHYSICS_HPP_



#include "drone_frames.hpp"


namespace hako::drone_physics {

/*
 *  Maths for anglular velocity and euler rate.
 */
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
