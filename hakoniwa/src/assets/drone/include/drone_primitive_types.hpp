#ifndef _DRONE_DATA_TYPES_HPP_
#define _DRONE_DATA_TYPES_HPP_

#include <glm/vec3.hpp>
#include <math.h>

/*
 * Coordinate System: Ground Coordinate System (NED)
 * Unit: Meter
 * x: North (Distance towards the North)
 * y: East (Distance towards the East)
 * z: Down (Distance downwards, height from ground)
 * 
 * REF: https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#position
 */
typedef struct {
    glm::dvec3 data;
} DronePositionType;

/*
 * Coordinate System: Ground Coordinate System (NED)
 * Unit: Meters per Second (m/s)
 * x: Velocity towards the North
 * y: Velocity towards the East
 * z: Velocity downwards (towards the ground)
 * 
 * REF: https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#velocity
 */
typedef struct {
    glm::dvec3 data;
} DroneVelocityType;


/*
 * Coordinate System: Body Coordinate System (FRD)
 * Unit: Meters per Second (m/s)
 * x: Velocity in the forward direction (nose)
 * y: Velocity towards the right wing
 * z: Velocity downwards (relative to aircraft body)
 * 
 * REF: https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#velocity-in-body-coordinate-system
 */
typedef struct {
    glm::dvec3 data;
} DroneVelocityBodyFrameType;

/*
 * Coordinate System: Ground Coordinate System (NED)
 * Unit: Radian
 * x: Rotation around the X-axis (North)
 * y: Rotation around the Y-axis (East)
 * z: Rotation around the Z-axis (Down)
 * 
 * REF: https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#angle
 */
typedef struct {
    glm::dvec3 data;
} DroneAngleType;

/*
 * Coordinate System: Ground Coordinate System (NED)
 * Unit: Radian per Second (rad/s)
 * x: Angular velocity around the X-axis (North)
 * y: Angular velocity around the Y-axis (East)
 * z: Angular velocity around the Z-axis (Down)
 * 
 * REF: https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#angularvelocity
 */
typedef struct {
    glm::dvec3 data;
} DroneAngularVelocityType;


/*
 * Coordinate System: Body Coordinate System (FRD)
 * Unit: Radian per Second (rad/s)
 * x: Angular velocity around the forward (X) axis
 * y: Angular velocity around the right (Y) axis
 * z: Angular velocity around the downward (Z) axis
 * 
 * REF: https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#angular-velocity-in-body-coordinate-system
 */
typedef struct {
    glm::dvec3 data;
} DroneAngularVelocityBodyFrameType;

/*
 * Unit: Revolutions per Second (RPM)
 * data: The rate of change in the drone's rotor speed, directly influencing lift and flight stability
 * 
 * REF: https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#rotor-speed
 */
typedef struct {
    double data;
} DroneRotorSpeedType;

/*
 * Unit: Newton (N)
 * data: The force exerted by the rotors to lift the drone against gravity
 * 
 * REF: https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#thrust
 */
typedef struct {
    double data;
} DroneThrustType;

/*
 * Unit: Newton-meter (Nm)
 * x: Roll Torque (φ direction): The force that rotates the drone around its lateral axis (side to side).
 * y: Pitch Torque (θ direction): The force that rotates the drone around its longitudinal axis (front to back).
 * z: Yaw Torque (ψ direction): The force that rotates the drone around its vertical axis (up and down).
 * 
 * REF: https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#torque
 */
typedef struct {
    glm::dvec3 data;
} DroneTorqueType;

typedef struct {
    int ccw; /* CW: -1 or CCW: 1 */
    glm::dvec3 data;
} RotorConfigType;

typedef struct {
    glm::dvec3 data;
} DroneAccelerationBodyFrameType;

typedef struct {
    double cos_phi;
    double cos_theta;
    double cos_psi;
    double sin_phi;
    double sin_theta;
    double sin_psi;
    double tan_theta;
    double sec_theta;
} DronePhysCalcCacheType;

static inline DronePhysCalcCacheType drone_phys_calc_cache(DroneAngleType angle)
{
    DronePhysCalcCacheType cache;
    cache.cos_phi = cos(angle.data.x);
    cache.cos_theta = cos(angle.data.y);
    cache.cos_psi = cos(angle.data.z);
    cache.sin_phi = sin(angle.data.x);
    cache.sin_theta = sin(angle.data.y);
    cache.sin_psi = sin(angle.data.z);
    cache.tan_theta = tan(angle.data.y);
    cache.sec_theta = 1 / cos(angle.data.y);
    return cache;
}

#endif /* _DRONE_DATA_TYPES_HPP_ */