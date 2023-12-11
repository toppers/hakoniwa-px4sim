#ifndef _MAVLINK_IO_HPP_
#define _MAVLINK_IO_HPP_

#include "iaircraft.hpp"
#include "hako/pdu/hako_pdu_data.hpp"
#include "config/config.hpp"
#include <iostream>

namespace hako::assets::drone {

class MavlinkIO {
private:
    void build_hil_sensor(IAirCraft& drone, Hako_HakoHilSensor& sensor)
    {
        //TODO 単位変換チェック
        sensor.time_usec = 0;
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
        sensor.yacc = static_cast<float>(NT_TO_G(mag.data.y));
        sensor.zmag = static_cast<float>(NT_TO_G(mag.data.z));

        DroneBarometricPressureType baro = drone.get_baro().sensor_value();
        sensor.abs_pressure = static_cast<float>(baro.abs_pressure);
        sensor.diff_pressure = static_cast<float>(baro.diff_pressure);
        sensor.pressure_alt = static_cast<float>(baro.pressure_alt);

        sensor.temperature = 0;
        sensor.fields_updated = 0x1FFF;  // Bitmask indicating which fields are valid (assuming all fields are updated for simplicity)
        sensor.id = 0;  // Sensor instance ID (use default 0)
    }
    void build_hil_gps(IAirCraft& drone, Hako_HakoHilGps& sensor)
    {
        //TODO 単位変換チェック
        sensor.time_usec = 0;
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
public:
    virtual ~MavlinkIO() {}

    bool read_actuator_data(double controls[hako::assets::drone::ROTOR_NUM])
    {
        Hako_HakoHilActuatorControls hil_actuator_controls;
        if (hako_read_hil_actuator_controls(hil_actuator_controls)) {
            for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
                controls[i] = hil_actuator_controls.controls[i];
            }
            return true;
        }
        return false;
    }

    void write_sensor_data(IAirCraft& drone)
    {
        Hako_HakoHilSensor hil_sensor;
        Hako_HakoHilGps hil_gps;

        build_hil_sensor(drone, hil_sensor);
        build_hil_gps(drone, hil_gps);

        hako_write_hil_sensor(hil_sensor);
        hako_write_hil_gps(hil_gps);
    }
};
}

#endif /* _MAVLINK_IO_HPP_ */