#ifndef _ISENSOR_GYRO_HPP_
#define _ISENSOR_GYRO_HPP_


#include "isensor.hpp"

namespace hako::assets::drone {

class ISensorGyro : public hako::assets::drone::ISensor {
public:
    virtual ~ISensorGyro() {}
    virtual void run(const DroneAngularVelocityBodyFrameType& data) = 0;
    virtual void run(const DroneAngularVelocityBodyFrameType& data, DroneDynamicsDisturbanceType& disturbance) = 0;
    virtual DroneAngularVelocityBodyFrameType sensor_value() = 0;
};

}

#endif /* _ISENSOR_GYRO_HPP_ */