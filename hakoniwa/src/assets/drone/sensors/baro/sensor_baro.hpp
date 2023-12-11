#ifndef _SENSOR_BARO_HPP_
#define _SENSOR_BARO_HPP_


#include "isensor_baro.hpp"
#include "../../utils/sensor_data_assembler.hpp"
#include "utils/icsv_log.hpp"
#include <iostream>

namespace hako::assets::drone {

class SensorBaro : public hako::assets::drone::ISensorBaro, public ICsvLog {
private:
    double delta_time_sec;
    double total_time_sec;

    hako::assets::drone::SensorDataAssembler asm_alt;
public:
    SensorBaro(double dt, int sample_num) : delta_time_sec(dt), asm_alt(sample_num)
    {
        this->noise = nullptr;
    }
    virtual ~SensorBaro() {}
    void run(const DronePositionType& data) override
    {
        asm_alt.add_data(ref_alt - data.data.z);
        total_time_sec += delta_time_sec;
    }
    DroneBarometricPressureType sensor_value() override
    {
        DroneBarometricPressureType value;
        value.abs_pressure = 10;
        value.diff_pressure = 0;
        value.pressure_alt = asm_alt.get_calculated_value();
        if (this->noise != nullptr) {
            value.abs_pressure = this->noise->add_noise(value.abs_pressure);
            value.diff_pressure = this->noise->add_noise(value.diff_pressure);
            value.pressure_alt = this->noise->add_noise(value.pressure_alt);
        }
        return value;
    }
    void print() override
    {
        auto result = sensor_value();
        std::cout << "baro( abs_p: "
                    << result.abs_pressure
                    << ", diff_p: "
                    << result.diff_pressure
                    << ", p_alt: "
                    << result.pressure_alt
                    << " )" 
                    << std::endl;
    }
    const std::vector<std::string> log_head() override
    {
        return { "TIME", "abs_p", "diff_p", "p_alt" };
    }
    const std::vector<std::string> log_data() override
    {
        DroneBarometricPressureType v = sensor_value();

        return {std::to_string(total_time_sec), std::to_string(v.abs_pressure), std::to_string(v.diff_pressure), std::to_string(v.pressure_alt)};
    }

};

}


#endif /* _SENSOR_BARO_HPP_ */