#ifndef _THRUST_DYNAMICS_NONLINEAR_HPP_
#define _THRUST_DYNAMICS_NONLINEAR_HPP_

#include "physics/rotor_physics.hpp"
#include "aircraft/interfaces/ithrust_dynamics.hpp"
#include "logger/ilog.hpp"
#include "logger/impl/hako_logger.hpp"

#include <glm/glm.hpp>
#include <iostream>

using namespace hako::logger;

namespace hako::aircraft {

class ThrustDynamicsNonLinear : public IThrustDynamics, public ILog {
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
        this->param_Ct = 0;
        this->param_Cq = 0;
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

    const std::vector<LogHeaderType>& log_head() override
    {
        static const std::vector<LogHeaderType> headers = {
            {"timestamp", LOG_TYPE_UINT64}, // timestamp: unsigned 64-bit integer
            {"Thrust", LOG_TYPE_DOUBLE},         // Thrust: double
            {"Tx", LOG_TYPE_DOUBLE},         // Tx: double
            {"Ty", LOG_TYPE_DOUBLE},         // Ty: double
            {"Tz", LOG_TYPE_DOUBLE}          // Tz: double
        };
        return headers;
    }
    const std::vector<LogDataType>& log_data() override
    {
        DroneThrustType _thrust = get_thrust();
        DroneTorqueType _torque = get_torque();
        static std::vector<LogDataType> data;
        data.clear();
        data.push_back(HakoLogger::get_time_usec()); // timestamp (uint64_t)
        data.push_back(_thrust.data);
        data.push_back(_torque.data.x); 
        data.push_back(_torque.data.y); 
        data.push_back(_torque.data.z); 
        return data;

    }

};

}

#endif /* _THRUST_DYNAMICS_NONLINEAR_HPP_ */
