#include "drone_phys.hpp"
#include <memory.h>
#include <math.h>
#include <iostream>

void drone_init(double delta_t, const DronePhysParamType&param, const DronePhysStateType& initial_value, DronePhysType& phys)
{
    memset(&phys, 0, sizeof(phys));
    phys.initial_value = initial_value;
    phys.param = param;
    phys.delta_t = delta_t;
    phys.current = initial_value;
    return;
}

static void drone_run_x(double u, DronePhysType& phys);
static void drone_run_y(double u, DronePhysType& phys);
static void drone_run_z(double u, DronePhysType& phys);

static void drone_run_rx(const DronePropellerRotationRateType& propeller, DronePhysType& phys);
static void drone_run_ry(const DronePropellerRotationRateType& propeller, DronePhysType& phys);
static void drone_run_rz(const DronePropellerRotationRateType& propeller, DronePhysType& phys);

//#define EXPERIMENTAL_CODE_ENABLE
#ifdef EXPERIMENTAL_CODE_ENABLE
#define fullThrust  KEISU
#define tau 0.1
#define DRAG_Z   0.01f
#define DRAG_X   0.01f
#define DRAG_Y   0.01f
#define DRAG_RX  0.0001f
#define DRAG_RY  0.0001f
#define DRAG_RZ  0.0001f

static double thr_weight = 0.0;
static double getThrust() 
{
    return thr_weight * fullThrust;
}
static void setControl(double control, double dt)
{
    thr_weight += (control - thr_weight) * (1.0 - exp(-dt / tau));
}
#endif


void drone_run(const DronePropellerRotationRateType& propeller, DronePhysType& phys)
{
    double u = \
           phys.param.p * ( propeller.w[0] * propeller.w[0]) \
         + phys.param.p * ( propeller.w[1] * propeller.w[1]) \
         + phys.param.p * ( propeller.w[2] * propeller.w[2]) \
         + phys.param.p * ( propeller.w[3] * propeller.w[3]) \
        ;
#ifdef EXPERIMENTAL_CODE_ENABLE
    setControl(u, phys.delta_t);
    u = getThrust();
#endif
    drone_run_x(u, phys);
    drone_run_y(u, phys);
    drone_run_z(u, phys);

    drone_run_rx(propeller, phys);
    drone_run_ry(propeller, phys);
    drone_run_rz(propeller, phys);

    phys.current = phys.next;
    phys.current_time += phys.delta_t;
    return;
}

static void drone_run_x(double u, DronePhysType& phys)
{
    phys.next.vec.x = ( phys.delta_t /  phys.param.m ) * u 
                * 
                  ( 
                      cos(phys.current.rot.x)
                    * sin(phys.current.rot.y)
                    * cos(phys.current.rot.z)
                    +
                      sin(phys.current.rot.x)
                    * sin(phys.current.rot.z) 
                  ) 
#ifdef EXPERIMENTAL_CODE_ENABLE
                  - (DRAG_X * phys.current.vec.x)
#endif
                  + phys.current.vec.x;

    phys.next.pos.x = (phys.current.vec.x * phys.delta_t) + phys.current.pos.x;
#ifdef ENABLE_DRONE_PHYS_DEBUG    
    std::cout << "next pos.x = " << phys.next.pos.x << std::endl;
    std::cout << "next vec.x = " << phys.next.vec.x << std::endl;
#endif
    return;
}

static void drone_run_y(double u, DronePhysType& phys)
{
    phys.next.vec.y = ( phys.delta_t /  phys.param.m ) * u 
                * 
                  ( 
                      cos(phys.current.rot.x) 
                    * sin(phys.current.rot.y) 
                    * sin(phys.current.rot.z) 
                    - 
                      sin(phys.current.rot.x) 
                    * cos(phys.current.rot.z) 
                  ) 
#ifdef EXPERIMENTAL_CODE_ENABLE
                  - (DRAG_Y * phys.current.vec.y)
#endif
                  + phys.current.vec.y;

    phys.next.pos.y = (phys.current.vec.y * phys.delta_t) + phys.current.pos.y;
#ifdef ENABLE_DRONE_PHYS_DEBUG    
    std::cout << "next pos.y = " << phys.next.pos.y << std::endl;
    std::cout << "next vec.y = " << phys.next.vec.y << std::endl;
#endif
    return;
}
static void drone_run_z(double u, DronePhysType& phys)
{
    phys.next.vec.z = ( phys.delta_t /  phys.param.m ) * u 
                * 
                  ( 
                      cos(phys.current.rot.y) 
                    * cos(phys.current.rot.x) 
                  ) 
#ifdef EXPERIMENTAL_CODE_ENABLE
                  - (DRAG_Z * phys.current.vec.z)
#endif
                  - (phys.param.gravity * phys.delta_t )
                  + phys.current.vec.z;

    phys.next.pos.z = (phys.current.vec.z * phys.delta_t) + phys.current.pos.z;
    /*
     * 境界条件：地面から下には落ちない
     */
    if (phys.next.pos.z < 0) {
      phys.next.pos.z = 0;
      phys.next.vec.z = 0;
    }
    //std::cout << "u = " << u << std::endl;
    //std::cout << "curr rot.y = " << phys.current.rot.y << std::endl;
    //std::cout << "curr rot.x = " << phys.current.rot.x << std::endl;
    //std::cout << "curr rot.y cos= " << cos(phys.current.rot.y) << std::endl;
    //std::cout << "curr rot.x cos= " << cos(phys.current.rot.x) << std::endl;
#ifdef ENABLE_DRONE_PHYS_DEBUG    
    std::cout << "next pos.z = " << phys.next.pos.z << std::endl;
    std::cout << "next vec.z = " << phys.next.vec.z << std::endl;
#endif
    return;
}

static void drone_run_rx(const DronePropellerRotationRateType& propeller, DronePhysType& phys)
{
#ifdef DRONE_PX4_AIRFRAME_ADJUSTMENT_ENABLE
    double torque_phi = + (phys.param.l * phys.param.p * ( propeller.w[0] * propeller.w[0] + propeller.w[3] * propeller.w[3]) / sqrt(2.0f))
                        - (phys.param.l * phys.param.p * ( propeller.w[1] * propeller.w[1] + propeller.w[2] * propeller.w[2]) / sqrt(2.0f));
#else
    double torque_phi = - phys.param.l * phys.param.p * propeller.w[1] * propeller.w[1]
                        + phys.param.l * phys.param.p * propeller.w[3] * propeller.w[3];
#endif
#ifdef EXPERIMENTAL_CODE_ENABLE
    phys.next.rot_vec.x = torque_phi * phys.delta_t + ((1.0 - DRAG_RX) * phys.current.rot_vec.x);
#else
    phys.next.rot_vec.x = torque_phi * phys.delta_t + phys.current.rot_vec.x;
#endif
    phys.next.rot.x     = (phys.current.rot_vec.x * phys.delta_t) + phys.current.rot.x;
#ifdef ENABLE_DRONE_PHYS_DEBUG    
    std::cout << "next rot.x = " << phys.next.rot.x << std::endl;
    std::cout << "next rot_vec.x = " << phys.next.rot_vec.x << std::endl;
#endif
}
static void drone_run_ry(const DronePropellerRotationRateType& propeller, DronePhysType& phys)
{
#ifdef DRONE_PX4_AIRFRAME_ADJUSTMENT_ENABLE
    double torque_theta = - (phys.param.l * phys.param.p * ( propeller.w[0] * propeller.w[0] + propeller.w[1] * propeller.w[1]) / sqrt(2.0f))
                          + (phys.param.l * phys.param.p * ( propeller.w[2] * propeller.w[2] + propeller.w[3] * propeller.w[3]) / sqrt(2.0f));
#else
    double torque_theta = - phys.param.l * phys.param.p * propeller.w[0] * propeller.w[0]
                        + phys.param.l * phys.param.p * propeller.w[2] * propeller.w[2];
#endif
#ifdef EXPERIMENTAL_CODE_ENABLE
    phys.next.rot_vec.y = torque_theta * phys.delta_t + ((1.0 - DRAG_RY) * phys.current.rot_vec.y);
#else
    phys.next.rot_vec.y = torque_theta * phys.delta_t + phys.current.rot_vec.y;
#endif
    phys.next.rot.y     = (phys.current.rot_vec.y * phys.delta_t) + phys.current.rot.y;
#ifdef ENABLE_DRONE_PHYS_DEBUG    
    std::cout << "next rot.y = " << phys.next.rot.y << std::endl;
    std::cout << "next rot_vec.y = " << phys.next.rot_vec.y << std::endl;
#endif
}
static void drone_run_rz(const DronePropellerRotationRateType& propeller, DronePhysType& phys)
{
    double torque_psi   = phys.param.k * propeller.w[0] * propeller.w[0]
                        - phys.param.k * propeller.w[1] * propeller.w[1]
                        + phys.param.k * propeller.w[2] * propeller.w[2]
                        - phys.param.k * propeller.w[3] * propeller.w[3];

#ifdef EXPERIMENTAL_CODE_ENABLE
    phys.next.rot_vec.z = torque_psi * phys.delta_t + ((1.0 - DRAG_RZ) * phys.current.rot_vec.z);
#else
    phys.next.rot_vec.z = torque_psi * phys.delta_t + phys.current.rot_vec.z;
#endif

    phys.next.rot.z     = (phys.current.rot_vec.z * phys.delta_t) + phys.current.rot.z;
#ifdef ENABLE_DRONE_PHYS_DEBUG    
    std::cout << "next rot.z = " << phys.next.rot.z << std::endl;
    std::cout << "next rot_vec.z = " << phys.next.rot_vec.z << std::endl;
#endif
}

