#include "drone_dynamics.hpp"
#include <math.h>

using hako::assets::drone::DroneDynamicsBodyFrame;

void DroneDynamicsBodyFrame::run_x(const DroneThrustype &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    (void)torque;

    this->next_velocityBodyFrame.data.x = 
        this->delta_time_sec * (
            - GRAVITY * sin(this->angle.data.y) 
            - (this->param_drag * this->velocityBodyFrame.data.x))
        + this->velocityBodyFrame.data.x;
    return;
}

void DroneDynamicsBodyFrame::run_y(const DroneThrustype &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    (void)torque;

    this->next_velocityBodyFrame.data.y = 
        this->delta_time_sec * (
            GRAVITY * cos(this->angle.data.y) * sin(this->angle.data.x) 
            - (this->param_drag * this->velocityBodyFrame.data.y))
        + this->velocityBodyFrame.data.y;
}

void DroneDynamicsBodyFrame::run_z(const DroneThrustype &thrust, const DroneTorqueType &torque) 
{
    (void)torque;

    this->next_velocityBodyFrame.data.z = 
        + this->delta_time_sec * (
              (thrust.data/this->param_mass)
            + (GRAVITY * cos(this->angle.data.y) * cos(this->angle.data.x))
            - (this->param_drag * this->velocityBodyFrame.data.z))
        + this->velocityBodyFrame.data.z;
}

void DroneDynamicsBodyFrame::run_rx(const DroneThrustype &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    this->next_angularVelocityBodyFrame.data.x = 
            (this->param_cx * torque.data.x * this->delta_time_sec) 
            + this->angularVelocityBodyFrame.data.x;
}

void DroneDynamicsBodyFrame::run_ry(const DroneThrustype &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    this->next_angularVelocityBodyFrame.data.y = 
            (this->param_cy * torque.data.y * this->delta_time_sec) 
            + this->angularVelocityBodyFrame.data.y;
}

void DroneDynamicsBodyFrame::run_rz(const DroneThrustype &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    this->next_angularVelocityBodyFrame.data.z = 
            (this->param_cz * torque.data.z * this->delta_time_sec) 
            + this->angularVelocityBodyFrame.data.z;
}