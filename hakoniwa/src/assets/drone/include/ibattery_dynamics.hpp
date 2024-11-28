#ifndef _IBATTERY_DYNAMICS_HPP_
#define _IBATTERY_DYNAMICS_HPP_

#include "icurrent_dynamics.hpp"
#include "config/drone_config_types.hpp"
#include "utils/hako_utils.hpp"
#include <vector>
#include <iostream>

namespace hako::assets::drone {

// バッテリーの放電特性を決める要因データ
struct BatteryModelFactor {
    double temperature; // 温度 (℃)
};

typedef struct {
    double full_voltage;
    double curr_voltage;
    double temperature;
    uint32_t status;
    uint32_t cycles;
} BatteryStatusType;

typedef struct {
    ICurrentDynamics *device;
    double discharge_capacity_sec;
} DischargeDynamicsType;

class IBatteryDynamics {
protected:
    void *vendor_model;
    void *context;
    BatteryModelParameters params;
    BatteryModelFactor current_factor = {};
    std::vector<DischargeDynamicsType*> devices;
public:
    virtual ~IBatteryDynamics() {}
    virtual void set_vendor(void *vendor, void *context)
    {
        this->vendor_model = vendor;
        this->context = context;
    }
    virtual void set_current_factor(BatteryModelFactor factor)
    {
        current_factor = factor;
    }
    virtual void add_device(ICurrentDynamics& device)
    {
        DischargeDynamicsType* entry = new DischargeDynamicsType();
        HAKO_ASSERT(entry != nullptr);
        entry->device = &device;
        entry->discharge_capacity_sec = 0;
        devices.push_back(entry);
    }
    virtual void reset() = 0;
    virtual double get_vbat() = 0;
    //calculate battery remained value
    virtual void run() = 0;
    virtual BatteryStatusType get_status() = 0;
    virtual void set_params(const BatteryModelParameters &p) = 0;

};
}

#endif /* _IBATTERY_DYNAMICS_HPP_ */