#ifndef _IDISCHARGE_DYNAMICS_HPP_
#define _IDISCHARGE_DYNAMICS_HPP_


namespace hako::assets::drone {

class IDischargeDynamics {
public:
    virtual ~IDischargeDynamics() {};
    
    // return value Unit: [As]
    virtual double get_discharged_capacity() = 0;
    // return value Unit: [A]
    virtual double get_current() = 0;
};
}

#endif /* _IDISCHARGE_DYNAMICS_HPP_ */
