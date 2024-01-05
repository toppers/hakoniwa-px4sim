#include "drone_primitive_types.hpp"

glm::dvec3 hako::assets::drone::tupleToVec3(const hako::drone_physics::VectorType&t) {
    return glm::dvec3(t.x, t.y, t.z);
}

glm::dvec3 hako::assets::drone::tupleToVec3(const hako::drone_physics::AngleType&t) {
    return glm::dvec3(t.phi, t.theta, t.psi);
}

