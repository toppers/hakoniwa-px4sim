#ifndef _FLIGHT_CONTROLLER_TYPES_HPP_
#define _FLIGHT_CONTROLLER_TYPES_HPP_

#include "pid_control.hpp"
#include "frame_types.h"

#define SIMULATION_DELTA_TIME     0.003 // 333.3Hz

/*
 * Position Control
 */
#define PID_PARM_POSX_Kp        1.5
#define PID_PARM_POSX_Ki        0.3
#define PID_PARM_POSX_Kd        2.0
#define PID_PARM_POSY_Kp        1.5
#define PID_PARM_POSY_Ki        0.3
#define PID_PARM_POSY_Kd        2.0
#define PID_PARAM_MAX_SPD       20.0
#define POS_CONTROL_CYCLE      (SIMULATION_DELTA_TIME * 10) //33.3Hz
#define POS_DELTA_VALUE_M       0.5   /* 50cm */
#define POS_VALUE_MAX           100.0 /* 100m */

/*
 * Altitude Control
 */
#define PID_PARM_ALT_Kp        100.0
#define PID_PARM_ALT_Ki        0.1
#define PID_PARM_ALT_Kd        100.0
#define PID_PARAM_MAX_POWER    10.0
#define ALT_CONTROL_CYCLE      (SIMULATION_DELTA_TIME * 10) //33.3Hz
#define ALT_DELTA_VALUE_M       0.1 /* 10cm */
#define ALT_VALUE_MAX           100.0 /* 100m */

/*
 * Speed Control
 */
#define PID_PARAM_V_BASE      10.0
#define PID_PARM_VX_Kp        PID_PARAM_V_BASE
#define PID_PARM_VX_Ki        0.1
#define PID_PARM_VX_Kd        PID_PARAM_V_BASE
#define PID_PARM_VY_Kp        PID_PARAM_V_BASE
#define PID_PARM_VY_Ki        0.1
#define PID_PARM_VY_Kd        PID_PARAM_V_BASE
#define PID_PARAM_MAX_ROLL    20
#define PID_PARAM_MAX_PITCH   20
#define SPD_CONTROL_CYCLE     (SIMULATION_DELTA_TIME * 10) //33.3Hz

/*
 * Radio Control
 */
#define RADIO_CONTROL_MASS       0.1
#define RADIO_CONTROL_GRAVITY    9.81
#define THROTTLE_GAIN            PID_PARAM_MAX_POWER

#define PID_PARM_ANGLE_BASE     40.0
#define PID_PARM_ROLL_Kp        PID_PARM_ANGLE_BASE
#define PID_PARM_ROLL_Ki        0.1
#define PID_PARM_ROLL_Kd        PID_PARM_ANGLE_BASE
#define PID_PARM_PITCH_Kp       PID_PARM_ANGLE_BASE
#define PID_PARM_PITCH_Ki       0.1
#define PID_PARM_PITCH_Kd       PID_PARM_ANGLE_BASE

#define PID_PARM_ROLL_RATE_Kp     0.001
#define PID_PARM_ROLL_RATE_Ki     0.0001
#define PID_PARM_ROLL_RATE_Kd     0.001
#define PID_PARM_PITCH_RATE_Kp    0.001
#define PID_PARM_PITCH_RATE_Ki    0.0001
#define PID_PARM_PITCH_RATE_Kd    0.001
#define PID_PARM_YAW_RATE_Kp      0.01
#define PID_PARM_YAW_RATE_Ki      0.001
#define PID_PARM_YAW_RATE_Kd      0.01

struct PidControlInputType {
    double target;
    double current;
    PidControlInputType() : target(0), current(0) {}
    PidControlInputType(double current_val, double target_val)
        : current(current_val), target(target_val) {}
};

struct FlightControllerInputEulerType
{
    double x;
    double y;
    double z;
    FlightControllerInputEulerType(): x(0), y(0), z(0) {}
    FlightControllerInputEulerType(double e_x, double e_y, double e_z) :
        x(e_x), y(e_y), z(e_z) {}
};
struct FlightControllerInputAngularRateType
{
    double p;
    double q;
    double r;
    FlightControllerInputAngularRateType(): p(0), q(0), r(0) {}
    FlightControllerInputAngularRateType(double _p, double _q, double _r) :
        p(_p), q(_q), r(_r) {}
};
struct FlightControllerInputVelocityType
{
    double u;
    double v;
    double w;
    FlightControllerInputVelocityType(): u(0), v(0), w(0) {}
    FlightControllerInputVelocityType(double _u, double _v, double _w) :
        u(_u), v(_v), w(_w) {}
};
struct FlightControllerInputPositionType
{
    double x;
    double y;
    double z;
    FlightControllerInputPositionType(): x(0), y(0), z(0) {}
    FlightControllerInputPositionType(double _x, double _y, double _z) :
        x(_x), y(_y), z(_z) {}
};

struct FlightControllerOutputType {
    double thrust;
    double torque_x;
    double torque_y;
    double torque_z;
};

static inline double flight_controller_get_limit_value(double input_value, double base_value, double min_value, double max_value)
{
    double limited_value = base_value + std::max(min_value, std::min(max_value, input_value));
    return limited_value;
}


#endif /* _FLIGHT_CONTROLLER_TYPES_HPP_ */