#ifndef _ALTITUDE_CONTROLLER_HPP_
#define _ALTITUDE_CONTROLLER_HPP_

#include "flight_controller_types.hpp"
#include "hako_controller_param_loader.hpp"

struct AltitudeControlInputType {
    FlightControllerInputPositionType pos;
    double target_altitude;
    AltitudeControlInputType() : pos() {}
    AltitudeControlInputType(FlightControllerInputPositionType p, double t_a) : pos(p), target_altitude(t_a) {}    
};

struct AltitudeControlPidControlOutputType {
    double throttle_power;
    AltitudeControlPidControlOutputType() : throttle_power(0) {}
    AltitudeControlPidControlOutputType(double throttle_power_val) : throttle_power(throttle_power_val) {}
};

class AltitudeController {
private:
    double delta_time;
    double max_power;
    PidControl altitude_control;
    AltitudeControlPidControlOutputType prev_out = {};
    double simulation_time = 0;

    void initializeFromLoader(HakoControllerParamLoader* loader) {
        if (loader) {
            delta_time = loader->getParameter("SIMULATION_DELTA_TIME");
            alt_control_cycle = loader->getParameter("ALT_CONTROL_CYCLE");
            max_power = loader->getParameter("PID_PARAM_MAX_POWER");
            altitude_control = PidControl(loader->getParameter("PID_PARM_ALT_Kp"), loader->getParameter("PID_PARM_ALT_Ki"), loader->getParameter("PID_PARM_ALT_Kd"), 0, delta_time);
        } else {
            delta_time = SIMULATION_DELTA_TIME;
            alt_control_cycle = ALT_CONTROL_CYCLE;
            max_power = PID_PARAM_MAX_POWER;
            altitude_control = PidControl(PID_PARM_ALT_Kp, PID_PARM_ALT_Ki, PID_PARM_ALT_Kd, 0, delta_time);
        }
    }

public:
    double alt_control_cycle;
    AltitudeController() 
        : delta_time(0), 
          alt_control_cycle(0), 
          max_power(0), 
          altitude_control(0, 0, 0, 0, 0) {
        if (HakoControllerParamLoader::is_exist_envpath()) {
            HakoControllerParamLoader loader;
            loader.loadParameters();
            initializeFromLoader(&loader);
        } else {
            initializeFromLoader(nullptr);
        }
    }

    AltitudeController(const std::string& filename) 
        : delta_time(0), 
          alt_control_cycle(0), 
          max_power(0), 
          altitude_control(0, 0, 0, 0, 0) {
        HakoControllerParamLoader loader(filename);
        loader.loadParameters();
        initializeFromLoader(&loader);
    }

    AltitudeController(double dt, double max_power_val, double kp, double ki, double kd) 
        : delta_time(dt), 
          alt_control_cycle(ALT_CONTROL_CYCLE), 
          max_power(max_power_val), 
          altitude_control(kp, ki, kd, 0, dt) {}

    virtual ~AltitudeController() {}

    AltitudeControlPidControlOutputType run(AltitudeControlInputType& in) {
        AltitudeControlPidControlOutputType out = prev_out;
        if (simulation_time >= alt_control_cycle) {
            simulation_time = 0;
            out.throttle_power = altitude_control.calculate(in.target_altitude, -in.pos.z);
            out.throttle_power = flight_controller_get_limit_value(out.throttle_power, 0, -max_power, max_power);
        }
        simulation_time += delta_time;
        return out;
    }
};

#endif /* _ALTITUDE_CONTROLLER_HPP_ */
