#include "drone_physics_c.h"
#include "drone_physics.hpp"
#include <cassert>

static hako::drone_physics::VectorType to_Vector(const dp_vector_t* v)
{
    assert(v);
    return hako::drone_physics::VectorType{v->x, v->y, v->z};
}

static hako::drone_physics::EulerType to_Euler(const dp_euler_t* e)
{
    assert(e);
    return hako::drone_physics::EulerType{e->phi, e->theta, e->psi};
}

static dp_vector_t to_dp_vector(const hako::drone_physics::VectorType& v)
{
    return dp_vector_t{v.x, v.y, v.z};
}

/* not used for now
static dp_euler_t to_dp_euler(const hako::drone_physics::EulerType& e)
{
    return dp_euler_t{e.phi, e.theta, e.psi};
}*/

extern "C" {

/* maths for frame transformations */
dp_vector_t dp_ground_vector_from_body(
    const dp_vector_t* body, /* non-null */
    const dp_euler_t* angle /* non-null */)
{
    assert(body);
    assert(angle);

    return to_dp_vector(
        hako::drone_physics::ground_vector_from_body(
            to_Vector(body),
            to_Euler(angle)
            )
        );
}

dp_vector_t dp_body_vector_from_ground(
    const dp_vector_t* ground, /* non-null */
    const dp_euler_t* angle /* non-null */)
{
    assert(ground);
    assert(angle);

    return to_dp_vector(
        hako::drone_physics::body_vector_from_ground(
            to_Vector(ground),
            to_Euler(angle)
            )
        );
}

dp_acceleration_t dp_acceleration_in_body_frame(
    const dp_vector_t* body_velocity,
    const dp_euler_t* angle,
    const dp_angular_velocity_t* body_angular_velocity,
    double thrust, double mass, double gravity, double drag)
{
    assert(body_velocity);
    assert(angle);
    assert(body_angular_velocity);

    return to_dp_vector(
        hako::drone_physics::acceleration_in_body_frame(
            to_Vector(body_velocity),
            to_Euler(angle),
            to_Vector(body_angular_velocity),
            thrust, mass, gravity, drag
            )
        );
}

dp_angular_acceleration_t dp_angular_acceleration_in_body_frame(
    const dp_angular_velocity_t* body_angular_velocity,
    const dp_torque_t* torque, /* in body frame */
    double I_xx, /* in body frame, 0 is not allowed */
    double I_yy, /* in body frame, 0 is not allowed */
    double I_zz /* in body frame, 0 is not allowed */)
{
    assert(body_angular_velocity);
    assert(torque);

    return to_dp_vector(
        hako::drone_physics::angular_acceleration_in_body_frame(
            to_Vector(body_angular_velocity),
            to_Vector(torque), I_xx, I_yy, I_zz
            )
        );
}



} // extern "C"
