#ifndef _DRON_DYNAMICS_GROUND_FRAME_HPP_
#define _DRON_DYNAMICS_GROUND_FRAME_HPP_

#include "idrone_dynamics.hpp"
#include "config/drone_config_types.hpp"

namespace hako::assets::drone {


class DroneDynamicsGroundFrame : public hako::assets::drone::IDroneDynamics {
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
     * Ground position
     */
    double ground_height;

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

    void set_out_of_bounds_values()
    {
        const OutOfBoundsReset& reset_value = param_out_of_bounds_reset.value();
        if (reset_value.position[2]) {
            this->position.data.z = this->ground_height;
        }
        if (reset_value.velocity[2]) {
            this->velocity.data.z = 0;
        }
        if (reset_value.velocity[0]) {
            this->velocity.data.x = 0;
        }
        if (reset_value.velocity[1]) {
            this->velocity.data.y = 0;
        }
        if (reset_value.angular_velocity[0]) {
            this->angularVelocity.data.x = 0;
        }
        if (reset_value.angular_velocity[1]) {
            this->angularVelocity.data.y = 0;
        }
        if (reset_value.angular_velocity[2]) {
            this->angularVelocity.data.z = 0;
        }
    }

public:
    // Constructor with zero initialization
    DroneDynamicsGroundFrame(double dt)
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
    }
    virtual ~DroneDynamicsGroundFrame() {}
    void reset() override
    {
        position = initial_position;
        angle = initial_angle;
    }
    void set_out_of_bounds_reset(const std::optional<OutOfBoundsReset>& reset_options) override {
        param_out_of_bounds_reset = reset_options;
    }
    void set_collision_detection(bool enable) override {
        this->param_collision_detection = enable;
    }
    bool has_collision_detection() override {
        return this->param_collision_detection;
    }
    void set_manual_control(bool enable) override {
        this->param_manual_control = enable;
    }
    bool has_manual_control() override {
        return this->param_manual_control;
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
        velocity = {0, 0, 0};
        angularVelocity = {0, 0, 0};
    }

    void set_vel(const DroneVelocityType &vel) override {
        velocity = vel;
    }

    void set_angle(const DroneEulerType &ang) override {
        angle = ang;
        initial_angle = ang;
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
        return drone_physics::body_vector_from_ground(velocity, angle);
    }
    DroneAngularVelocityBodyFrameType get_angular_vel_body_frame() const override {
        drone_physics::EulerRateType rate = {angularVelocity.data.x, angularVelocity.data.y, angularVelocity.data.z};
        drone_physics::AngularVelocityType ret =  drone_physics::body_angular_velocity_from_euler_rate(rate, angle);
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
                                                            thrust.data, this->param_mass, GRAVITY, this->param_drag1, this->param_drag2);
        drone_physics::EulerAccelerationType acc_angular_body = drone_physics::euler_acceleration_in_ground_frame(
                                                            this->angularVelocity, this->angle,
                                                            torque,
                                                            this->param_cx, this->param_cy, this->param_cz);

        //integral to velocity on ground frame
        this->velocity.data = integral(this->velocity.data, {acc.x, acc.y, acc.z});
        this->angularVelocity.data = integral(this->angularVelocity.data, 
                                    {acc_angular_body.phi, acc_angular_body.theta, acc_angular_body.psi});

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
                this->velocity.data.x = 0;
                this->velocity.data.y = 0;
                this->velocity.data.z = 0;
                this->angularVelocity.data.z = 0;
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


#endif /* _DRON_DYNAMICS_GROUND_FRAME_HPP_ */