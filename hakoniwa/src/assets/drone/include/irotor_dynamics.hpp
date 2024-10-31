#ifndef _IROTOR_DYNAMICS_HPP_
#define _IROTOR_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"

namespace hako::assets::drone {


class IRotorDynamics {
public:
    virtual ~IRotorDynamics() {}

    virtual void set_rotor_speed(DroneRotorSpeedType &rotor_speed) = 0;

    virtual DroneRotorSpeedType get_rotor_speed() const = 0;

    virtual double get_rad_per_sec_max() const = 0;

    virtual void run(double control) = 0;
};

}

#endif /* _IROTOR_DYNAMICS_HPP_ */
