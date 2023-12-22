#include "math_and_physics.hpp"
#include <cassert>

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
    const auto x = get<0>(body), y = get<1>(body), z = get<2>(body);

    // will be return value
    VectorType ground;

    // ground coordinates (x_e, y_e, z_e), where 'e' means earth.
    auto& x_e = get<0>(ground);
    auto& y_e = get<1>(ground);
    auto& z_e = get<2>(ground);

    /*
     * See https://mtkbirdman.com/flight-dynamics-body-axes-system
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
            + (s_psi * c_theta)                         * y
            + (c_psi * c_theta)                         * z;
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
    const auto
        x_e = get<0>(ground),
        y_e = get<1>(ground),
        z_e = get<2>(ground);

    // will be return value
    VectorType body;
    
    auto& x = get<0>(body);
    auto& y = get<1>(body);
    auto& z = get<2>(body);
    
     /*
     * See https://mtkbirdman.com/flight-dynamics-body-axes-system
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

/* Tranlsform angular velocity in body frame to ground frame */
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

    const auto p = get<0>(body), q = get<1>(body), r = get<2>(body);

   // will be return value
   AngularVelocityType ground_angular_velocity;
    
    auto& dot_phi   = get<0>(ground_angular_velocity);
    auto& dot_theta = get<1>(ground_angular_velocity);
    auto& dot_psi   = get<2>(ground_angular_velocity);

     /*
     * See https://mtkbirdman.com/flight-dynamics-body-axes-system
     * for the transformation equations.
     */
    /*****************************************************************/  
    dot_phi   = p + (r * c_phi + q * s_phi) * t_theta; /** overflow INF possible */
    dot_theta = q * c_phi - r * s_phi;
    dot_psi   = (r * c_phi + q * s_phi) / c_theta; /** zero div INF possible */
    /*****************************************************************/  

    return ground_angular_velocity;
}

/* Tranlsform angular velocity in ground frame to body frame */
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
    
    auto& p = get<0>(body_angular_velocity);
    auto& q = get<1>(body_angular_velocity);
    auto& r = get<2>(body_angular_velocity);

    /*
     * See https://mtkbirdman.com/flight-dynamics-body-axes-system
     * for the transformation equations.
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

/* acceleration in body frame based on mV'+ w x mV = F ... Nonami (2.31)*/
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

    // velocities in body frame (u, v, w)
    const auto
        u = get<0>(body_velocity),
        v = get<1>(body_velocity),
        w = get<2>(body_velocity);

    // angular velocities in body frame (p, q, r)
    const auto
        p = get<0>(body_angular_velocity),
        q = get<1>(body_angular_velocity),
        r = get<2>(body_angular_velocity);

    // will be return value
    AccelerationType body_acceleration;

    // accelerations in body frame (u', v', w'), where primes(') mean time derivative.
    auto& dot_u = get<0>(body_acceleration);
    auto& dot_v = get<1>(body_acceleration);
    auto& dot_w = get<2>(body_acceleration);

    const auto g = gravity;
    const auto m = mass;
    const auto c = drag;
    const auto T = thrust;

    /*
     * See nonami's book eq.(1.136). Colioris's force is (p, q, r) x (u, v, w)
     */
    /*****************************************************************/  
    dot_u =       - g * s_theta            - (q*w - r*v) - c/m * u;
    dot_v =       + g * c_theta * s_phi    - (r*u - p*w) - c/m * v;
    dot_w = -T/m  + g * c_theta * c_phi    - (p*v - q*u) - c/m * w;
    /*****************************************************************/  

    return body_acceleration;
}

/* angular acceleration in body frame based on JW' = W x JW =Tb ... Nonami (2.31) */
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
    const auto p = get<0>(angular_velocity_in_body_frame);
    const auto q = get<1>(angular_velocity_in_body_frame);
    const auto r = get<2>(angular_velocity_in_body_frame);
    
    // will be return value
    AngularAccelerationType body_angular_acceleration;

    // angular accelerations to calculate
    auto& dot_p = get<0>(body_angular_acceleration);
    auto& dot_q = get<1>(body_angular_acceleration);
    auto& dot_r = get<2>(body_angular_acceleration);

 
    /*
     * For this equations, 
     * See https://www.sky-engin.jp/blog/eulers-equations-of-motion/ eq. (21)
     * and the rest.
     * See also Nonami's book eq. (1.136),(1.137), where L=tau_x, M=tau_y, N=tau_z.
     * and Ixz = Iyz = Izx = 0 is assumed.
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
    const auto u = get<0>(body), v = get<1>(body), w = get<2>(body);

    AccelerationType body_acceleration;

    // accelerations in body frame (u', v', w'), where primes(') mean time derivative.
    auto& dot_u = get<0>(body_acceleration);
    auto& dot_v = get<1>(body_acceleration);
    auto& dot_w = get<2>(body_acceleration);

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

    const auto
        dot_x = get<0>(ground),
        dot_y = get<1>(ground),
        dot_z = get<2>(ground);

    // will be return value
    AccelerationType ground_acceleration;

    auto& dotdot_x = get<0>(ground_acceleration);
    auto& dotdot_y = get<1>(ground_acceleration);
    auto& dotdot_z = get<2>(ground_acceleration);

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
    dotdot_x = -T/m * (c_phi * s_theta * c_psi + s_phi * s_psi) - c * dot_x;
    dotdot_y = -T/m * (c_phi * s_theta * s_psi - s_phi * c_psi) - c * dot_y;
    dotdot_z = -T/m * (c_phi * c_theta)                   + g   - c * dot_z;
    /*****************************************************************/  

    return ground_acceleration;
}
