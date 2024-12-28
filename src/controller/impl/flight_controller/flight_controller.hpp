#ifndef _FLIGHT_CONTROLLER_HPP_
#define _FLIGHT_CONTROLLER_HPP_

#include "controller/iaircraft_controller.hpp"
#include "controller/impl/drone_controller.hpp"

using namespace hako::controller;

namespace hako::controller::impl {
class FlightController : public IAircraftController {
public:
    FlightController();
    virtual ~FlightController() {}
    virtual bool is_radio_control() override {
        return false;
    }
    virtual void reset() override;
    virtual mi_aircraft_control_out_t run(mi_aircraft_control_in_t& in) override;
private:
    DroneController ctrl_;
};
} // namespace hako::controller::impl

#endif /* _FLIGHT_CONTROLLER_HPP_ */