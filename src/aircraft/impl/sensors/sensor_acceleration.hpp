#ifndef _SENSOR_ACCELERATION_HPP_
#define _SENSOR_ACCELERATION_HPP_

#include "aircraft/interfaces/isensor_acceleration.hpp"
#include "aircraft/impl/sensors/sensor_acceleration.hpp"
#include "aircraft/impl/noise/sensor_data_assembler.hpp"
#include "logger/ilog.hpp"
#include "logger/impl/hako_logger.hpp"

using namespace hako::logger;
namespace hako::aircraft {

class SensorAcceleration : public ISensorAcceleration, public ILog {
private:
    double delta_time_sec;
    double total_time_sec;
    SensorDataAssembler acc_x;
    SensorDataAssembler acc_y;
    SensorDataAssembler acc_z;
public:
    SensorAcceleration(double dt, int sample_num) : delta_time_sec(dt), acc_x(sample_num), acc_y(sample_num), acc_z(sample_num) 
    {
        this->noise = nullptr;
        this->has_prev_data = false;
    }
    virtual ~SensorAcceleration() {}
    void run(const DroneVelocityBodyFrameType& data) override
    {
        if (this->has_prev_data) {
            this->acc_x.add_data(data.data.x - this->prev_data.data.x);
            this->acc_y.add_data(data.data.y - this->prev_data.data.y);
            this->acc_z.add_data(data.data.z - this->prev_data.data.z);
        }
        else {
            this->has_prev_data = true;
        }
        this->prev_data = data;
        total_time_sec += delta_time_sec;
    }
    DroneAccelerationBodyFrameType sensor_value() override
    {
        DroneAccelerationBodyFrameType value;
        value.data.x = this->acc_x.get_calculated_value() / this->delta_time_sec;
        value.data.y = this->acc_y.get_calculated_value() / this->delta_time_sec;
        value.data.z = this->acc_z.get_calculated_value() / this->delta_time_sec;
        value.data.z -= GRAVITY;
        if (this->noise != nullptr) {
            value.data.x = this->noise->add_random_noise(value.data.x);
            value.data.y = this->noise->add_random_noise(value.data.y);
            value.data.z = this->noise->add_random_noise(value.data.z);
        }
        return value;
    }

    const std::vector<LogHeaderType>& log_head() override {
        static const std::vector<LogHeaderType> headers = {
            {"timestamp", LOG_TYPE_UINT64}, // timestamp: unsigned 64-bit integer
            {"X", LOG_TYPE_DOUBLE},         // X: double
            {"Y", LOG_TYPE_DOUBLE},         // Y: double
            {"Z", LOG_TYPE_DOUBLE}          // Z: double
        };
        return headers;
    }

    const std::vector<LogDataType>& log_data() override {
        static std::vector<LogDataType> data;
        data.clear();
        DroneAccelerationBodyFrameType v = sensor_value();
        data.push_back(HakoLogger::get_time_usec()); // timestamp (uint64_t)
        data.push_back(v.data.x);                  // X (double)
        data.push_back(v.data.y);                  // Y (double)
        data.push_back(v.data.z);                  // Z (double)
        return data;
    }
};

}


#endif /* _SENSOR_ACCELERATION_HPP_ */