#ifndef _AIRCRAFT_CONTROLLER_CONTAINER_HPP_
#define _AIRCRAFT_CONTROLLER_CONTAINER_HPP_

#include "controller/iaircraft_controller.hpp"
#include "controller/impl/aircraft_controller_factory.hpp"
#include <functional>
#include <vector>

namespace hako::controller {
class AircraftControllerContainer {
public:
    AircraftControllerContainer() = default;
    ~AircraftControllerContainer() = default;
    std::vector<std::shared_ptr<IAircraftController>>& getControllers() {
        return controllers_;
    }
    void createAircraftControllers(DroneConfigManager& configManager) {
        size_t configCount = configManager.getConfigCount();
        for (size_t i = 0; i < configCount; ++i) {
            DroneConfig config;
            if (configManager.getConfig(i, config)) {
                auto controller = create_aircraft_controller(i, config);
                if (controller == nullptr) {
                    throw std::runtime_error("Failed to create controller");
                }
                addController(controller);
            }
        }
    }
private:
    void addController(std::shared_ptr<IAircraftController> controller) {
        controllers_.push_back(std::move(controller));
    }
    std::vector<std::shared_ptr<IAircraftController>> controllers_;};
}

#endif /* _AIRCRAFT_CONTROLLER_CONTAINER_HPP_ */