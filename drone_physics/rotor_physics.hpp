#ifndef _ROTOR_PHYSICS_HPP_
#define _ROTOR_PHYSICS_HPP_

#include "math_and_physics.hpp"
typedef VectorType
    TorqueType, PositionType;

/**
 * These functions are rotor dynamics.
 * Treats the rotor as a single unit.
 */
double rotor_omega_acceleration(
    double Kr, /* gain constant */
    double Tr, /* time constant */
    double omega, /* in rpm */
    double duty_rate /* 0-1 of PWM */
    );

double rotor_thrust(
    double A, /* parameter A in Trust = A*(Omega)^2 */
    double omega /* in rpm */ );

double rotor_anti_torque(
    double B, /* parameter B in Ta = B*(Omega)^2 + Jr* (d(Omega)/dt) */
    double Jr,
    double omega, /* in rpm */
    double omega_acceleratoin, /* in rpm/s */
    double ccw /* 1 or -1 */ );

/**
 * These functions are body dynamics.
 * Treats one body with multiple rotors.
 */
double body_thrust(
    double A, /* parameter A in Trust = A*(Omega)^2 in each motor */
    unsigned n, /* number of rotors */
    double omega[] /* in rpm */ );

TorqueType body_torque(
    double A, /* parameter A in Trust = A*(Omega)^2 */
    double B, /* parameter B in Ta = B*(Omega)^2 + Jr* (d(Omega)/dt) */
    double Jr,
    unsigned n, /* number of rotors */
    PositionType position[], /* position of each rotor */
    double ccw[], /* 1 or -1 */
    double omega[], /* in rpm */
    double omega_acceleration[] /* in rpm/s */ );

#endif /* _ROTOR_PHYSICS_HPP_ */