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
#include <map>

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
    // バッテリーの放電特性を決める要因データ
    // インデックス番号は、要因IDとして扱う
    std::vector<BatteryModelFactor> discharge_factors;
    //要因IDと放電データのマップ
    std::map<int, std::vector<DischargeData>> battery_model_map;


    // 1段階目：要因データを読み込み、要因IDを確定する関数
    void loadDischargeFactors(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::istringstream ss(line);
            std::string temperatureStr, capacityStr, voltageStr;
            if (std::getline(ss, temperatureStr, ',')) {
                // 要因データを取得
                double temperature = std::stod(temperatureStr);
                
                // 重複を避けるため、既に存在する温度かどうかを確認
                auto it = std::find_if(discharge_factors.begin(), discharge_factors.end(),
                                    [temperature](const BatteryModelFactor& factor) {
                                        return factor.temperature == temperature;
                                    });

                // まだ存在しない場合のみ追加
                if (it == discharge_factors.end()) {
                    BatteryModelFactor factor;
                    factor.temperature = temperature;
                    std::cout << "INFO: battery factor: temperature = " << factor.temperature << std::endl;
                    discharge_factors.push_back(factor); // 要因データを格納
                }
            }
        }
    }
    // 2段階目：要因データに基づいて、要因IDをマップに割り当てる
    void loadDischargeData(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::istringstream ss(line);
            std::string temperatureStr, capacityStr, voltageStr;

            if (std::getline(ss, temperatureStr, ',') &&
                std::getline(ss, capacityStr, ',') &&
                std::getline(ss, voltageStr, ',')) {

                // 要因データから対応する要因IDを取得
                double temperature = std::stod(temperatureStr);
                int factor_id = findFactorOrCreateID(temperature);

                // 放電データを生成
                DischargeData record;
                record.capacity = std::stod(capacityStr);
                record.voltage = std::stod(voltageStr);

                // 要因IDをキーにしてデータをマップに格納
                battery_model_map[factor_id].push_back(record);
            }
        }

        // 各要因IDに対するデータを容量でソート
        for (auto& [factor_id, data] : battery_model_map) {
            std::sort(data.begin(), data.end(), [](const DischargeData& a, const DischargeData& b) {
                return a.capacity < b.capacity;
            });
        }
    }

    // 要因データから要因IDを取得する関数
    int findFactorOrCreateID(double temperature) {
        for (uint i = 0; i < discharge_factors.size(); ++i) {
            if (discharge_factors[i].temperature == temperature) {
                return i; // 要因のインデックスをIDとして使用
            }
        }
        // 該当する要因がない場合、新しい要因を追加しIDを返す
        BatteryModelFactor new_factor{temperature};
        discharge_factors.push_back(new_factor);
        return discharge_factors.size() - 1;
    }
    // current_factorの温度に最も近い要因IDを検索する関数
    int findClosestFactorID() {
        int closest_id = 0;
        double min_difference = std::abs(current_factor.temperature - discharge_factors[0].temperature);

        for (uint i = 1; i < discharge_factors.size(); ++i) {
            double difference = std::abs(current_factor.temperature - discharge_factors[i].temperature);
            if (difference < min_difference) {
                closest_id = i;
                min_difference = difference;
            }
        }
        //std::cout << "INFO: closest factor ID = " << closest_id << " temp: " << discharge_factors[closest_id].temperature << std::endl;
        return closest_id;
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
    std::pair<DischargeData, DischargeData> findSurroundingData(double targetCapacity) 
    {
        int closest_factor_id = findClosestFactorID();
        const auto& data = battery_model_map.at(closest_factor_id);        
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
        auto [lower, upper] = findSurroundingData(discharged_capacity);
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
                // 1段階目：要因データの読み込み
                loadDischargeFactors(params.BatteryModelCsvFilePath);

                // 2段階目：放電データの読み込みとマップ化
                loadDischargeData(params.BatteryModelCsvFilePath);
                this->is_battery_model_enabled = true;
            }
            else {
                std::cout << "BatteryModelCsvFilePath does not exist." << std::endl;
            }
        }
    }


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
