#ifndef _IROTOR_DYNAMICS_HPP_
#define _IROTOR_DYNAMICS_HPP_

#include "primitive_types.hpp"
#include "config/drone_config_types.hpp"

using namespace hako::config;

namespace hako::aircraft {


class IRotorDynamics {
public:
    virtual ~IRotorDynamics() {}

    virtual void set_rotor_speed(DroneRotorSpeedType &rotor_speed) = 0;

    virtual DroneRotorSpeedType get_rotor_speed() const = 0;

    virtual bool has_battery_dynamics() = 0;
    virtual void set_battery_dynamics_constants(const RotorBatteryModelConstants &c) = 0;

    virtual double get_rad_per_sec_max() const = 0;
    virtual void reset() = 0;

    virtual void run(double control) = 0;
    virtual void run(double control, double vbat) = 0;
};

}

#endif /* _IROTOR_DYNAMICS_HPP_ */
