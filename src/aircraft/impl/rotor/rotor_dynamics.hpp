#ifndef _ROTOR_DYNAMICS_HPP_
#define _ROTOR_DYNAMICS_HPP_

#include "physics/rotor_physics.hpp"
#include "aircraft/interfaces/irotor_dynamics.hpp"
#include "aircraft/interfaces/icurrent_dynamics.hpp"
#include "logger/ilog.hpp"
#include "logger/impl/hako_logger.hpp"
#include <math.h>

using namespace hako::logger;

namespace hako::aircraft {

class RotorDynamics : public IRotorDynamics, public ICurrentDynamics, public ILog {
private:
    double param_rad_per_sec_max = 6000.0;
    double param_tr = 1.0;
    double param_kr = 1.0;
    bool battery_dynamics = false;
    RotorBatteryModelConstants constants;
    DroneRotorSpeedType speed;
    DroneRotorSpeedType next_speed;
    double delta_time_sec;
    double total_time_sec;
    double current; /* [A] */
    double duty;

    void run_current(double vbat, double omega, double duty_rate)
    {
        /* current in [A] */
        this->current = drone_physics::rotor_current(
                            vbat, /* battery voltage in volt [V]*/
                            this->constants.R, /* resistance in ohm [V/A] */
                            this->constants.K, /* back electromotive force coeff in [N m/A] */
                            omega, /* angular velocity in [rad/sec] */
                            duty_rate /* 0.0-1.0 (ratio of PWM) */
                            );
    }
public:
    virtual ~RotorDynamics() {}
    RotorDynamics(double dt)
    {
        this->delta_time_sec = dt;
        this->total_time_sec = 0;
        this->speed.data = 0;
        this->current = 0;
    }
    void reset() override
    {
        this->speed.data = 0;
        this->current = 0;
        this->total_time_sec = 0;
    }
    void set_battery_dynamics_constants(const RotorBatteryModelConstants &c) override
    {
        battery_dynamics = true;
        this->constants = c;
    }
    bool has_battery_dynamics() override
    {
        return battery_dynamics;
    }

    void set_params(double rad_per_sec_max, double tr, double kr)
    {
        this->param_rad_per_sec_max = rad_per_sec_max;
        this->param_tr = tr;
        this->param_kr = kr;
    }
    void set_rotor_speed(DroneRotorSpeedType &rotor_speed) override 
    {
        this->speed = rotor_speed;
    }
    double get_rad_per_sec_max() const override
    {
        return this->param_kr;
    }
    DroneRotorSpeedType get_rotor_speed() const override
    {
        DroneRotorSpeedType value;
        value.data = this->speed.data;
        return value;
    }

    void run(double control) override
    {
        this->duty = control;
        this->next_speed.data =   (
                                    drone_physics::rotor_omega_acceleration(param_kr, param_tr, speed.data, control)
                                  ) * this->delta_time_sec
                                + this->speed.data;
        // Cap the next speed at the maximum RPS if it exceeds it
        if (this->next_speed.data > this->param_rad_per_sec_max) {
            this->next_speed.data = this->param_rad_per_sec_max;
        }
        // Ensure the next speed does not fall below the minimum RPS (assuming 0 for this example)
        else if (this->next_speed.data < 0) {
            this->next_speed.data = 0;
        }
        this->speed.data = this->next_speed.data;
        this->total_time_sec += this->delta_time_sec;
    }
    void run(double control, double vbat) override
    {
        this->duty = control;
        this->run_current(vbat, this->speed.data, control);
        this->next_speed.data =   (
                                    drone_physics::rotor_omega_acceleration(vbat, constants.R, constants.Cq, constants.J, constants.K, constants.D, speed.data, control)
                                  ) * this->delta_time_sec
                                + this->speed.data;
        // Cap the next speed at the maximum RPS if it exceeds it
        if (this->next_speed.data > this->param_rad_per_sec_max) {
            this->next_speed.data = this->param_rad_per_sec_max;
        }
        // Ensure the next speed does not fall below the minimum RPS (assuming 0 for this example)
        else if (this->next_speed.data < 0) {
            this->next_speed.data = 0;
        }
        this->speed.data = this->next_speed.data;
        this->total_time_sec += this->delta_time_sec;
    }
    double get_current() override
    {
        return this->current;
    }
    const std::vector<LogHeaderType>& log_head() override
    {
        static const std::vector<LogHeaderType> headers = {
            {"timestamp", LOG_TYPE_UINT64}, // timestamp: unsigned 64-bit integer
            {"Duty", LOG_TYPE_DOUBLE},         // X: double
            {"RadPerSec", LOG_TYPE_DOUBLE},         // Y: double
            {"Current", LOG_TYPE_DOUBLE}          // Z: double
        };
        return headers;
    }
    const std::vector<LogDataType>& log_data() override
    {
        DroneRotorSpeedType v = get_rotor_speed();
        static std::vector<LogDataType> data;
        data.clear();
        data.push_back(HakoLogger::get_time_usec()); // timestamp (uint64_t)
        data.push_back(duty);
        data.push_back(v.data); 
        data.push_back(current); 
        return data;
    }
};

}

#endif /* _ROTOR_DYNAMICS_HPP_ */
