#include "service/drone/impl/drone_service.hpp"

using namespace hako::service::impl;

void DroneService::advanceTimeStep() 
{
    //setup input data for controller
    setup_controller_inputs();

    //run controller
    controller_outputs_ = controller_.run(controller_inputs_);

    //run mixer
    pwm_duty_ = mixer_.run(controller_outputs_);

    //setup input data for aircraft
    setup_aircraft_inputs();

    //run aircraft
    aircraft_.run(aircraft_inputs_);

    //TODO run drone service operation
    
    simulation_time_usec_ += delta_time_usec_;
}

void DroneService::setup_controller_inputs()
{
    //TODO setup controller inputs
}

void DroneService::setup_aircraft_inputs()
{
    //TODO setup aircraft inputs
}
