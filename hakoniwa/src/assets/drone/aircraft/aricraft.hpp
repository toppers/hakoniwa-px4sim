#ifndef _AIRCRAFT_HPP_
#define _AIRCRAFT_HPP_

#include "iaircraft.hpp"
#include "utils/csv_logger.hpp"

namespace hako::assets::drone {

class AirCraft : public hako::assets::drone::IAirCraft {
private:
    CsvLogger logger;
public:
    virtual ~AirCraft() 
    {
        logger.close();
    }
    void run(DroneDynamicsInputType& input) override
    {
        //actuators
        if (input.no_use_actuator == false) {
            DroneRotorSpeedType rotor_speed[ROTOR_NUM];
            for (int i = 0; i < ROTOR_NUM; i++) {
                rotor_dynamics[i]->run(input.controls[i]);
                rotor_speed[i] = rotor_dynamics[i]->get_rotor_speed();
            }
            thrust_dynamis->run(rotor_speed);
            input.thrust = thrust_dynamis->get_thrust();
            input.torque = thrust_dynamis->get_torque();
        }
        drone_dynamics->run(input);
        if (input.manual.control) {
            drone_dynamics->set_angle(input.manual.angle);
        }

        //sensors
        acc->run(drone_dynamics->get_vel_body_frame());
        gyro->run(drone_dynamics->get_angular_vel_body_frame(), input.disturbance);
        gps->run(drone_dynamics->get_pos(), drone_dynamics->get_vel());
        mag->run(drone_dynamics->get_angle());
        baro->run(drone_dynamics->get_pos());

        logger.run();
    }
    CsvLogger& get_logger()
    {
        return logger;
    }

};
}

#endif /* _AIRCRAFT_HPP_ */