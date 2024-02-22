#ifndef _DRONE_DATA_TYPES_HPP_
#define _DRONE_DATA_TYPES_HPP_

#include <glm/vec3.hpp>
#include <math.h>
#include "body_physics.hpp"

namespace hako::assets::drone {

const double GRAVITY = 9.81;
const int HOVERING_ROTOR_RPM = 6000;

/*
 * Coordinate System: Ground Coordinate System (NED)
 * Unit: Meter
 * x: North (Distance towards the North)
 * y: East (Distance towards the East)
 * z: Down (Distance downwards, height from ground)
 * 
 * REF: https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#position
 */
typedef struct DronePosition {
    glm::dvec3 data;
    DronePosition() { data.x = 0; data.y = 0; data.z = 0; }
    DronePosition(const hako::drone_physics::VectorType& rhs) : data(rhs.x, rhs.y, rhs.z) {} 
    DronePosition& operator=(const hako::drone_physics::VectorType& rhs) {
        this->data = {rhs.x, rhs.y, rhs.z};
        return *this;
    }
    operator hako::drone_physics::VectorType() const {
        return hako::drone_physics::VectorType{data.x, data.y, data.z};
    }
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
typedef struct DroneVelocity {
    glm::dvec3 data;
    DroneVelocity(){ data.x = 0; data.y = 0; data.z = 0; }
    DroneVelocity(const hako::drone_physics::VectorType& rhs) : data(rhs.x, rhs.y, rhs.z) {} 
    DroneVelocity& operator=(const hako::drone_physics::VectorType& rhs) {
        this->data = {rhs.x, rhs.y, rhs.z};
        return *this;
    }
    operator hako::drone_physics::VectorType() const {
        return {data.x, data.y, data.z};
    }
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
typedef struct DroneVelocityBodyFrame {
    glm::dvec3 data;
    DroneVelocityBodyFrame(){ data.x = 0; data.y = 0; data.z = 0; }
    DroneVelocityBodyFrame(const hako::drone_physics::VectorType& rhs) : data(rhs.x, rhs.y, rhs.z) {} 
    DroneVelocityBodyFrame& operator=(const hako::drone_physics::VectorType& rhs) {
        this->data = {rhs.x, rhs.y, rhs.z};
        return *this;
    }
    operator hako::drone_physics::VectorType() const {
        return {data.x, data.y, data.z};
    }
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
typedef struct DroneAngle {
    glm::dvec3 data;
    DroneAngle(){ data.x = 0; data.y = 0; data.z = 0; }
    DroneAngle(const hako::drone_physics::EulerType& rhs) : data(rhs.phi, rhs.theta, rhs.psi) {} 
    DroneAngle& operator=(const hako::drone_physics::EulerType& rhs) {
        this->data = {rhs.phi, rhs.theta, rhs.psi};
        return *this;
    }
    operator hako::drone_physics::EulerType() const {
        return {data.x, data.y, data.z};
    }
} DroneEulerType;

/*
 * Coordinate System: Ground Coordinate System (NED)
 * Unit: Radian per Second (rad/s)
 * x: Angular velocity around the X-axis (North)
 * y: Angular velocity around the Y-axis (East)
 * z: Angular velocity around the Z-axis (Down)
 * 
 * REF: https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#angularvelocity
 */
typedef struct DroneAngularRate {
    glm::dvec3 data;
    DroneAngularRate(){ data.x = 0; data.y = 0; data.z = 0; }
    DroneAngularRate(const hako::drone_physics::EulerRateType& rhs) : data(rhs.phi, rhs.theta, rhs.psi) {} 
    DroneAngularRate& operator=(const hako::drone_physics::EulerRateType& rhs) {
        this->data = {rhs.phi, rhs.theta, rhs.psi};
        return *this;
    }
    operator hako::drone_physics::EulerRateType() const {
        return {data.x, data.y, data.z};
    }
} DroneEulerRateType;


/*
 * Coordinate System: Body Coordinate System (FRD)
 * Unit: Radian per Second (rad/s)
 * x: Angular velocity around the forward (X) axis
 * y: Angular velocity around the right (Y) axis
 * z: Angular velocity around the downward (Z) axis
 * 
 * REF: https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#angular-velocity-in-body-coordinate-system
 */
typedef struct DroneAngularVelocityBodyFrame {
    glm::dvec3 data;
    DroneAngularVelocityBodyFrame(){ data.x = 0; data.y = 0; data.z = 0; }
    DroneAngularVelocityBodyFrame(const hako::drone_physics::AngularVelocityType& rhs) : data(rhs.x, rhs.y, rhs.z) {} 
    DroneAngularVelocityBodyFrame& operator=(const hako::drone_physics::AngularVelocityType& rhs) {
        this->data = {rhs.x, rhs.y, rhs.z};
        return *this;
    }
    operator hako::drone_physics::AngularVelocityType() const {
        return {data.x, data.y, data.z};
    }
} DroneAngularVelocityBodyFrameType;

typedef struct DroneAngularAccelerationBodyFrame {
    glm::dvec3 data;
    DroneAngularAccelerationBodyFrame(){ data.x = 0; data.y = 0; data.z = 0; }
    DroneAngularAccelerationBodyFrame(const hako::drone_physics::AngularAccelerationType& rhs) : data(rhs.x, rhs.y, rhs.z) {} 
    DroneAngularAccelerationBodyFrame& operator=(const hako::drone_physics::AngularAccelerationType& rhs) {
        this->data = {rhs.x, rhs.y, rhs.z};
        return *this;
    }
    operator hako::drone_physics::AngularVelocityType() const {
        return {data.x, data.y, data.z};
    }
} DroneAngularAccelerationBodyFrameType;

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
typedef struct DroneTorque {
    glm::dvec3 data;
    DroneTorque(){ data.x = 0; data.y = 0; data.z = 0; }
    DroneTorque(const hako::drone_physics::VectorType& rhs) : data(rhs.x, rhs.y, rhs.z) {} 
    DroneTorque& operator=(const hako::drone_physics::VectorType& rhs) {
        this->data = {rhs.x, rhs.y, rhs.z};
        return *this;
    }
    operator hako::drone_physics::VectorType() const {
        return {data.x, data.y, data.z};
    }
} DroneTorqueType;

typedef struct {
    double ccw; /* CW: -1 or CCW: 1 */
    glm::dvec3 data;
} RotorConfigType;

/*
 * Unit: Meters per second squared (m/s^2)
 * Description: Represents the acceleration of the drone in its body frame.
 * The 'data' field is a 3-dimensional vector (glm::dvec3) where:
 *   - data.x: Acceleration along the drone's forward axis (front to back).
 *   - data.y: Acceleration along the drone's lateral axis (side to side).
 *   - data.z: Acceleration along the drone's vertical axis (up and down).
 *
 * This structure is used to quantify the rate of change of the drone's velocity in its body frame,
 * accounting for movements in all three spatial dimensions.
 */
typedef struct DroneAccelerationBodyFrame {
    glm::dvec3 data;
    DroneAccelerationBodyFrame(){ data.x = 0; data.y = 0; data.z = 0; }
    DroneAccelerationBodyFrame(const hako::drone_physics::VectorType& rhs) : data(rhs.x, rhs.y, rhs.z) {} 
    DroneAccelerationBodyFrame& operator=(const hako::drone_physics::VectorType& rhs) {
        this->data = {rhs.x, rhs.y, rhs.z};
        return *this;
    }
    operator hako::drone_physics::VectorType() const {
        return {data.x, data.y, data.z};
    }
} DroneAccelerationBodyFrameType;

/*
 * Description: Represents the barometric pressure data for the drone.
 * This structure mirrors the barometric pressure-related members of the MAVLink HIL_SENSOR message.
 *
 * For detailed information and descriptions of each field, refer to:
 * https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/mavlink/HIL_SENSOR
 *
 * Fields:
 *   - abs_pressure: Absolute pressure (double)
 *   - diff_pressure: Differential pressure (double)
 *   - pressure_alt: Pressure-based altitude (double)
 */
typedef struct {
    double abs_pressure;    // Absolute pressure
    double diff_pressure;   // Differential pressure
    double pressure_alt;    // Pressure altitude
} DroneBarometricPressureType;


/*
 * Description: Represents the GPS data for the drone.
 * This structure is designed to capture various aspects of GPS data, with specific units for each field.
 *
 * Fields:
 *   - lat: Latitude in degrees (double). Not scaled by degE7.
 *   - lon: Longitude in degrees (double). Not scaled by degE7.
 *   - alt: Altitude in meters (double).
 *   - vel: GPS velocity in meters per second (m/s) (double).
 *   - vn: GPS ground speed (North) in meters per second (m/s) (double).
 *   - ve: GPS ground speed (East) in meters per second (m/s) (double).
 *   - vd: GPS ground speed (Down) in meters per second (m/s) (double).
 *   - cog: Course over ground in centidegrees (cdeg) (double). A value of -1 indicates undefined.
 *   - num_satelites_visible: Number of GPS satellites visible (int), same as satelites_visible.
 *   - eph: GPS horizontal dilution of position (double).
 *   - epv: GPS vertical dilution of position (double).
 *
 * Note: For more information on the structure and units, refer to the MAVLink HIL_GPS message specifications.
 * URL: https://mavlink.io/en/messages/common.html#HIL_GPS
 */
typedef struct {
    double lat;                  // Latitude in degrees
    double lon;                  // Longitude in degrees
    double alt;                  // Altitude in meters
    double vel;                  // GPS velocity in m/s
    double vn;                   // North speed in m/s
    double ve;                   // East speed in m/s
    double vd;                   // Down speed in m/s
    double cog;                  // Course over ground in cdeg (-1 if undefined)

    int num_satelites_visible;   // Number of satellites visible
    double eph;                  // Horizontal dilution of position
    double epv;                  // Vertical dilution of position
} DroneGpsDataType;

/*
 * Conversion Macros for GPS Data to MAVLink Format
 *
 * - LAT_LON_TO_DEGE7: Converts latitude/longitude in degrees to MAVLink's degE7 format.
 *   1 deg = 10^7 degE7.
 * - ALT_TO_MM: Converts altitude in meters to MAVLink's mm format.
 *   1 meter = 1000 mm.
 */

#define LAT_LON_TO_DEGE7(deg) (static_cast<int32_t>((deg) * 1e7))
#define ALT_TO_MM(meters) (static_cast<int32_t>((meters) * 1000))

// Example usage:
// int32_t mavlink_lat = LAT_LON_TO_DEGE7(drone_gps_data.lat);
// int32_t mavlink_lon = LAT_LON_TO_DEGE7(drone_gps_data.lon);
// int32_t mavlink_alt = ALT_TO_MM(drone_gps_data.alt);

/*
 * Conversion Macros for GPS DOP Data to MAVLink Format
 *
 * - DOP_TO_UINT16: Converts DOP value in double to MAVLink's uint16_t format.
 *   The DOP value is multiplied by 1 and cast to uint16_t.
 *   If the DOP value is unknown, UINT16_MAX is used.
 */

#define DOP_TO_UINT16(dop) ((dop) >= 0 ? static_cast<uint16_t>((dop) * 1) : UINT16_MAX)

// Example usage:
// uint16_t mavlink_eph = DOP_TO_UINT16(drone_gps_data.eph);
// uint16_t mavlink_epv = DOP_TO_UINT16(drone_gps_data.epv);

/*
 * Conversion Macros for GPS Velocity Data to MAVLink Format
 *
 * - VELOCITY_TO_UINT16: Converts velocity in m/s (double) to MAVLink's uint16_t format (cm/s).
 *   If the velocity is unknown, UINT16_MAX is used.
 * - VELOCITY_TO_INT16: Converts velocity in m/s (double) to MAVLink's int16_t format (cm/s).
 */

#define VELOCITY_TO_UINT16(vel) ((vel) >= 0 ? static_cast<uint16_t>((vel) * 100) : UINT16_MAX)
#define VELOCITY_TO_INT16(vel) (static_cast<int16_t>((vel) * 100))

// Example usage:
// uint16_t mavlink_vel = VELOCITY_TO_UINT16(drone_gps_data.vel);
// int16_t mavlink_vn = VELOCITY_TO_INT16(drone_gps_data.vn);
// int16_t mavlink_ve = VELOCITY_TO_INT16(drone_gps_data.ve);
// int16_t mavlink_vd = VELOCITY_TO_INT16(drone_gps_data.vd);

/*
 * Conversion Macro for GPS Course Over Ground to MAVLink Format
 *
 * - COG_TO_UINT16: Converts course over ground in degrees (double) to MAVLink's uint16_t format (cdeg).
 *   The course value is multiplied by 100 and cast to uint16_t.
 *   If the course value is unknown or invalid, UINT16_MAX is used.
 */

#define COG_TO_UINT16(cog) ((cog) >= 0 && (cog) < 360 ? static_cast<uint16_t>((cog) * 100) : UINT16_MAX)

// Example usage:
// uint16_t mavlink_cog = COG_TO_UINT16(drone_gps_data.cog);

/*
 * Conversion Macro for GPS Satellites Visible to MAVLink Format
 *
 * - SATELLITES_VISIBLE_TO_UINT8: Converts the number of satellites visible (int) to MAVLink's uint8_t format.
 *   If the number of satellites is unknown, UINT8_MAX is used.
 */

#define SATELLITES_VISIBLE_TO_UINT8(num) ((num) >= 0 ? static_cast<uint8_t>(num) : UINT8_MAX)

// Example usage:
// uint8_t mavlink_satellites_visible = SATELLITES_VISIBLE_TO_UINT8(drone_gps_data.satellites_visible);

/*
 * Description: Represents the magnetic field data for the drone.
 * This structure is designed to capture the magnetic field vector measured by the drone's magnetometer.
 * The data is aligned with the MAVLink HIL_SENSOR message's magnetometer data.
 * 
 * Unit: NanoTesla (nT)
 * The 'data' field is a 3-dimensional vector (glm::dvec3) where:
 *   - data.x: Magnetic field strength along the drone's x-axis.
 *   - data.y: Magnetic field strength along the drone's y-axis.
 *   - data.z: Magnetic field strength along the drone's z-axis.
 *
 * For detailed information on the structure and units, refer to the MAVLink HIL_SENSOR message specifications:
 * https://mavlink.io/en/messages/common.html#HIL_SENSOR
 */
typedef struct {
    glm::dvec3 data;  // Magnetic field vector in nT
} DroneMagDataType;
/*
 * Conversion Macro: Convert nT to G
 * 1 nT = 1e-5 G
 */
#define NT_TO_G(nT) ((nT) * 1e-5)

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

static inline DronePhysCalcCacheType drone_phys_calc_cache(DroneEulerType angle)
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

}
#endif /* _DRONE_DATA_TYPES_HPP_ */