#ifndef _IDRONE_DYNAMICS_HPP_
#define _IDRONE_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"
#include "utils/icsv_log.hpp"

namespace hako::assets::drone {

#define MAX_CONTAT_NUM 10
#define MAX_ROTOR_NUM  16

typedef struct {
    bool collision;
    int contact_num;
    drone_physics::VectorType relative_velocity;
    drone_physics::VectorType contact_position[MAX_CONTAT_NUM];
    double restitution_coefficient;
} DroneDynamicsCollisionType;

typedef struct {
    bool no_use_actuator;
    double controls[MAX_ROTOR_NUM];
    DroneDynamicsCollisionType collision;
    DroneThrustType thrust;
    DroneTorqueType torque;
} DroneDynamicsInputType;


class IDroneDynamics: public ICsvLog {
protected:
    DronePhysCalcCacheType cache;
public:
    virtual ~IDroneDynamics() {}

    virtual void set_drag(double drag1, double drag2) = 0;
    virtual void set_collision_detection(bool enable) = 0;
    virtual void set_body_size(double x, double y, double z) = 0;
    virtual void set_torque_constants(double cx, double cy, double cz) = 0;
    virtual void set_pos(const DronePositionType &pos) = 0;
    virtual void set_vel(const DroneVelocityType &vel) = 0;
    virtual void set_angle(const DroneEulerType &angle) = 0;
    virtual void set_angular_vel(const DroneEulerRateType &angular_vel) = 0;

    virtual DronePositionType get_pos() const = 0;
    virtual DroneVelocityType get_vel() const = 0;
    virtual DroneEulerType get_angle() const = 0;
    virtual DroneEulerRateType get_angular_vel() const = 0;

    virtual DroneVelocityBodyFrameType get_vel_body_frame() const = 0;
    virtual DroneAngularVelocityBodyFrameType get_angular_vel_body_frame() const = 0;
    virtual double get_mass() const = 0;
    virtual void set_mass(double mass) = 0;
    virtual bool has_collision_detection() = 0;

    virtual void run(const DroneDynamicsInputType &input) = 0;
};

}

#endif /* _IDRONE_DYNAMICS_HPP_ */
