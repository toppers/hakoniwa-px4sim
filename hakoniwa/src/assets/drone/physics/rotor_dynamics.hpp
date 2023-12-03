#ifndef _ROTOR_DYNAMICS_HPP_
#define _ROTOR_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"
#include "irotor_dynamics.hpp"
#include <math.h>

namespace hako::assets::drone {


class RotorDynamics : public hako::assets::drone::IRotorDynamics {
private:
    DroneRotorSpeedType speed;
    double delta_time_sec;
    double w;
    double tau;

public:
    virtual ~RotorDynamics() {}
    RotorDynamics(double dt)
    {
        this->delta_time_sec = dt;
        this->w = 0;
        this->tau = 1.0;
    }
    void set_rotor_speed(DroneRotorSpeedType &rotor_speed) override 
    {
        this->speed = rotor_speed;
    }
    void set_tau(double tau)
    {
        this->tau = tau;
    }
    DroneRotorSpeedType get_rotor_speed() const override
    {
        return this->speed;
    }

    void run(double control) override
    {
        this->w += (control - this->w) * (1.0 - exp(-this->delta_time_sec / this->tau));
        this->speed.data = this->w;
    }
};

}

#endif /* _ROTOR_DYNAMICS_HPP_ */
