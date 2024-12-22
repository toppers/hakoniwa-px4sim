#ifndef _SENSOR_GYRO_HPP_
#define _SENSOR_GYRO_HPP_


#include "aircraft/interfaces/isensor_gyro.hpp"
#include "aircraft/impl/sensors/sensor_gyro.hpp"
#include "aircraft/impl/noise/sensor_data_assembler.hpp"
#include "logger/ilog.hpp"
#include "logger/impl/hako_logger.hpp"
#include <iostream>

using namespace hako::logger;

namespace hako::aircraft {

class SensorGyro : public ISensorGyro, public ILog {
private:
    double delta_time_sec;
    double total_time_sec;
    SensorDataAssembler gyro_x;
    SensorDataAssembler gyro_y;
    SensorDataAssembler gyro_z;
public:
    SensorGyro(double dt, int sample_num) : delta_time_sec(dt), gyro_x(sample_num), gyro_y(sample_num), gyro_z(sample_num) 
    {
        this->vendor_model = nullptr;
        this->noise = nullptr;
    }
    virtual ~SensorGyro() {}
    void run(const DroneAngularVelocityBodyFrameType& data) override
    {
        this->gyro_x.add_data(data.data.x);
        this->gyro_y.add_data(data.data.y);
        this->gyro_z.add_data(data.data.z);
        total_time_sec += delta_time_sec;
    }
    DroneAngularVelocityBodyFrameType sensor_value() override
    {
        DroneAngularVelocityBodyFrameType value;
        value.data.x = this->gyro_x.get_calculated_value();
        value.data.y = this->gyro_y.get_calculated_value();
        value.data.z = this->gyro_z.get_calculated_value();
        if (this->noise != nullptr) {
            value.data.x = this->noise->add_random_noise(value.data.x);
            value.data.y = this->noise->add_random_noise(value.data.y);
            value.data.z = this->noise->add_random_noise(value.data.z);
        }
        return value;
    }
    const std::vector<LogHeaderType>& log_head() override
    {
        static const std::vector<LogHeaderType> headers = {
            {"timestamp", LOG_TYPE_UINT64}, // timestamp: unsigned 64-bit integer
            {"X", LOG_TYPE_DOUBLE},         // X: double
            {"Y", LOG_TYPE_DOUBLE},         // Y: double
            {"Z", LOG_TYPE_DOUBLE}          // Z: double
        };
        return headers;
    }
    const std::vector<LogDataType>& log_data() override
    {
        auto v = sensor_value();
        static std::vector<LogDataType> data;
        data.clear();
        data.push_back(HakoLogger::get_time_usec()); // timestamp (uint64_t)
        data.push_back(v.data.x);                  // X (double)
        data.push_back(v.data.y);                  // Y (double)
        data.push_back(v.data.z);                  // Z (double)
        return data;
    }

};

}


#endif /* _SENSOR_GYRO_HPP_ */