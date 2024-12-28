#ifndef _DRON_DYNAMICS_BODY_FRAME_MATLAB_HPP_
#define _DRON_DYNAMICS_BODY_FRAME_MATLAB_HPP_

#include "idrone_dynamics.hpp"
#include "config/drone_config_types.hpp"
#include <math.h>
#include <iostream>
#include "utils/csv_logger.hpp"
#include "drone_physics_matlab.h"

namespace hako::assets::drone {


class DroneDynamicsBodyFrameMatlab : public hako::assets::drone::IDroneDynamics {
private:
    /*
     * parameters
     */
    double param_mass;
    double param_drag1;
    double param_drag2;
    double param_cx;
    double param_cy;
    double param_cz;
    double param_size_x;
    double param_size_y;
    double param_size_z;
    bool param_collision_detection;
    bool param_manual_control;
    std::optional<OutOfBoundsReset> param_out_of_bounds_reset;

    /*
     * initial state
     */
    DronePositionType initial_position;
    DroneEulerType initial_angle;

    /*
     * internal state
     */
    DronePositionType position;
    DroneVelocityType velocity;
    DroneEulerType angle;
    DroneEulerRateType angularVelocity;

    DroneVelocityBodyFrameType velocityBodyFrame;
    DroneAngularVelocityBodyFrameType angularVelocityBodyFrame;

    double delta_time_sec;
    double total_time_sec;
    /*
     * Ground position
     */
    double ground_height;

    DroneVelocityType convert(const DroneVelocityBodyFrameType& src)
    {
        return drone_physics::ground_vector_from_body(src, angle);
    }

    DroneEulerRateType convert(const DroneAngularVelocityBodyFrameType& src)
    {
        drone_physics::EulerRateType rate = drone_physics::euler_rate_from_body_angular_velocity(src, angle);
        drone_physics::EulerRateType dest = { rate.phi, rate.theta, rate.psi };
        return dest;
    }
    glm::dvec3 integral(const glm::dvec3& p, const glm::dvec3& v)
    {
        glm::dvec3 r;
        r.x = p.x + (v.x * this->delta_time_sec);
        r.y = p.y + (v.y * this->delta_time_sec);
        r.z = p.z + (v.z * this->delta_time_sec);
        return r;
    }
    void set_out_of_bounds_values()
    {
        const OutOfBoundsReset& reset_value = param_out_of_bounds_reset.value();
        if (reset_value.position[2]) {
            this->position.data.z = this->ground_height;
        }
        if (reset_value.velocity[2]) {
            this->velocity.data.z = 0;
            this->velocityBodyFrame.data.z = 0;
        }
        if (reset_value.velocity[0]) {
            this->velocityBodyFrame.data.x = 0;
        }
        if (reset_value.velocity[1]) {
            this->velocityBodyFrame.data.y = 0;
        }
        if (reset_value.angular_velocity[0]) {
            this->angularVelocityBodyFrame.data.x = 0;
        }
        if (reset_value.angular_velocity[1]) {
            this->angularVelocityBodyFrame.data.y = 0;
        }
        if (reset_value.angular_velocity[2]) {
            this->angularVelocityBodyFrame.data.z = 0;
        }
    }

public:
    // Constructor with zero initialization
    DroneDynamicsBodyFrameMatlab(double dt)
    {
        this->total_time_sec = 0;
        this->delta_time_sec = dt;
        this->param_mass = 1;
        this->param_drag1 = 0;
        this->param_drag2 = 0;
        this->param_cx = 1;
        this->param_cy = 1;
        this->param_cz = 1;
        this->param_size_x = 1;
        this->param_size_y = 1;
        this->param_size_z = 0.1;
        this->param_collision_detection = false;
        this->param_manual_control = false;
        this->ground_height = 0;
        mi_drone_acceleration_initialize();
    }
    virtual ~DroneDynamicsBodyFrameMatlab() {}
    void reset() override
    {
        position = initial_position;
        angle = initial_angle;
        velocity = {0, 0, 0};
        angularVelocity = {0, 0, 0};
        velocityBodyFrame = {0, 0, 0};
        angularVelocityBodyFrame = {0, 0, 0};
    }
    void set_body_size(double x, double y, double z) override
    {
        this->param_size_x = x;
        this->param_size_y = y;
        this->param_size_z = z;
    }
    void set_torque_constants(double cx, double cy, double cz) override
    {
        this->param_cx = cx;
        this->param_cy = cy;
        this->param_cz = cz;
    }
    void set_mass(double mass) override
    {
        this->param_mass = mass;
    }
    double get_mass() const override
    {
        return this->param_mass;
    }
    void set_drag(double drag1, double drag2) override
    {
        this->param_drag1 = drag1;
        this->param_drag2 = drag2;
    }
    double get_drag() const override
    {
        return this->param_drag1;
    }
    // Setters
    void set_pos(const DronePositionType &pos) override {
        position = pos;
        initial_position = pos;
    }

    void set_vel(const DroneVelocityType &vel) override {
        velocity = vel;
    }

    void set_angle(const DroneEulerType &ang) override {
        angle = ang;
        initial_angle = ang;
    }
    void set_manual_control(bool enable) override {
        this->param_manual_control = enable;
    }
    bool has_manual_control() override {
        return this->param_manual_control;
    }
    void set_collision_detection(bool enable) override {
        this->param_collision_detection = enable;
    }
    void set_out_of_bounds_reset(const std::optional<OutOfBoundsReset>& reset_options) override {
        param_out_of_bounds_reset = reset_options;
    }
    bool has_collision_detection() override {
        return this->param_collision_detection;
    }

    // Getters
    DronePositionType get_pos() const override {
        return position;
    }

    DroneVelocityType get_vel() const override {
        return velocity;
    }

    DroneEulerType get_angle() const override {
        return angle;
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
        DroneTorqueType torque = input.torque;
        DroneThrustType thrust = input.thrust;
        mi_drone_acceleration_in_t in;
        in.phi = angle.data.x;   /* euler angle x */
        in.theta = angle.data.y; /* euler angle y */
        in.psi = angle.data.z;   /* euler angle z */
        in.u = velocityBodyFrame.data.x; /* velocity x */
        in.v = velocityBodyFrame.data.y; /* velocity y */
        in.w = velocityBodyFrame.data.z; /* velocity z */
        in.p = angularVelocityBodyFrame.data.x; /* anglular velocity x */
        in.q = angularVelocityBodyFrame.data.y; /* anglular velocity y */
        in.r = angularVelocityBodyFrame.data.z; /* anglular velocity z */
        in.thrust = thrust.data;
        in.torque_x = torque.data.x;
        in.torque_y = torque.data.y;
        in.torque_z = torque.data.z;
        in.mass = param_mass;
        in.Ixx = param_cx;
        in.Iyy = param_cy;
        in.Izz = param_cz;
        in.gravity = GRAVITY;
        in.drag = param_drag1;
        mi_drone_acceleration_out_t out = mi_drone_acceleration(&in);

        DroneAccelerationBodyFrame acc;
        DroneAngularAccelerationBodyFrame acc_angular;

        acc.data.x = out.du;
        acc.data.y = out.dv;
        acc.data.z = out.dw;

        acc_angular.data.x = out.dp;
        acc_angular.data.y = out.dq;
        acc_angular.data.z = out.dr;

        //integral to velocity on body frame
        this->velocityBodyFrame.data = integral(this->velocityBodyFrame.data, acc.data);
        this->angularVelocityBodyFrame.data = integral(this->angularVelocityBodyFrame.data, acc_angular.data);

        //convert to ground frame
        this->velocity = this->convert(this->velocityBodyFrame);
        this->angularVelocity = this->convert(this->angularVelocityBodyFrame);

        //collision detection
        if (param_collision_detection) {
            if (input.collision.collision) {
                hako::drone_physics::VectorType velocity_before_contact = this->velocity;
                hako::drone_physics::VectorType center_position = this->position;
                hako::drone_physics::VectorType contact_position = { 
                    input.collision.contact_position[0].x,
                    input.collision.contact_position[0].y,
                    input.collision.contact_position[0].z
                };
                double restitution_coefficient = input.collision.restitution_coefficient;
                if (restitution_coefficient <= 0.0) {
                    this->ground_height = this->position.data.z;
                }
                else {
                    //std::cout << "velocity_before_contact.x: " << velocity_before_contact.x << std::endl;
                    //std::cout << "velocity_before_contact.y: " << velocity_before_contact.y << std::endl;
                    //std::cout << "velocity_before_contact.z: " << velocity_before_contact.z << std::endl;
                    hako::drone_physics::VectorType col_vel = hako::drone_physics::velocity_after_contact_with_wall(
                            velocity_before_contact, center_position, contact_position, restitution_coefficient);
                    //std::cout << "velocity_after_contact.x: " << col_vel.x << std::endl;
                    //std::cout << "velocity_after_contact.y: " << col_vel.y << std::endl;
                    //std::cout << "velocity_after_contact.z: " << col_vel.z << std::endl;
                    this->velocity = col_vel;
                    this->velocityBodyFrame = drone_physics::body_vector_from_ground(this->velocity, angle);
                }
            }
        }

        //integral to pos, angle on ground frame
        this->position.data = integral(this->position.data, this->velocity.data);
        this->angle.data = integral(this->angle.data, this->angularVelocity.data);

        //boundary condition
        if (this->position.data.z > this->ground_height) {
            if (param_out_of_bounds_reset) {
                set_out_of_bounds_values();
            } else {
                // オプション未設定時の元の処理
                this->position.data.z = this->ground_height;
                this->velocity.data.z = 0;
                this->velocityBodyFrame.data.x = 0;
                this->velocityBodyFrame.data.y = 0;
                this->velocityBodyFrame.data.z = 0;
                //this->angularVelocityBodyFrame.data.x = 0;
                //this->angularVelocityBodyFrame.data.y = 0;
                this->angularVelocityBodyFrame.data.z = 0;
            }
        } else {
            this->ground_height = 0;
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
            std::to_string(CsvLogger::get_time_usec()), 
            std::to_string(position.data.x), std::to_string(position.data.y), std::to_string(position.data.z),
            std::to_string(angle.data.x), std::to_string(angle.data.y), std::to_string(angle.data.z)
            };
    }

};

}


#endif /* _DRON_DYNAMICS_BODY_FRAME_MATLAB_HPP_ */