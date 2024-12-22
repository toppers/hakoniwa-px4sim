#ifndef _SENSOR_BARO_HPP_
#define _SENSOR_BARO_HPP_

#include "aircraft/interfaces/isensor_baro.hpp"
#include "aircraft/impl/sensors/sensor_baro.hpp"
#include "aircraft/impl/noise/sensor_data_assembler.hpp"
#include "logger/ilog.hpp"
#include "logger/impl/hako_logger.hpp"
#include <iostream>

using namespace hako::logger;
namespace hako::aircraft {

class SensorBaro : public ISensorBaro, public ILog {
private:
    double delta_time_sec;
    double total_time_sec;
    double Pb = 101325.0;  // static pressure at sea level [Pa]
    double Tb = 288.15;    // standard temperature at sea level [K]
    double Lb = -0.0065;   // standard temperature lapse rate [K/m]
    double M = 0.0289644;  // molar mass of Earth's air [kg/mol]
    double G = GRAVITY;    // gravity
    double R = 8.31432;    // universal gas constant

    SensorDataAssembler asm_alt;
    double alt2baro(double alt) {
        if (alt <= 11000.0) {
            return Pb * pow(Tb / (Tb + (Lb * alt)), (G * M) / (R * Lb));
        } else if (alt <= 20000.0) {
            double f = 11000.0;
            double a = alt2baro(f);
            double c = Tb + (f * Lb);
            return a * exp(((-G) * M * (alt - f)) / (R * c));
        }
        return 0.0;
    } 
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
        value.diff_pressure = 0;
        value.pressure_alt = asm_alt.get_calculated_value();
        value.abs_pressure = alt2baro(value.pressure_alt);
        if (this->noise != nullptr) {
            value.abs_pressure = this->noise->add_random_noise(value.abs_pressure);
            value.diff_pressure = this->noise->add_random_noise(value.diff_pressure);
            value.pressure_alt = this->noise->add_random_noise(value.pressure_alt);
        }
        return value;
    }
    const std::vector<LogHeaderType>& log_head() override
    {
        static const std::vector<LogHeaderType> headers = {
            {"timestamp", LOG_TYPE_UINT64}, // timestamp: unsigned 64-bit integer
            {"abs_p", LOG_TYPE_DOUBLE},
            {"diff_p", LOG_TYPE_DOUBLE},
            {"p_alt", LOG_TYPE_DOUBLE}
        };
        return headers;
    }
    const std::vector<LogDataType>& log_data() override
    {
        DroneBarometricPressureType v = sensor_value();
        static std::vector<LogDataType> data;
        data.clear();
        data.push_back(HakoLogger::get_time_usec()); // timestamp (uint64_t)
        data.push_back(v.abs_pressure); 
        data.push_back(v.diff_pressure); 
        data.push_back(v.pressure_alt); 
        return data;
    }
};

}


#endif /* _SENSOR_BARO_HPP_ */