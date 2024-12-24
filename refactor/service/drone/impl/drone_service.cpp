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
    controller_inputs_ = {};
    controller_outputs_ = {};
    DronePositionType pos = aircraft_.get_drone_dynamics().get_pos();
    DroneEulerType angle = aircraft_.get_drone_dynamics().get_angle();
    DroneVelocityBodyFrameType velocity = aircraft_.get_drone_dynamics().get_vel_body_frame();
    DroneAngularVelocityBodyFrameType angular_velocity = aircraft_.get_gyro().sensor_value();

    drone_service_operation_->setup_controller_inputs(controller_inputs_, pdu_data_);
    controller_inputs_.max_rpm = aircraft_.get_rpm_max(0);
    controller_inputs_.pos_x = pos.data.x;
    controller_inputs_.pos_y = pos.data.y;
    controller_inputs_.pos_z = pos.data.z;
    controller_inputs_.euler_x = angle.data.x;
    controller_inputs_.euler_y = angle.data.y;
    controller_inputs_.euler_z = angle.data.z;
    controller_inputs_.u = velocity.data.x;
    controller_inputs_.v = velocity.data.y;
    controller_inputs_.w = velocity.data.z;
    controller_inputs_.p = angular_velocity.data.x;
    controller_inputs_.q = angular_velocity.data.y;
    controller_inputs_.r = angular_velocity.data.z;
}

void DroneService::setup_aircraft_inputs()
{
    //TODO setup aircraft inputs
}
