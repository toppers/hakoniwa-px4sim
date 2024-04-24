#ifndef _FLIGHT_CONTROLLER_TYPES_HPP_
#define _FLIGHT_CONTROLLER_TYPES_HPP_

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