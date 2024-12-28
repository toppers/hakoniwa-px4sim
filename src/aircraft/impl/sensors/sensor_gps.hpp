#ifndef _SENSOR_GPS_HPP_
#define _SENSOR_GPS_HPP_


#include "aircraft/interfaces/isensor_gps.hpp"
#include "aircraft/impl/sensors/sensor_gps.hpp"
#include "aircraft/impl/noise/sensor_data_assembler.hpp"
#include "logger/ilog.hpp"
#include "logger/impl/hako_logger.hpp"
#include <iostream>

using namespace hako::logger;

namespace hako::aircraft {

class SensorGps : public ISensorGps, public ILog {
private:
    double delta_time_sec;
    double total_time_sec;
    SensorDataAssembler asm_lat;
    SensorDataAssembler asm_lon;
    SensorDataAssembler asm_alt;
    SensorDataAssembler asm_vel;
    SensorDataAssembler asm_vn;
    SensorDataAssembler asm_ve;
    SensorDataAssembler asm_vd;
    SensorDataAssembler asm_cog;

    void run_pos(const DronePositionType& p)
    {
        double lat = this->ref_lat + (p.data.x / 111000.0);
        double lon = this->ref_lon + (p.data.y / 111000.0);
        //高度はプラス
        double alt = this->ref_alt - p.data.z;

        this->asm_lat.add_data(lat);
        this->asm_lon.add_data(lon);
        this->asm_alt.add_data(alt);
    }

    void run_velocity(const DroneVelocityType& v)
    {
        double vn = v.data.x;
        double ve = v.data.y;
        double vd = v.data.z;
        double vel = sqrt(vn * vn + ve * ve /* +  vd * vd */);
        this->asm_vn.add_data(vn);
        this->asm_ve.add_data(ve);
        this->asm_vd.add_data(vd);
        this->asm_vel.add_data(vel);
    }
    void run_cog(const DroneVelocityType& v)
    {
        double vn = v.data.x;
        double ve = v.data.y;

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
        run_pos(p);
        run_velocity(v);
        run_cog(v);
        total_time_sec += delta_time_sec;
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
            value.lat= this->noise->add_random_noise(value.lat);
            value.lon= this->noise->add_random_noise(value.lon);
            value.alt= this->noise->add_random_noise(value.alt);

            value.vel = this->noise->add_random_noise(value.vel);
            value.vn = this->noise->add_random_noise(value.vn);
            value.ve = this->noise->add_random_noise(value.ve);
            value.vd = this->noise->add_random_noise(value.vd);
            if (value.cog >= 0) {
                value.cog = this->noise->add_random_noise(value.cog);
            }
        }
        value.eph = 10;
        value.epv = 10;
        value.num_satelites_visible = 10;
        return value;
    }
    const std::vector<LogHeaderType>& log_head() override
    {
        static const std::vector<LogHeaderType> headers = {
            {"timestamp", LOG_TYPE_UINT64}, // timestamp: unsigned 64-bit integer
            {"lat", LOG_TYPE_DOUBLE},
            {"lon", LOG_TYPE_DOUBLE},
            {"alt", LOG_TYPE_DOUBLE},
            {"vel", LOG_TYPE_DOUBLE},
            {"vn", LOG_TYPE_DOUBLE},
            {"ve", LOG_TYPE_DOUBLE},
            {"vd", LOG_TYPE_DOUBLE},
            {"cog", LOG_TYPE_DOUBLE}
        };
        return headers;
    }
    const std::vector<LogDataType>& log_data() override
    {
        DroneGpsDataType v = sensor_value();
        static std::vector<LogDataType> data;
        data.clear();
        data.push_back(HakoLogger::get_time_usec()); // timestamp (uint64_t)
        data.push_back(v.lat); 
        data.push_back(v.lon); 
        data.push_back(v.alt); 
        data.push_back(v.vel); 
        data.push_back(v.vn); 
        data.push_back(v.ve); 
        data.push_back(v.vd); 
        data.push_back(v.cog); 
        return data;
    }

};

}


#endif /* _SENSOR_GPS_HPP_ */