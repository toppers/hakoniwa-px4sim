#ifndef _DRONE_PHYS_CONTROL_HPP_
#define _DRONE_PHYS_CONTROL_HPP_

#include "../common/drone_types.hpp"

typedef struct {
    double w[DRONE_PROPELLER_NUM];
} DronePropellerRotationRateType;


typedef struct {
    /*
     * ドローンの質量： 1 kg
     */
    double m;
    /*
     * 重力加速度：9.81 m/s^2
     */
    double gravity;
    /*
     * プロペラ回転中心から機体重心までの距離：0.3m
     */
    double l;
    /*
     * ρ と κ はプロペラの形状等で決まる空力特性に 依存するパラメータ
     */
    double p;
    double k;
} DronePhysParamType;

#define CONST_P    (double)(0.25f)
#define CONST_LP   (double)(0.5f)
#define CONST_K    (double)(0.25f)

static inline void convert2RotationRate(const DroneControlSignalType& signal, const DronePhysParamType& param, DronePropellerRotationRateType &out)
{
    out.w[0] =    ( ( CONST_P  / param.p ) * signal.thrust )
                - ( ( CONST_LP / (param.l * param.p )  ) * signal.torque.y )
                + ( ( CONST_K  / (param.k )  ) * signal.torque.z )
                ;
    out.w[1] =    ( ( CONST_P  / param.p ) * signal.thrust )
                - ( ( CONST_LP / (param.l * param.p )  ) * signal.torque.x )
                - ( ( CONST_K  / (param.k )  ) * signal.torque.z )
                ;
    out.w[2] =    ( ( CONST_P  / param.p ) * signal.thrust )
                + ( ( CONST_LP / (param.l * param.p )  ) * signal.torque.y )
                + ( ( CONST_K  / (param.k )  ) * signal.torque.z )
                ;
    out.w[3] =    ( ( CONST_P  / param.p ) * signal.thrust )
                + ( ( CONST_LP / (param.l * param.p )  ) * signal.torque.x )
                - ( ( CONST_K  / (param.k )  ) * signal.torque.z )
                ;
    for (int i = 0; i < DRONE_PROPELLER_NUM; i++) {
        if (out.w[i] < 0) {
            out.w[i] = 0;
        }
        out.w[i] = sqrt(out.w[i]);
    }
}

#endif /* _DRONE_PHYS_CONTROL_HPP_ */