#include "drone_dynamics.hpp"
#include <math.h>

using hako::assets::drone::DroneDynamicsBodyFrame;

void DroneDynamicsBodyFrame::run_x(const DroneThrustype &thrust, const DroneTorqueType &torque) 
{
    (void)torque;

    this->next_velocity.data.x = 
        ( - (thrust.data * this->delta_time_sec) /  this->param_mass ) * 
        ( cos(this->angle.data.x) * sin(this->angle.data.y) * cos(this->angle.data.z) + sin(this->angle.data.x) * sin(this->angle.data.z) ) 
        +
        (1.0 - this->param_drag) * this->velocity.data.x;

    this->next_position.data.x = (this->velocity.data.x * this->delta_time_sec) + this->position.data.x;
    return;
}

void DroneDynamicsBodyFrame::run_y(const DroneThrustype &thrust, const DroneTorqueType &torque) 
{
    (void)torque;

    this->next_velocity.data.y = 
        ( - (thrust.data * this->delta_time_sec) /  this->param_mass ) * 
        ( cos(this->angle.data.x) * sin(this->angle.data.y) * sin(this->angle.data.z) - sin(this->angle.data.x) * cos(this->angle.data.z) ) 
        +
        (1.0 - this->param_drag) * this->velocity.data.y;

    this->next_position.data.y = (this->velocity.data.y * this->delta_time_sec) + this->position.data.y;
}

void DroneDynamicsBodyFrame::run_z(const DroneThrustype &thrust, const DroneTorqueType &torque) 
{
    (void)torque;

    this->next_velocity.data.z = 
        - ( (thrust.data  /  this->param_mass ) *  cos(this->angle.data.y) * cos(this->angle.data.x) - GRAVITY ) * this->delta_time_sec
        +
        (1.0 - this->param_drag) * this->velocity.data.z;

    this->next_position.data.z = (this->velocity.data.z * this->delta_time_sec) + this->position.data.z;
}

void DroneDynamicsBodyFrame::run_rx(const DroneThrustype &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    this->next_angularVelocity.data.x = (torque.data.x * this->delta_time_sec) + this->angularVelocity.data.x;
    this->next_angle.data.x           = (this->angularVelocity.data.x * this->delta_time_sec) + this->angle.data.x;
}

void DroneDynamicsBodyFrame::run_ry(const DroneThrustype &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    this->next_angularVelocity.data.y = (torque.data.y * this->delta_time_sec) + this->angularVelocity.data.y;
    this->next_angle.data.y           = (this->angularVelocity.data.y * this->delta_time_sec) + this->angle.data.y;
}

void DroneDynamicsBodyFrame::run_rz(const DroneThrustype &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    this->next_angularVelocity.data.z = (torque.data.z * this->delta_time_sec) + this->angularVelocity.data.z;
    this->next_angle.data.z           = (this->angularVelocity.data.z * this->delta_time_sec) + this->angle.data.z;
}