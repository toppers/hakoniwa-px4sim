#ifndef _DRONE_SERVICE_HPP_
#define _DRONE_SERVICE_HPP_

#include "service/drone/idrone_service.hpp"
#include "service/drone/impl/idrone_service_operation.hpp"
#include "service/drone/impl/drone_service_rc.hpp"
#include "service/drone/impl/drone_service_api.hpp"

#include "aircraft/iaircraft_container.hpp"
#include "controller/iaircraft_mixer.hpp"
#include "controller/aircraft_controller_container.hpp"
#include <array>
#include <stdexcept>

using namespace hako::aircraft;
using namespace hako::controller;

namespace hako::service::impl {


class DroneService : public IDroneService {
public:
    DroneService(IAirCraft& aircraft, IAircraftController& controller, IAircraftMixer& mixer)
        : aircraft_(aircraft), controller_(controller), mixer_(mixer) {
        simulation_time_usec_ = 0;
        delta_time_usec_ = 0;
        if (controller_.is_radio_control()) {
            drone_service_operation_ = std::make_unique<DroneServiceRC>();
        }
        else {
            drone_service_operation_ = std::make_unique<DroneServiceAPI>(aircraft_);
        }
    }
    ~DroneService() override = default;

    bool startService(uint64_t deltaTimeUsec) override {
        delta_time_usec_ = deltaTimeUsec;
        simulation_time_usec_ = 0;
        return true;
    }

    void advanceTimeStep() override;

    void stopService() override {
        // Nothing to do
    }

    void resetService() override {
        controller_.reset();
        aircraft_.reset();
    }

    uint64_t getSimulationTimeUsec() override {
        return simulation_time_usec_;
    }

    bool write_pdu(HakoniwaDronePduDataType& pdu) override {
        if (pdu.id >= HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM) {
            throw std::out_of_range("write_pdu id out of range");
        }
        while (pdu_data_[pdu.id].is_busy.exchange(true)) {
            std::this_thread::yield();
        }
        pdu_data_[pdu.id].data.pdu = pdu.pdu;
        pdu_data_[pdu.id].is_dirty.store(true);
        pdu_data_[pdu.id].is_busy.store(false);
        return true;
    }

    bool read_pdu(HakoniwaDronePduDataType& pdu) override {
        if (pdu.id >= HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM) {
            throw std::out_of_range("read_pdu id out of range");
        }
        while (pdu_data_[pdu.id].is_busy.exchange(true)) {
            std::this_thread::yield();
        }
        pdu.pdu = pdu_data_[pdu.id].data.pdu;
        pdu_data_[pdu.id].is_dirty.store(false);
        pdu_data_[pdu.id].is_busy.store(false);
        return true;
    }

private:
    uint64_t simulation_time_usec_ = 0;
    uint64_t delta_time_usec_ = 0;
    IAirCraft& aircraft_;
    IAircraftController& controller_;
    IAircraftMixer& mixer_;
    std::array<HakoniwaDronePduDataControlType, HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM> pdu_data_ = {};
    AircraftInputType aircraft_inputs_ = {};
    mi_aircraft_control_in_t controller_inputs_ = {};
    mi_aircraft_control_out_t controller_outputs_ = {};
    PwmDuty pwm_duty_ = {};
    std::unique_ptr<IDroneServiceOperation> drone_service_operation_;

    void setup_aircraft_inputs();
    void setup_controller_inputs();
};

}

#endif /* _DRONE_SERVICE_HPP_ */
