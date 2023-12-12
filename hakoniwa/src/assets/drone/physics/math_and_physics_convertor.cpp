#include "drone_primitive_types.hpp"

glm::dvec3 hako::assets::drone::tupleToVec3(const std::tuple<double, double, double>& tuple) {
    return glm::dvec3(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
}

