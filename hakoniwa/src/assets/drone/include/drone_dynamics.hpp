#ifndef _DRONE_DYNAMICS_HPP_
#define _DRONE_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"

namespace hako::assets::drone {

class IDroneDynamics {
public:
    virtual ~IDroneDynamics() {}

    virtual void set_pos(DronePositionType &pos) = 0;
    virtual void set_vel(DroneVelocityType &vel) = 0;
    virtual void set_angle(DroneAngleType &angle) = 0;
    virtual void set_angular_vel(DroneAngularVelocityType &angular_vel) = 0;

    virtual DronePositionType get_pos() const = 0;
    virtual DroneVelocityType get_vel() const = 0;
    virtual DroneAngleType get_angle() const = 0;
    virtual DroneAngularVelocityType get_angular_vel() const = 0;

    virtual void run(DroneThrustype &thrust, DroneTorqueType& torque) = 0;
};

}

#endif /* _DRONE_DYNAMICS_HPP_ */
