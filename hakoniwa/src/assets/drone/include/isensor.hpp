#ifndef _ISENSOR_HPP_
#define _ISENSOR_HPP_

#include "drone_primitive_types.hpp"

#include "isensor_noise.hpp"
#include "isensor_data_assembler.hpp"

namespace hako::assets::drone {

class ISensor {
protected:
    ISensorNoise *noise;
public:
    virtual ~ISensor() {}
    virtual void set_noise(ISensorNoise *n)
    {
        this->noise = n;
    }
    virtual void print() = 0;
};

}

#endif /* _ISENSOR_HPP_ */