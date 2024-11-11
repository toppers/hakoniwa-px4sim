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
    double total_discharged_value;
    double get_current_charge_value(double discharged_value) 
    {
        if (discharged_value > params.ActualCapacity) {
            discharged_value = params.ActualCapacity;
        }
        double V_initial = params.NominalVoltage;    // 初期電圧（公称電圧） [V]
        double V_final = params.RatedVoltage;        // 放電終了電圧（定格電圧） [V]
        double MaxCapacity = params.ActualCapacity;  // 実容量（実際に使用可能な容量） [Ah]

        // 放電容量に基づく電圧低下の傾きを計算
        double slope = (V_initial - V_final) / (MaxCapacity * 1000.0); // mAhに変換
        double battery_voltage = V_initial - (slope * discharged_value);
        
        return battery_voltage;
    }

public:
    virtual ~BatteryDynamics() {}
    BatteryDynamics()
    {
        this->current_charge_voltage = 0;
    }
    double get_vbat() override 
    {
        double total_discharge_value = 0;
        for (auto* entry : devices) {
            total_discharge_value += entry->get_discharged();
        }
        // Convert total_discharge_value from As (Ampere-seconds) to Ah (Ampere-hours)
        // 1 A・s = 1 / 3600 Ah
        // Therefore: Ah = A・s * (1 / 3600)
        total_discharged_value = total_discharge_value * (1.0 / 3600.0); // Unit conversion from As to Ah    
        this->current_charge_voltage = this->get_current_charge_value(total_discharged_value);
        return this->current_charge_voltage;
    }
    const std::vector<std::string> log_head() override
    {
        return { "timestamp", "DischargedValue", "CurrentVoltage" };
    }

    const std::vector<std::string> log_data() override
    {
        return {std::to_string(CsvLogger::get_time_usec()), std::to_string(total_discharged_value), std::to_string(current_charge_voltage)};
    }
};

}

#endif /* _BATTERY_DYNAMICS_HPP_ */
