#ifndef _AIRCRAFT_CONTROLLER_FACTORY_HPP_
#define _AIRCRAFT_CONTROLLER_FACTORY_HPP_

#include "config/drone_config.hpp"
#include "controller/iaircraft_controller.hpp"
#include <memory>

using namespace hako::config;

namespace hako::controller {

extern std::shared_ptr<IAircraftController> create_aircraft_controller(int index, const DroneConfig& drone_config);

}

#endif /* _AIRCRAFT_CONTROLLER_FACTORY_HPP_ */