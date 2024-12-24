#ifndef _IDRONE_DYNAMICS_HPP_
#define _IDRONE_DYNAMICS_HPP_

#include "primitive_types.hpp"
#include "logger/ilog.hpp"
#include "aircraft/interfaces/idisturbance.hpp"
#include "config/drone_config_types.hpp"

using namespace hako::logger;
using namespace hako::config;

namespace hako::aircraft {

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
    bool control;
    DronePositionType pos;
    DroneEulerType angle;
} DroneDynamicsManualControlType;

typedef struct {
    mi_drone_sensor_disturbance_t values;
} DroneDynamicsDisturbanceType;

typedef struct {
#if 0 //TODO
    bool no_use_actuator;
#endif
    double controls[MAX_ROTOR_NUM]; // rotor duty rate
    DroneDynamicsManualControlType manual;
    DroneDynamicsCollisionType collision;
    DroneDynamicsDisturbanceType disturbance;
    DroneThrustType thrust;
    DroneTorqueType torque;
} AircraftInputType;


class IDroneDynamics: public ILog {
protected:
    bool use_quaternion = false;
public:
    virtual ~IDroneDynamics() {}
    virtual void set_use_quaternion(bool use) {
        use_quaternion = use;
    }
    virtual void reset() = 0;
    virtual void set_drag(double drag1, double drag2) = 0;
    virtual double get_drag() const = 0;
    virtual void set_collision_detection(bool enable) = 0;
    virtual void set_manual_control(bool enable) = 0;
    virtual void set_body_size(double x, double y, double z) = 0;
    virtual void set_torque_constants(double cx, double cy, double cz) = 0;
    virtual void set_out_of_bounds_reset(const std::optional<OutOfBoundsReset>& reset_options) = 0;
    virtual void set_pos(const DronePositionType &pos) = 0;
    virtual void set_vel(const DroneVelocityType &vel) = 0;
    virtual void set_angle(const DroneEulerType &angle) = 0;

    virtual DronePositionType get_pos() const = 0;
    virtual DroneVelocityType get_vel() const = 0;
    virtual DroneEulerType get_angle() const = 0;

    virtual DroneVelocityBodyFrameType get_vel_body_frame() const = 0;
    virtual DroneAngularVelocityBodyFrameType get_angular_vel_body_frame() const = 0;
    virtual double get_mass() const = 0;
    virtual void set_mass(double mass) = 0;
    virtual bool has_collision_detection() = 0;
    virtual bool has_manual_control() = 0;

    virtual void run(const AircraftInputType &input) = 0;
};

}

#endif /* _IDRONE_DYNAMICS_HPP_ */
