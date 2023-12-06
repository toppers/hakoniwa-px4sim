#include "drone_dynamics_ground_frame.hpp"
#include <math.h>

using hako::assets::drone::DroneDynamicsGroundFrame;

void DroneDynamicsGroundFrame::run_x(const DroneThrustType &thrust, const DroneTorqueType &torque) 
{
    (void)torque;

    this->next_velocity.data.x =
        this->delta_time_sec *  
        ( 
            - (thrust.data /  this->param_mass ) *  ( cos(this->angle.data.x) * sin(this->angle.data.y) * cos(this->angle.data.z) + sin(this->angle.data.x) * sin(this->angle.data.z) )
            - this->param_drag * this->velocity.data.x
        ) 
        +
        this->velocity.data.x;

    this->next_position.data.x = (this->velocity.data.x * this->delta_time_sec) + this->position.data.x;
    return;
}

void DroneDynamicsGroundFrame::run_y(const DroneThrustType &thrust, const DroneTorqueType &torque) 
{
    (void)torque;

    this->next_velocity.data.y = 
        this->delta_time_sec *  
        ( 
            - (thrust.data /  this->param_mass ) * ( cos(this->angle.data.x) * sin(this->angle.data.y) * sin(this->angle.data.z) - sin(this->angle.data.x) * cos(this->angle.data.z) )
            - this->param_drag * this->velocity.data.y
        ) 
        +
        this->velocity.data.y;

    this->next_position.data.y = (this->velocity.data.y * this->delta_time_sec) + this->position.data.y;
}

void DroneDynamicsGroundFrame::run_z(const DroneThrustType &thrust, const DroneTorqueType &torque) 
{
    (void)torque;

    this->next_velocity.data.z = 
        this->delta_time_sec * 
        (
            - ( (thrust.data  /  this->param_mass ) *  cos(this->angle.data.y) * cos(this->angle.data.x) - GRAVITY ) 
            - this->param_drag * this->velocity.data.z
        )
        +
        this->velocity.data.z;

    this->next_position.data.z = (this->velocity.data.z * this->delta_time_sec) + this->position.data.z;
}

void DroneDynamicsGroundFrame::run_rx(const DroneThrustType &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    this->next_angularVelocity.data.x = (torque.data.x * this->delta_time_sec) + this->angularVelocity.data.x;
    this->next_angle.data.x           = (this->angularVelocity.data.x * this->delta_time_sec) + this->angle.data.x;
}

void DroneDynamicsGroundFrame::run_ry(const DroneThrustType &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    this->next_angularVelocity.data.y = (torque.data.y * this->delta_time_sec) + this->angularVelocity.data.y;
    this->next_angle.data.y           = (this->angularVelocity.data.y * this->delta_time_sec) + this->angle.data.y;
}

void DroneDynamicsGroundFrame::run_rz(const DroneThrustType &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    this->next_angularVelocity.data.z = (torque.data.z * this->delta_time_sec) + this->angularVelocity.data.z;
    this->next_angle.data.z           = (this->angularVelocity.data.z * this->delta_time_sec) + this->angle.data.z;
}