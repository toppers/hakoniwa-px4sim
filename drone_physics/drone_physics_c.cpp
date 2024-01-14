#include "drone_physics_c.h"
#include "drone_physics.hpp"
#include <cassert>

static hako::drone_physics::VectorType to_Vector(const dp_vector_t* v)
{
    assert(v);
    return hako::drone_physics::VectorType{v->x, v->y, v->z};
}

static hako::drone_physics::EulerType to_Angle(const dp_euler_t* v)
{
    assert(v);
    return hako::drone_physics::EulerType{v->phi, v->theta, v->psi};
}

static dp_vector_t to_dp_vector(const hako::drone_physics::VectorType& v)
{
    return dp_vector_t{v.x, v.y, v.z};
}

#ifdef TO_BE_IMPLEMENTED
static dp_euler_t to_dp_angle(const hako::drone_physics::EulerType& v)
{
    return dp_euler_t{v.phi, v.theta, v.psi};
}
#endif

extern "C" {

/* maths for frame transformations */
dp_velocity_t dp_ground_vector_from_body(
    const dp_velocity_t* body, /* non-null */
    const dp_euler_t* angle /* non-null */)
{
    assert(body);
    assert(angle);

    return to_dp_vector(
        hako::drone_physics::ground_vector_from_body(
            to_Vector(body),
            to_Angle(angle)
            )
        );
}

dp_velocity_t dp_body_vector_from_ground(
    const dp_velocity_t* ground, /* non-null */
    const dp_euler_t* angle /* non-null */)
{
    assert(ground);
    assert(angle);

    return to_dp_vector(
        hako::drone_physics::body_vector_from_ground(
            to_Vector(ground),
            to_Angle(angle)
            )
        );
}




} // extern "C"
