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
double rotor_omega_acceleration( /* omega acceleration in [rad/s^2]*/
    double Kr /* gain constant */,
    double Tr /* time constant */,
    double omega, /* in [rad/s] */
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
double rotor_omega_acceleration( /* omega acceleration in [rad/s^2]*/
    double Vbat, /* battery voltage in volt [V]*/
    double R, /* resistance in ohm [V/A] */
    double Cq, /* torque coeff (= B param in anti-torque) [N ms^2/rad^2] */
    double J, /* propeller inertia in [kg m^2] */
    double K, /* back electromotive force coeff in [N m/A] */
    double D, /* Kinematic viscosity coefficient [Nms/rad] */
    double omega, /* angular velocity in [rad/s] */
    double duty_rate /* 0.0-1.0 (ratio of PWM) */)
{
    assert(R != 0.0);
    assert(J != 0.0);
    /**
     * See. Kohoei model of motor/propeller(= roter)
     * https://www.docswell.com/s/Kouhei_Ito/KDVNVK-2022-06-15-193343#p2 eq (3)
     * where, e = duty_rate * Vbat.
     * Assuming inductance L and Qf is very small(zero).
     */
    return ( K * Vbat * duty_rate - (Cq*R*omega + K*K + D*R)*omega)  /  (J*R);
}

double rotor_current(  /* current in [A] */
    double Vbat, /* battery voltage in volt [V]*/
    double R, /* resistance in ohm [V/A] */
    double K, /* back electromotive force coeff in [N m/A] */
    double omega, /* angular velocity in [rad/sec] */
    double duty_rate /* 0.0-1.0 (ratio of PWM) */)
{
    assert(R != 0.0);
    return (Vbat * duty_rate  - K * omega)/R;
}

/* thrust from omega in radian/sec eq.(2.50)*/
double rotor_thrust( /* thrust in [N] */
    double Ct, /* thrust coeff, in Trust = Ct*(Omega)^2 (referred to as 'A' in Nonami's book ) in [N s^2/rad^2]*/
    double omega /* in [rad/s] */ )
{
    /**
     * Nonami's book (2.50)
     * T = Ct * (Omega)^2 
     * The name 'Ct' is from Kohei's doc and referred to as 'A' in Nonami's eq.(2.50)
     * Kohei's doc https://www.docswell.com/s/Kouhei_Ito/KDVNVK-2022-06-15-193343#p2
     */
    return Ct * (omega * omega);
}

/* this makes z-axis rotation eq.(2.56) */
double rotor_anti_torque( /* anti torque(z-axis) in [Nm]*/
    double Cq, /* torque coefficient (referred to as B in nonami's book) in [N m s^2/rad^2] */
    double J, /* innertia of propeller [kg m^2] */
    double omega, /* in [rad/s] */
    double omega_acceleratoin, /* in [rad/s^2] */
    double ccw /* 1 or -1 */ )
{
    /**
     * See Nonami's book eq.(2.56)
     * Ta = Cq * (Omega)^2 + Jr * (d(Omega)/dt)
     * The name 'Cq' is from Kohei's doc and referred to as 'B' in Nonami's eq.(2.56)
     * Kohei's doc https://www.docswell.com/s/Kouhei_Ito/KDVNVK-2022-06-15-193343#p2     
     */
   return ccw * ( Cq * omega * omega + J * omega_acceleratoin );
}


/* the sum of the n trust from the rotors eq.(2.61) */
double body_thrust( /* thrust in [N] */
    double Ct, /* thrust coeff, in Trust = Ct*(Omega)^2 (referred to as 'A' in Nonami's book ) in [N s^2/rad^2] */
    unsigned n, /* number of rotors */
    double omega[] /* in radian/sec */ )
{
    double thrust = 0;
    for (unsigned i = 0; i < n; i++) {
        thrust += rotor_thrust(Ct, omega[i]);
    }
    return thrust;
}

/* the sum of the n torques from the rotors including anti-torque */
/* eq.(2.60)-(2.62)*/
TorqueType body_torque( /* torque in [Nm] */
    double Ct, /* thrust coeff, in Trust = Ct*(Omega)^2 (referred to as 'A' in Nonami's book ) in [N s^2/rad^2] */
    double Cq, /* torque coefficient (referred to as B in nonami's book in [N m s^2/rad^2] */
    double J, /* innertia of propeller [kg m^2] */
    unsigned n, /* number of rotors */
    VectorType position[], /* position of each rotor in [m] */
    double ccw[], /* 1 or -1 */
    double omega[], /* in [rad/s] */
    double omega_acceleration[] /* in [rad/s^2] */ )
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
        const VectorType thrust_vector = { 0, 0, -rotor_thrust(Ct, omega[i]) };
        auto thrust_torque = cross(position[i], thrust_vector);

        // (2) anti-torque around z-axis = yaw.
        const auto anti_torque =
            rotor_anti_torque(Cq, J, omega[i], omega_acceleration[i], ccw[i]);
        
        total_torque += thrust_torque;
        total_torque.z += anti_torque;
    }
    return total_torque;
}

/**
 * Linear approximation versions
 * used in jMAVsim implemntation.
 * Used in comparing with the non-linear(our) model.
 */
double rotor_thrust_linear(double Ct2, double omega)
{
    return Ct2 * omega;
}
double rotor_anti_torque_linear(double B2, double omega, double ccw)
{
   return ccw * ( B2 * omega );
}
TorqueType body_torque_linear(double Ct2, double Cq2, unsigned n,
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
        const VectorType thrust_vector = { 0, 0, -rotor_thrust_linear(Ct2, omega[i]) };
        auto thrust_torque = cross(position[i], thrust_vector);

        // (2) anti-torque around z-axis = yaw.
        const auto anti_torque =
            rotor_anti_torque_linear(Cq2, omega[i], ccw[i]);
        
        total_torque += thrust_torque;
        total_torque.z += anti_torque;
    }
    return total_torque;
}
double body_thrust_linear(double Ct2, unsigned n, double omega[])
{
    double thrust = 0;
    for (unsigned i = 0; i < n; i++) {
        thrust += rotor_thrust_linear(Ct2, omega[i]);
    }
    return thrust;
}



} /* namespace hako::drone_physics */