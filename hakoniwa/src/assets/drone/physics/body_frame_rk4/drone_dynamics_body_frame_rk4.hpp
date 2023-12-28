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
        return drone_physics::angular_velocity_body_to_ground(src, angle);
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
    template<typename Func>
    double rungeKutta4(Func df, double v, double delta_time) 
    {
        double k1 = df(v);
        double k2 = df(v + 0.5 * k1 * delta_time);
        double k3 = df(v + 0.5 * k2 * delta_time);
        double k4 = df(v + k3 * delta_time);

        return v + (delta_time / 6.0) * (k1 + 2 * k2 + 2 * k3 + k4);
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
    }
    virtual ~DroneDynamicsBodyFrameRK4() {}
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
        run_x(thrust, torque);
        run_y(thrust, torque);
        run_z(thrust, torque);
        run_rx(thrust, torque);
        run_ry(thrust, torque);
        run_rz(thrust, torque);

        this->velocityBodyFrame = this->next_velocityBodyFrame;
        this->angularVelocityBodyFrame = this->next_angularVelocityBodyFrame;

        this->velocity = this->convert(this->velocityBodyFrame);
        this->angularVelocity = this->convert(this->angularVelocityBodyFrame);
        this->integral(this->velocity);
        this->integral(this->angularVelocity);

        //boundary condition
        if (this->position.data.z > 0) {
            this->position.data.z = 0;
            this->velocity.data.z = 0;
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
            std::to_string(total_time_sec), 
            std::to_string(position.data.x), std::to_string(position.data.y), std::to_string(position.data.z),
            std::to_string(angle.data.x), std::to_string(angle.data.y), std::to_string(angle.data.z)
            };
    }

};

}


#endif /* _DRON_DYNAMICS_BODY_FRAME_RK4_HPP_ */