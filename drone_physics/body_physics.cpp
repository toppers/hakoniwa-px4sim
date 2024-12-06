#include "body_physics.hpp"
#include <cassert>
#include <cmath>

const double eps = 1.0e-30; // for double values are ZERO for assertion. almost MIN_FLT.
static bool is_zero(double a){return std::abs(a) < eps;}

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
VectorType cross(const VectorType& u, const VectorType& v)
{ return { u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x };}
double dot(const VectorType& u, const VectorType& v) { return u.x * v.x + u.y * v.y + u.z * v.z;}
double length_squared(const VectorType& v) { return dot(v, v);}
double length(const VectorType& v) { return std::sqrt(length_squared(v));}
double length_squared(const QuaternionType& q) { return q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z;}
double length(const QuaternionType& q) { return std::sqrt(length_squared(q));}
VectorType& operator += (VectorType& u, const VectorType& v) { u.x += v.x; u.y += v.y; u.z += v.z; return u;}
VectorType operator + (const VectorType& u, const VectorType& v) { VectorType result = u; return result += v;}
VectorType& operator -= (VectorType& u, const VectorType& v) { u.x -= v.x;    u.y -= v.y;    u.z -= v.z; return u;}
VectorType operator - (const VectorType& u, const VectorType& v) { VectorType result = u; return result -= v;}
VectorType operator * (double s, const VectorType& v) { return {s*v.x, s*v.y, s*v.z};}
VectorType operator * (const VectorType& v, double s) { return s*v;}
VectorType& operator /= (VectorType& v, const double s) { v.x /= s; v.y /= s; v.z /= s; return v;}
VectorType operator / (const VectorType& v, double s) { return {v.x/s, v.y/s, v.z/s};}
QuaternionType& operator += (QuaternionType& q1, const QuaternionType& q2) { q1.w += q2.w; q1.x += q2.x; q1.y += q2.y; q1.z += q2.z; return q1;}
QuaternionType operator + (const QuaternionType& q1, const QuaternionType& q2) { QuaternionType result = q1; return result += q2;}
QuaternionType& operator -= (QuaternionType& q1, const QuaternionType& q2) { q1.w -= q2.w; q1.x -= q2.x; q1.y -= q2.y; q1.z -= q2.z; return q1;}
QuaternionType operator - (const QuaternionType& q1, const QuaternionType& q2) { QuaternionType result = q1; return result -= q2;}
QuaternionType& operator *= (QuaternionType& q, double s) { q.w *= s; q.x *= s; q.y *= s; q.z *= s; return q;}
QuaternionType operator * (const QuaternionType& q, double s) { QuaternionType result = q; return result *= s;}
QuaternionType operator * (double s, const QuaternionType& q) { return q * s;}
QuaternionType& operator /= (QuaternionType& q, double s) { q.w /= s; q.x /= s; q.y /= s; q.z /= s; return q;}
QuaternionType operator / (const QuaternionType& q, double s) { QuaternionType result = q; return result /= s;}

/**
 *  Maths section (3D frame transformations between body and ground)
 */

/*
 * For generic vectors. all vector types can be available including
 * velocity, acceleration, angular ones, but NOT for angles/angular rates(EULERS).
 */
VectorType ground_vector_from_body(
    const VectorType& body,
    const EulerType& angle)
{
    using std::sin; using std::cos;
    const auto
        c_phi   = cos(angle.phi),   s_phi = sin(angle.phi),
        c_theta = cos(angle.theta), s_theta = sin(angle.theta),
        c_psi   = cos(angle.psi),   s_psi = sin(angle.psi);
    
    const auto [x, y, z] = body;

    /*
     * eq.(1.71),(1.124) in Nonami's book.
     * See also https://mtkbirdman.com/flight-dynamics-body-axes-system
     * for the transformation equations.
     */
    /*****************************************************************/  
    double
    x_e =   (c_theta * c_psi)                           * x
            + (s_phi * s_theta * c_psi - c_phi * s_psi) * y
            + (c_phi * s_theta * c_psi + s_phi * s_psi) * z;
    double
    y_e =   (c_theta * s_psi)                           * x
            + (s_phi * s_theta * s_psi + c_phi * c_psi) * y
            + (c_phi * s_theta * s_psi - s_phi * c_psi) * z;
    double
    z_e =   (- s_theta)                                 * x
            + (s_phi * c_theta)                         * y
            + (c_phi * c_theta)                         * z;
    /*****************************************************************/

    return {x_e, y_e, z_e};
}

/* for generic vectors. use the meaningful aliases below  */
VectorType body_vector_from_ground(
    const VectorType& ground,
    const EulerType& angle)
{
    using std::sin; using std::cos;
    const auto
        c_phi   = cos(angle.phi),   s_phi = sin(angle.phi),
        c_theta = cos(angle.theta), s_theta = sin(angle.theta),
        c_psi   = cos(angle.psi),   s_psi = sin(angle.psi);
    const auto [x_e, y_e, z_e] = ground;
    /*
     * See eq.(1.69), inverse of (1.124) in Nonami's book.
     * See also https://mtkbirdman.com/flight-dynamics-body-axes-system
     * for the transformation equations.
     */
    /*****************************************************************/  
    double
    x =   (c_theta * c_psi)                         * x_e
        + (c_theta * s_psi)                         * y_e
        - (s_theta)                                 * z_e;

    double
    y =   (s_phi * s_theta * c_psi - c_phi * s_psi) * x_e
        + (s_phi * s_theta * s_psi + c_phi * c_psi) * y_e
        + (s_phi * c_theta)                         * z_e;

    double
    z = (c_phi * s_theta * c_psi + s_phi * s_psi)   * x_e
        + (c_phi * s_theta * s_psi - s_phi * c_psi) * y_e
        + (c_phi * c_theta)                         * z_e;
    /*****************************************************************/  

    return {x, y, z};
}

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

    double dot_u =       - g * s_theta            - (q*w - r*v) - d1.x/m * (air_u) - d2.x/m * air_u * std::fabs(air_u);
    double dot_v =       + g * c_theta * s_phi    - (r*u - p*w) - d1.y/m * (air_v) - d2.y/m * air_v * std::fabs(air_v);
    double dot_w = -T/m  + g * c_theta * c_phi    - (p*v - q*u) - d1.z/m * (air_w) - d2.z/m * air_w * std::fabs(air_w);
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
QuaternionVelocityType quaternion_velocity_from_body_angular_velocity(
    const AngularVelocityType& body_angular_velocity,
    const QuaternionType& quaternion) 
{

    auto [p, q, r] = body_angular_velocity;
    auto [q0, q1, q2, q3] = quaternion;
    double dq0 = 0.5 * (-p*q1 - q*q2 - r*q3);
    double dq1 = 0.5 * ( p*q0 + r*q2 - q*q3);
    double dq2 = 0.5 * ( q*q0 - r*q1 + p*q3);
    double dq3 = 0.5 * ( r*q0 + q*q1 - p*q2);
    return {dq0, dq1, dq2, dq3};
}

/* Quaternion from euler angle */
QuaternionType quaternion_from_euler(const EulerType& euler)
{
    /**
     * See eq.(1.66) in Nonami's book,
     * and also https://qiita.com/aa_debdeb/items/3d02e28fb9ebfa357eaf (rotation order is XYZ)
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

    return {q0, q1, q2, q3};
}

/* Euler angle from Quaternion */
EulerType euler_from_quaternion(const QuaternionType& quaternion)
{ 
    /**
     * See eq.(1.74)-(1.77) in Nonami's book,
     * 
     * and also;
     * - https://qiita.com/aa_debdeb/items/3d02e28fb9ebfa357eaf (rotation order is XYZ)
     * - https://www.docswell.com/s/Kouhei_Ito/KDVNVK-2022-06-15-193343#p3
     * - 
     */
    using std::atan2; using std::asin;
    auto [q0, q1, q2, q3] = quaternion; /* [w, x, y, z] aligned name with the book for reviewability */

    auto phi   = atan2(2*(q2*q3 + q0*q1), 2*(q0*q0 + q3*q3) - 1);
    auto theta = asin (2*(q0*q2 - q1*q3));
    auto psi   = atan2(2*(q1*q2 + q0*q3), 2*(q0*q0 + q1*q1) - 1);

    return {phi, theta, psi};
}

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