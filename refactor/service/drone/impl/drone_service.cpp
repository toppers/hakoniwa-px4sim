#include "service/drone/impl/drone_service.hpp"

using namespace hako::service::impl;

void DroneService::advanceTimeStep() {
    //TODO

    simulation_time_usec_ += delta_time_usec_;
}
