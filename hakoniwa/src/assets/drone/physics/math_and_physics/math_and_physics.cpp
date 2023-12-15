#include "math_and_physics.hpp"
#include <cassert>

/* all the equations are from 
https://mtkbirdman.com/flight-dynamics-body-axes-system */

/* maths */
VelocityType velocity_body_to_ground(const VelocityType& b, const AngleType& a)
{
    const auto c_phi = cos(std::get<0>(a)), s_phi = sin(std::get<0>(a)),
        c_theta = cos(std::get<1>(a)), s_theta = sin(std::get<1>(a)),
        c_psi = cos(std::get<2>(a)), s_psi = sin(std::get<2>(a));
    const auto u = std::get<0>(b), v = std::get<1>(b), w = std::get<2>(b);

    VelocityType ground_velocity;
    
    /* see https://mtkbirdman.com/flight-dynamics-body-axes-system */

    auto& dot_x = std::get<0>(ground_velocity);
    auto& dot_y = std::get<1>(ground_velocity);
    auto& dot_z = std::get<2>(ground_velocity);
    
    dot_x =   (c_theta * c_psi)                         * u
            + (s_phi * s_theta * c_psi - c_phi * s_psi) * v
            + (c_phi * s_theta * c_psi + s_phi * s_psi) * w;

    dot_y =   (c_theta * s_psi)                         * u
            + (s_phi * s_theta * s_psi + c_phi * c_psi) * v
            + (c_phi * s_theta * s_psi - s_phi * c_psi) * w;
    
    dot_z =   (- s_theta)                                 * u
            + (s_psi * c_theta)                           * v
            + (c_psi * c_theta)                           * w;

    return ground_velocity;
}

VelocityType velocity_ground_to_body(
    const VelocityType& ground,
    const AngleType& a)
{
    auto c_phi = cos(std::get<0>(a)), s_phi = sin(std::get<0>(a)),
        c_theta = cos(std::get<1>(a)), s_theta = sin(std::get<1>(a)),
        c_psi = cos(std::get<2>(a)), s_psi = sin(std::get<2>(a));
    auto dot_x = std::get<0>(ground), dot_y = std::get<1>(ground), dot_z = std::get<2>(ground);

    VelocityType body_velocity;
    
    /* see (blue book 1.9-9 and URL here )*/
    auto& u = std::get<0>(body_velocity);
    auto& v = std::get<1>(body_velocity);
    auto& w = std::get<2>(body_velocity);
    
    u =   (c_theta * c_psi)                         * dot_x
        + (c_theta * s_psi)                         * dot_y
        - (s_theta)                                 * dot_z;

    v =   (s_phi * s_theta * c_psi - c_phi * s_psi) * dot_x
        + (s_phi * s_theta * s_psi + c_phi * c_psi) * dot_y
        + (s_phi * c_theta)                         * dot_z;

    w = (c_phi * s_theta * c_psi + s_phi * s_psi)   * dot_x
        + (c_phi * s_theta * s_psi - s_phi * c_psi) * dot_y
        + (c_phi * c_theta)                         * dot_z;

    return body_velocity;
}

AngularVelocityType angular_velocity_body_to_ground(
    const AngularVelocityType& body,
    const AngleType& angle)
{
    auto c_phi = cos(std::get<0>(angle)), s_phi = sin(std::get<0>(angle)),
        c_theta = cos(std::get<1>(angle)), t_theta = tan(std::get<1>(angle));

    auto p = std::get<0>(body), q = std::get<1>(body), r = std::get<2>(body);

    AngularVelocityType ground_angular_velocity;
    
    auto& dot_phi = std::get<0>(ground_angular_velocity);
    auto& dot_theta = std::get<1>(ground_angular_velocity);
    auto& dot_psi = std::get<2>(ground_angular_velocity);
    
    dot_phi = p + (r * c_phi + q * s_phi) * t_theta; /** zero div possible */
    dot_theta = q * c_phi - r * s_phi;
    dot_psi = (r * c_phi + q * s_phi) / c_theta; /** zero div possible */
    return ground_angular_velocity;
}

AngularVelocityType angular_velocity_ground_to_body(
    const AngularVelocityType& ground,
    const AngleType& angle)
{
    auto c_phi = cos(std::get<0>(angle)), s_phi = sin(std::get<0>(angle)),
        c_theta = cos(std::get<1>(angle)), s_theta = sin(std::get<1>(angle));
    auto dot_phi = std::get<0>(ground), dot_theta = std::get<1>(ground), dot_psi = std::get<2>(ground);

    AngularVelocityType body_angular_velocity;
    
    auto& p = std::get<0>(body_angular_velocity);
    auto& q = std::get<1>(body_angular_velocity);
    auto& r = std::get<2>(body_angular_velocity);
    
    p = dot_phi - dot_psi * s_theta;
    q = dot_theta * c_phi + dot_psi * s_phi * c_theta;
    r = dot_psi * c_phi * c_theta - dot_theta * s_phi;

    return body_angular_velocity;
}

/* physics */

AccelerationType acceleration_in_body_frame(
    const VelocityType& body,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag)
{
    assert(mass != 0.0); // TODO: remove this line

    auto c_phi = cos(std::get<0>(angle)), s_phi = sin(std::get<0>(angle)),
        c_theta = cos(std::get<1>(angle)), s_theta = sin(std::get<1>(angle));

    // velocities in body frame (u, v, w)
    auto u = std::get<0>(body), v = std::get<1>(body), w = std::get<2>(body);


    AccelerationType body_acceleration;

    // accelerations in body frame (u', v', w'), where primes(') mean time derivative.
    auto& dot_u = std::get<0>(body_acceleration);
    auto& dot_v = std::get<1>(body_acceleration);
    auto& dot_w = std::get<2>(body_acceleration);

    double g = gravity;
    double m = mass;
    double c = drag;
    double T = thrust;

    dot_u =       - g * s_theta            - c * u;
    dot_v =         g * c_theta * s_phi    - c * v;
    dot_w = -T/m  + g * c_theta * c_phi    - c * w;

    return body_acceleration;
}

AccelerationType acceleration_in_ground_frame(
    const VelocityType& ground,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag)
{
    assert(mass != 0.0); // TODO: remove this line

    auto c_phi = cos(std::get<0>(angle)), s_phi = sin(std::get<0>(angle)),
        c_theta = cos(std::get<1>(angle)), s_theta = sin(std::get<1>(angle)),
        c_psi = cos(std::get<2>(angle)), s_psi = sin(std::get<2>(angle));

    auto dot_x = std::get<0>(ground), dot_y = std::get<1>(ground), dot_z = std::get<2>(ground);


    AccelerationType ground_acceleration;

    auto& dotdot_x = std::get<0>(ground_acceleration);
    auto& dotdot_y = std::get<1>(ground_acceleration);
    auto& dotdot_z = std::get<2>(ground_acceleration);

    double g = gravity;
    double m = mass;
    double c = drag;
    double T = thrust;

    // see run_x, run_y, run_z in drone_dynamics_ground_frame.cpp
    // which comes from https://www.jstage.jst.go.jp/article/sicejl/56/1/56_3/_pdf
    // and z-axis is inverted(and also psi is inverted).
    // these inversions lead to the following minus signs in EVERY LINE(diff from PDF).

    dotdot_x = -T/m * (c_phi * s_theta * c_psi + s_phi * s_psi)
        - c * dot_x;
    dotdot_y = -T/m * (c_phi * s_theta * s_psi - s_phi * c_psi)
        - c * dot_y;
    dotdot_z = -T/m * (c_phi * c_theta)
        + g
        - c * dot_z;

    return ground_acceleration;
}
AngularAccelerationType angular_acceleration_in_body_frame(
    const AngularVelocityType& angular_velocity_in_body_frame,
    const AngleType& angle,
    double torque_x, /* in body frame */
    double torque_y, /* in body frame */
    double torque_z, /* in body frame */
    double I_xx, /* in body frame */
    double I_yy, /* in body frame */
    double I_zz /* in body frame */)
{
    (void)angle;
    assert(I_xx != 0.0); // TODO: remove this line
    assert(I_yy != 0.0); // TODO: remove this line
    assert(I_zz != 0.0); // TODO: remove this line

    /* current angular velocities in body frame */
    auto p = std::get<0>(angular_velocity_in_body_frame);
    auto q = std::get<1>(angular_velocity_in_body_frame);
    auto r = std::get<2>(angular_velocity_in_body_frame);
    
    AngularAccelerationType body_angular_acceleration;

    /* angular accelerations to calculate */
    auto& dot_p = std::get<0>(body_angular_acceleration);
    auto& dot_q = std::get<1>(body_angular_acceleration);
    auto& dot_r = std::get<2>(body_angular_acceleration);

    /*
     * For this equations, 
     * See https://www.sky-engin.jp/blog/eulers-equations-of-motion/ eq. (21) and the rest
     */

    dot_p = (torque_x - q*r*(I_zz -I_yy)) / I_xx;
    dot_q = (torque_y - r*p*(I_xx -I_zz)) / I_yy;
    dot_r = (torque_z - p*q*(I_yy -I_xx)) / I_zz;

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
