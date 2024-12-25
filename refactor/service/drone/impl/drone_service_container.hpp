#ifndef _DRONE_SERVICE_CONTAINER_HPP_
#define _DRONE_SERVICE_CONTAINER_HPP_

#include "service/drone/idrone_service_container.hpp"
#include "service/drone/impl/drone_service.hpp"
#include "service/drone/idrone_service.hpp"
#include "aircraft/iaircraft_container.hpp"
#include "controller/aircraft_controller_container.hpp"

using namespace hako::aircraft;
using namespace hako::controller;

namespace hako::service::impl {

class DroneServiceContainer : IDroneServiceContainer {
public:
    DroneServiceContainer(IAirCraftContainer& aircraft_container, AircraftControllerContainer& controller_container) {
        if (aircraft_container.getAllAirCrafts().size() != controller_container.getControllers().size()) {
            throw std::runtime_error("aircraft and controller size mismatch");
        }
        for (std::shared_ptr<IAirCraft> aircraft : aircraft_container.getAllAirCrafts()) {
            std::shared_ptr<DroneService> drone_service = std::make_shared<DroneService>(aircraft, controller_container.getControllers()[aircraft->get_index()]);
            drone_services_.push_back(drone_service);
        }
    };
    ~DroneServiceContainer() = default;

    bool startService(uint64_t deltaTimeUsec) override {
        for (auto& drone_service : drone_services_) {
            drone_service->startService(deltaTimeUsec);
        }
        return true;
    }

    void advanceTimeStep(uint32_t index) override {
        if (index >= drone_services_.size()) {
            throw std::runtime_error("advanceTimeStep index out of range");
        }
        drone_services_[index]->advanceTimeStep();
    }
    void advanceTimeStep() override {
        for (auto& drone_service : drone_services_) {
            drone_service->advanceTimeStep();
        }
    }

    void stopService() override {
        for (auto& drone_service : drone_services_) {
            drone_service->stopService();
        }
    }

    void resetService() override {
        for (auto& drone_service : drone_services_) {
            drone_service->resetService();
        }
    }

    uint64_t getSimulationTimeUsec(uint32_t index) override {
        if (index >= drone_services_.size()) {
            throw std::runtime_error("getSimulationTimeUsec index out of range");
        }
        return drone_services_[index]->getSimulationTimeUsec();
    }

    bool write_pdu(uint32_t index, HakoniwaDronePduDataType& pdu) override {
        if (index >= drone_services_.size()) {
            throw std::runtime_error("write_pdu index out of range");
        }
        return drone_services_[index]->write_pdu(pdu);
    }

    bool read_pdu(uint32_t index, HakoniwaDronePduDataType& pdu) override {
        if (index >= drone_services_.size()) {
            throw std::runtime_error("read_pdu index out of range");
        }
        return drone_services_[index]->read_pdu(pdu);
    };
private:
    std::vector<std::shared_ptr<DroneService>> drone_services_;
};
}

#endif /* _DRONE_SERVICE_CONTAINER_HPP_ */