#ifndef _BATTERY_DYNAMICS_HPP_
#define _BATTERY_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"
#include "ibattery_dynamics.hpp"
#include "icurrent_dynamics.hpp"
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
        // 放電容量は増え続けるため、最大容量を超えたら、
        // 最大容量にすることで、電圧レベルを固定値にする
        if (discharged_capacity > params.NominalCapacity) {
            discharged_capacity = params.NominalCapacity;
        }

        double slope = 0;
        double battery_voltage = 0;
        // 放電容量に基づく電圧低下の傾きを計算
        if (discharged_capacity < this->params.CapacityLevelYellow) {
            // 放電容量がyellowレベルを下回っていない場合、電圧を緩やかに低下させる
            slope = (params.NominalVoltage - params.VoltageLevelGreen) / this->params.CapacityLevelYellow;
            battery_voltage = params.NominalVoltage - (slope * discharged_capacity);
        }
        else {
            // 放電容量がyellowレベルを下回ったら、電圧を急降下させる
            slope = (params.VoltageLevelGreen - params.EODVoltage) / (params.NominalCapacity - this->params.CapacityLevelYellow);
            battery_voltage = params.VoltageLevelGreen - (slope * (discharged_capacity - this->params.CapacityLevelYellow));
        }
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
    // TODO グラフをCSVで読み込ませる
    void run() override
    {
        double discharge_capacity_sec = 0;
        this->discharge_current = 0;
        for (auto* entry : devices) {
            entry->discharge_capacity_sec += (entry->device->get_current() * this->delta_time_sec);
            this->discharge_current += entry->device->get_current();
            discharge_capacity_sec += entry->discharge_capacity_sec;
        }
        // Convert total_discharge_value from As (Ampere-seconds) to Ah (Ampere-hours)
        // 1 A・s = 1 / 3600 Ah
        // Therefore: Ah = A・s * (1 / 3600)
        this->discharge_capacity_hour = discharge_capacity_sec /3600.0; // Unit conversion from As to Ah    
        this->current_charge_voltage = this->get_current_charge_voltage(discharge_capacity_hour);
        this->total_discharged_capacity_sec = discharge_capacity_sec;
    }
    double get_vbat() override 
    {
        return this->current_charge_voltage;
    }
    BatteryStatusType get_status() override
    {
        BatteryStatusType status;
        status.full_voltage = this->params.NominalVoltage;
        status.curr_voltage = this->current_charge_voltage;
        status.cycles = 0;
        if (status.curr_voltage > this->params.VoltageLevelGreen) {
            status.status = 0; //green
        }
        else if (status.curr_voltage > this->params.VoltageLevelYellow) {
            status.status = 1; //yellow
        }
        else {
            status.status = 2; //red
        }
        return status;
    }
    const std::vector<std::string> log_head() override
    {
        return { "timestamp", "DischargeCurrent", "Voltage", "DischargeCapacity" };
    }

    const std::vector<std::string> log_data() override
    {
        return {std::to_string(CsvLogger::get_time_usec()), std::to_string(discharge_current), std::to_string(current_charge_voltage), std::to_string(discharge_capacity_hour)};
    }
};

}

#endif /* _BATTERY_DYNAMICS_HPP_ */
