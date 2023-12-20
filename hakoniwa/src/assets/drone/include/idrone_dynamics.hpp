#ifndef _IDRONE_DYNAMICS_HPP_
#define _IDRONE_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"
#include "utils/icsv_log.hpp"

namespace hako::assets::drone {

class IDroneDynamics: public ICsvLog {
protected:
    DronePhysCalcCacheType cache;
public:
    virtual ~IDroneDynamics() {}

    virtual void set_drag(double drag) = 0;
    virtual void set_torque_constants(double cx, double cy, double cz) = 0;
    virtual void set_pos(const DronePositionType &pos) = 0;
    virtual void set_vel(const DroneVelocityType &vel) = 0;
    virtual void set_angle(const DroneAngleType &angle) = 0;
    virtual void set_angular_vel(const DroneAngularVelocityType &angular_vel) = 0;

    virtual DronePositionType get_pos() const = 0;
    virtual DroneVelocityType get_vel() const = 0;
    virtual DroneAngleType get_angle() const = 0;
    virtual DroneAngularVelocityType get_angular_vel() const = 0;

    virtual DroneVelocityBodyFrameType get_vel_body_frame() const = 0;
    virtual DroneAngularVelocityBodyFrameType get_angular_vel_body_frame() const = 0;
    virtual double get_mass() const = 0;

    virtual void run(const DroneThrustType &thrust, const DroneTorqueType& torque) = 0;
};

}

#endif /* _IDRONE_DYNAMICS_HPP_ */
