#ifndef _ICURRENT_DYNAMICS_HPP_
#define _ICURRENT_DYNAMICS_HPP_


namespace hako::aircraft {

class ICurrentDynamics {
public:
    virtual ~ICurrentDynamics() {};
    
    // return value Unit: [A]
    virtual double get_current() = 0;
};
}

#endif /* _ICURRENT_DYNAMICS_HPP_ */