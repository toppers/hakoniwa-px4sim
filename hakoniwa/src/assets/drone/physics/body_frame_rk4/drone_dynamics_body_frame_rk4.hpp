#ifndef _DRON_DYNAMICS_BODY_FRAME_RK4_HPP_
#define _DRON_DYNAMICS_BODY_FRAME_RK4_HPP_

#include "idrone_dynamics.hpp"
#include <math.h>
#include <iostream>

namespace hako::assets::drone {


class DroneDynamicsBodyFrameRK4 : public hako::assets::drone::IDroneDynamics {
private:
    /*
     * parameters
     */
    double param_mass;
    double param_drag;
    double param_cx;
    double param_cy;
    double param_cz;
    double param_size_x;
    double param_size_y;
    double param_size_z;
    bool param_collision_detection;
    /*
     * internal state
     */
    DronePositionType position;
    DroneVelocityType velocity;
    DroneAngleType angle;
    DroneAngularVelocityType angularVelocity;

    DroneVelocityBodyFrameType velocityBodyFrame;
    DroneAngularVelocityBodyFrameType angularVelocityBodyFrame;

    double delta_time_sec;
    double total_time_sec;

    void run_x(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_y(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_z(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_rx(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_ry(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_rz(const DroneThrustType &thrust, const DroneTorqueType &torque);

    DroneVelocityType convert(const DroneVelocityBodyFrameType& src)
    {
        return drone_physics::velocity_body_to_ground(src, angle);
    }

    DroneAngularVelocityType convert(const DroneAngularVelocityBodyFrameType& src)
    {
        // TODO hiranabe 2020/12/10
        hako::drone_physics::AngularRateType rate = drone_physics::body_angular_velocity_to_euler_rate(src, angle);
        hako::drone_physics::AngularVectorType lhs{rate.phi, rate.theta, rate.psi};
        return lhs;
    }
    void integral(const DroneVelocityType& src)
    {
        this->position.data.x += src.data.x * this->delta_time_sec;
        this->position.data.y += src.data.y * this->delta_time_sec;
        this->position.data.z += src.data.z * this->delta_time_sec;
    }
    void integral(const DroneAngularVelocityType& src)
    {
        this->angle.data.x += src.data.x * this->delta_time_sec;
        this->angle.data.y += src.data.y * this->delta_time_sec;
        this->angle.data.z += src.data.z * this->delta_time_sec;
    }
    void rungeKutta4(const DroneThrustType& thrust, const DroneTorqueType& torque) 
    {
        DroneVelocityBodyFrame v_vel(this->velocityBodyFrame);
        DroneAngularVelocityBodyFrame v_rate(this->angularVelocityBodyFrame);
        //k1
        DroneAccelerationBodyFrame k1_acc = drone_physics::acceleration_in_body_frame(
                        v_vel, 
                        this->angle, 
                        v_rate, 
                        thrust.data, this->param_mass, GRAVITY, this->param_drag);
        DroneAngularAccelerationBodyFrame k1_acc_angular = drone_physics::angular_acceleration_in_body_frame(
                        v_rate,
                        torque.data.x, torque.data.y, torque.data.z,
                        this->param_cx, this->param_cy, this->param_cz);
        //k2
        DroneVelocityBodyFrame k2_vec;
        DroneAngularVelocityBodyFrame k2_rate;
        k2_vec.data.x = v_vel.data.x + 0.5 * k1_acc.data.x * this->delta_time_sec; 
        k2_vec.data.y = v_vel.data.y + 0.5 * k1_acc.data.y * this->delta_time_sec; 
        k2_vec.data.z = v_vel.data.z + 0.5 * k1_acc.data.z * this->delta_time_sec;
        k2_rate.data.x = v_rate.data.x + 0.5 * k1_acc_angular.data.x * this->delta_time_sec; 
        k2_rate.data.y = v_rate.data.y + 0.5 * k1_acc_angular.data.y * this->delta_time_sec; 
        k2_rate.data.z = v_rate.data.z + 0.5 * k1_acc_angular.data.z * this->delta_time_sec;
        DroneAccelerationBodyFrame k2_acc = drone_physics::acceleration_in_body_frame(
                        k2_vec, 
                        this->angle, 
                        k2_rate, 
                        thrust.data, this->param_mass, GRAVITY, this->param_drag);
        DroneAngularAccelerationBodyFrame k2_acc_angular = drone_physics::angular_acceleration_in_body_frame(
                        k2_rate,
                        torque.data.x, torque.data.y, torque.data.z,
                        this->param_cx, this->param_cy, this->param_cz);

        //k3
        DroneVelocityBodyFrame k3_vec;
        DroneAngularVelocityBodyFrame k3_rate;
        k3_vec.data.x = v_vel.data.x + 0.5 * k2_acc.data.x * this->delta_time_sec; 
        k3_vec.data.y = v_vel.data.y + 0.5 * k2_acc.data.y * this->delta_time_sec; 
        k3_vec.data.z = v_vel.data.z + 0.5 * k2_acc.data.z * this->delta_time_sec;
        k3_rate.data.x = v_rate.data.x + 0.5 * k2_acc_angular.data.x * this->delta_time_sec; 
        k3_rate.data.y = v_rate.data.y + 0.5 * k2_acc_angular.data.y * this->delta_time_sec; 
        k3_rate.data.z = v_rate.data.z + 0.5 * k2_acc_angular.data.z * this->delta_time_sec;
        DroneAccelerationBodyFrame k3_acc = drone_physics::acceleration_in_body_frame(
                        k3_vec, 
                        this->angle, 
                        k3_rate, 
                        thrust.data, this->param_mass, GRAVITY, this->param_drag);
        DroneAngularAccelerationBodyFrame k3_acc_angular = drone_physics::angular_acceleration_in_body_frame(
                        k3_rate,
                        torque.data.x, torque.data.y, torque.data.z,
                        this->param_cx, this->param_cy, this->param_cz);

        //k4
        DroneVelocityBodyFrame k4_vec;
        DroneAngularVelocityBodyFrame k4_rate;
        k4_vec.data.x = v_vel.data.x + k3_acc.data.x * this->delta_time_sec; 
        k4_vec.data.y = v_vel.data.y + k3_acc.data.y * this->delta_time_sec; 
        k4_vec.data.z = v_vel.data.z + k3_acc.data.z * this->delta_time_sec;
        k4_rate.data.x = v_rate.data.x + k3_acc_angular.data.x * this->delta_time_sec; 
        k4_rate.data.y = v_rate.data.y + k3_acc_angular.data.y * this->delta_time_sec; 
        k4_rate.data.z = v_rate.data.z + k3_acc_angular.data.z * this->delta_time_sec;
        DroneAccelerationBodyFrame k4_acc = drone_physics::acceleration_in_body_frame(
                        k4_vec, 
                        this->angle, 
                        k4_rate, 
                        thrust.data, this->param_mass, GRAVITY, this->param_drag);
        DroneAngularAccelerationBodyFrame k4_acc_angular = drone_physics::angular_acceleration_in_body_frame(
                        k4_rate,
                        torque.data.x, torque.data.y, torque.data.z,
                        this->param_cx, this->param_cy, this->param_cz);


        this->velocityBodyFrame.data.x = v_vel.data.x + (delta_time_sec / 6.0) * (k1_acc.data.x + 2 * k2_acc.data.x + 2 * k3_acc.data.x + k4_acc.data.x);
        this->velocityBodyFrame.data.y = v_vel.data.y + (delta_time_sec / 6.0) * (k1_acc.data.y + 2 * k2_acc.data.y + 2 * k3_acc.data.y + k4_acc.data.y);
        this->velocityBodyFrame.data.z = v_vel.data.z + (delta_time_sec / 6.0) * (k1_acc.data.z + 2 * k2_acc.data.z + 2 * k3_acc.data.z + k4_acc.data.z);
        this->angularVelocityBodyFrame.data.x = v_rate.data.x + (delta_time_sec / 6.0) * (k1_acc_angular.data.x + 2 * k2_acc_angular.data.x + 2 * k3_acc_angular.data.x + k4_acc_angular.data.x);
        this->angularVelocityBodyFrame.data.y = v_rate.data.y + (delta_time_sec / 6.0) * (k1_acc_angular.data.y + 2 * k2_acc_angular.data.y + 2 * k3_acc_angular.data.y + k4_acc_angular.data.y);
        this->angularVelocityBodyFrame.data.z = v_rate.data.z + (delta_time_sec / 6.0) * (k1_acc_angular.data.z + 2 * k2_acc_angular.data.z + 2 * k3_acc_angular.data.z + k4_acc_angular.data.z);
    }


public:
    // Constructor with zero initialization
    DroneDynamicsBodyFrameRK4(double dt)
    {
        this->total_time_sec = 0;
        this->delta_time_sec = dt;
        this->param_mass = 1;
        this->param_drag = 0;
        this->param_cx = 1;
        this->param_cy = 1;
        this->param_cz = 1;
        this->param_size_x = 1;
        this->param_size_y = 1;
        this->param_size_z = 0.1;
        this->param_collision_detection = false;
    }
    virtual ~DroneDynamicsBodyFrameRK4() {}
    void set_collision_detection(bool enable) override {
        this->param_collision_detection = enable;
    }
    bool has_collision_detection() override {
        return this->param_collision_detection;
    }

    void set_torque_constants(double cx, double cy, double cz) override
    {
        this->param_cx = cx;
        this->param_cy = cy;
        this->param_cz = cz;
    }
    void set_body_size(double x, double y, double z) override
    {
        this->param_size_x = x;
        this->param_size_y = y;
        this->param_size_z = z;
    }
    void set_mass(double mass) override
    {
        this->param_mass = mass;
    }
    double get_mass() const override
    {
        return this->param_mass;
    }

    void set_drag(double drag) override
    {
        this->param_drag = drag;
    }
    // Setters
    void set_pos(const DronePositionType &pos) override {
        position = pos;
    }

    void set_vel(const DroneVelocityType &vel) override {
        velocity = vel;
    }

    void set_angle(const DroneAngleType &ang) override {
        angle = ang;
    }

    void set_angular_vel(const DroneAngularVelocityType &angularVel) override {
        angularVelocity = angularVel;
    }

    // Getters
    DronePositionType get_pos() const override {
        return position;
    }

    DroneVelocityType get_vel() const override {
        return velocity;
    }

    DroneAngleType get_angle() const override {
        return angle;
    }

    DroneAngularVelocityType get_angular_vel() const override {
        return angularVelocity;
    }
    DroneVelocityBodyFrameType get_vel_body_frame() const override {
        return velocityBodyFrame;
    }
    DroneAngularVelocityBodyFrameType get_angular_vel_body_frame() const override {
        return angularVelocityBodyFrame;
    }

    // Implementation for the run function is required
    void run(const DroneDynamicsInputType &input) override 
    {
        this->rungeKutta4(input.thrust, input.torque);

        this->velocity = this->convert(this->velocityBodyFrame);
        this->angularVelocity = this->convert(this->angularVelocityBodyFrame);
        this->integral(this->velocity);
        this->integral(this->angularVelocity);

        //boundary condition
        if (this->position.data.z > 0) {
            this->position.data.z = 0;
            this->velocity.data.x = 0;
            this->velocity.data.y = 0;
        }        
        this->total_time_sec += this->delta_time_sec;
    }
    const std::vector<std::string> log_head() override
    {
        return { "timestamp", "X", "Y", "Z", "Rx", "Ry", "Rz" };
    }
    const std::vector<std::string> log_data() override
    {
        return {
            std::to_string(total_time_sec), 
            std::to_string(position.data.x), std::to_string(position.data.y), std::to_string(position.data.z),
            std::to_string(angle.data.x), std::to_string(angle.data.y), std::to_string(angle.data.z)
            };
    }

};

}


#endif /* _DRON_DYNAMICS_BODY_FRAME_RK4_HPP_ */