#ifndef _ROTOR_PHYSICS_HPP_
#define _ROTOR_PHYSICS_HPP_

#include "body_physics.hpp"

namespace hako::drone_physics {

/**
 * These functions are rotor dynamics.
 * Treats the rotor as a single unit.
 */

/* The rotor dynamics modeled as a first-order lag system. eq.(2.48) */
double rotor_omega_acceleration(
    double Kr, /* gain constant */
    double Tr, /* time constant */
    double omega, /* in radian/sec */
    double duty_rate /* 0.0-1.0 (ratio of PWM) */ );

/* realistic model for rotor */
double rotor_omega_acceleration(
    double Vbat, /* battery voltage in volt [V]*/
    double R, /* resistance in ohm [V/A] */
    double Cq, /* torque coeff (= B param in anti-torque) [N ms^2/rad^2] */
    double J, /* propeller inertia in [kg m^2] */
    double K, /* back electromotive force coeff in [N m/A] */
    double D, /* Kinematic viscosity coefficient [Nms/rad] */
    double omega, /* in radian/sec */
    double duty_rate /* 0.0-1.0 (ratio of PWM) */);

/* thrust from omega in radian/sec eq.(2.50)*/
double rotor_thrust(
    double A, /* parameter A in Trust = A*(Omega)^2 */
    double omega /* in radian/sec */ );

/* this makes z-axis rotation eq.(2.56) */
/* Ta = B*(Omega)^2 + Jr* (d(Omega)/dt) */
double rotor_anti_torque(
    double B,
    double Jr,
    double omega, /* in radian/sec */
    double omega_acceleratoin, /* in radian/s^2 */
    double ccw /* 1 or -1 */ );

/**
 * These functions are body dynamics.
 * Treats one body with multiple rotors.
 */

/* the sum of the n trust from the rotors eq.(2.61) */
double body_thrust(
    double A, /* parameter A in Trust = A*(Omega)^2 in each motor */
    unsigned n, /* number of rotors */
    double omega[] /* in radian/sec */ );

/* the sum of the n torques from the rotors including anti-torque */
/* eq.(2.60)-(2.62)*/
TorqueType body_torque(
    double A, /* parameter A in Trust = A*(Omega)^2 */
    double B, /* parameter B in Ta = B*(Omega)^2 + Jr* (d(Omega)/dt) */
    double Jr,
    unsigned n, /* number of rotors */
    VectorType position[], /* position of each rotor */
    double ccw[], /* 1 or -1 */
    double omega[], /* in radian/sec */
    double omega_acceleration[] /* in radian/s^2 */ );


double rotor_thrust_linear(
    double A, /* the A parameter in Trust = A*(Omega) */
    double omega /* in radian/sec */ );
double rotor_anti_torque_linear(double B2, double omega, double ccw);
TorqueType body_torque_linear(double A2, double B2, unsigned n,
    VectorType position[], double ccw[], double omega[]);
double body_thrust_linear(double A2, unsigned n, double omega[]);



} /* namespace hako::drone_physics */

#endif /* _ROTOR_PHYSICS_HPP_ */