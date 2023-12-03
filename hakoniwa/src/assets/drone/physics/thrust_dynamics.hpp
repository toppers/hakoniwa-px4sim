#ifndef _THRUST_DYNAMICS_HPP_
#define _THRUST_DYNAMICS_HPP_

#include "drone_primitive_types.hpp"
#include "ithrust_dynamics.hpp"

namespace hako::assets::drone {

class ThrustDynamics : public hako::assets::drone::IThrustDynamics {
private:
    DroneThrustype thrust;
    DroneTorqueType torque;
    void run_thrust(const DroneRotorSpeedType rotor_speed[ROTOR_NUM]);
    void run_torque(const DroneRotorSpeedType rotor_speed[ROTOR_NUM]);
public:
    virtual ~ThrustDynamics() {}

    void set_thrust(DroneThrustype &thrust) override 
    {
        this->thrust = thrust;
    }
    void set_torque(DroneTorqueType &torque) override
    {
        this->torque = torque;
    }

    DroneThrustype get_thrust() const override
    {
        return this->thrust;
    }
    DroneTorqueType get_torque() const override
    {
        return this->torque;
    }

    void run(const DroneRotorSpeedType rotor_speed[ROTOR_NUM]) override
    {
        this->run_thrust(rotor_speed);
        this->run_torque(rotor_speed);
    }
};

}

#endif /* _THRUST_DYNAMICS_HPP_ */
