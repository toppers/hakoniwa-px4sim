#ifndef _ROTOR_DYNAMICS_HPP_
#define _ROTOR_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"
#include "irotor_dynamics.hpp"
#include <math.h>

namespace hako::assets::drone {


class RotorDynamics : public hako::assets::drone::IRotorDynamics {
private:
    double param_rpm_max = 6000.0;
    double param_tau = 1.0;
    double param_k = 1.0;
    DroneRotorSpeedType speed;
    DroneRotorSpeedType next_speed;
    double delta_time_sec;
    double total_time_sec;

public:
    virtual ~RotorDynamics() {}
    RotorDynamics(double dt)
    {
        this->delta_time_sec = dt;
        this->total_time_sec = 0;
        this->speed.data = 0;
    }
    void set_params(double rpm_max, double tau, double k)
    {
        this->param_rpm_max = rpm_max;
        this->param_tau = tau;
        this->param_k = k;
    }
    void set_rotor_speed(DroneRotorSpeedType &rotor_speed) override 
    {
        this->speed = rotor_speed;
    }
    DroneRotorSpeedType get_rotor_speed() const override
    {
        DroneRotorSpeedType value;
        value.data = this->speed.data * this->param_rpm_max;
        return value;
    }

    void run(double control) override
    {
        this->next_speed.data =   ( this->param_k * control * this->delta_time_sec ) 
                                - ( this->delta_time_sec / this->param_tau ) * this->speed.data
                                + this->speed.data;
        // Cap the next speed at the maximum RPM if it exceeds it
        if (this->next_speed.data > this->param_rpm_max) {
            this->next_speed.data = this->param_rpm_max;
        }
        // Ensure the next speed does not fall below the minimum RPM (assuming 0 for this example)
        else if (this->next_speed.data < 0) {
            this->next_speed.data = 0;
        }
        this->speed.data = this->next_speed.data;
        this->total_time_sec += this->delta_time_sec;
    }
};

}

#endif /* _ROTOR_DYNAMICS_HPP_ */
