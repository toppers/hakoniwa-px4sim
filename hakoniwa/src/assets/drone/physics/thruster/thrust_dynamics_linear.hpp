#ifndef _THRUST_DYNAMICS_LINEAR_HPP_
#define _THRUST_DYNAMICS_LINEAR_HPP_

#include "drone_primitive_types.hpp"
#include "ithrust_dynamics.hpp"
#include "utils/icsv_log.hpp"
#include "rotor_physics.hpp"
#include <glm/glm.hpp>
#include <iostream>

namespace hako::assets::drone {

class ThrustDynamicsLinear : public hako::assets::drone::IThrustDynamics, public ICsvLog {
private:
    double delta_time_sec;
    double total_time_sec = 0;
    double param_A;
    double param_B;
    DroneThrustType thrust;
    DroneTorqueType torque;
    RotorConfigType rotor_config[ROTOR_NUM];
    double omega[ROTOR_NUM];
    drone_physics::VectorType position[ROTOR_NUM];
    double ccw[ROTOR_NUM];

public:
    ThrustDynamicsLinear(double dt)
    {
        this->delta_time_sec = dt;
        this->param_A =  1;
        this->param_B = 1.0/8000.0;

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
    virtual ~ThrustDynamicsLinear() {}

    void set_params(double a, double b)
    {
        this->param_A = a;
        this->param_B = b;
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
        }
        this->thrust.data = drone_physics::body_thrust_linear(param_A, ROTOR_NUM, omega);
        this->torque = drone_physics::body_torque_linear(param_A, param_B, ROTOR_NUM, position, ccw, omega);

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
        return { "timestamp", "Thrust", "Tx", "Ty", "Tz" };
    }
    const std::vector<std::string> log_data() override
    {
        DroneThrustType thrust = get_thrust();
        DroneTorqueType torque = get_torque();

        return {std::to_string(CsvLogger::get_time_usec()), std::to_string(thrust.data), std::to_string(torque.data.x), std::to_string(torque.data.y), std::to_string(torque.data.z)};
    }

};

}
//https://github.com/PX4/jMAVSim/blob/66b764ada522893c05224950aa6268c809f8e48a/src/me/drton/jmavsim/vehicle/AbstractMulticopter.java

#endif /* _THRUST_DYNAMICS_LINEAR_HPP_ */
