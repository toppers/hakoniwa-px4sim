#ifndef _ITHRUST_DYNAMICS_HPP_
#define _ITHRUST_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"

namespace hako::assets::drone {

const int ROTOR_NUM = 4;

class IThrustDynamics {
public:
    virtual ~IThrustDynamics() {}

    virtual void set_rotor_config(const RotorConfigType rotor_config[ROTOR_NUM]) = 0;
    virtual void set_thrust(const DroneThrustType &thrust) = 0;
    virtual void set_torque(const DroneTorqueType &torque) = 0;

    virtual DroneThrustType get_thrust() const = 0;
    virtual DroneTorqueType get_torque() const = 0;

    virtual void run(const DroneRotorSpeedType rotor_speed[ROTOR_NUM]) = 0;
};

}

#endif /* _ITHRUST_DYNAMICS_HPP_ */
