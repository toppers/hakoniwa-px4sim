#ifndef _SENSOR_ACCELERATION_HPP_
#define _SENSOR_ACCELERATION_HPP_


#include "isensor_acceleration.hpp"
#include "../../utils/sensor_data_assembler.hpp"

namespace hako::assets::drone {

class SensorAcceleration : public hako::assets::drone::ISensorAcceleration {
private:
    double delta_time_sec;
    hako::assets::drone::SensorDataAssembler acc_x;
    hako::assets::drone::SensorDataAssembler acc_y;
    hako::assets::drone::SensorDataAssembler acc_z;
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
    }
    DroneAccelerationBodyFrameType sensor_value() override
    {
        DroneAccelerationBodyFrameType value;
        value.data.x = this->acc_x.get_calculated_value() / this->delta_time_sec;
        value.data.y = this->acc_y.get_calculated_value() / this->delta_time_sec;
        value.data.z = this->acc_z.get_calculated_value() / this->delta_time_sec;
        value.data.z -= GRAVITY;
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
        std::cout << "acc( "
                    << result.data.x
                    << ", "
                    << result.data.y
                    << ", "
                    << result.data.z
                    << " )" 
                    << std::endl;
    }
};

}


#endif /* _SENSOR_ACCELERATION_HPP_ */