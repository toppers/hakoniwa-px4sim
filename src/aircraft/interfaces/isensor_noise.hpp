#ifndef _ISENSOR_NOISE_HPP_
#define _ISENSOR_NOISE_HPP_

#include "aircraft/interfaces/primitive_types.hpp"

namespace hako::aircraft {

class ISensorNoise {
public:
    virtual ~ISensorNoise() {}
    virtual double add_random_noise(double data) = 0;
};

}


#endif /* _ISENSOR_NOISE_HPP_ */