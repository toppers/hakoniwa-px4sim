#ifndef _IAIRCRAFT_HPP_
#define _IAIRCRAFT_HPP_

#include "aircraft/interfaces/idrone_dynamics.hpp"
#include "aircraft/interfaces/irotor_dynamics.hpp"
#include "aircraft/interfaces/ithrust_dynamics.hpp"
#include "aircraft/interfaces/ibattery_dynamics.hpp"
#include "aircraft/interfaces/isensor_acceleration.hpp"
#include "aircraft/interfaces/isensor_baro.hpp"
#include "aircraft/interfaces/isensor_gps.hpp"
#include "aircraft/interfaces/isensor_gyro.hpp"
#include "aircraft/interfaces/isensor_mag.hpp"

namespace hako::aircraft {

class IAirCraft {
protected:
    bool            enable_disturbance = false;
    IDroneDynamics *drone_dynamics = nullptr;
    IRotorDynamics *rotor_dynamics[ROTOR_NUM];
    IThrustDynamics *thrust_dynamis = nullptr;
    IBatteryDynamics *battery_dynamics = nullptr;

    ISensorAcceleration *acc = nullptr;
    ISensorBaro *baro = nullptr;
    ISensorGps *gps = nullptr;
    ISensorGyro *gyro = nullptr;
    ISensorMag *mag = nullptr;
    std::string robo_name;
    int index = 0;
    bool enable_rotor_control = false;
    double radPerSecToRPM(double rad_per_sec) {
        return rad_per_sec * (60.0 / (2 * M_PI));
    }
    uint64_t simulation_time_usec = 0;
    uint64_t delta_time_usec = 0;
public:
    virtual ~IAirCraft() {}
    virtual void run(AircraftInputType& input) = 0;
    virtual void reset() = 0;
    void set_delta_time_usec(uint64_t d_time_usec)
    {
        this->delta_time_usec = d_time_usec;
    }
    uint64_t get_simulation_time_usec()
    {
        return simulation_time_usec;
    }
    void enable_disturb()
    {
        this->enable_disturbance = true;
    }
    bool is_enabled_disturbance()
    {
        return this->enable_disturbance;
    }
    void set_rotor_control_enabled()
    {
        enable_rotor_control = true;
    }

    bool is_rotor_control_enabled()
    {
        return enable_rotor_control;
    }
    void set_name(const std::string& name)
    {
        this->robo_name = name;
    }
    std::string get_name() const
    {
        return this->robo_name;
    }
    void set_index(int _index)
    {
        this->index = _index;
    }
    uint32_t get_index()
    {
        return this->index;
    }
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
    void set_battery_dynamics(IBatteryDynamics *src)
    {
        this->battery_dynamics = src;
    }
    IBatteryDynamics *get_battery_dynamics()
    {
        return this->battery_dynamics;
    }
    double get_rpm_max(int rotor_index)
    {
        if (rotor_index < ROTOR_NUM) {
            return radPerSecToRPM(this->rotor_dynamics[rotor_index]->get_rad_per_sec_max());
        }
        else {
            return -1;
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