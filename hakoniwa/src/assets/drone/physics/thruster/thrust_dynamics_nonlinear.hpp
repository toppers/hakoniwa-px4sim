#ifndef _THRUST_DYNAMICS_NONLINEAR_HPP_
#define _THRUST_DYNAMICS_NONLINEAR_HPP_

#include "drone_primitive_types.hpp"
#include "ithrust_dynamics.hpp"
#include "utils/icsv_log.hpp"
#include "rotor_physics.hpp"
#include <glm/glm.hpp>
#include <iostream>

namespace hako::assets::drone {

class ThrustDynamicsNonLinear : public hako::assets::drone::IThrustDynamics, public ICsvLog {
private:
    double delta_time_sec;
    double total_time_sec = 0;
    double param_Ct;
    double param_Cq;
    double param_J;
    DroneThrustType thrust;
    DroneTorqueType torque;
    DroneRotorSpeedType prev_rotor_speed[ROTOR_NUM];
    RotorConfigType rotor_config[ROTOR_NUM];
    double omega[ROTOR_NUM];
    drone_physics::VectorType position[ROTOR_NUM];
    double ccw[ROTOR_NUM];
    double omega_acceleration[ROTOR_NUM];

public:
    ThrustDynamicsNonLinear(double dt)
    {
        this->delta_time_sec = dt;
        // 1kg の機体が 6000 rpm でホバリングする定数
        // A = mg / (Ω0^2 * ROTOR_NUM)
        this->param_Ct =  GRAVITY / (ROTOR_NUM * HOVERING_ROTOR_RPM * HOVERING_ROTOR_RPM);
        // 1kg の機体が 6000 rpm でホバリングする場合に、1Nmで反トルクがかかる定数
        // B = 1 / (Ω0^2 * ROTOR_NUM)
        this->param_Cq = 1.0 / (ROTOR_NUM * HOVERING_ROTOR_RPM * HOVERING_ROTOR_RPM);
        this->param_J = 0.1;

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
    virtual ~ThrustDynamicsNonLinear() {}

    void set_params(double Ct, double Cq, double J)
    {
        this->param_Ct = Ct;
        this->param_Cq = Cq;
        this->param_J = J;
    }

    void set_rotor_config(const RotorConfigType _rotor_config[ROTOR_NUM]) override
    {
        for (int i = 0; i < ROTOR_NUM; i++) {
            this->rotor_config[i] = _rotor_config[i];
            this->position[i] = { _rotor_config[i].data.x, _rotor_config[i].data.y, _rotor_config[i].data.z};
            this->ccw[i] = _rotor_config[i].ccw;
        }
    }
    void set_thrust(const DroneThrustType &_thrust) override 
    {
        this->thrust = _thrust;
    }
    void set_torque(const DroneTorqueType &_torque) override
    {
        this->torque = _torque;
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
        this->thrust.data = drone_physics::body_thrust(param_Ct, ROTOR_NUM, omega);
        this->torque = drone_physics::body_torque(param_Ct, param_Cq, param_J, ROTOR_NUM,
                                            position, ccw, omega, omega_acceleration);

        for (int i = 0; i < ROTOR_NUM; i++) {
            this->prev_rotor_speed[i] = rotor_speed[i];
        }

        total_time_sec += delta_time_sec;
    }
    void reset() override
    {
        this->thrust.data = 0;
        this->torque.data = {0, 0, 0};
        for (int i = 0; i < ROTOR_NUM; i++) {
            this->prev_rotor_speed[i].data = 0;
        }
        total_time_sec = 0;
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
        DroneThrustType _thrust = get_thrust();
        DroneTorqueType _torque = get_torque();

        return {std::to_string(CsvLogger::get_time_usec()), std::to_string(_thrust.data), std::to_string(_torque.data.x), std::to_string(_torque.data.y), std::to_string(_torque.data.z)};
    }

};

}

#endif /* _THRUST_DYNAMICS_NONLINEAR_HPP_ */
