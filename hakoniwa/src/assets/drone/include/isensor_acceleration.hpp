#ifndef _ISENSOR_ACCELERATION_HPP_
#define _ISENSOR_ACCELERATION_HPP_

#include "isensor.hpp"

namespace hako::assets::drone {

class ISensorAcceleration : public hako::assets::drone::ISensor {
protected:
    bool has_prev_data;
    DroneAngularVelocityBodyFrameType prev_data;
    ISensorNoise *noise;
public:
    virtual ~ISensorAcceleration() {}
    virtual void set_noise(ISensorNoise *n)
    {
        this->noise = n;
    }
    virtual void run(const DroneAngularVelocityBodyFrameType& data) = 0;
    virtual DroneAccelerationBodyFrameType sensor_value() const = 0;
};

}


#endif /* _ISENSOR_ACCELERATION_HPP_ */