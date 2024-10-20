#include "rotor_physics.hpp"
#include <cmath>
#include <cassert>

namespace hako::drone_physics {

/*
 * All the equations are from the references below.
 * Nonami's book "Introduction to Drone Engineering" from Corona Publishing Co., Ltd.
 * Some of the them can be found in the following links, too.
 * 
 * https://www.docswell.com/s/Kouhei_Ito/KDVNVK-2022-06-15-193343#p2
 * https://mtkbirdman.com/flight-dynamics-body-axes-system
 * https://www.jstage.jst.go.jp/article/sicejl/56/1/56_3/_pdf
 * https://www.sky-engin.jp/blog/eulers-equations-of-motion/
 */


/**
 *  The rotor dynamics is modeled as a first-order lag system.
 * 
 *  Input(s) ...  duty rate(0..1)
 *  Output(s) ... Omega in radian/sec
 *  Output(s)/Input(s) = G(s), G(s) = Kr/(Tr s + 1). 
 * 
 *  See: Nonami's book eq.(2.48)
 * 
 *  Kr ... gain constant, which is the coefficient of the duty rate to the radian/sec.
 *        and it is the maximum radian/sec of duty 1.0 as time equals infinity.
 *  Tr ... time constant of the rotor.
 * 
 *  Note that the above first-order lag model is originally not for raw Omega,
 *  but for Delta-Omega, the difference from the Omega0 (the hover equiblium omega).
 *  But this model usually works fare. Possibliy make a non-linear model in the future.
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
 *  the above equation is as follows(step response).;
 * 
 *     Omega = Kr * (1 - exp(-t/Tr) * (duty rate))
 */
double rotor_omega_acceleration(
    double Kr /* gain constant */,
    double Tr /* time constant */,
    double omega, /* in radian/sec */
    double duty_rate /* 0.0-1.0 (ratio of PWM) */)
{
    /**
     * See Nonami's book (2.48)
     * G(s) = Kr/(Tr s + 1). 
     */
    return (Kr * duty_rate - omega) / Tr;
}

/**
 * Realistic modeling of the rotor, using battery voltage, intertia and etc.
 */
double rotor_omega_acceleration(
    double Vbat, /* battery voltage in volt [V]*/
    double R, /* resistance in ohm [V/A] */
    double Cq, /* torque coeff (= B param in anti-torque) [N ms^2/rad^2] */
    double J, /* propeller inertia in [kg m^2] */
    double K, /* back electromotive force coeff in [N m/A] */
    double omega, /* in radian/sec */
    double duty_rate /* 0.0-1.0 (ratio of PWM) */)
{
    assert(R != 0.0);
    assert(J != 0.0);
    /**
     * See. Kohoei model of motor/propeller(= roter)
     * https://www.docswell.com/s/Kouhei_Ito/KDVNVK-2022-06-15-193343#p2 eq (3)
     * where, e = duty_rate * Vbat.
     * 
     * Later: add 'Qf' and 'D' to the parameter and subtract the two anti-forces,
    *  return ... -(D/J)*omega - Qf/J;
     */
    return ( K*Vbat*duty_rate - (Cq*R*omega + K*K)*omega )  /  (J*R);
}

/* thrust from omega in radian/sec eq.(2.50)*/
double rotor_thrust(
    double A, /* the A parameter in Trust = A*(Omega)^2 */
    double omega /* in radian/sec */ )
{
    /**
     * Nonami's book (2.50)
     * T = A * (Omega)^2
     * A is referred to as Ct in Kohei's doc.
     */
    return A * (omega * omega);
}

/* this makes z-axis rotation eq.(2.56) */
double rotor_anti_torque(double B, 
 double Jr, double omega, double omega_acceleratoin, double ccw)
{
    /**
     * See Nonami's book eq.(2.56)
     * Ta = B * (Omega)^2 + Jr * (d(Omega)/dt)
     * B is referred to as Cq in Kohei's doc.
     */
   return ccw * ( B * omega * omega + Jr * omega_acceleratoin );
}


/* the sum of the n trust from the rotors eq.(2.61) */
double body_thrust(double A, unsigned n, double omega[])
{
    double thrust = 0;
    for (unsigned i = 0; i < n; i++) {
        thrust += rotor_thrust(A, omega[i]);
    }
    return thrust;
}

/* the sum of the n torques from the rotors including anti-torque */
/* eq.(2.60)-(2.62)*/
TorqueType body_torque(double A, double B, double Jr, unsigned n,
    VectorType position[], double ccw[], double omega[],
    double omega_acceleration[])
{
    TorqueType total_torque = {0, 0, 0};
    for (unsigned i = 0; i < n; i++) {
        /**
         * The Torque from each rotor is made of two parts.
         * 1. the torque from the thrust of the rotor.
         * 2. the torque from the anti-torque of the rotor.
         * 
         * See Nonami's book (2.60)-(2.62)
         */
        // (1)thrust(calculated always +) is upper direction. change to alight z-axis.
        // then the torque is (position vector) x (thrust vector).
        const VectorType thrust_vector = { 0, 0, -rotor_thrust(A, omega[i]) };
        auto thrust_torque = cross(position[i], thrust_vector);

        // (2) anti-torque around z-axis = yaw.
        const auto anti_torque =
            rotor_anti_torque(B, Jr, omega[i], omega_acceleration[i], ccw[i]);
        
        total_torque += thrust_torque;
        total_torque.z += anti_torque;
    }
    return total_torque;
}

/**
 * jMAVsim implemntation
 * thrust from omega linearly
 */
double rotor_thrust_linear(
    double A2, /* the A parameter in Trust = A*(Omega) */
    double omega /* in radian/sec */ )
{
    return A2 * omega;
}
double rotor_anti_torque_linear(double B2, double omega, double ccw)
{
   return ccw * ( B2 * omega );
}
TorqueType body_torque_linear(double A2, double B2, unsigned n,
    VectorType position[], double ccw[], double omega[])
{
    TorqueType total_torque = {0, 0, 0};
    for (unsigned i = 0; i < n; i++) {
        /**
         * The Torque from each rotor is made of two parts.
         * 1. the torque from the thrust of the rotor.
         * 2. the torque from the anti-torque of the rotor.
         * 
         * See Nonami's book (2.60)-(2.62)
         */
        // (1)thrust(calculated always +) is upper direction. change to alight z-axis.
        // then the torque is (position vector) x (thrust vector).
        const VectorType thrust_vector = { 0, 0, -rotor_thrust_linear(A2, omega[i]) };
        auto thrust_torque = cross(position[i], thrust_vector);

        // (2) anti-torque around z-axis = yaw.
        const auto anti_torque =
            rotor_anti_torque_linear(B2, omega[i], ccw[i]);
        
        total_torque += thrust_torque;
        total_torque.z += anti_torque;
    }
    return total_torque;
}
double body_thrust_linear(double A2, unsigned n, double omega[])
{
    double thrust = 0;
    for (unsigned i = 0; i < n; i++) {
        thrust += rotor_thrust_linear(A2, omega[i]);
    }
    return thrust;
}



} /* namespace hako::drone_physics */