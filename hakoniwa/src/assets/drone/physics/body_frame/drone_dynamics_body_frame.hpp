#ifndef _DRON_DYNAMICS_BODY_FRAME_HPP_
#define _DRON_DYNAMICS_BODY_FRAME_HPP_

#include "idrone_dynamics.hpp"
#include <math.h>
#include <iostream>
#include "utils/csv_logger.hpp"

namespace hako::assets::drone {


class DroneDynamicsBodyFrame : public hako::assets::drone::IDroneDynamics {
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
    /*
     * internal state
     */
    DronePositionType position;
    DroneVelocityType velocity;
    DroneEulerType angle;
    DroneEulerRateType angularVelocity;

    DroneVelocityBodyFrameType velocityBodyFrame;
    DroneAngularVelocityBodyFrameType angularVelocityBodyFrame;
    DroneVelocityBodyFrameType next_velocityBodyFrame;
    DroneAngularVelocityBodyFrameType next_angularVelocityBodyFrame;

    double delta_time_sec;
    double total_time_sec;

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

public:
    // Constructor with zero initialization
    DroneDynamicsBodyFrame(double dt)
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
    }
    virtual ~DroneDynamicsBodyFrame() {}
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
    }

    void set_vel(const DroneVelocityType &vel) override {
        velocity = vel;
    }

    void set_angle(const DroneEulerType &ang) override {
        angle = ang;
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
    bool has_collision_detection() override {
        return this->param_collision_detection;
    }
    void set_angular_vel(const DroneEulerRateType &angularVel) override {
        angularVelocity = angularVel;
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

    DroneEulerRateType get_angular_vel() const override {
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
        DroneTorqueType torque = input.torque;
        DroneThrustType thrust = input.thrust;
        this->cache = drone_phys_calc_cache(this->angle);

        DroneAccelerationBodyFrame acc = drone_physics::acceleration_in_body_frame(
                                                            this->velocityBodyFrame, this->angle, 
                                                            this->angularVelocityBodyFrame,
                                                            thrust.data, this->param_mass, GRAVITY, this->param_drag1, this->param_drag2);
        DroneAngularAccelerationBodyFrame acc_angular = drone_physics::angular_acceleration_in_body_frame(
                                                            this->angularVelocityBodyFrame,
                                                            torque,
                                                            this->param_cx, this->param_cy, this->param_cz);
        //integral to velocity on body frame
        this->velocityBodyFrame.data = integral(this->velocityBodyFrame.data, acc.data);
        this->angularVelocityBodyFrame.data = integral(this->angularVelocityBodyFrame.data, acc_angular.data);

        //convert to ground frame
        this->velocity = this->convert(this->velocityBodyFrame);

        // ADD WIND CONDITION HERE. (wind vector, in ground frame)
        // this->velocity += (wind_x, wind_y, wind_z);

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

        //integral to pos, angle on ground frame
        this->position.data = integral(this->position.data, this->velocity.data);
        this->angle.data = integral(this->angle.data, this->angularVelocity.data);

        //boundary condition
        if (this->position.data.z > 0) {
            this->position.data.z = 0;
            this->velocity.data.z = 0;
            this->velocityBodyFrame.data.x = 0;
            this->velocityBodyFrame.data.y = 0;
            this->velocityBodyFrame.data.z = 0;
            //this->angularVelocityBodyFrame.data.x = 0;
            //this->angularVelocityBodyFrame.data.y = 0;
            this->angularVelocityBodyFrame.data.z = 0;
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


#endif /* _DRON_DYNAMICS_BODY_FRAME_HPP_ */