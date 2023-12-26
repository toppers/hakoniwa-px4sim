#ifndef _SENSOR_BARO_HPP_
#define _SENSOR_BARO_HPP_


#include "isensor_baro.hpp"
#include "../../utils/sensor_data_assembler.hpp"
#include "utils/icsv_log.hpp"
#include "utils/csv_logger.hpp"
#include <iostream>

namespace hako::assets::drone {

class SensorBaro : public hako::assets::drone::ISensorBaro, public ICsvLog {
private:
    double delta_time_sec;
    double total_time_sec;
    double Pb = 101325.0;  // static pressure at sea level [Pa]
    double Tb = 288.15;    // standard temperature at sea level [K]
    double Lb = -0.0065;   // standard temperature lapse rate [K/m]
    double M = 0.0289644;  // molar mass of Earth's air [kg/mol]
    double G = GRAVITY;    // gravity
    double R = 8.31432;    // universal gas constant

    hako::assets::drone::SensorDataAssembler asm_alt;
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

        return {std::to_string(CsvLogger::get_time_usec()), std::to_string(v.abs_pressure), std::to_string(v.diff_pressure), std::to_string(v.pressure_alt)};
    }

};

}


#endif /* _SENSOR_BARO_HPP_ */