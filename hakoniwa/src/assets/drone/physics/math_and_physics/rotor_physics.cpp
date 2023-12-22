#include "rotor_physics.hpp"


/**
 *  The rotor dynamics is modeled as a first order system.
 * 
 *  Input(s) ...  duty rate(0..1)
 *  Output(s) ... Omega in rpm
 *  Output(s)/Input(s) = G(s), G(s) = Kr/(Tr s + 1). 
 * 
 *  See: Nonami's book (2.48)
 * 
 *  Kr ... gain constant, which is the coefficient of the duty rate to the rpm.
 *        and it is the maximum rpm as time equals infinity.
 *  Tr ... time constant of the rotor.
 * 
 *  MAKE SURE that the unit of the simulation delta time be smaller than Tr.
 *  Otherwise, the simulation will be unstable. 
 *  (The target Kr*(duty rate) == Omega is reached in one iteration)
 * 
 *  By inverse Laplace transform, we get the following equation.
 *  (this function returns the lhs of the equation)
 * 
 *     d(Omega)/dt = ( Kr * (duty rate) - (Omega) ) / Tr
 * 
 *  If the input duty rate is a constant, the analytical solution for
 *  the above equation is as follows(step response).);
 * 
 *     Omega = Kr * (1 - exp(-t/Tr) * (duty rate))
 */
double rotor_omega_acceleration(
    double Kr /* gain constant */,
    double Tr /* time constant */,
    double omega, /* in rpm */
    double duty_rate /* 0-1 of PWM */)
{

    return (Kr * duty_rate - omega) / Tr;
}

double rotor_thrust(
    double A, /* the A parameter in Trust = A*(Omega)^2 */
    double omega /* in rpm */ )
{
    /**
     * Nonami's book (2.50)
     * T = A * (Omega)^2
     */
    return A * (omega * omega);
}

/* this makes z-axis rotation */
/* Ta = B*(Omega)^2 + Jr* (d(Omega)/dt) */
double rotor_anti_torque(double B, 
 double Jr, double omega, double omega_acceleratoin, int ccw)
{
    /**
     * See Nonami's book (2.56)
     * Ta = B * (Omega)^2 + Jr * (d(Omega)/dt)
     */
   return double(ccw) * ( B * omega * omega + Jr * omega_acceleratoin );
}

/* the sum of the n trust from the rotors */
double body_thrust(double A, unsigned n, double omega[])
{
    double thrust = 0;
    for (unsigned i = 0; i < n; i++) {
        thrust += rotor_thrust(A, omega[i]);
    }
    return thrust;
}

TorqueType body_torque(double A, double B, double Jr, unsigned n,
    PositionType position[], double ccw[], double omega[],
    double omega_acceleration[])
{
    TorqueType total_torque = {0, 0, 0};
    for (unsigned i = 0; i < n; i++) {
        /**
         * The Torque from each rotor is made of two parts.
         * 1. the torque from the thrust of the rotor.
         * 2. the torque from the anti-torque of the rotor.
         */
        // (1)thrust(calculated always +) is upper direction. change to alight z-axis.
        // then the torque is (position vector) x (thrust vector).
        const VectorType thrust_vector = { 0, 0, -rotor_thrust(A, omega[i]) };
        auto thrust_torque = cross(position[i], thrust_vector);

        // (2) anti-torque around z-axis = yaw.
        const auto anti_torque =
            rotor_anti_torque(B, Jr, omega[i], omega_acceleration[i], ccw[i]);
        
        total_torque += thrust_torque;
        std::get<2>(total_torque) += anti_torque;
    }
    return total_torque;
}