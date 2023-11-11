#ifndef _DRONE_PHYS_CONTROL_HPP_
#define _DRONE_PHYS_CONTROL_HPP_

#include "drone_phys.hpp"
#include <iostream>
#define CONST_P    (0.25f)
#define CONST_LP   (0.5f)
#define CONST_K    (0.25f)

static inline void convert2RotationRate(const DroneControlSignalType& signal, const DronePhysType& phys, DronePropellerRotationRateType &out)
{
    out.w[0] =    ( ( CONST_P  / phys.param.p ) * signal.thrust )
                - ( ( CONST_LP / (phys.param.l * phys.param.p )  ) * signal.torque.y )
                + ( ( CONST_K  / (phys.param.k )  ) * signal.torque.z )
                ;
    out.w[1] =    ( ( CONST_P  / phys.param.p ) * signal.thrust )
                - ( ( CONST_LP / (phys.param.l * phys.param.p )  ) * signal.torque.x )
                - ( ( CONST_K  / (phys.param.k )  ) * signal.torque.z )
                ;
    out.w[2] =    ( ( CONST_P  / phys.param.p ) * signal.thrust )
                + ( ( CONST_LP / (phys.param.l * phys.param.p )  ) * signal.torque.y )
                + ( ( CONST_K  / (phys.param.k )  ) * signal.torque.z )
                ;
    out.w[3] =    ( ( CONST_P  / phys.param.p ) * signal.thrust )
                + ( ( CONST_LP / (phys.param.l * phys.param.p )  ) * signal.torque.x )
                - ( ( CONST_K  / (phys.param.k )  ) * signal.torque.z )
                ;
#ifdef ENABLE_DRONE_PHYS_DEBUG
    std::cout << "thrust = " << signal.thrust << std::endl;
    std::cout << "torque.x = " << signal.torque.x << std::endl;
    std::cout << "torque.y = " << signal.torque.y << std::endl;
    std::cout << "torque.z = " << signal.torque.z << std::endl;
#endif
    for (int i = 0; i < DRONE_PROPELLER_NUM; i++) {
#ifdef ENABLE_DRONE_PHYS_DEBUG      
        std::cout << "B: w[" << i << "] = " << out.w[i] << std::endl;
#endif
        if (out.w[i] < 0) {
            std::cout << "ERROR: minus value w[" << i << "] = " << out.w[i] << std::endl;
            out.w[i] = 0;
        }
        out.w[i] = sqrt(out.w[i]);
    }
}

#endif /* _DRONE_PHYS_CONTROL_HPP_ */