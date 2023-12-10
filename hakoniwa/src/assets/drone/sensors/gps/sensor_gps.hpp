#ifndef _SENSOR_GPS_HPP_
#define _SENSOR_GPS_HPP_


#include "isensor_gps.hpp"
#include "../../utils/sensor_data_assembler.hpp"
#include <iostream>

namespace hako::assets::drone {

class SensorGps : public hako::assets::drone::ISensorGps {
private:
    double delta_time_sec;
    hako::assets::drone::SensorDataAssembler asm_lat;
    hako::assets::drone::SensorDataAssembler asm_lon;
    hako::assets::drone::SensorDataAssembler asm_alt;
    hako::assets::drone::SensorDataAssembler asm_vel;
    hako::assets::drone::SensorDataAssembler asm_vn;
    hako::assets::drone::SensorDataAssembler asm_ve;
    hako::assets::drone::SensorDataAssembler asm_vd;
    hako::assets::drone::SensorDataAssembler asm_cog;

    void run_pos(const DronePositionType& p)
    {
        double lat = this->ref_lat + (p.data.x / 111000.0);
        double lon = this->ref_lon + (p.data.y / 111000.0);
        double alt = this->ref_alt + p.data.z;

        this->asm_lat.add_data(lat);
        this->asm_lon.add_data(lon);
        this->asm_alt.add_data(alt);
    }

    void run_velocity(const DroneVelocityType& v)
    {
        double vn = v.data.x;
        double ve = v.data.y;
        double vd = v.data.z;
        double vel = sqrt(vn * vn + ve * ve + vd * vd);
        this->asm_vn.add_data(vn);
        this->asm_ve.add_data(ve);
        this->asm_vd.add_data(vd);
        this->asm_vel.add_data(vel);
    }
    void run_cog(const DroneVelocityType& v)
    {
        double vn = v.data.x;
        double ve = v.data.y;

        if (vn == 0 && ve == 0) {
            this->asm_cog.reset();
            return;
        }
        // 角度を-πからπで範囲で返す
        double angleRadians = atan2(ve, vn);

        // ラジアンから度に変換
        double angleDegrees = angleRadians * (180.0 / M_PI);

        // 角度を0〜360度の範囲に正規化
        if (angleDegrees < 0.0) {
            angleDegrees += 360.0;
        }
        this->asm_cog.add_data(angleDegrees);
    }
public:
    SensorGps(double dt, int sample_num) 
        : delta_time_sec(dt), asm_lat(sample_num), asm_lon(sample_num), asm_alt(sample_num),
            asm_vel(sample_num), asm_vn(sample_num), asm_ve(sample_num), asm_vd(sample_num), asm_cog(sample_num)
    {
        this->noise = nullptr;
    }
    virtual ~SensorGps() {}
    void run(const DronePositionType& p, const DroneVelocityType& v) override
    {
        (void)this->delta_time_sec;
        run_pos(p);
        run_velocity(v);
        run_cog(v);
    }
    DroneGpsDataType sensor_value() override
    {
        DroneGpsDataType value;
        value.lat = this->asm_lat.get_calculated_value();
        value.lon = this->asm_lon.get_calculated_value();
        value.alt = this->asm_alt.get_calculated_value();

        value.vel = this->asm_vel.get_calculated_value();
        value.vn = this->asm_vn.get_calculated_value();
        value.ve = this->asm_ve.get_calculated_value();
        value.vd = this->asm_vd.get_calculated_value();

        if (this->asm_cog.size() > 0) {
            value.cog = this->asm_cog.get_calculated_value();
        }
        else {
            value.cog = -1;
        }
        if (this->noise != nullptr) {
            value.lat= this->noise->add_noise(value.lat);
            value.lon= this->noise->add_noise(value.lon);
            value.alt= this->noise->add_noise(value.alt);

            value.vel = this->noise->add_noise(value.vel);
            value.vn = this->noise->add_noise(value.vn);
            value.ve = this->noise->add_noise(value.ve);
            value.vd = this->noise->add_noise(value.vd);
            if (value.cog >= 0) {
                value.cog = this->noise->add_noise(value.cog);
            }
        }
        value.eph = 10;
        value.epv = 10;
        value.num_satelites_visible = 10;
        return value;
    }
    void print() override
    {
        auto result = sensor_value();
        std::cout << "gps.pos( lat: "
                    << result.lat
                    << ", lon: "
                    << result.lon
                    << ", alt: "
                    << result.alt
                    << " )" 
                    << std::endl;
        std::cout << "gps.velocity( vel: "
                    << result.vel
                    << ", vn: "
                    << result.vn
                    << ", ve: "
                    << result.ve
                    << ", vd: "
                    << result.vd
                    << ", cog: "
                    << result.cog
                    << " )" 
                    << std::endl;
    }

};

}


#endif /* _SENSOR_GPS_HPP_ */