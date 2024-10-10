#ifndef _FLIGHT_CONTROLLER_TYPES_HPP_
#define _FLIGHT_CONTROLLER_TYPES_HPP_

#include <cmath>
#include "frame_types.h"

#define ENV_HAKO_CONTROLLER_PARAM_FILE "HAKO_CONTROLLER_PARAM_FILE"

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