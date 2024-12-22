#ifndef _ISENSOR_HPP_
#define _ISENSOR_HPP_

#include "primitive_types.hpp"

#include "isensor_noise.hpp"
#include "isensor_data_assembler.hpp"

namespace hako::aircraft {

class ISensor {
protected:
    ISensorNoise *noise;
    void *vendor_model;
    void *context;
public:
    virtual ~ISensor() {}
    virtual void set_vendor(void *vendor, void *context)
    {
        this->vendor_model = vendor;
        this->context = context;
    }
    virtual void set_noise(ISensorNoise *n)
    {
        this->noise = n;
    }
};

}

#endif /* _ISENSOR_HPP_ */