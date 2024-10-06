#ifndef _ROTOR_DYNAMICS_JMAVSIM_HPP_
#define _ROTOR_DYNAMICS_JMAVSIM_HPP_

#include "drone_primitive_types.hpp"
#include "irotor_dynamics.hpp"
#include "utils/icsv_log.hpp"
#include "rotor_physics.hpp"
#include <math.h>

namespace hako::assets::drone {


class RotorDynamicsJmavsim : public hako::assets::drone::IRotorDynamics, public ICsvLog {
private:
    double param_rpm_max = 6000.0;
    double param_tr = 1.0;
    double param_kr = 1.0;
    double w;
    double delta_time_sec;
    double total_time_sec;

public:
    virtual ~RotorDynamicsJmavsim() {}
    RotorDynamicsJmavsim(double dt)
    {
        this->delta_time_sec = dt;
        this->total_time_sec = 0;
        this->w = 0;
    }
    void set_params(double rpm_max, double tr, double kr)
    {
        this->param_rpm_max = rpm_max;
        this->param_tr = tr;
        this->param_kr = kr;
    }
    void set_rotor_speed(DroneRotorSpeedType &rotor_speed) override 
    {
        this->w = rotor_speed.data / this->param_kr;
    }
    DroneRotorSpeedType get_rotor_speed() const override
    {
        DroneRotorSpeedType value;
        value.data = this->w * this->param_kr;
        return value;
    }
    double get_rpm_max() const override
    {
        return this->param_kr;
    }

    void run(double control) override
    {
        //w += (control - w) * (1.0 - Math.exp(-dt / tau));
        this->w += (control - this->w) * (1.0 - exp(-this->delta_time_sec/ this->param_tr));
        this->total_time_sec += this->delta_time_sec;
    }
    const std::vector<std::string> log_head() override
    {
        return { "timestamp", "RPM" };
    }
    const std::vector<std::string> log_data() override
    {
        DroneRotorSpeedType v = get_rotor_speed();
        return {std::to_string(CsvLogger::get_time_usec()), std::to_string(v.data)};
    }
};

}

#endif /* _ROTOR_DYNAMICS_JMAVSIM_HPP_ */
