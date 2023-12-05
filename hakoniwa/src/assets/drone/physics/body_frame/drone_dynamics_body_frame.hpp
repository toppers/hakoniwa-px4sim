#ifndef _DRON_DYNAMICS_BODY_FRAME_HPP_
#define _DRON_DYNAMICS_BODY_FRAME_HPP_

#include "idrone_dynamics.hpp"
#include <math.h>
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

    void run_x(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_y(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_z(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_rx(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_ry(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_rz(const DroneThrustType &thrust, const DroneTorqueType &torque);

    DroneVelocityType convert(DroneVelocityBodyFrameType const src)
    {
        DroneVelocityType earthFrameVelocity;
        double cos_phi = cos(this->angle.data.x);
        double cos_theta = cos(this->angle.data.y);
        double cos_psi = cos(this->angle.data.z);
        double sin_phi = sin(this->angle.data.x);
        double sin_theta = sin(this->angle.data.y);
        double sin_psi = sin(this->angle.data.z);

        earthFrameVelocity.data.x = cos_theta * cos_psi * src.data.x +
                                    (sin_phi * sin_theta * cos_psi - cos_phi * sin_psi) * src.data.y +
                                    (cos_phi * sin_theta * cos_psi + sin_phi * sin_psi) * src.data.z;

        earthFrameVelocity.data.y = cos_theta * sin_psi * src.data.x +
                                    (sin_phi * sin_theta * sin_psi + cos_phi * cos_psi) * src.data.y +
                                    (cos_phi * sin_theta * sin_psi - sin_phi * cos_psi) * src.data.z;

        earthFrameVelocity.data.z = -sin_theta * src.data.x +
                                    sin_phi * cos_theta * src.data.y +
                                    cos_phi * cos_theta * src.data.z;

        return earthFrameVelocity;
    }

    DroneAngularVelocityType convert(DroneAngularVelocityBodyFrameType const src)
    {
        DroneAngularVelocityType eulerRate;
        double sin_phi = sin(this->angle.data.x);
        double cos_phi = cos(this->angle.data.x);
        double tan_theta = tan(this->angle.data.y);
        double sec_theta = 1 / cos(this->angle.data.y); // sec is the reciprocal of cos

        eulerRate.data.x = src.data.x + sin_phi * tan_theta * src.data.y + cos_phi * tan_theta * src.data.z;
        eulerRate.data.y = cos_phi * src.data.y - sin_phi * src.data.z;
        eulerRate.data.z = sin_phi * sec_theta * src.data.y + cos_phi * sec_theta * src.data.z;

        return eulerRate;
    }
    void integral(DroneVelocityType const src)
    {
        this->position.data.x += src.data.x * this->delta_time_sec;
        this->position.data.y += src.data.y * this->delta_time_sec;
        this->position.data.z += src.data.y * this->delta_time_sec;
    }
    void integral(DroneAngularVelocityType const src)
    {
        this->angle.data.x += src.data.x * this->delta_time_sec;
        this->angle.data.y += src.data.y * this->delta_time_sec;
        this->angle.data.z += src.data.z * this->delta_time_sec;
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

    void set_mass(double mass)
    {
        this->param_mass = mass;
    }
    void set_drag(double drag) override
    {
        this->param_drag = drag;
    }
    // Setters
    void set_pos(DronePositionType &pos) override {
        position = pos;
    }

    void set_vel(DroneVelocityType &vel) override {
        velocity = vel;
    }

    void set_angle(DroneAngleType &ang) override {
        angle = ang;
    }

    void set_angular_vel(DroneAngularVelocityType &angularVel) override {
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
    void run(DroneThrustType &thrust, DroneTorqueType &torque) override 
    {
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
        }        
        this->total_time_sec += this->delta_time_sec;
    }
};

}


#endif /* _DRON_DYNAMICS_BODY_FRAME_HPP_ */