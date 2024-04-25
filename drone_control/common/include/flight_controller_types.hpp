#ifndef _FLIGHT_CONTROLLER_TYPES_HPP_
#define _FLIGHT_CONTROLLER_TYPES_HPP_

#include "pid_control.hpp"

#define ALMOST_EQUAL(target, current, range) ( ( (current) >= ((target) - (range)) ) &&  ( (current) <= ((target) + (range)) ) )

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEGREE2RADIAN(v)    ( (v) * M_PI / (180.0) )
#define RADIAN2DEGREE(v)    ( (180.0 * (v)) / M_PI )
#define RPM2EULER_RATE(v)   ( ((v) * 2 * M_PI) / 60.0 )
#define TWO_PI (2.0 * M_PI)
#define NORMALIZE_RADIAN(x) (fmod((x), TWO_PI))
#define SIMULATION_DELTA_TIME     0.003 // 333.3Hz

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
#define PID_PARAM_MAX_ROLL    30
#define PID_PARAM_MAX_PITCH   30
#define SPD_CONTROL_CYCLE     (SIMULATION_DELTA_TIME * 10) //33.3Hz

/*
 * Radio Control
 */
#define RADIO_CONTROL_MASS       0.1
#define RADIO_CONTROL_GRAVITY    9.81
#define THROTTLE_GAIN            0.3

#define PID_PARM_ROLL_Kp        100.0
#define PID_PARM_ROLL_Ki        0.1
#define PID_PARM_ROLL_Kd        100.0
#define PID_PARM_PITCH_Kp       100.0
#define PID_PARM_PITCH_Ki       0.1
#define PID_PARM_PITCH_Kd       100.0

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