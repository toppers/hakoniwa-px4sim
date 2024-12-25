#ifndef _AIRCRAFT_CONTROLLER_CONTAINER_HPP_
#define _AIRCRAFT_CONTROLLER_CONTAINER_HPP_

#include "controller/iaircraft_controller.hpp"
#include <functional>
#include <vector>

namespace hako::controller {
class AircraftControllerContainer {
public:
    AircraftControllerContainer() = default;
    ~AircraftControllerContainer() = default;
    std::vector<std::reference_wrapper<IAircraftController>>& getControllers() {
        return controllers_;
    }
    void createAircraftControllers(DroneConfigManager& configManager) {
        size_t configCount = configManager.getConfigCount();
        for (size_t i = 0; i < configCount; ++i) {
            DroneConfig config;
            if (configManager.getConfig(i, config)) {
                //TODO: create controller
            }
        }
    }
private:
    void addController(IAircraftController& controller) {
        controllers_.push_back(std::ref(controller));
    }
    std::vector<std::reference_wrapper<IAircraftController>> controllers_;};
}

#endif /* _AIRCRAFT_CONTROLLER_CONTAINER_HPP_ */