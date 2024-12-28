#ifndef _ISENSOR_BARO_HPP_
#define _ISENSOR_BARO_HPP_


#include "isensor.hpp"

namespace hako::assets::drone {

class ISensorBaro : public hako::assets::drone::ISensor {
protected:
    double ref_lat;
    double ref_lon;
    double ref_alt;
public:
    virtual ~ISensorBaro() {}
    virtual void init_pos(double lat_data, double lon_data, double alt_data)
    {
        this->ref_lat = lat_data;
        this->ref_lon = lon_data;
        this->ref_alt = alt_data;
    }
    virtual void run(const DronePositionType& data) = 0;
    virtual DroneBarometricPressureType sensor_value() = 0;
};

}

#endif /* _ISENSOR_BARO_HPP_ */