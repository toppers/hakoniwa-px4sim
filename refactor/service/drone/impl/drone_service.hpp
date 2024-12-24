#ifndef _DRONE_SERVICE_HPP_
#define _DRONE_SERVICE_HPP_

#include "service/drone/idrone_service.hpp"
#include "aircraft/iaircraft_container.hpp"
#include "controller/aircraft_controller_container.hpp"

using namespace hako::aircraft;
using namespace hako::controller;

namespace hako::service::impl {

class DroneService : IDroneService {
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
    AircraftInputType aircraft_inputs_ = {};
    mi_aircraft_control_in_t controller_inputs_ = {};
    mi_aircraft_control_out_t controller_outputs_ = {};

    void setup_aircraft_inputs();
    void setup_controller_inputs();
};

}

#endif /* _DRONE_SERVICE_HPP_ */