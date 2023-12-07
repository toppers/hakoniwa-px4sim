#ifndef _ISENSOR_ACCELERATION_HPP_
#define _ISENSOR_ACCELERATION_HPP_

#include "isensor.hpp"

namespace hako::assets::drone {

class ISensorAcceleration : ISensor {
public:
    virtual ~ISensorAcceleration() {}
    virtual void run(const DroneAngularVelocityBodyFrameType& data) = 0;
    virtual DroneAccelerationBodyFrameType sensor_value() const = 0;
};

}


#endif /* _ISENSOR_ACCELERATION_HPP_ */