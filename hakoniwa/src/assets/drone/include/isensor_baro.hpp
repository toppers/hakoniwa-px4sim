#ifndef _ISENSOR_BARO_HPP_
#define _ISENSOR_BARO_HPP_


#include "isensor.hpp"

namespace hako::assets::drone {

class ISensorBaro : public hako::assets::drone::ISensor {
protected:
public:
    virtual ~ISensorBaro() {}
    virtual void run(const DronePositionType& data) = 0;
    virtual DroneBarometricPressureType sensor_value() = 0;
};

}

#endif /* _ISENSOR_BARO_HPP_ */