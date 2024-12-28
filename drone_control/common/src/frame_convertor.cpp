#include "frame_convertor.hpp"
#include <cmath>

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
