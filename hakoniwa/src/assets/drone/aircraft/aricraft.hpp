#ifndef _AIRCRAFT_HPP_
#define _AIRCRAFT_HPP_

#include "iaircraft.hpp"

namespace hako::assets::drone {

class AirCraft : public hako::assets::drone::IAirCraft {

public:
    virtual ~AirCraft() {}
    void run(double controls[ROTOR_NUM]) override
    {
        //actuators
        DroneRotorSpeedType rotor_speed[ROTOR_NUM];
        for (int i = 0; i < ROTOR_NUM; i++) {
            rotor_dynamics[i]->run(controls[i]);
            rotor_speed[i] = rotor_dynamics[i]->get_rotor_speed();
        }
        thrust_dynamis->run(rotor_speed);
        drone_dynamics->run(thrust_dynamis->get_thrust(), thrust_dynamis->get_torque());

        //sensors
        acc->run(drone_dynamics->get_vel_body_frame());
        gyro->run(drone_dynamics->get_angular_vel_body_frame());
        gps->run(drone_dynamics->get_pos(), drone_dynamics->get_vel());
        mag->run(drone_dynamics->get_angle());
        baro->run(drone_dynamics->get_pos());
    }

};
}

#endif /* _AIRCRAFT_HPP_ */