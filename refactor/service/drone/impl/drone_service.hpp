#ifndef _DRONE_SERVICE_HPP_
#define _DRONE_SERVICE_HPP_

#include "service/drone/idrone_service.hpp"
#include "service/drone/impl/idrone_service_operation.hpp"
#include "service/drone/impl/drone_service_rc.hpp"
#include "service/drone/impl/drone_service_api.hpp"

#include "aircraft/iaircraft_container.hpp"
#include "controller/iaircraft_mixer.hpp"
#include "controller/aircraft_controller_container.hpp"
#include "service/iservice_pdu_syncher.hpp"
#include "service/drone/impl/drone_service_pdu_syncher.hpp"
#include <array>
#include <stdexcept>
#include <memory>

using namespace hako::aircraft;
using namespace hako::controller;

namespace hako::service::impl {


class DroneService : public IDroneService {
public:
    DroneService(std::shared_ptr<IAirCraft> aircraft, std::shared_ptr<IAircraftController> controller)
        : aircraft_(aircraft), controller_(controller) {
        simulation_time_usec_ = 0;
        delta_time_usec_ = 0;
        pdu_syncher_ = std::make_shared<DroneServicePduSyncher>();
        if (controller_->is_radio_control()) {
            drone_service_operation_ = std::make_unique<DroneServiceRC>(aircraft_);
        }
        else {
            drone_service_operation_ = std::make_unique<DroneServiceAPI>(aircraft_);
        }
        if (drone_service_operation_ == nullptr) {
            throw std::runtime_error("Failed to create drone service operation");
        }
        drone_service_operation_->setServicePduSyncher(pdu_syncher_);
        reset();
    }
    ~DroneService() override = default;

    bool startService(uint64_t deltaTimeUsec) override {
        delta_time_usec_ = deltaTimeUsec;
        simulation_time_usec_ = 0;
        is_service_available_ = true;
        return true;
    }

    void advanceTimeStep() override;

    void stopService() override {
        // Nothing to do
        is_service_available_ = false;
    }

    void resetService() override {
        controller_->reset();
        aircraft_->reset();
        reset();
        drone_service_operation_->reset();
    }
    bool isServiceAvailable() override {
        return is_service_available_;
    }
    uint64_t getSimulationTimeUsec() override {
        return simulation_time_usec_;
    }

    bool write_pdu(ServicePduDataType& pdu) override {
        return pdu_syncher_->flush(aircraft_->get_index(), pdu);
    }

    bool read_pdu(ServicePduDataType& pdu) override {
        return pdu_syncher_->load(aircraft_->get_index(), pdu);
    }
    void peek_pdu(ServicePduDataType& pdu) override {
        pdu_syncher_->load(aircraft_->get_index(), pdu);
    }
    std::string getRobotName() const {
        return aircraft_->get_name();
    }
    void setPduSyncher(std::shared_ptr<IServicePduSyncher> pdu_syncher) {
        pdu_syncher_ = pdu_syncher;
        drone_service_operation_->setServicePduSyncher(pdu_syncher);
    }
private:
    uint64_t simulation_time_usec_ = 0;
    uint64_t delta_time_usec_ = 0;
    bool is_service_available_ = false;
    std::shared_ptr<IAirCraft> aircraft_;
    std::shared_ptr<IAircraftController> controller_;
    AircraftInputType aircraft_inputs_ = {};
    mi_aircraft_control_in_t controller_inputs_ = {};
    mi_aircraft_control_out_t controller_outputs_ = {};
    PwmDuty pwm_duty_ = {};
    std::unique_ptr<IDroneServiceOperation> drone_service_operation_;
    std::shared_ptr<IServicePduSyncher> pdu_syncher_;

    void reset()
    {
        aircraft_inputs_ = {};
        controller_inputs_ = {};
        controller_outputs_ = {};
        pwm_duty_ = {};

    }

    void setup_aircraft_inputs();
    void setup_controller_inputs();
    void write_back_pdu();
};

}

#endif /* _DRONE_SERVICE_HPP_ */
