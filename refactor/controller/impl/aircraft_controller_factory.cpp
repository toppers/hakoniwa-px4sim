#include "controller/impl/aircraft_controller_factory.hpp"
#include "controller/impl/radio_controller/radio_controller.hpp"
#include "controller/impl/flight_controller/flight_controller.hpp"
#include "controller/impl/mixer/aircraft_mixer.hpp"

using namespace hako::controller;
using namespace hako::controller::impl;

std::shared_ptr<IAircraftController> create_aircraft_controller(int index, const DroneConfig& drone_config)
{
    std::string module_name = drone_config.getControllerModuleName();
    std::shared_ptr<IAircraftController> controller = nullptr;
    if (module_name == "RadioController") {
        controller = std::make_shared<RadioController>();
    }
    else if (module_name == "FlightController") {
        controller = std::make_shared<FlightController>();
    }
    else {
        throw std::runtime_error("Unknown controller module name: " + module_name);
    }
    if (controller == nullptr) {
        throw std::runtime_error("Failed to create controller: " + module_name);
    }
    controller->set_index(index);

    // mixer
    double param_Ct = drone_config.getCompThrusterParameter("Ct");
    double HoveringRadPerSec = sqrt(drone_config.getCompDroneDynamicsMass() * GRAVITY / (param_Ct * ROTOR_NUM));
    double RadPerSecMax = HoveringRadPerSec * 2.0;
    RotorConfigType rotor_config[ROTOR_NUM];
    std::vector<RotorPosition> pos = drone_config.getCompThrusterRotorPositions();
    if (pos.empty()) {
        throw std::runtime_error("Failed to get rotor positions");
    }
    else if (pos.size() != ROTOR_NUM) {
        throw std::runtime_error("Invalid rotor position size");
    }
    for (size_t i = 0; i < pos.size(); ++i) {
        rotor_config[i].ccw = pos[i].rotationDirection;
        rotor_config[i].data.x = pos[i].position[0];
        rotor_config[i].data.y = pos[i].position[1];
        rotor_config[i].data.z = pos[i].position[2];
    } 
    auto rotor_constants = drone_config.getCompDroneDynamicsRotorDynamicsConstants();
    BatteryModelParameters battery_config = drone_config.getComDroneDynamicsBattery();
    DroneConfig::MixerInfo mixer_info;
    if (drone_config.getControllerMixerInfo(mixer_info)) {
        std::cout << "INFO: mixer is enabled" << std::endl;
        auto mixer = std::make_unique<AircraftMixer>(RadPerSecMax, param_Ct, rotor_constants.Cq, rotor_config);
        if (mixer == nullptr) {
            throw std::runtime_error("Failed to create mixer");
        }
        bool inv_m = mixer->calculate_M_inv();
        if (inv_m == false) {
            throw std::runtime_error("Failed to calculate M_inv");
        }
        mixer_info.K = rotor_constants.K;
        mixer_info.R = rotor_constants.R;
        mixer_info.Cq = rotor_constants.Cq;
        mixer_info.V_bat = battery_config.NominalVoltage;
        mixer->setMixerInfo(mixer_info);
        controller->set_mixer(std::move(mixer));
    }
    else {
        throw std::runtime_error("Failed to get mixer info");
    }
    return controller;
}
