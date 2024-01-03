#include "drone_physics_c.h"
#include "drone_physics.hpp"
#include <cassert>

extern "C" {

static inline hako::drone_physics::VectorType to_vector(const dp_vector_t* v)
{
    assert(v);
    return hako::drone_physics::VectorType(v->x, v->y, v->z);
}

static inline dp_vector_t to_dp_vector(const hako::drone_physics::VectorType& v)
{
    dp_vector_t dp_vector = {std::get<0>(v), std::get<1>(v), std::get<2>(v)};
    return dp_vector;
}


/* maths for frame transformations */
dp_velocity_t dp_velocity_body_to_ground(
    const dp_velocity_t* body, /* non-null */
    const dp_angle_t* angle /* non-null */)
{
    assert(body);
    assert(angle);

    return to_dp_vector(
        hako::drone_physics::velocity_body_to_ground(
            to_vector(body),
            to_vector(angle)
            )
        );
}

/* maths for frame transformations */
dp_velocity_t dp_velocity_ground_to_body(
    const dp_velocity_t* ground, /* non-null */
    const dp_angle_t* angle /* non-null */)
{
    assert(ground);
    assert(angle);

    return to_dp_vector(
        hako::drone_physics::velocity_ground_to_body(
            to_vector(ground),
            to_vector(angle)
            )
        );
}




} // extern "C"
