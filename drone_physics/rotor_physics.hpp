#ifndef _ROTOR_PHYSICS_HPP_
#define _ROTOR_PHYSICS_HPP_

#include "body_physics.hpp"

namespace hako::drone_physics {

typedef VectorType TorqueType;

/**
 * These functions are rotor dynamics.
 * Treats the rotor as a single unit.
 */

/* The rotor dynamics modeled as a first-order lag system. eq.(2.48) */
double rotor_omega_acceleration(
    double Kr, /* gain constant */
    double Tr, /* time constant */
    double omega, /* in rpm */
    double duty_rate /* 0-1 of PWM */
    );

/* thrust from omega in rpm eq.(2.50)*/
double rotor_thrust(
    double A, /* parameter A in Trust = A*(Omega)^2 */
    double omega /* in rpm */ );

/* this makes z-axis rotation eq.(2.56) */
/* Ta = B*(Omega)^2 + Jr* (d(Omega)/dt) */
double rotor_anti_torque(
    double B,
    double Jr,
    double omega, /* in rpm */
    double omega_acceleratoin, /* in rpm/s */
    double ccw /* 1 or -1 */ );

/**
 * These functions are body dynamics.
 * Treats one body with multiple rotors.
 */

/* the sum of the n trust from the rotors eq.(2.61) */
double body_thrust(
    double A, /* parameter A in Trust = A*(Omega)^2 in each motor */
    unsigned n, /* number of rotors */
    double omega[] /* in rpm */ );

/* the sum of the n torques from the rotors including anti-torque */
/* eq.(2.60)-(2.62)*/
TorqueType body_torque(
    double A, /* parameter A in Trust = A*(Omega)^2 */
    double B, /* parameter B in Ta = B*(Omega)^2 + Jr* (d(Omega)/dt) */
    double Jr,
    unsigned n, /* number of rotors */
    VectorType position[], /* position of each rotor */
    double ccw[], /* 1 or -1 */
    double omega[], /* in rpm */
    double omega_acceleration[] /* in rpm/s */ );

} /* namespace hako::drone_physics */

#endif /* _ROTOR_PHYSICS_HPP_ */