#include "math_and_physics.hpp"

/* maths */
VelocityType velocity_body_to_ground(VelocityType b, AngleType a)
{
    const auto c_phi = cos(std::get<0>(a)), s_phi = sin(std::get<0>(a)),
        c_theta = cos(std::get<1>(a)), s_theta = sin(std::get<1>(a)),
        c_psi = cos(std::get<2>(a)), s_psi = sin(std::get<2>(a));
    const auto u = std::get<0>(b), v = std::get<1>(b), w = std::get<2>(b);

    VelocityType ground_velocity;
    
    /* see (blue book 1.9-9 and URL here )*/
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

VelocityType velocity_ground_to_body(VelocityType ground, AngleType a)
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
