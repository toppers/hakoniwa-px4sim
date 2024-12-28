#ifndef _IAIRCRAFT_MIXER_HPP_
#define _IAIRCRAFT_MIXER_HPP_

#include "controller/aircraft_controller_types.h"

namespace hako::controller {
struct PwmDuty {
    double d[HAKO_AIRCRAFT_MAX_ROTOR_NUM];
};


class IAircraftMixer {
public:
    virtual ~IAircraftMixer() {}
    virtual PwmDuty run(mi_aircraft_control_out_t& in) = 0;
};

}

#endif /* _IAIRCRAFT_MIXER_HPP_ */