#ifndef _ITHRUST_DYNAMICS_HPP_
#define _ITHRUST_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"

namespace hako::assets::drone {

const int ROTOR_NUM = 4;

class IThrustDynamics {
public:
    virtual ~IThrustDynamics() {}

    virtual void set_thrust(DroneThrustype &thrust) = 0;
    virtual void set_torque(DroneTorqueType &torque) = 0;

    virtual DroneThrustype get_thrust() const = 0;
    virtual DroneTorqueType get_torque() const = 0;

    virtual void run(const DroneRotorSpeedType rotor_speed[ROTOR_NUM]) = 0;
};

}

#endif /* _ITHRUST_DYNAMICS_HPP_ */
