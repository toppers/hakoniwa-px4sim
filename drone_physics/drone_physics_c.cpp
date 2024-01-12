#include "drone_physics_c.h"
#include "drone_physics.hpp"
#include <cassert>

static hako::drone_physics::VectorType to_Vector(const dp_vector_t* v)
{
    assert(v);
    return hako::drone_physics::VectorType{v->x, v->y, v->z};
}

static hako::drone_physics::AngleType to_Angle(const dp_angle_t* v)
{
    assert(v);
    return hako::drone_physics::AngleType{v->phi, v->theta, v->psi};
}

static dp_vector_t to_dp_vector(const hako::drone_physics::VectorType& v)
{
    return dp_vector_t{v.x, v.y, v.z};
}

#ifdef TO_BE_IMPLEMENTED
static dp_angle_t to_dp_angle(const hako::drone_physics::AngleType& v)
{
    return dp_angle_t{v.phi, v.theta, v.psi};
}
#endif

extern "C" {

/* maths for frame transformations */
dp_velocity_t dp_vector_body_to_ground(
    const dp_velocity_t* body, /* non-null */
    const dp_angle_t* angle /* non-null */)
{
    assert(body);
    assert(angle);

    return to_dp_vector(
        hako::drone_physics::vector_body_to_ground(
            to_Vector(body),
            to_Angle(angle)
            )
        );
}

dp_velocity_t dp_vector_ground_to_body(
    const dp_velocity_t* ground, /* non-null */
    const dp_angle_t* angle /* non-null */)
{
    assert(ground);
    assert(angle);

    return to_dp_vector(
        hako::drone_physics::vector_ground_to_body(
            to_Vector(ground),
            to_Angle(angle)
            )
        );
}




} // extern "C"
