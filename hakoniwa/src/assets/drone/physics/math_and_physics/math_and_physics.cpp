#include "math_and_physics.hpp"
#include <cassert>

/*
 *  All the equations are from 
 * https://mtkbirdman.com/flight-dynamics-body-axes-system
 * https://www.jstage.jst.go.jp/article/sicejl/56/1/56_3/_pdf
 * https://www.sky-engin.jp/blog/eulers-equations-of-motion/
 */

/* utility functions */
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


/* maths */
VelocityType velocity_body_to_ground(const VelocityType& b, const AngleType& a)
{
    using namespace std;
    const auto
        c_phi   = cos(get<0>(a)),   s_phi = sin(get<0>(a)),
        c_theta = cos(get<1>(a)), s_theta = sin(get<1>(a)),
        c_psi   = cos(get<2>(a)),   s_psi = sin(get<2>(a));
    const auto u = get<0>(b), v = get<1>(b), w = get<2>(b);

    // will be return value
    VelocityType ground_velocity;
    
    auto& dot_x = get<0>(ground_velocity);
    auto& dot_y = get<1>(ground_velocity);
    auto& dot_z = get<2>(ground_velocity);

    /*
     * See https://mtkbirdman.com/flight-dynamics-body-axes-system
     * for the transformation equations.
     */
    /*****************************************************************/  
    dot_x =   (c_theta * c_psi)                         * u
            + (s_phi * s_theta * c_psi - c_phi * s_psi) * v
            + (c_phi * s_theta * c_psi + s_phi * s_psi) * w;

    dot_y =   (c_theta * s_psi)                         * u
            + (s_phi * s_theta * s_psi + c_phi * c_psi) * v
            + (c_phi * s_theta * s_psi - s_phi * c_psi) * w;
    
    dot_z =   (- s_theta)                               * u
            + (s_psi * c_theta)                         * v
            + (c_psi * c_theta)                         * w;
    /*****************************************************************/

    return ground_velocity;
}

VelocityType velocity_ground_to_body(
    const VelocityType& ground,
    const AngleType& a)
{
    using namespace std;
    const auto
        c_phi   = cos(get<0>(a)), s_phi   = sin(get<0>(a)),
        c_theta = cos(get<1>(a)), s_theta = sin(get<1>(a)),
        c_psi   = cos(get<2>(a)), s_psi   = sin(get<2>(a));
    const auto
        dot_x = get<0>(ground),
        dot_y = get<1>(ground),
        dot_z = get<2>(ground);

    // will be return value
    VelocityType body_velocity;
    
    auto& u = get<0>(body_velocity);
    auto& v = get<1>(body_velocity);
    auto& w = get<2>(body_velocity);
    
     /*
     * See https://mtkbirdman.com/flight-dynamics-body-axes-system
     * for the transformation equations.
     */
    /*****************************************************************/  
    u =   (c_theta * c_psi)                         * dot_x
        + (c_theta * s_psi)                         * dot_y
        - (s_theta)                                 * dot_z;

    v =   (s_phi * s_theta * c_psi - c_phi * s_psi) * dot_x
        + (s_phi * s_theta * s_psi + c_phi * c_psi) * dot_y
        + (s_phi * c_theta)                         * dot_z;

    w = (c_phi * s_theta * c_psi + s_phi * s_psi)   * dot_x
        + (c_phi * s_theta * s_psi - s_phi * c_psi) * dot_y
        + (c_phi * c_theta)                         * dot_z;
    /*****************************************************************/  

    return body_velocity;
}

AngularVelocityType angular_velocity_body_to_ground(
    const AngularVelocityType& body,
    const AngleType& angle)
{
    using namespace std;
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

AngularVelocityType angular_velocity_ground_to_body(
    const AngularVelocityType& ground,
    const AngleType& angle)
{
    using namespace std;
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

/* physics */

AccelerationType acceleration_in_body_frame_without_Coriolis(
    const VelocityType& body,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag)
{
    assert(mass != 0.0); // TODO: remove this line
    using namespace std;

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

AccelerationType acceleration_in_body_frame(
    const VelocityType& body_velocity,
    const AngleType& angle,
    const AngularVelocityType& body_angular_velocity,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag)
{
    assert(mass != 0.0); // TODO: remove this line
    using namespace std;

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

/* NOTE : this is not well-implemented, use acceleration_in_body_frame instead */
AccelerationType acceleration_in_ground_frame(
    const VelocityType& ground,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag)
{
    assert(false && "this is not well-implemented, use acceleration_in_body_frame instead");
    assert(mass != 0.0); // TODO: remove this line
    using namespace std;

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
    using namespace std;
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

#if 0 /* not well-implemented yet */
AngularAccelerationType angular_acceleration_in_ground_frame(
    const AngularVelocityType& angular_velocity_in_ground_frame,
    const AngleType& angle,
    double torque_x, /* in ground frame */
    double torque_y, /* in ground frame */
    double torque_z, /* in ground frame */
    double inertia_x, /* in body frame */
    double inertia_y, /* in body frame */
    double inertia_z /* in body frame */)
{
    /* for now, use the same equation but NEEDS CONVERSION */
    return angular_acceleration_in_body_frame(angular_velocity_in_ground_frame, angle, torque_x, torque_y, torque_z, inertia_x, inertia_y, inertia_z);
}
#endif
