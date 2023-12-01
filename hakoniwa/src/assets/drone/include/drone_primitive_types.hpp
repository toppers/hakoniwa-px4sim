#ifndef _DRONE_DATA_TYPES_HPP_
#define _DRONE_DATA_TYPES_HPP_

#include <glm/vec3.hpp>

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
 * Unit: Revolutions per Second (RPS)
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
} DroneThrustype;

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

#endif /* _DRONE_DATA_TYPES_HPP_ */