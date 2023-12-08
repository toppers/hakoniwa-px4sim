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
    SensorAcceleration() {}
public:
    SensorAcceleration(double dt, int sample_num) : delta_time_sec(dt), acc_x(sample_num), acc_y(sample_num), acc_z(sample_num) {}
    virtual ~ISensorAcceleration() {}
    void run(const DroneAngularVelocityBodyFrameType& data)
    {
        if (this->has_prev_value) {
            this->acc_x.add_data(data.x - this->prev_data.data.x);
            this->acc_y.add_data(data.y - this->prev_data.data.y);
            this->acc_z.add_data(data.z - this->prev_data.data.z);
        }
        else {
            this->acc_x.add_data(data.x);
            this->acc_y.add_data(data.y);
            this->acc_z.add_data(data.z);
            this->has_prev_data = true;
        }
        this->prev_data = data;
    }
    DroneAccelerationBodyFrameType sensor_value()
    {
        DroneAccelerationBodyFrameType value;
        value.x = this->acc_x.get_calculated_value() / this->delta_time_sec;
        value.y = this->acc_y.get_calculated_value() / this->delta_time_sec;
        value.z = this->acc_z.get_calculated_value() / this->delta_time_sec;
        if (this->noise != nullptr) {
            value.x = this->noise->add_noise(value.x);
            value.y = this->noise->add_noise(value.y);
            value.z = this->noise->add_noise(value.z);
        }
        return value;
    }
};

}


#endif /* _SENSOR_ACCELERATION_HPP_ */