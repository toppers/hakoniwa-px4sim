#include "drone_dynamics_body_frame_rk4.hpp"
#include <math.h>

using hako::assets::drone::DroneDynamicsBodyFrameRK4;

void DroneDynamicsBodyFrameRK4::run_x(const DroneThrustType &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    (void)torque;

    // 微分方程式をラムダ関数として定義
    auto df = [&](double v) {
        return -GRAVITY * this->cache.sin_theta - this->param_drag * v;
    };
    // 現在の状態
    double v = this->velocityBodyFrame.data.x;
    this->next_velocityBodyFrame.data.x = this->rungeKutta4(df, v, this->delta_time_sec);
    return;
}

void DroneDynamicsBodyFrameRK4::run_y(const DroneThrustType &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    (void)torque;

    // 微分方程式をラムダ関数として定義
    auto df = [&](double v) {
        return GRAVITY * this->cache.cos_theta * this->cache.sin_phi - (this->param_drag * v);
    };
    // 現在の状態
    double v = this->velocityBodyFrame.data.y;
    this->next_velocityBodyFrame.data.y = this->rungeKutta4(df, v, this->delta_time_sec);
}

void DroneDynamicsBodyFrameRK4::run_z(const DroneThrustType &thrust, const DroneTorqueType &torque) 
{
    (void)torque;

    // 微分方程式をラムダ関数として定義
    auto df = [&](double v) {
        return - (thrust.data/this->param_mass) + (GRAVITY * this->cache.cos_theta * this->cache.cos_phi) - (this->param_drag * v);
    };
    // 現在の状態
    double v = this->velocityBodyFrame.data.z;
    this->next_velocityBodyFrame.data.z = this->rungeKutta4(df, v, this->delta_time_sec);
}

void DroneDynamicsBodyFrameRK4::run_rx(const DroneThrustType &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    // 微分方程式をラムダ関数として定義
    auto df = [&](double v) {
        (void)v;
        return this->param_cx * torque.data.x;
    };
    // 現在の状態
    double v = this->next_angularVelocityBodyFrame.data.x;
    this->next_angularVelocityBodyFrame.data.x = this->rungeKutta4(df, v, this->delta_time_sec);
}

void DroneDynamicsBodyFrameRK4::run_ry(const DroneThrustType &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    // 微分方程式をラムダ関数として定義
    auto df = [&](double v) {
        (void)v;
        return this->param_cy * torque.data.y;
    };
    // 現在の状態
    double v = this->next_angularVelocityBodyFrame.data.y;
    this->next_angularVelocityBodyFrame.data.y = this->rungeKutta4(df, v, this->delta_time_sec);
}

void DroneDynamicsBodyFrameRK4::run_rz(const DroneThrustType &thrust, const DroneTorqueType &torque) 
{
    (void)thrust;
    auto df = [&](double v) {
        (void)v;
        return this->param_cz * torque.data.z;
    };
    // 現在の状態
    double v = this->next_angularVelocityBodyFrame.data.z;
    this->next_angularVelocityBodyFrame.data.z = this->rungeKutta4(df, v, this->delta_time_sec);
}