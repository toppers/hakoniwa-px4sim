#ifndef _THRUST_DYNAMICS_HPP_
#define _THRUST_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"
#include "ithrust_dynamics.hpp"
#include "utils/icsv_log.hpp"
#include "rotor_physics.hpp"
#include <glm/glm.hpp>
#include <iostream>

namespace hako::assets::drone {

const int HOVERING_ROTOR_RPM = 6000;

class ThrustDynamics : public hako::assets::drone::IThrustDynamics, public ICsvLog {
private:
    double delta_time_sec;
    double total_time_sec = 0;
    double param_A;
    double param_B;
    double param_Jr;
    DroneThrustType thrust;
    DroneTorqueType torque;
    DroneRotorSpeedType prev_rotor_speed[ROTOR_NUM];
    RotorConfigType rotor_config[ROTOR_NUM];
    double omega[ROTOR_NUM];
    drone_physics::PositionType position[ROTOR_NUM];
    double ccw[ROTOR_NUM];
    double omega_acceleration[ROTOR_NUM];

public:
    ThrustDynamics(double dt)
    {
        this->delta_time_sec = dt;
        // 1kg の機体が 6000 rpm でホバリングする定数
        // A = mg / (Ω0^2 * ROTOR_NUM)
        this->param_A =  GRAVITY / (ROTOR_NUM * HOVERING_ROTOR_RPM * HOVERING_ROTOR_RPM);
        // 1kg の機体が 6000 rpm でホバリングする場合に、1Nmで反トルクがかかる定数
        // B = 1 / (Ω0^2 * ROTOR_NUM)
        this->param_B = 1.0 / (ROTOR_NUM * HOVERING_ROTOR_RPM * HOVERING_ROTOR_RPM);
        this->param_Jr = 0.1;

        this->rotor_config[0].ccw = -1;
        this->rotor_config[0].data = { 0.3, 0.0, 0 };
        this->rotor_config[1].ccw = 1;
        this->rotor_config[1].data = { 0.0, -0.3, 0 };
        this->rotor_config[2].ccw = -1;
        this->rotor_config[2].data = { -0.3, 0.0, 0 };
        this->rotor_config[3].ccw = 1;
        this->rotor_config[3].data = { 0.0, 0.3, 0 };
        set_rotor_config(rotor_config);
    }
    virtual ~ThrustDynamics() {}

    void set_params(double a, double b, double jr)
    {
        this->param_A = a;
        this->param_B = b;
        this->param_Jr = jr;
    }

    void set_rotor_config(const RotorConfigType rotor_config[ROTOR_NUM]) override
    {
        for (int i = 0; i < ROTOR_NUM; i++) {
            this->rotor_config[i] = rotor_config[i];
            this->position[i] = {rotor_config[i].data.x, rotor_config[i].data.y, rotor_config[i].data.z};
            this->ccw[i] = rotor_config[i].ccw;
        }
    }
    void set_thrust(const DroneThrustType &thrust) override 
    {
        this->thrust = thrust;
    }
    void set_torque(const DroneTorqueType &torque) override
    {
        this->torque = torque;
    }

    DroneThrustType get_thrust() const override
    {
        return this->thrust;
    }
    DroneTorqueType get_torque() const override
    {
        return this->torque;
    }

    void run(const DroneRotorSpeedType rotor_speed[ROTOR_NUM]) override
    {
        for (int i = 0; i < ROTOR_NUM; i++) {
            omega[i] = rotor_speed[i].data;
            omega_acceleration[i] = (rotor_speed[i].data - this->prev_rotor_speed[i].data) / this->delta_time_sec;
        }
        this->thrust.data = drone_physics::body_thrust(param_A, ROTOR_NUM, omega);
        this->torque = drone_physics::body_torque(param_A, param_B, param_Jr, ROTOR_NUM,
                                            position, ccw, omega, omega_acceleration);

        for (int i = 0; i < ROTOR_NUM; i++) {
            this->prev_rotor_speed[i] = rotor_speed[i];
        }

        total_time_sec += delta_time_sec;
    }

    void print() override
    {
        std::cout << "thrust: " << this->thrust.data << std::endl;
        std::cout << "torque( " << this->torque.data.x 
                  << " , " << this->torque.data.y
                  << " , " << this->torque.data.z 
                  << " )" << std::endl;
    }
    const std::vector<std::string> log_head() override
    {
        return { "TIME", "Thrust", "Tx", "Ty", "Tz" };
    }
    const std::vector<std::string> log_data() override
    {
        DroneThrustType thrust = get_thrust();
        DroneTorqueType torque = get_torque();

        return {std::to_string(total_time_sec), std::to_string(thrust.data), std::to_string(torque.data.x), std::to_string(torque.data.y), std::to_string(torque.data.z)};
    }

};

}
//https://github.com/PX4/jMAVSim/blob/66b764ada522893c05224950aa6268c809f8e48a/src/me/drton/jmavsim/vehicle/AbstractMulticopter.java

#endif /* _THRUST_DYNAMICS_HPP_ */
