#ifndef _AIRCRAFT_FACTORY_HPP_
#define _AIRCRAFT_FACTORY_HPP_

#include "aircraft/iaircraft.hpp"
#include "aircraft/interfaces/idrone_dynamics.hpp"
#include "aircraft/interfaces/irotor_dynamics.hpp"
#include "aircraft/interfaces/ithrust_dynamics.hpp"
#include "aircraft/interfaces/ibattery_dynamics.hpp"
#include "aircraft/interfaces/isensor_acceleration.hpp"
#include "aircraft/interfaces/isensor_baro.hpp"
#include "aircraft/interfaces/isensor_gps.hpp"
#include "aircraft/interfaces/isensor_gyro.hpp"
#include "aircraft/interfaces/isensor_mag.hpp"
#include "config/drone_config.hpp"
#include <vector>
#include <memory>

namespace hako::aircraft {

extern IAirCraft* create_aircraft(int index, const DroneConfig& drone_config);

}


#endif /* _AIRCRAFT_FACTORY_HPP_ */