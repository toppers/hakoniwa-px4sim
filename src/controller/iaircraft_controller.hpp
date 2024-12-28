#ifndef _IAIRCRAFT_CONTROLLER_HPP_
#define _IAIRCRAFT_CONTROLLER_HPP_

#include "controller/aircraft_controller_types.h"
#include "controller/iaircraft_mixer.hpp"
#include <memory>

namespace hako::controller {

class IAircraftController {
private:
    std::shared_ptr<IAircraftMixer> mixer_;
    int index_;
public:
    virtual ~IAircraftController() {}
    virtual bool is_radio_control() = 0;
    virtual void reset() = 0;
    virtual void set_index(int index) {
        index_ = index;
    }
    virtual void set_mixer(std::unique_ptr<IAircraftMixer> mixer) {
        mixer_ = std::move(mixer);
    }
    virtual std::shared_ptr<IAircraftMixer> mixer() {
        return mixer_;
    }
    virtual mi_aircraft_control_out_t run(mi_aircraft_control_in_t& in) = 0;
};
}


#endif /* _IAIRCRAFT_CONTROLLER_HPP_ */