#ifndef _BATTERY_DYNAMICS_HPP_
#define _BATTERY_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"
#include "ibattery_dynamics.hpp"
#include "idischarge_dynamics.hpp"
#include "utils/icsv_log.hpp"
#include <math.h>

namespace hako::assets::drone {

class BatteryDynamics : public hako::assets::drone::IBatteryDynamics, public ICsvLog {
private:
    double current_charge_voltage;
    double total_discharged_capacity_sec;
    double discharge_capacity_hour;
    double discharge_current;
    double delta_time_sec;
    
    double get_current_charge_voltage(double discharged_capacity) 
    {
        if (discharged_capacity > params.ActualCapacity) {
            discharged_capacity = params.ActualCapacity;
        }
        double V_initial = params.NominalVoltage;    // 初期電圧（公称電圧） [V]
        double V_final = params.RatedVoltage;        // 放電終了電圧（定格電圧） [V]
        double MaxCapacity = params.ActualCapacity;  // 実容量（実際に使用可能な容量） [Ah]

        // 放電容量に基づく電圧低下の傾きを計算
        double slope = (V_initial - V_final) / (MaxCapacity);
        double battery_voltage = V_initial - (slope * discharged_capacity);
        
        return battery_voltage;
    }

public:
    virtual ~BatteryDynamics() {}
    BatteryDynamics(double dt)
    {
        this->delta_time_sec = dt;
        this->current_charge_voltage = 0;
        this->total_discharged_capacity_sec = 0;
        this->discharge_current = 0;
        this->discharge_capacity_hour = 0;
    }
    double get_vbat() override 
    {
        double discharge_capacity_sec = 0;
        this->discharge_current = 0;
        for (auto* entry : devices) {
            discharge_capacity_sec += entry->get_discharged_capacity();
            this->discharge_current += entry->get_current();
        }
        // Convert total_discharge_value from As (Ampere-seconds) to Ah (Ampere-hours)
        // 1 A・s = 1 / 3600 Ah
        // Therefore: Ah = A・s * (1 / 3600)
        this->discharge_capacity_hour = discharge_capacity_sec /3600.0; // Unit conversion from As to Ah    
        this->current_charge_voltage = this->get_current_charge_voltage(discharge_capacity_hour);

        this->total_discharged_capacity_sec = discharge_capacity_sec;
        return this->current_charge_voltage;
    }
    const std::vector<std::string> log_head() override
    {
        return { "timestamp", "DischargeCurrent", "CurrentVoltage", "DischargeCapacity" };
    }

    const std::vector<std::string> log_data() override
    {
        return {std::to_string(CsvLogger::get_time_usec()), std::to_string(discharge_current), std::to_string(current_charge_voltage), std::to_string(discharge_capacity_hour)};
    }
};

}

#endif /* _BATTERY_DYNAMICS_HPP_ */
