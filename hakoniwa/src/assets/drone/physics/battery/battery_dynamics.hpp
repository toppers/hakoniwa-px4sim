#ifndef _BATTERY_DYNAMICS_HPP_
#define _BATTERY_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"
#include "ibattery_dynamics.hpp"
#include "icurrent_dynamics.hpp"
#include "utils/icsv_log.hpp"
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>

namespace hako::assets::drone {

struct DischargeData {
    double capacity; // 放電容量 (Ah)
    double voltage;  // 電圧レベル (V)
};

class BatteryDynamics : public hako::assets::drone::IBatteryDynamics, public ICsvLog {
private:
    double current_charge_voltage;
    double accumulated_capacity_sec;
    double discharge_capacity_hour;
    double discharge_current;
    double delta_time_sec;
    bool is_battery_model_enabled;
    std::vector<DischargeData> battery_model_data;

    // CSVファイルを読み込み、データをメモリにロードする関数
    std::vector<DischargeData> loadCSV(const std::string& filename) {
        std::vector<DischargeData> data;
        std::ifstream file(filename);
        std::string line;

        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string capacityStr, voltageStr;
            if (std::getline(ss, capacityStr, ',') && std::getline(ss, voltageStr, ',')) {
                DischargeData record;
                record.capacity = std::stod(capacityStr);
                record.voltage = std::stod(voltageStr);
                data.push_back(record);
            }
        }

        // 放電容量で昇順にソートしておく
        std::sort(data.begin(), data.end(), [](const DischargeData& a, const DischargeData& b) {
            return a.capacity < b.capacity;
        });
        return data;
    }
    void run_discharged_capacity()
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
        this->accumulated_capacity_sec = discharge_capacity_sec;
    }
    // 2分探索で指定した容量に近い前後のデータを取得する関数（境界条件を考慮）
    std::pair<DischargeData, DischargeData> findSurroundingData(const std::vector<DischargeData>& data, double targetCapacity) 
    {
        if (data.size() < 2) {
            throw std::runtime_error("Insufficient data in battery model.");
        }
        auto it = std::lower_bound(data.begin(), data.end(), targetCapacity, [&](const DischargeData& a, double b) {
            return a.capacity < b;
        });
        // 境界条件の処理
        if (it == data.begin()) {
            return { *it, *(it + 1) };
        } else if (it == data.end()) {
            return { *(it - 2), *(it - 1) };
        } else {
            return { *(it - 1), *it };
        }
    }

    // 線形補間で電圧を計算する関数
    double interpolateVoltage(const DischargeData& lower, const DischargeData& upper, double targetCapacity) {
        double slope = (upper.voltage - lower.voltage) / (upper.capacity - lower.capacity);
        return lower.voltage + slope * (targetCapacity - lower.capacity);
    }
    void run_battery_model()
    {
        double discharged_capacity = this->discharge_capacity_hour;
        // 放電容量は増え続けるため、最大容量を超えたら、
        // 最大容量にすることで、電圧レベルを固定値にする
        if (discharged_capacity > params.NominalCapacity) {
            discharged_capacity = params.NominalCapacity;
        }
        // 前後のデータ取得
        auto [lower, upper] = findSurroundingData(this->battery_model_data, discharged_capacity);
        // 線形補間で電圧を計算
        double interpolatedVoltage = interpolateVoltage(lower, upper, discharged_capacity);
        this->current_charge_voltage = interpolatedVoltage;
    }
    void run_linear_model()
    {
        double discharged_capacity = this->discharge_capacity_hour;
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
        this->current_charge_voltage = battery_voltage;
    }
public:
    virtual ~BatteryDynamics() {}
    BatteryDynamics(double dt)
    {
        this->delta_time_sec = dt;
        this->current_charge_voltage = 0;
        this->accumulated_capacity_sec = 0;
        this->discharge_current = 0;
        this->discharge_capacity_hour = 0;
        this->is_battery_model_enabled = false;
    }
    void set_params(const BatteryModelParameters &p) override
    {
        this->params = p;
        if (params.BatteryModelCsvFilePath.empty()) {
            std::cout << "BatteryModelCsvFilePath is empty." << std::endl;
        }
        else {
            std::cout << "BatteryModelCsvFilePath: " << params.BatteryModelCsvFilePath << std::endl;
            if (std::filesystem::exists(params.BatteryModelCsvFilePath)) {
                std::cout << "BatteryModelCsvFilePath exists." << std::endl;
                this->battery_model_data = loadCSV(params.BatteryModelCsvFilePath);
                this->is_battery_model_enabled = true;
            }
            else {
                std::cout << "BatteryModelCsvFilePath does not exist." << std::endl;
            }
        }
    }


    // TODO グラフをCSVで読み込ませる
    void run() override
    {
        run_discharged_capacity();
        if (!this->is_battery_model_enabled) {
            run_linear_model();
        }
        else {
            run_battery_model();
        }
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
