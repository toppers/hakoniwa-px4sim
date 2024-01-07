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
    double param_drag;
    double param_cx;
    double param_cy;
    double param_cz;
    /*
     * internal state
     */
    DronePositionType position;
    DroneVelocityType velocity;
    DroneAngleType angle;
    DroneAngularVelocityType angularVelocity;

    DroneVelocityBodyFrameType velocityBodyFrame;
    DroneAngularVelocityBodyFrameType angularVelocityBodyFrame;
    DroneVelocityBodyFrameType next_velocityBodyFrame;
    DroneAngularVelocityBodyFrameType next_angularVelocityBodyFrame;

    double delta_time_sec;
    double total_time_sec;

    DroneVelocityType convert(const DroneVelocityBodyFrameType& src)
    {
        return drone_physics::velocity_body_to_ground(src, angle);
    }

    DroneAngularVelocityType convert(const DroneAngularVelocityBodyFrameType& src)
    {
        return drone_physics::angular_velocity_body_to_ground(src, angle);
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
        this->param_drag = 0;
        this->param_cx = 1;
        this->param_cy = 1;
        this->param_cz = 1;
    }
    virtual ~DroneDynamicsBodyFrame() {}
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
    void run(const DroneThrustType &thrust, const DroneTorqueType &torque) override 
    {
        this->cache = drone_phys_calc_cache(this->angle);

        DroneAccelerationBodyFrame acc = drone_physics::acceleration_in_body_frame(
                                                            this->velocityBodyFrame, this->angle, 
                                                            this->angularVelocityBodyFrame,
                                                            thrust.data, this->param_mass, GRAVITY, this->param_drag);
        DroneAngularAccelerationBodyFrame acc_angular = drone_physics::angular_acceleration_in_body_frame(
                                                            this->angularVelocityBodyFrame,
                                                            torque.data.x, torque.data.y, torque.data.z,
                                                            this->param_cx, this->param_cy, this->param_cz);

        //integral to velocity on body frame
        this->velocityBodyFrame.data = integral(this->velocityBodyFrame.data, acc.data);
        this->angularVelocityBodyFrame.data = integral(this->angularVelocityBodyFrame.data, acc_angular.data);

        //convert to ground frame
        this->velocity = this->convert(this->velocityBodyFrame);
        this->angularVelocity = this->convert(this->angularVelocityBodyFrame);

        //integral to pos, angle on ground frame
        this->position.data = integral(this->position.data, this->velocity.data);
        this->angle.data = integral(this->angle.data, this->angularVelocity.data);

        //boundary condition
        if (this->position.data.z > 0) {
            this->position.data.z = 0;
            this->velocity.data.z = 0;
            this->velocityBodyFrame.data.z = 0;
            this->velocityBodyFrame.data.y = 0;
            this->velocityBodyFrame.data.z = 0;
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