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
    DroneAngularRateType angularVelocity;

    DroneVelocityBodyFrameType velocityBodyFrame;
    DroneAngularVelocityBodyFrameType angularVelocityBodyFrame;

    double delta_time_sec;
    double total_time_sec;

    DroneVelocityType convert(const DroneVelocityBodyFrameType& src)
    {
        return drone_physics::vector_body_to_ground(src, angle);
    }

    DroneAngularRateType convert(const DroneAngularVelocityBodyFrameType& src)
    {
        // TODO hiranabe 2020/12/10
        return drone_physics::body_angular_velocity_to_euler_rate(src, angle);
    }
    void integral(const DroneVelocityType& src)
    {
        this->position.data.x += src.data.x * this->delta_time_sec;
        this->position.data.y += src.data.y * this->delta_time_sec;
        this->position.data.z += src.data.z * this->delta_time_sec;
    }
    void integral(const DroneAngularRateType& src)
    {
        this->angle.data.x += src.data.x * this->delta_time_sec;
        this->angle.data.y += src.data.y * this->delta_time_sec;
        this->angle.data.z += src.data.z * this->delta_time_sec;
    }
    void rungeKutta4_k(const DroneVelocityBodyFrame& v_vel, const DroneAngularVelocityBodyFrame& v_rate, 
                       const DroneAccelerationBodyFrame& k_acc, DroneAngularAccelerationBodyFrame& k_acc_angular,
                       double rate, DroneVelocityBodyFrame& k_vec, DroneAngularVelocityBodyFrame& k_rate)
    {
        k_vec.data.x  = v_vel.data.x  + rate * k_acc.data.x * this->delta_time_sec; 
        k_vec.data.y  = v_vel.data.y  + rate * k_acc.data.y * this->delta_time_sec; 
        k_vec.data.z  = v_vel.data.z  + rate * k_acc.data.z * this->delta_time_sec;
        k_rate.data.x = v_rate.data.x + rate * k_acc_angular.data.x * this->delta_time_sec; 
        k_rate.data.y = v_rate.data.y + rate * k_acc_angular.data.y * this->delta_time_sec; 
        k_rate.data.z = v_rate.data.z + rate * k_acc_angular.data.z * this->delta_time_sec;
    }
    DroneAccelerationBodyFrame rungeKutta4_acc(const DroneVelocityBodyFrame &v_vel, const DroneAngularVelocityBodyFrame& v_rate, const DroneThrustType& thrust)
    {
        return drone_physics::acceleration_in_body_frame(
                        v_vel, 
                        this->angle, 
                        v_rate, 
                        thrust.data, this->param_mass, GRAVITY, this->param_drag);
    }
    DroneAngularAccelerationBodyFrame rungeKutta4_acc_angular(const DroneAngularVelocityBodyFrame& v_rate, const DroneTorqueType& torque)
    {
        return drone_physics::angular_acceleration_in_body_frame(
                        v_rate,
                        torque.data.x, torque.data.y, torque.data.z,
                        this->param_cx, this->param_cy, this->param_cz);
    }
    glm::dvec3 rungeKutta4_sum(const glm::dvec3& v, const glm::dvec3& k1, const glm::dvec3& k2, const glm::dvec3& k3, const glm::dvec3& k4)
    {
        glm::dvec3 ret;
        ret.x = v.x + (this->delta_time_sec / 6.0) * (k1.x + 2 * k2.x + 2 * k3.x + k4.x);
        ret.y = v.y + (this->delta_time_sec / 6.0) * (k1.y + 2 * k2.y + 2 * k3.y + k4.y);
        ret.z = v.z + (this->delta_time_sec / 6.0) * (k1.z + 2 * k2.z + 2 * k3.z + k4.z);
        return ret;
    }
    void rungeKutta4(const DroneThrustType& thrust, const DroneTorqueType& torque) 
    {
        DroneVelocityBodyFrame v_vel(this->velocityBodyFrame);
        DroneAngularVelocityBodyFrame v_rate(this->angularVelocityBodyFrame);
        //k1
        DroneAccelerationBodyFrame k1_acc = rungeKutta4_acc(v_vel, v_rate, thrust);
        DroneAngularAccelerationBodyFrame k1_acc_angular = rungeKutta4_acc_angular(v_rate, torque);
        //k2
        DroneVelocityBodyFrame k2_vec;
        DroneAngularVelocityBodyFrame k2_rate;
        rungeKutta4_k(v_vel, v_rate, k1_acc, k1_acc_angular, 0.5, k2_vec, k2_rate);
        DroneAccelerationBodyFrame k2_acc = rungeKutta4_acc(k2_vec, k2_rate, thrust);
        DroneAngularAccelerationBodyFrame k2_acc_angular = rungeKutta4_acc_angular(k2_rate, torque);
        //k3
        DroneVelocityBodyFrame k3_vec;
        DroneAngularVelocityBodyFrame k3_rate;
        rungeKutta4_k(v_vel, v_rate, k2_acc, k2_acc_angular, 0.5, k3_vec, k3_rate);
        DroneAccelerationBodyFrame k3_acc = rungeKutta4_acc(k3_vec, k3_rate, thrust);
        DroneAngularAccelerationBodyFrame k3_acc_angular = rungeKutta4_acc_angular(k3_rate, torque);
        //k4
        DroneVelocityBodyFrame k4_vec;
        DroneAngularVelocityBodyFrame k4_rate;
        rungeKutta4_k(v_vel, v_rate, k3_acc, k3_acc_angular, 1.0, k4_vec, k4_rate);
        DroneAccelerationBodyFrame k4_acc = rungeKutta4_acc(k4_vec, k4_rate, thrust);
        DroneAngularAccelerationBodyFrame k4_acc_angular = rungeKutta4_acc_angular(k4_rate, torque);

        this->velocityBodyFrame.data = rungeKutta4_sum(v_vel.data, k1_acc.data, k2_acc.data, k3_acc.data, k4_acc.data);
        this->angularVelocityBodyFrame.data = rungeKutta4_sum(v_rate.data, k1_acc_angular.data, k2_acc_angular.data, k3_acc_angular.data, k4_acc_angular.data);
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

    void set_angular_vel(const DroneAngularRateType &angularVel) override {
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

    DroneAngularRateType get_angular_vel() const override {
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
            this->velocity.data.z = 0;
            this->velocityBodyFrame.data.x = 0;
            //this->velocityBodyFrame.data.y = 0;
            this->velocityBodyFrame.data.z = 0;
            //this->angularVelocityBodyFrame.data.x = 0;
            //this->angularVelocityBodyFrame.data.y = 0;
            //this->angularVelocityBodyFrame.data.z = 0;
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