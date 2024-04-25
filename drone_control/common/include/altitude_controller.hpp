#ifndef _ALTITUDE_CONTROLLER_HPP_
#define _ALTITUDE_CONTROLLER_HPP_

#include "flight_controller_types.hpp"

struct AltitudeControlInputType {
    FlightControllerInputPositionType pos;
    double target_altitude;
    AltitudeControlInputType() : pos() {}
    AltitudeControlInputType(
        FlightControllerInputPositionType p, double t_a) : 
        pos(p), 
        target_altitude(t_a) {}    
};

struct AltitudeControlPidControlOutputType {
    double throttle_power;
    AltitudeControlPidControlOutputType() : throttle_power(0) {}
    AltitudeControlPidControlOutputType(double throttle_power_val)
        : throttle_power(throttle_power_val) {}
};

class AltitudeController {
private:
    double delta_time;
    double max_power;
    PidControl altitude_control;
    AltitudeControlPidControlOutputType prev_out = {};
    double simulation_time = 0;
public:
    AltitudeController() :
        delta_time(SIMULATION_DELTA_TIME), 
        max_power(PID_PARAM_MAX_POWER), altitude_control(PID_PARM_ALT_Kp, PID_PARM_ALT_Ki, PID_PARM_ALT_Kd, 0, SIMULATION_DELTA_TIME) {}
    AltitudeController(double dt, double max_power_val, double kp, double ki, double kd) :
        delta_time(dt), max_power(max_power_val), altitude_control(kp, ki, kd, 0, dt) {}
    virtual ~AltitudeController() {}

    AltitudeControlPidControlOutputType run(AltitudeControlInputType& in)
    {
        AltitudeControlPidControlOutputType out = prev_out;
        if (simulation_time >= ALT_CONTROL_CYCLE) {
            simulation_time = 0;
            out.throttle_power = altitude_control.calculate(in.target_altitude, -in.pos.z);
            out.throttle_power = flight_controller_get_limit_value(out.throttle_power, 0, -max_power, max_power);
        }
        simulation_time += delta_time;
        return out;
    }
};

#endif /* _ALTITUDE_CONTROLLER_HPP_ */