#ifndef _ISENSOR_ACCELERATION_HPP_
#define _ISENSOR_ACCELERATION_HPP_

#include "isensor.hpp"

namespace hako::assets::drone {

class ISensorAcceleration : public hako::assets::drone::ISensor {
protected:
    bool has_prev_data;
    DroneVelocityBodyFrameType prev_data;
    ISensorNoise *noise;
public:
    virtual ~ISensorAcceleration() {}
    virtual void set_noise(ISensorNoise *n)
    {
        this->noise = n;
    }
    virtual void run(const DroneVelocityBodyFrameType& data) = 0;
    virtual DroneAccelerationBodyFrameType sensor_value() = 0;
};

}


#endif /* _ISENSOR_ACCELERATION_HPP_ */