#ifndef _SENSOR_DATA_ASSEMBLER_HPP_
#define _SENSOR_DATA_ASSEMBLER_HPP_

#include "aircraft/interfaces/isensor_data_assembler.hpp"
#include <vector>
#include <numeric> 

namespace hako::aircraft {

class SensorDataAssembler : public ISensorDataAssembler {
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
        data_vector.push_back(data);
        if (data_vector.size() > (unsigned long)sample_num) {
            data_vector.erase(data_vector.begin());
        }
    }
    double get_calculated_value() override
    {
        if (!data_vector.empty()) {
            double sum = std::accumulate(data_vector.begin(), data_vector.end(), 0.0);
            return sum / (double)data_vector.size();
        } else {
            return 0.0;
        }
    }
    void reset() override
    {
        data_vector.clear();
    }
    int size() override
    {
        return data_vector.size();
    }
};

}

#endif /* _SENSOR_DATA_ASSEMBLER_HPP_ */