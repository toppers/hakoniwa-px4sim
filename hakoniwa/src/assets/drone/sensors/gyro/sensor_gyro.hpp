#ifndef _SENSOR_GYRO_HPP_
#define _SENSOR_GYRO_HPP_


#include "isensor_gyro.hpp"
#include "utils/icsv_log.hpp"
#include "../../utils/sensor_data_assembler.hpp"
#include "utils/csv_logger.hpp"
#include <iostream>

namespace hako::assets::drone {

class SensorGyro : public hako::assets::drone::ISensorGyro, public ICsvLog {
private:
    double delta_time_sec;
    double total_time_sec;
    hako::assets::drone::SensorDataAssembler gyro_x;
    hako::assets::drone::SensorDataAssembler gyro_y;
    hako::assets::drone::SensorDataAssembler gyro_z;
public:
    SensorGyro(double dt, int sample_num) : delta_time_sec(dt), gyro_x(sample_num), gyro_y(sample_num), gyro_z(sample_num) 
    {
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
            value.data.x = this->noise->add_noise(value.data.x);
            value.data.y = this->noise->add_noise(value.data.y);
            value.data.z = this->noise->add_noise(value.data.z);
        }
        return value;
    }
    void print() override
    {
        auto result = sensor_value();
        std::cout << "gyro( "
                    << result.data.x
                    << ", "
                    << result.data.y
                    << ", "
                    << result.data.z
                    << " )" 
                    << std::endl;
    }
    const std::vector<std::string> log_head() override
    {
        return { "TIME", "X", "Y", "Z" };
    }
    const std::vector<std::string> log_data() override
    {
        auto v = sensor_value();

        return {std::to_string(CsvLogger::get_time_usec()), std::to_string(v.data.x), std::to_string(v.data.y), std::to_string(v.data.z)};
    }

};

}


#endif /* _SENSOR_GYRO_HPP_ */