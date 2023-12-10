#ifndef _MAVLINK_IO_HPP_
#define _MAVLINK_IO_HPP_

#include "iaircraft.hpp"
#include "hako/pdu/hako_pdu_data.hpp"

namespace hako::assets::drone {

class MavlinkIO {
private:
    void build_hil_sensor(IAirCraft& drone, Hako_HakoHilSensor& sensor)
    {
        //TODO 単位変換チェック
        sensor.time_usec = 0;
        auto acc = drone.get_acc().sensor_value();
        sensor.xacc = acc.data.x;
        sensor.yacc = acc.data.y;
        sensor.zacc = acc.data.z;

        auto gyro = drone.get_gyro().sensor_value();
        sensor.xgyro = gyro.data.x;
        sensor.ygyro = gyro.data.y;
        sensor.zgyro = gyro.data.z;

        auto mag = drone.get_mag().sensor_value();
        sensor.xmag = mag.data.x;
        sensor.yacc = mag.data.y;
        sensor.zmag = mag.data.z;

        auto baro = drone.get_baro().sensor_value();
        sensor.abs_pressure = baro.abs_pressure;
        sensor.diff_pressure = baro.diff_pressure;
        sensor.pressure_alt = baro.pressure_alt;

        sensor.temperature = 0;
        sensor.fields_updated = 0x1FFF;  // Bitmask indicating which fields are valid (assuming all fields are updated for simplicity)
        sensor.id = 0;  // Sensor instance ID (use default 0)
    }
    void build_hil_gps(IAirCraft& drone, Hako_HakoHilGps& sensor)
    {
        //TODO 単位変換チェック
        sensor.time_usec = 0;
        sensor.fix_type = 3;

        auto gps = drone.get_gps().sensor_value();
        sensor.lat = gps.lat;
        sensor.lon = gps.lon;
        sensor.alt = gps.alt;

        sensor.eph = gps.eph;
        sensor.epv = gps.epv;

        sensor.vel = gps.vel;
        sensor.vn  = gps.vn;
        sensor.ve  = gps.ve;
        sensor.vd  = gps.vd;
        sensor.cog = gps.cog;
        sensor.satellites_visible = gps.num_satelites_visible;

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