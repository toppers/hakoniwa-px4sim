#ifndef _DRON_DYNAMICS_GROUND_FRAME_HPP_
#define _DRON_DYNAMICS_GROUND_FRAME_HPP_

#include "idrone_dynamics.hpp"

namespace hako::assets::drone {


class DroneDynamicsGroundFrame : public hako::assets::drone::IDroneDynamics {
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
    DronePositionType position;           // Initialized to zero by default (glm::dvec3)
    DroneVelocityType velocity;           // Initialized to zero by default (glm::dvec3)
    DroneEulerType angle;                 // Initialized to zero by default (glm::dvec3)
    DroneAngularRate angularVelocity; // Initialized to zero by default (glm::dvec3)

    double delta_time_sec;
    double total_time_sec;
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
    DroneDynamicsGroundFrame(double dt)
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
    virtual ~DroneDynamicsGroundFrame() {}
    void set_collision_detection(bool enable) override {
        this->param_collision_detection = enable;
    }
    bool has_collision_detection() override {
        return this->param_collision_detection;
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

    void set_angle(const DroneEulerType &ang) override {
        angle = ang;
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
        return drone_physics::vector_ground_to_body(velocity, angle);
    }
    DroneAngularVelocityBodyFrameType get_angular_vel_body_frame() const override {
        // TODO: hiranabe 2021/10/13
        drone_physics::EulerRateType rate = {angularVelocity.data.x, angularVelocity.data.y, angularVelocity.data.z};
        drone_physics::AngularVelocityType ret =  drone_physics::euler_rate_to_body_angular_velocity(rate, angle);
        return ret;
    }

    // Implementation for the run function is required
    void run(const DroneDynamicsInputType &input) override 
    {
        (void)input;
        DroneTorqueType torque = input.torque;
        DroneThrustType thrust = input.thrust;
        drone_physics::AccelerationType acc = drone_physics::acceleration_in_ground_frame(
                                                            this->velocity, this->angle, 
                                                            thrust.data, this->param_mass, GRAVITY, this->param_drag);
        drone_physics::EulerAccelerationType acc_angular_body = drone_physics::euler_acceleration_in_ground_frame(
                                                            this->angularVelocity, this->angle,
                                                            torque.data.x, torque.data.y, torque.data.z,
                                                            this->param_cx, this->param_cy, this->param_cz);

        //integral to velocity on ground frame
        this->velocity.data = integral(this->velocity.data, {acc.x, acc.y, acc.z});
        this->angularVelocity.data = integral(this->angularVelocity.data, 
                                    {acc_angular_body.phi, acc_angular_body.theta, acc_angular_body.psi});

        //integral to pos, angle on ground frame
        this->position.data = integral(this->position.data, this->velocity.data);
        this->angle.data = integral(this->angle.data, this->angularVelocity.data);
        //boundary condition
        if (this->position.data.z > 0) {
            this->position.data.z = 0;
            this->velocity.data.z = 0;
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


#endif /* _DRON_DYNAMICS_GROUND_FRAME_HPP_ */