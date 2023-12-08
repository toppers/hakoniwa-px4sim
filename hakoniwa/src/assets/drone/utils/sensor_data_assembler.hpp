#ifndef _SENSOR_DATA_ASSEMBLER_HPP_
#define _SENSOR_DATA_ASSEMBLER_HPP_

#include "isensor_data_assembler.hpp"
#include <vector>
#include <numeric> 

namespace hako::assets::drone {

class SensorDataAssembler : public hako::assets::drone::ISensorDataAssembler {
private:
    std::vector<double> data_vector;
    SensorDataAssembler() {}
public:
    SensorDataAssembler(int sample_num) 
    {
        this->set_sample_num(sample_num);
    }
    virtual ~SensorDataAssembler() {}
    void add_data(double data) override
    {
        // 新しいデータをベクターに追加
        data_vector.push_back(data);
        // 必要に応じて、古いデータを削除
        if (data_vector.size() > sample_num) {
            data_vector.erase(data_vector.begin());
        }
    }
    double get_calculated_value() override
    {
        // 平均値を計算
        if (!data_vector.empty()) {
            double sum = std::accumulate(data_vector.begin(), data_vector.end(), 0.0);
            return sum / data_vector.size();
        } else {
            return 0.0;  // データがない場合は0を返却
        }
    }
};

}

#endif /* _SENSOR_DATA_ASSEMBLER_HPP_ */