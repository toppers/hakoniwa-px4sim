#ifndef _AIRCRAFT_FACTORY_HPP_
#define _AIRCRAFT_FACTORY_HPP_

#include "iaircraft.hpp"
#include "idrone_dynamics.hpp"
#include "isensor_acceleration.hpp"
#include "isensor_baro.hpp"
#include "isensor_gps.hpp"
#include "isensor_gyro.hpp"
#include "isensor_mag.hpp"
#include "irotor_dynamics.hpp"
#include "ithrust_dynamics.hpp"

namespace hako::assets::drone {

extern IAirCraft* create_aircraft(const char* drone_type);

}

using hako::assets::drone::IAirCraft;
using hako::assets::drone::IDroneDynamics;
using hako::assets::drone::ISensorAcceleration;
using hako::assets::drone::ISensorBaro;
using hako::assets::drone::ISensorGps;
using hako::assets::drone::ISensorMag;
using hako::assets::drone::ISensorGyro;
using hako::assets::drone::IRotorDynamics;
using hako::assets::drone::IThrustDynamics;


using hako::assets::drone::DroneThrustType;
using hako::assets::drone::DroneTorqueType;
using hako::assets::drone::DronePositionType;
using hako::assets::drone::DroneEulerType;

#endif /* _AIRCRAFT_FACTORY_HPP_ */