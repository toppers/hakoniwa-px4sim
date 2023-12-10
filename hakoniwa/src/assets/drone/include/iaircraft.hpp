#ifndef _IAIRCRAFT_HPP_
#define _IAIRCRAFT_HPP_

#include "idrone_dynamics.hpp"
#include "irotor_dynamics.hpp"
#include "ithrust_dynamics.hpp"
#include "isensor_acceleration.hpp"
#include "isensor_baro.hpp"
#include "isensor_gps.hpp"
#include "isensor_gyro.hpp"
#include "isensor_mag.hpp"

namespace hako::assets::drone {

class IAirCraft {
protected:
    IDroneDynamics *drone_dynamics;
    IRotorDynamics *rotor_dynamics[ROTOR_NUM];
    IThrustDynamics *thrust_dynamis;

    ISensorAcceleration *acc;
    ISensorBaro *baro;
    ISensorGps *gps;
    ISensorGyro *gyro;
    ISensorMag *mag;
public:
    virtual ~IAirCraft() {}
    virtual void run(double controls[ROTOR_NUM]) = 0;

     void set_drone_dynamics(IDroneDynamics *src)
    {
        this->drone_dynamics = src;
    }
    IDroneDynamics& get_drone_dynamics()
    {
        return *drone_dynamics;
    }
    void set_rotor_dynamics(IRotorDynamics *src[ROTOR_NUM])
    {
        for (int i = 0; i < ROTOR_NUM; i++) {
            this->rotor_dynamics[i] = src[i];
        }
    }
    void set_thrus_dynamics(IThrustDynamics *src)
    {
        this->thrust_dynamis = src;
    }
    void set_acc(ISensorAcceleration *src)
    {
        this->acc = src;
    }
    ISensorAcceleration& get_acc()
    {
        return *acc;
    }
    void set_gps(ISensorGps *src)
    {
        this->gps = src;
    }
    ISensorGps& get_gps()
    {
        return *gps;
    }
    void set_baro(ISensorBaro *src)
    {
        this->baro = src;
    }
    ISensorBaro& get_baro()
    {
        return *baro;
    }
    void set_gyro(ISensorGyro *src)
    {
        this->gyro = src;
    }
    ISensorGyro& get_gyro()
    {
        return *gyro;
    }

    void set_mag(ISensorMag *src)
    {
        this->mag = src;
    }
    ISensorMag& get_mag()
    {
        return *mag;
    }

};
}

#endif /* _IAIRCRAFT_HPP_ */