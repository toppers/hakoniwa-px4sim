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
VectorType cross(const VectorType& u, const VectorType& v)
{
    using std::get;
    return VectorType(
        get<1>(u) * get<2>(v) - get<2>(u) * get<1>(v),
        get<2>(u) * get<0>(v) - get<0>(u) * get<2>(v),
        get<0>(u) * get<1>(v) - get<1>(u) * get<0>(v)
    );
}
VectorType& operator += (VectorType& u, const VectorType& v)
{
    using std::get;
    get<0>(u) += get<0>(v);
    get<1>(u) += get<1>(v);
    get<2>(u) += get<2>(v);
    return u;
}
VectorType operator + (const VectorType& u, const VectorType& v)
{
    VectorType result = u;
    return result += v;
}

/**
 *  Maths section (3D frame transformations between body and ground)
 */

/* for generic vectors. use the meaningful aliases below  */
VectorType vector_body_to_ground(const VectorType& body, const AngleType& angle)
{
    using std::get; using std::sin; using std::cos;
    const auto
        c_phi   = cos(get<0>(angle)),   s_phi = sin(get<0>(angle)),
        c_theta = cos(get<1>(angle)), s_theta = sin(get<1>(angle)),
        c_psi   = cos(get<2>(angle)),   s_psi = sin(get<2>(angle));
    
    const auto [x, y, z] = body;

    // will be return value
    VectorType ground;
    // ground coordinates (x_e, y_e, z_e) reference to ground, where 'e' means earth.
    auto& [x_e, y_e, z_e] = ground;

    /*
     * eq.(1.71),(1.124) in Nonami's book.
     * See also https://mtkbirdman.com/flight-dynamics-body-axes-system
     * for the transformation equations.
     */
    /*****************************************************************/  
    x_e =   (c_theta * c_psi)                           * x
            + (s_phi * s_theta * c_psi - c_phi * s_psi) * y
            + (c_phi * s_theta * c_psi + s_phi * s_psi) * z;

    y_e =   (c_theta * s_psi)                           * x
            + (s_phi * s_theta * s_psi + c_phi * c_psi) * y
            + (c_phi * s_theta * s_psi - s_phi * c_psi) * z;
    
    z_e =   (- s_theta)                                 * x
            + (s_phi * c_theta)                         * y
            + (c_phi * c_theta)                         * z;
    /*****************************************************************/

    return ground;
}

/* Transform velocity in body frame to ground frame */
VelocityType velocity_body_to_ground(const VelocityType& body, const AngleType& angle)
{
    return vector_body_to_ground(body, angle);
}

/* for generic vectors. use the meaningful aliases below  */
VectorType vector_ground_to_body(const VectorType& ground,
    const AngleType& angle)
{
    using std::get; using std::sin; using std::cos;
    const auto
        c_phi   = cos(get<0>(angle)), s_phi   = sin(get<0>(angle)),
        c_theta = cos(get<1>(angle)), s_theta = sin(get<1>(angle)),
        c_psi   = cos(get<2>(angle)), s_psi   = sin(get<2>(angle));
    const auto [x_e, y_e, z_e] = ground;

    // will be return value
    VectorType body;
    auto& [x, y, z] = body;

    /*
     * See eq.(1.69), inverse of (1.124) in Nonami's book.
     * See also https://mtkbirdman.com/flight-dynamics-body-axes-system
     * for the transformation equations.
     */
    /*****************************************************************/  
    x =   (c_theta * c_psi)                         * x_e
        + (c_theta * s_psi)                         * y_e
        - (s_theta)                                 * z_e;

    y =   (s_phi * s_theta * c_psi - c_phi * s_psi) * x_e
        + (s_phi * s_theta * s_psi + c_phi * c_psi) * y_e
        + (s_phi * c_theta)                         * z_e;

    z = (c_phi * s_theta * c_psi + s_phi * s_psi)   * x_e
        + (c_phi * s_theta * s_psi - s_phi * c_psi) * y_e
        + (c_phi * c_theta)                         * z_e;
    /*****************************************************************/  

    return body;
}

/* Tranlsform velocity in ground frame to body frame */
VelocityType velocity_ground_to_body(
    const VelocityType& ground,
    const AngleType& angle)
{
    return vector_ground_to_body(ground, angle);
}

/* Tranlsform angular velocity in body frame to ground frame eq.(1.109)*/
AngularVelocityType angular_velocity_body_to_ground(
    const AngularVelocityType& body,
    const AngleType& angle)
{
    using std::get; using std::sin; using std::cos; using std::tan;
    const auto
        c_phi   = cos(get<0>(angle)),
        s_phi   = sin(get<0>(angle)),
        c_theta = cos(get<1>(angle)),
        t_theta = tan(get<1>(angle));
    const auto [p, q, r] = body;

   // will be return value
   AngularVelocityType ground_angular_velocity;
   auto& [dot_phi, dot_theta, dot_psi] = ground_angular_velocity;

     /*
     * See eq.(1.109) in Nonami's book.
     * for the transformation equations.
     * See also https://mtkbirdman.com/flight-dynamics-body-axes-system
     */
    /*****************************************************************/  
    dot_phi   = p + (r * c_phi + q * s_phi) * t_theta; /** overflow INF possible */
    dot_theta = q * c_phi - r * s_phi;
    dot_psi   = (r * c_phi + q * s_phi) / c_theta; /** zero div INF possible */
    /*****************************************************************/  

    return ground_angular_velocity;
}

/* Tranlsform angular velocity in ground frame to body frame (eq.106)*/
AngularVelocityType angular_velocity_ground_to_body(
    const AngularVelocityType& ground,
    const AngleType& angle)
{
    using std::get; using std::sin; using std::cos; using std::tan;
    const auto
        c_phi   = cos(get<0>(angle)), s_phi   = sin(get<0>(angle)),
        c_theta = cos(get<1>(angle)), s_theta = sin(get<1>(angle));

    const auto
        dot_phi = get<0>(ground),
        dot_theta = get<1>(ground),
        dot_psi = get<2>(ground);

    // will be return value
    AngularVelocityType body_angular_velocity;
    auto& [p, q, r] = body_angular_velocity;

    /*
     * See eq.(1.106) in Nonami's book.
     * for the transformation equations.
     * See also https://mtkbirdman.com/flight-dynamics-body-axes-system
     */
    /*****************************************************************/  
    p = dot_phi - dot_psi * s_theta;
    q = dot_theta * c_phi + dot_psi * s_phi * c_theta;
    r = dot_psi * c_phi * c_theta - dot_theta * s_phi;
    /*****************************************************************/  

    return body_angular_velocity;
}

/**
 * Physics section.
 * The functions below includes Force, Mass, Torque, and Inertia,
 * and calculates accelerations and angular accelerations.
 * 
 * Calculations are based in the body frame.
 * So the results should be transformed to the ground frame, if needed.
 */

/* acceleration in body frame based on mV'+ w x mV = F ... eq.(1.36),(2.31)*/
AccelerationType acceleration_in_body_frame(
    const VelocityType& body_velocity,
    const AngleType& angle,
    const AngularVelocityType& body_angular_velocity,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag)
{
    assert(mass != 0.0); // TODO: remove this line
    using std::get; using std::sin; using std::cos;

    const auto
        c_phi   = cos(get<0>(angle)), s_phi   = sin(get<0>(angle)),
        c_theta = cos(get<1>(angle)), s_theta = sin(get<1>(angle));

    const auto [u, v, w] = body_velocity;
    const auto [p, q, r] = body_angular_velocity;

    // will be return value
    AccelerationType body_acceleration;
    // (u', v', w'), where primes(') mean time derivative.
    // references to return value
    auto& [dot_u, dot_v, dot_w] = body_acceleration;
    
    const auto g = gravity;
    const auto m = mass;
    const auto c = drag;
    const auto T = thrust;

    /*
     * See nonami's book eq.(1.136).(2.31)
     * Colioris's force is (p, q, r) x (u, v, w)
     */
    /*****************************************************************/  
    dot_u =       - g * s_theta            - (q*w - r*v) - c/m * u;
    dot_v =       + g * c_theta * s_phi    - (r*u - p*w) - c/m * v;
    dot_w = -T/m  + g * c_theta * c_phi    - (p*v - q*u) - c/m * w;
    /*****************************************************************/  

    return body_acceleration;
}

/* angular acceleration in body frame based on JW' = W x JW =Tb ...eq.(1.37),(2.31) */
AngularAccelerationType angular_acceleration_in_body_frame(
    const AngularVelocityType& angular_velocity_in_body_frame,
    const AngleType& angle,
    double torque_x, /* in body frame */
    double torque_y, /* in body frame */
    double torque_z, /* in body frame */
    double I_xx, /* in body frame, 0 is not allowed */
    double I_yy, /* in body frame, 0 is not allowed */
    double I_zz /* in body frame, 0 is not allowed */)
{
    using std::get;
    (void)angle; // not used for now

    assert(I_xx != 0.0); // TODO: remove this line
    assert(I_yy != 0.0); // TODO: remove this line
    assert(I_zz != 0.0); // TODO: remove this line

    // current angular velocities in body frame
    const auto [p, q, r] = angular_velocity_in_body_frame;
    
    // will be return value
    AngularAccelerationType body_angular_acceleration;
    // (p', q', r'), where primes(') mean time derivative.
    // references to return value
   auto& [dot_p, dot_q, dot_r] = body_angular_acceleration;
 
    /*
     * See also Nonami's book eq. (2.31)(1.137), where L=tau_x, M=tau_y, N=tau_z.
     * and Ixz = Iyz = Izx = 0 is assumed.
     * See also https://www.sky-engin.jp/blog/eulers-equations-of-motion/ eq.(21)
     * and the rest.
     */
 
    /*****************************************************************/
    dot_p = (torque_x - q*r*(I_zz -I_yy)) / I_xx;
    dot_q = (torque_y - r*p*(I_xx -I_zz)) / I_yy;
    dot_r = (torque_z - p*q*(I_yy -I_xx)) / I_zz;
    /*****************************************************************/  

    return body_angular_acceleration;
}

/* Obsolete. for testing only. */
AccelerationType acceleration_in_body_frame_without_Coriolis_for_testing_only(
    const VelocityType& body,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag)
{
    assert(mass != 0.0); // TODO: remove this line
    using std::get; using std::sin; using std::cos;

    const auto
        c_phi   = cos(get<0>(angle)), s_phi   = sin(get<0>(angle)),
        c_theta = cos(get<1>(angle)), s_theta = sin(get<1>(angle));
    // velocities in body frame (u, v, w)
    const auto [u, v, w]  = body;

    AccelerationType body_acceleration;
    // accelerations in body frame (u', v', w'), where primes(') mean time derivative.
    auto& [dot_u, dot_v, dot_w] = body_acceleration;

    const auto g = gravity;
    const auto m = mass;
    const auto c = drag;
    const auto T = thrust;

    /*****************************************************************/  
    dot_u =       - g * s_theta            - c/m * u;
    dot_v =         g * c_theta * s_phi    - c/m * v;
    dot_w = -T/m  + g * c_theta * c_phi    - c/m * w;
    /*****************************************************************/  

    return body_acceleration;
}


/* Obsolete: this is not well-implemented, use acceleration_in_body_frame, then translate to ground instead */
AccelerationType acceleration_in_ground_frame(
    const VelocityType& ground,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag)
{
    assert(false && "this is not well-implemented, use acceleration_in_body_frame instead");
     assert(mass != 0.0); // TODO: remove this line
    using std::get; using std::sin; using std::cos;

    const auto
        c_phi   = cos(get<0>(angle)), s_phi   = sin(get<0>(angle)),
        c_theta = cos(get<1>(angle)), s_theta = sin(get<1>(angle)),
        c_psi   = cos(get<2>(angle)), s_psi   = sin(get<2>(angle));

    const auto [u, v, w] = ground;

    // will be return value
    AccelerationType ground_acceleration;
    // (u', v', w'), where primes(') mean time derivative.
    // references to return value
    auto& [dot_u, dot_v, dot_w] = ground_acceleration;


    const auto g = gravity;
    const auto m = mass;
    const auto c = drag;
    const auto T = thrust;

    /*
     * See https://www.jstage.jst.go.jp/article/sicejl/56/1/56_3/_pdf
     * and z-axis is inverted(and also psi is inverted).
     * these inversions lead to the following minus signs in EVERY LINE(diff from PDF).
     */

    /*****************************************************************/  
    dot_u = -T/m * (c_phi * s_theta * c_psi + s_phi * s_psi) - c * u;
    dot_v = -T/m * (c_phi * s_theta * s_psi - s_phi * c_psi) - c * v;
    dot_w = -T/m * (c_phi * c_theta)                   + g   - c * w;
    /*****************************************************************/  

    return ground_acceleration;
}

} /* namespace hako::drone_physics */