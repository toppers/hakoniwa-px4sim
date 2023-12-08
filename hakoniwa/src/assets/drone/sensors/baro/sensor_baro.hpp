#ifndef _SENSOR_BARO_HPP_
#define _SENSOR_BARO_HPP_


#include "isensor_baro.hpp"
#include "../../utils/sensor_data_assembler.hpp"

namespace hako::assets::drone {

class SensorBaro : public hako::assets::drone::ISensorBaro {
private:
    double delta_time_sec;

public:
    SensorBaro(double dt, int sample_num) : delta_time_sec(dt)
    {
        (void)sample_num;
        this->noise = nullptr;
    }
    virtual ~SensorBaro() {}
    void run(const DronePositionType& data) override
    {
        (void)data;
        (void)this->delta_time_sec;
        //nop
    }
    DroneBarometricPressureType sensor_value() override
    {
        DroneBarometricPressureType value;
        value.abs_pressure = 10;
        value.diff_pressure = 0;
        value.pressure_alt = 0;
        if (this->noise != nullptr) {
            value.abs_pressure = this->noise->add_noise(value.abs_pressure);
            value.diff_pressure = this->noise->add_noise(value.diff_pressure);
            value.pressure_alt = this->noise->add_noise(value.pressure_alt);
        }
        return value;
    }
};

}


#endif /* _SENSOR_BARO_HPP_ */