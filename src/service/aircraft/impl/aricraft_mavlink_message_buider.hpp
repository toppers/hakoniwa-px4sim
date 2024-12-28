#ifndef _AIRCRAFT_MAVLINK_MESSAGE_BUILDER_HPP_
#define _AIRCRAFT_MAVLINK_MESSAGE_BUILDER_HPP_

#include "mavlink/mavlink_service.hpp"
#include "aircraft/impl/aircraft_container.hpp"

using namespace hako::aircraft;
using namespace hako::mavlink;

namespace hako::service::impl {
class AircraftMavlinkMessageBuilder {
public:
    AircraftMavlinkMessageBuilder() = default;
    ~AircraftMavlinkMessageBuilder() = default;

    static void build_hil_sensor(IAirCraft& drone, Hako_HakoHilSensor& sensor, uint64_t time_usec)
    {
        sensor.time_usec = time_usec;
        DroneAccelerationBodyFrameType acc = drone.get_acc().sensor_value();
        sensor.xacc = static_cast<float>(acc.data.x);
        sensor.yacc = static_cast<float>(acc.data.y);
        sensor.zacc = static_cast<float>(acc.data.z);

        DroneAngularVelocityBodyFrameType gyro = drone.get_gyro().sensor_value();
        sensor.xgyro = static_cast<float>(gyro.data.x);
        sensor.ygyro = static_cast<float>(gyro.data.y);
        sensor.zgyro = static_cast<float>(gyro.data.z);

        DroneMagDataType mag = drone.get_mag().sensor_value();
        sensor.xmag = static_cast<float>(NT_TO_G(mag.data.x));
        sensor.ymag = static_cast<float>(NT_TO_G(mag.data.y));
        sensor.zmag = static_cast<float>(NT_TO_G(mag.data.z));

        DroneBarometricPressureType baro = drone.get_baro().sensor_value();
        sensor.abs_pressure = static_cast<float>(baro.abs_pressure * 0.01); // Pa to millibar
        sensor.diff_pressure = static_cast<float>(baro.diff_pressure);
        sensor.pressure_alt = static_cast<float>(baro.pressure_alt);

        sensor.temperature = 0;
        sensor.fields_updated = 0x1FFF;  // Bitmask indicating which fields are valid (assuming all fields are updated for simplicity)
        sensor.id = 0;  // Sensor instance ID (use default 0)
    }

    static void build_hil_gps(IAirCraft& drone, Hako_HakoHilGps& sensor, uint64_t time_usec)
    {
        sensor.time_usec = time_usec;
        sensor.fix_type = 3;

        DroneGpsDataType gps = drone.get_gps().sensor_value();
        sensor.lat = LAT_LON_TO_DEGE7(gps.lat);
        sensor.lon = LAT_LON_TO_DEGE7(gps.lon);
        sensor.alt = ALT_TO_MM(gps.alt);

        sensor.eph = DOP_TO_UINT16(gps.eph);
        sensor.epv = DOP_TO_UINT16(gps.epv);

        sensor.vel = VELOCITY_TO_UINT16(gps.vel);
        sensor.vn  = VELOCITY_TO_INT16(gps.vn);
        sensor.ve  = VELOCITY_TO_INT16(gps.ve);
        sensor.vd  = VELOCITY_TO_INT16(gps.vd);
        sensor.cog = COG_TO_UINT16(gps.cog);
        sensor.satellites_visible = SATELLITES_VISIBLE_TO_UINT8(gps.num_satelites_visible);
        sensor.id = 0;
        sensor.yaw = 0;
    }
};
}

#endif /* _AIRCRAFT_MAVLINK_MESSAGE_BUILDER_HPP_ */