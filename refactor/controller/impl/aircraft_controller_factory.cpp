#include "controller/impl/aircraft_controller_factory.hpp"
#include "controller/impl/radio_controller/radio_controller.hpp"
#include "controller/impl/flight_controller/flight_controller.hpp"

using namespace hako::controller;
using namespace hako::controller::impl;

IAircraftController* create_aircraft_controller(int index, const DroneConfig& drone_config)
{
    std::string module_name = drone_config.getControllerModuleName();
    IAircraftController* controller = nullptr;
    if (module_name == "RadioController") {
        controller = new RadioController();
    }
    else if (module_name == "FlightController") {
        controller = new FlightController();
    }
    else {
        throw std::runtime_error("Unknown controller module name: " + module_name);
    }
    controller->set_index(index);
    return nullptr;
}
