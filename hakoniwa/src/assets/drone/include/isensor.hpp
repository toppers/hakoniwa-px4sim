#ifndef _ISENSOR_HPP_
#define _ISENSOR_HPP_

#include "drone_primitive_types.hpp"

#include "isensor_noise.hpp"
#include "isensor_data_assembler.hpp"

namespace hako::assets::drone {

class ISensor {
public:
    virtual ~ISensor() {}
};

}

#endif /* _ISENSOR_HPP_ */