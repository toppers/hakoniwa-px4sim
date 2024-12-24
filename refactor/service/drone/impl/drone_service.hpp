#ifndef _DRONE_SERVICE_HPP_
#define _DRONE_SERVICE_HPP_

#include "service/drone/idrone_service.hpp"
#include "aircraft/iaircraft_container.hpp"
#include "controller/aircraft_controller_container.hpp"

using namespace hako::aircraft;
using namespace hako::controller;

namespace hako::service::impl {

class DroneService {
public:
    DroneService(IAirCraft& aircraft, IAircraftController& controller): aircraft_(aircraft), controller_(controller) {
        simulation_time_usec_ = 0;
        delta_time_usec_ = 0;
    };
    ~DroneService() = default;

    bool startService(uint64_t deltaTimeUsec) {
        delta_time_usec_ = deltaTimeUsec;
        simulation_time_usec_ = 0;
        return true;
    }

    void advanceTimeStep();

    void stopService() {
        // nothing to do
    }

    void resetService() {
        controller_.reset();
        aircraft_.reset();
    }

    uint64_t getSimulationTimeUsec() {
        return simulation_time_usec_;
    }

    bool write_pdu(HakoniwaDronePduDataType& pdu) {
        if (pdu.id >= HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM) {
            throw std::runtime_error("write_pdu id out of range");
        }
        pdu_data[pdu.id] = pdu;
        return true;
    }

    bool read_pdu(HakoniwaDronePduDataType& pdu) {
        if (pdu.id >= HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM) {
            throw std::runtime_error("read_pdu id out of range");
        }
        pdu = pdu_data[pdu.id];
        return true;
    };
private:
    uint64_t simulation_time_usec_ = 0;
    uint64_t delta_time_usec_ = 0;
    IAirCraft& aircraft_;
    IAircraftController& controller_;
    HakoniwaDronePduDataType pdu_data[HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM] = {};
};

class DroneServiceContainer : IDroneService {
public:
    DroneServiceContainer(IAirCraftContainer& aircraft_container, AircraftControllerContainer& controller_container) {
        if (aircraft_container.getAllAirCrafts().size() != controller_container.getControllers().size()) {
            throw std::runtime_error("aircraft and controller size mismatch");
        }
        for (auto& aircraft : aircraft_container.getAllAirCrafts()) {
            drone_services_.push_back(DroneService(*aircraft, controller_container.getControllers()[aircraft->get_index()]));
        }
    };
    ~DroneServiceContainer() = default;

    bool startService(uint64_t deltaTimeUsec) override {
        for (auto& drone_service : drone_services_) {
            drone_service.startService(deltaTimeUsec);
        }
        return true;
    }

    void advanceTimeStep(uint32_t index) override {
        if (index >= drone_services_.size()) {
            throw std::runtime_error("advanceTimeStep index out of range");
        }
        drone_services_[index].advanceTimeStep();
    }

    void stopService() override {
        for (auto& drone_service : drone_services_) {
            drone_service.stopService();
        }
    }

    void resetService() override {
        for (auto& drone_service : drone_services_) {
            drone_service.resetService();
        }
    }

    uint64_t getSimulationTimeUsec(uint32_t index) override {
        if (index >= drone_services_.size()) {
            throw std::runtime_error("getSimulationTimeUsec index out of range");
        }
        return drone_services_[index].getSimulationTimeUsec();
    }

    bool write_pdu(uint32_t index, HakoniwaDronePduDataType& pdu) override {
        if (index >= drone_services_.size()) {
            throw std::runtime_error("write_pdu index out of range");
        }
        return drone_services_[index].write_pdu(pdu);
    }

    bool read_pdu(uint32_t index, HakoniwaDronePduDataType& pdu) override {
        if (index >= drone_services_.size()) {
            throw std::runtime_error("read_pdu index out of range");
        }
        return drone_services_[index].read_pdu(pdu);
    };
private:
    std::vector<DroneService> drone_services_;
};
}

#endif /* _DRONE_SERVICE_HPP_ */