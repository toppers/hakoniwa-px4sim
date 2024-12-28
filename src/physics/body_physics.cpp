#include "body_physics.hpp"
#include <cassert>
#include <cmath>


namespace hako::drone_physics {

/*
 * All the equations are from the references below.
 * Nonami's book "Introduction to Drone Engineering" from Corona Publishing Co., Ltd.
 * Some of the them can be found in the following links, too.
 * https://mtkbirdman.com/flight-dynamics-body-axes-system
 * https://www.jstage.jst.go.jp/article/sicejl/56/1/56_3/_pdf
 * https://www.sky-engin.jp/blog/eulers-equations-of-motion/
 */

/**
 *  Utility section (tried not to depend on other libraries, except for C++ std)
 */


/**
 *  Maths section (3D frame transformations between body and ground)
 */

/* Tranlsform angular rate in body frame to ground frame eq.(1.109)*/
EulerRateType euler_rate_from_body_angular_velocity(
    const AngularVelocityType& body,
    const EulerType& angle)
{
    using std::sin; using std::cos;
    const auto
        c_phi   = cos(angle.phi),
        s_phi   = sin(angle.phi),
        c_theta = cos(angle.theta),
        t_theta = tan(angle.theta);
    const auto [p, q, r] = body;
     /*
     * See eq.(1.109) in Nonami's book.
     * for the transformation equations.
     * See also https://mtkbirdman.com/flight-dynamics-body-axes-system
     */
    /*****************************************************************/  
    double dot_phi   = p + (r * c_phi + q * s_phi) * t_theta; /** overflow INF possible */
    double dot_theta = q * c_phi - r * s_phi;
    double dot_psi   = (r * c_phi + q * s_phi) / c_theta; /** zero div INF possible */
    /*****************************************************************/  

    return {dot_phi, dot_theta, dot_psi};
}

/* Tranlsform angular rate in ground frame to body frame (eq.106)*/
AngularVelocityType body_angular_velocity_from_euler_rate(
    const EulerRateType& euler_rate,
    const EulerType& euler)
{
    using std::sin; using std::cos; using std::tan;
    const auto
        c_phi   = cos(euler.phi),   s_phi   = sin(euler.phi),
        c_theta = cos(euler.theta), s_theta = sin(euler.theta);
    const auto [dot_phi, dot_theta, dot_psi] = euler_rate;

    /*
     * See eq.(1.106) in Nonami's book.
     * for the transformation equations.
     * See also https://mtkbirdman.com/flight-dynamics-body-axes-system
     */
    /*****************************************************************/  
    double p =  1 * (dot_phi)                       - s_theta * (dot_psi);
    double q =        c_phi  * (dot_theta)  + s_phi * c_theta * (dot_psi);
    double r =        -s_phi * (dot_theta)  + c_phi * c_theta * (dot_psi);
    /*****************************************************************/  

    return {p, q, r};
}

/**
 * Physics section.
 * The functions below includes Force, Mass, Torque, and Inertia,
 * and calculates accelerations and angular accelerations.
 * 
 * Calculations are based in the body frame.
 * So the results should be transformed to the ground frame, if needed.
 */

/* acceleration in body frame based on mV'+ w x mV = F ... eq.(1.136),(2.31)*/
AccelerationType acceleration_in_body_frame(
    const VelocityType& body_velocity,
    const EulerType& angle,
    const AngularVelocityType& body_angular_velocity,
    double thrust, double mass /* 0 is not allowed */,
    double gravity, /* usually 9.8 > 0*/
    const VectorType& wind, /* wind vector in frame */
    const VectorType& drag1,  /* air friction of 1-st order(-d1*v) counter to velocity */
    const VectorType& drag2 /* air friction of 2-nd order(-d2*v*v) counter to velocity */)
{
    assert(!is_zero(mass));
    using std::sin; using std::cos;

    const auto
        c_phi   = cos(angle.phi),   s_phi   = sin(angle.phi),
        c_theta = cos(angle.theta), s_theta = sin(angle.theta);
    const auto [u, v, w] = body_velocity;
    const auto [p, q, r] = body_angular_velocity;
    const auto T = thrust;
    const auto m = mass;
    const auto g = gravity;
    const auto d1 = drag1;
    const auto d2 = drag2;

    /*
     * See nonami's book eq.(1.136).(2.31)
     * Colioris's force is (p, q, r) x (u, v, w).
     * 
     * Difference with the 'ground' version is that
     * (1) 'g' is broken down to x, y, z components.
     * (2) T is only relavant to z-axis.
     * (3) Coriolis force(using uvw,pqr) IS needed(because the body frame is rotating!)
     * 
     * 10/31/2024: added wind effect to 1-st, 2-nd order drag.
     * Note: ma = T - colioris - d1 v - d2 sign(v) (v*v), where v = v - wind = air_v.
     * the air friction is always counter to the velocity vs wind, in the 1st and sencond order.
     */
    /*****************************************************************/
    double air_u = u - wind.x;
    double air_v = v - wind.y;
    double air_w = w - wind.z;

    double dot_u =       - g * s_theta            - (q*w - r*v) - d1.x/m * (air_u) - d2.x/m * air_u * std::abs(air_u);
    double dot_v =       + g * c_theta * s_phi    - (r*u - p*w) - d1.y/m * (air_v) - d2.y/m * air_v * std::abs(air_v);
    double dot_w = -T/m  + g * c_theta * c_phi    - (p*v - q*u) - d1.z/m * (air_w) - d2.z/m * air_w * std::abs(air_w);
    /*****************************************************************/  

    return {dot_u, dot_v, dot_w};
}

/* simplified version of the above */
AccelerationType acceleration_in_body_frame(
    const VelocityType& body_velocity,
    const EulerType& angle,
    const AngularVelocityType& body_angular_velocity,
    double thrust, double mass /* 0 is not allowed */,
    double gravity, /* usually 9.8 > 0*/
    double drag1,  /* air friction of 1-st order(-d1*v) counter to velocity */
    double drag2 /* air friction of 2-nd order(-d2*v*v) counter to velocity */)
{
    return acceleration_in_body_frame(
        body_velocity, angle, body_angular_velocity,
        thrust, mass, gravity, {0, 0, 0}, {drag1, drag1, drag1}, {drag2, drag2, drag2});
}



/**
 * Acceleration in body frame based on eq.(2.46), (2.47) in Nonami's book.
 * A mistake in the book: (2.46) z-axis should be inverted (and also psi is inverted) in (2.47).
 * 
 * This is OBSOLETE because not considering wind, 3-dimentional drag coefficient.
 * Used in the old ground coordinate system drone to check the correctness of the new one.
 */
AccelerationType acceleration_in_ground_frame(
    const VelocityType& ground,
    const EulerType& angle,
    double thrust, double mass /* 0 is not allowed */,
    double gravity, /* usually 9.8 > 0*/
    double drag1,  /* air friction of 1-st order(-d1*v) counter to velocity */
    double drag2 /* air friction of 2-nd order(-d2*v*v) counter to velocity */)
{
    using std::sin; using std::cos;

    const auto
        c_phi   = cos(angle.phi),   s_phi   = sin(angle.phi),
        c_theta = cos(angle.theta), s_theta = sin(angle.theta),
        c_psi   = cos(angle.psi),   s_psi   = sin(angle.psi);

    const auto [u, v, w] = ground;
    const auto T = thrust;
    const auto m = mass;
    const auto g = gravity;
    const auto d1 = drag1;
    const auto d2 = drag2;

    /**
     * See eq.(2.46), (2.47) in Nonami's book.
     * and y, z-axis is inverted(and also theta, psi is inverted).
     * these inversions lead to the following minus signs in EVERY LINE(diff from PDF).
     * See https://www.jstage.jst.go.jp/article/sicejl/56/1/56_3/_pdf
     * 
     * Difference with the 'body' version is that
     * (1) 'g' is only relavant to z-axis.
     * (2) T is broken down to x, y, z components.
     * (3) Coriolis force is not needed, which is great.
     * 
     * I found exactly the same equations in the following link.
     * (2.7) The Equations of Motion
     * https://github.com/SKYnSPACE/AE450/blob/master/Lec10/AE450_Lec10_Quadcopter_Dynamics_and_Control.pdf
     */
    /*****************************************************************/  
    double dot_u =  -T/m * (c_phi * s_theta * c_psi + s_phi * s_psi) - d1/m * u - d2/m * u * u;
    double dot_v =  -T/m * (c_phi * s_theta * s_psi - s_phi * c_psi) - d1/m * v - d2/m * v * v;
    double dot_w =  -T/m * (c_phi * c_theta)                   + g   - d1/m * w - d2/m * w * w;
    /*****************************************************************/  

    return {dot_u, dot_v, dot_w};
}


/* angular acceleration in body frame based on JW' = W x JW =Tb ...eq.(1.137),(2.31) */
AngularAccelerationType angular_acceleration_in_body_frame(
    const AngularVelocityType& body_angular_velocity,
    const TorqueType& torque, /* in body frame */
    double I_xx, /* in body frame, 0 is not allowed */
    double I_yy, /* in body frame, 0 is not allowed */
    double I_zz /* in body frame, 0 is not allowed */)
{
    assert(!is_zero(I_xx)); assert(!is_zero(I_yy)); assert(!is_zero(I_zz));

    // current angular velocities in body frame
    const auto [p, q, r] = body_angular_velocity;
    const auto [torque_x, torque_y, torque_z] = torque;
 
    /*
     * See also Nonami's book eq.(2.31)(1.137)(1.109), where L=tau_x, M=tau_y, N=tau_z.
     * and Ixz = Iyz = Izx = 0 is assumed.
     * See also https://www.sky-engin.jp/blog/eulers-equations-of-motion/ eq.(21)
     * and the rest.
     * 
     * I found exactly the same equations in the following link.
     * (2.7) The Equations of Motion
     * https://github.com/SKYnSPACE/AE450/blob/master/Lec10/AE450_Lec10_Quadcopter_Dynamics_and_Control.pdf
     */
 
    /*****************************************************************/
    double dot_p = (torque_x - q*r*(I_zz - I_yy)) / I_xx;
    double dot_q = (torque_y - r*p*(I_xx - I_zz)) / I_yy;
    double dot_r = (torque_z - p*q*(I_yy - I_xx)) / I_zz;
    /*****************************************************************/  

    return {dot_p, dot_q, dot_r};
}

EulerAccelerationType euler_acceleration_in_ground_frame(
    const EulerRateType& current_euler_rate,
    const EulerType& current_euler,
    const TorqueType& torque, /* in BODY FRAME!! */
    double I_xx, double I_yy, double I_zz /* in BODY FRAME!! */)
{
    /* You need to 
     * 1. convert euler -> body(pqr)
     * 2. get acceleration
     * 3. get new body(pqr)
     * 4. calculate dot_dot_euler using body(pqr), dot_body, euler, dot_euler.
     *    (very complicated messy equations)
     * */
    const auto [phi, theta, psi] = current_euler;
    const auto [dot_phi, dot_theta, dot_psi] = current_euler_rate;

    /* transform euler angle velocity to BODY frame anglular velocity */
    const auto [p, q, r] = body_angular_velocity_from_euler_rate(current_euler_rate, current_euler);
    const auto [dot_p, dot_q, dot_r] = angular_acceleration_in_body_frame(
        {p, q, r}, torque, I_xx, I_yy, I_zz);

    const double c_phi = cos(phi), s_phi = sin(phi);
    const double c_theta = cos(theta), s_theta = sin(theta);
    const double sec_theta = 1.0 / c_theta, t_theta = tan(theta);

    /**
     * I calculated the differential of eq.(1.109) in Nonami's book
     * by hand, checked with Walfraam Alpha.
     */
    /************************************************************************/
    double dot_dot_phi
        = q * (c_phi * t_theta * dot_phi + s_phi * sec_theta * sec_theta * dot_theta)
        + dot_q * s_phi * t_theta
        + r * (c_phi * c_theta * dot_theta - s_phi * s_theta * dot_phi)
        + dot_r * c_phi * s_theta
        + dot_p;

    double dot_dot_theta
        = - q * s_phi * dot_phi   +       dot_q * c_phi
        - r * c_phi * dot_phi     -       dot_r * s_phi;
    
    double dot_dot_psi
        = sec_theta * (
            q * (c_phi * dot_phi + s_phi * t_theta * dot_theta)
            + dot_q * s_phi
            + r * (c_phi * t_theta * dot_theta - s_phi * dot_phi)
            + dot_r * c_phi
            );
    /************************************************************************/

    return {dot_dot_phi, dot_dot_theta, dot_dot_psi};
}

/* Quaternion velocity(dq/dt) from body angllar velocity */
/* Use this for euler_rate_from_body_angular_velocity() */
QuaternionVelocityType quaternion_velocity_from_body_angular_velocity(
    const AngularVelocityType& body_angular_velocity,
    const QuaternionType& quaternion) 
{
    /**
     * See eq.(1.87) in Nonami's book,
     * https://www.docswell.com/s/Kouhei_Ito/KDVNVK-2022-06-15-193343#p3
     */
    auto [p, q, r] = body_angular_velocity;
    auto [q0, q1, q2, q3] = quaternion;
    double dot_q0 = (-p*q1 - q*q2 - r*q3) / 2;
    double dot_q1 = ( p*q0 - q*q3 + r*q2) / 2;
    double dot_q2 = ( p*q3 + q*q0 - r*q1) / 2;
    double dot_q3 = (-p*q2 + q*q1 + r*q0) / 2;
    return {dot_q0, dot_q1, dot_q2, dot_q3};
}

/* Quaternion from euler angle */
QuaternionType quaternion_from_euler(const EulerType& euler)
{
    /**
     * See eq.(1.66) in Nonami's book,
     * and also https://qiita.com/aa_debdeb/items/3d02e28fb9ebfa357eaf
     *  (rotation order is ZYX, with the quaternion is [q0,q1,q2,q3]=[X,Y,Z,W])
     * - https://www.docswell.com/s/Kouhei_Ito/KDVNVK-2022-06-15-193343#p3
     */
    auto cx = std::cos(0.5 * euler.phi);
    auto sx = std::sin(0.5 * euler.phi);
    auto cy = std::cos(0.5 * euler.theta);
    auto sy = std::sin(0.5 * euler.theta);
    auto cz = std::cos(0.5 * euler.psi);
    auto sz = std::sin(0.5 * euler.psi);

    auto q0 = cz * cy * cx  +  sz * sy * sx;
    auto q1 = cz * cy * sx  -  sz * sy * cx;
    auto q2 = cz * sy * cx  +  sz * cy * sx;
    auto q3 = sz * cy * cx  -  cz * sy * sx;

    return {q0, q1, q2, q3}; // [w, x, y, z], normalized by the creation process.
}

/* helper for the next function */
static double clip(double value) {
    using std::fmax; using std::fmin;
    return fmax(-1.0, fmin(1.0, value));
}


/* Euler angle from Quaternion */
EulerType euler_from_quaternion(const QuaternionType& quaternion)
{ 
    /**
     * See eq.(1.74)-(1.77) in Nonami's book,
     * 
     * and also;
     * - https://qiita.com/aa_debdeb/items/3d02e28fb9ebfa357eaf
     *   (Gimbal lock case. Rotation order XYZ in this doc.)
     * - https://www.docswell.com/s/Kouhei_Ito/KDVNVK-2022-06-15-193343#p3
     * - 
     */
    using std::atan2; using std::asin; using std::cos;
    auto [q0, q1, q2, q3] = quaternion; /* [w, x, y, z] aligned name with the book for reviewability */

    // First, see theta,  asin is nearly 1.0, cos(theta) is nearly 0.0
    double theta = asin(clip(2*(q0*q2 - q1*q3)));
    double phi = 0, psi = 0;

    /**
     *  Treated cos(theta) = 0.0 case separately.
     *  For example, the quaternion (-0.5, -0.5, -0.5, 0.5), euler angle should be (90, 90, 0) or (0, 90, -90).
     *  Here, the gimbal lock happens, and the euler angle is not unique.
     *  Choose the one with the phi = 0, namely (0, 90, -90).
     **/
    if (std::abs(cos(theta)) < 0.0001) {
        phi = 0;
        psi = atan2(2*(q0*q3 - q1*q2), 2*(q0*q0 + q2*q2) - 1);
    } else {
        phi = atan2(2*(q2*q3 + q0*q1), 2*(q0*q0 + q3*q3) - 1);
        psi = atan2(2*(q1*q2 + q0*q3), 2*(q0*q0 + q1*q1) - 1);
    }

    return {phi, theta, psi};
}

/* note: this changes the argument */
void normalize(QuaternionType& quaternion)
{
    double norm = length(quaternion);
    assert(!is_zero(norm));
    quaternion /= norm;
}


/**
 * Collision section.
 * The functions below calculates collision related quantities.
 */
VectorType velocity_after_contact_with_wall(
    const VectorType& before_contact,
    const VectorType& normal_vector, /* of the wall, will be normalized internally */
    double restitution_coeff /* 0.0 - 1.0 */)
{
    assert(!is_zero(length_squared(normal_vector)));

    /* normalize the normal vector */
    const auto n = normal_vector / length(normal_vector);

    return restitution_coeff
        * (before_contact - 2*dot(before_contact, n) * n);
}

VectorType velocity_after_contact_with_wall(
    const VectorType& velocity_before_contact,
    const VectorType& center_position,
    const VectorType& contact_position,
    double restitution_coefficient /* 0.0 - 1.0 */)
{
    const auto normal_vector = contact_position - center_position;
    if (length_squared(normal_vector) <= 1.0e-10)
        return {0, 0, 0};

    return velocity_after_contact_with_wall(velocity_before_contact, normal_vector, restitution_coefficient);
}


} /* namespace hako::drone_physics */