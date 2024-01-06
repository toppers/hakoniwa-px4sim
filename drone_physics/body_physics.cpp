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
    return {
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
}
double dot(const VectorType& u, const VectorType& v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}
double length_squared(const VectorType& v)
{
    return dot(v, v);
}
double length(const VectorType& v)
{
    return std::sqrt(length_squared(v));
}
VectorType& operator += (VectorType& u, const VectorType& v)
{
    u.x += v.x;    u.y += v.y;    u.z += v.z;
    return u;
}
VectorType operator + (const VectorType& u, const VectorType& v)
{
    VectorType result = u;
    return result += v;
}
VectorType& operator -= (VectorType& u, const VectorType& v)
{
    u.x -= v.x;    u.y -= v.y;    u.z -= v.z;
    return u;
}
VectorType operator - (const VectorType& u, const VectorType& v)
{
    VectorType result = u;
    return result -= v;
}
VectorType operator * (double s, const VectorType& v)
{
    return {s*v.x, s*v.y, s*v.z};
}
VectorType operator * (const VectorType& v, double s)
{
    return {s*v.x, s*v.y, s*v.z};
}
VectorType operator / (const VectorType& v, double s)
{
    return {v.x/s, v.y/s, v.z/s};
}


/**
 *  Maths section (3D frame transformations between body and ground)
 */

/* for generic vectors. use the meaningful aliases below  */
VectorType vector_body_to_ground(const VectorType& body, const AngleType& angle)
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

    return VectorType{x_e, y_e, z_e};
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

    return VectorType{x, y, z};
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

    return AngularVelocityType{dot_phi, dot_theta, dot_psi};
}

/* Tranlsform angular velocity in ground frame to body frame (eq.106)*/
AngularVelocityType angular_velocity_ground_to_body(
    const AngularVelocityType& ground,
    const AngleType& angle)
{
    using std::sin; using std::cos; using std::tan;
    const auto
        c_phi   = cos(angle.phi),   s_phi   = sin(angle.phi),
        c_theta = cos(angle.theta), s_theta = sin(angle.theta);
    const auto [dot_phi, dot_theta, dot_psi] = ground;

    /*
     * See eq.(1.106) in Nonami's book.
     * for the transformation equations.
     * See also https://mtkbirdman.com/flight-dynamics-body-axes-system
     */
    /*****************************************************************/  
    double p = dot_phi                   - dot_psi * s_theta;
    double q = dot_theta * c_phi         + dot_psi * s_phi * c_theta;
    double r = dot_psi * c_phi * c_theta - dot_theta * s_phi;
    /*****************************************************************/  

    return AngularVelocityType{p, q, r};
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
    using std::sin; using std::cos;

    const auto
        c_phi   = cos(angle.phi),   s_phi   = sin(angle.phi),
        c_theta = cos(angle.theta), s_theta = sin(angle.theta);
    const auto [u, v, w] = body_velocity;
    const auto [p, q, r] = body_angular_velocity;
    const auto g = gravity;
    const auto m = mass;
    const auto c = drag;
    const auto T = thrust;

    /*
     * See nonami's book eq.(1.136).(2.31)
     * Colioris's force is (p, q, r) x (u, v, w)
     */
    /*****************************************************************/  
    double dot_u =       - g * s_theta            - (q*w - r*v) - c/m * u;
    double dot_v =       + g * c_theta * s_phi    - (r*u - p*w) - c/m * v;
    double dot_w = -T/m  + g * c_theta * c_phi    - (p*v - q*u) - c/m * w;
    /*****************************************************************/  

    return AccelerationType{dot_u, dot_v, dot_w};
}

/* angular acceleration in body frame based on JW' = W x JW =Tb ...eq.(1.37),(2.31) */
AngularAccelerationType angular_acceleration_in_body_frame(
    const AngularVelocityType& angular_velocity_in_body_frame,
    double torque_x, /* in body frame */
    double torque_y, /* in body frame */
    double torque_z, /* in body frame */
    double I_xx, /* in body frame, 0 is not allowed */
    double I_yy, /* in body frame, 0 is not allowed */
    double I_zz /* in body frame, 0 is not allowed */)
{
    assert(I_xx != 0.0); // TODO: remove this line
    assert(I_yy != 0.0); // TODO: remove this line
    assert(I_zz != 0.0); // TODO: remove this line

    // current angular velocities in body frame
    const auto [p, q, r] = angular_velocity_in_body_frame;
 
    /*
     * See also Nonami's book eq. (2.31)(1.137), where L=tau_x, M=tau_y, N=tau_z.
     * and Ixz = Iyz = Izx = 0 is assumed.
     * See also https://www.sky-engin.jp/blog/eulers-equations-of-motion/ eq.(21)
     * and the rest.
     */
 
    /*****************************************************************/
    double dot_p = (torque_x - q*r*(I_zz -I_yy)) / I_xx;
    double dot_q = (torque_y - r*p*(I_xx -I_zz)) / I_yy;
    double dot_r = (torque_z - p*q*(I_yy -I_xx)) / I_zz;
    /*****************************************************************/  

    return AngularAccelerationType{dot_p, dot_q, dot_r};
}

/* Obsolete. for testing only. */
AccelerationType acceleration_in_body_frame_without_Coriolis_for_testing_only(
    const VelocityType& body,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag)
{
    assert(mass != 0.0); // TODO: remove this line
    using std::sin; using std::cos;

    const auto
        c_phi   = cos(angle.phi),   s_phi   = sin(angle.phi),
        c_theta = cos(angle.theta), s_theta = sin(angle.theta);
    const auto [u, v, w]  = body;
    const auto g = gravity;
    const auto m = mass;
    const auto c = drag;
    const auto T = thrust;

    /*****************************************************************/  
    double dot_u =       - g * s_theta            - c/m * u;
    double dot_v =         g * c_theta * s_phi    - c/m * v;
    double dot_w = -T/m  + g * c_theta * c_phi    - c/m * w;
    /*****************************************************************/  

    return AccelerationType{dot_u, dot_v, dot_w};
}


/* Obsolete: this is not well-implemented, use acceleration_in_body_frame, then translate to ground instead */
AccelerationType acceleration_in_ground_frame(
    const VelocityType& ground,
    const AngleType& angle,
    double thrust, double mass /* 0 is not allowed */, double gravity, double drag)
{
    assert(false && "this is not well-implemented, use acceleration_in_body_frame instead");
     assert(mass != 0.0); // TODO: remove this line
    using std::sin; using std::cos;

    const auto
        c_phi   = cos(angle.phi),   s_phi   = sin(angle.phi),
        c_theta = cos(angle.theta), s_theta = sin(angle.theta),
        c_psi   = cos(angle.psi),   s_psi   = sin(angle.psi);

    const auto [u, v, w] = ground;
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
    double dot_u = -T/m * (c_phi * s_theta * c_psi + s_phi * s_psi) - c * u;
    double dot_v = -T/m * (c_phi * s_theta * s_psi - s_phi * c_psi) - c * v;
    double dot_w = -T/m * (c_phi * c_theta)                   + g   - c * w;
    /*****************************************************************/  

    return AccelerationType{dot_u, dot_v, dot_w};
}

VectorType velocity_after_contact_with_wall(
    const VectorType& before_contact,
    const VectorType& normal_vector, /* of the wall, will be normalized internally */
    double restitution_coeff /* 0.0 - 1.0 */)
{
    assert(length_squared(normal_vector) > 1.0e-10);

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