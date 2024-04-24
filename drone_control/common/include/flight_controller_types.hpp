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

struct FlightControllerOutputType {
    double thrust;
    double torque_x;
    double torque_y;
    double torque_z;
};


#endif /* _FLIGHT_CONTROLLER_TYPES_HPP_ */