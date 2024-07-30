#ifndef _HEADING_CONTROLLER_HPP_
#define _HEADING_CONTROLLER_HPP_

#include "flight_controller_types.hpp"
#include "hako_controller_param_loader.hpp"

struct HeadingControlInputType {
    FlightControllerInputEulerType euler;
    double target_angle_deg;
    HeadingControlInputType() : target_angle_deg(0) {}
    HeadingControlInputType(FlightControllerInputEulerType e, double t_e) : euler(e), target_angle_deg(t_e) {}    
};

struct HeadingControlPidControlOutputType {
    double angular_rate_r;
    HeadingControlPidControlOutputType() : angular_rate_r(0) {}
    HeadingControlPidControlOutputType(double angular_rate_r_val) : angular_rate_r(angular_rate_r_val) {}
};

class HeadingController {
private:
    double delta_time;
    double pid_param_max_rrate;
    PidControl heading_control;
    HeadingControlPidControlOutputType prev_out = {};
    double simulation_time = 0;

    void initializeFromLoader(HakoControllerParamLoader* loader) {
        if (loader) {
            delta_time = loader->getParameter("SIMULATION_DELTA_TIME");
            head_control_cycle = loader->getParameter("HEAD_CONTROL_CYCLE");
            pid_param_max_rrate = loader->getParameter("PID_PARAM_MAX_RRATE");
            heading_control = PidControl(loader->getParameter("PID_PARM_HEAD_Kp"), loader->getParameter("PID_PARM_HEAD_Ki"), loader->getParameter("PID_PARM_HEAD_Kd"), 0, delta_time);
        } else {
            delta_time = SIMULATION_DELTA_TIME;
            head_control_cycle = HEAD_CONTROL_CYCLE;
            pid_param_max_rrate = PID_PARAM_MAX_RRATE;
            heading_control = PidControl(PID_PARM_HEAD_Kp, PID_PARM_HEAD_Ki, PID_PARM_HEAD_Kd, 0, delta_time);
        }
    }

    double normalize_angle(double angle) {
        // Normalize angle to be within the range [0, 360)
        angle = std::fmod(angle, 360.0);
        if (angle < 0) {
            angle += 360.0;
        }
        return angle;
    }

    double shortest_angle(double current, double target) {
        double diff = normalize_angle(target - current);
        if (diff > 180.0) {
            diff -= 360.0;
        } else if (diff < -180.0) {
            diff += 360.0;
        }
        return diff;
    }

public:
    double head_control_cycle;
    HeadingController()
        : delta_time(SIMULATION_DELTA_TIME), 
          head_control_cycle(HEAD_CONTROL_CYCLE), 
          pid_param_max_rrate(PID_PARAM_MAX_RRATE), 
          heading_control(PID_PARM_HEAD_Kp, PID_PARM_HEAD_Ki, PID_PARM_HEAD_Kd, 0, SIMULATION_DELTA_TIME) {
        if (HakoControllerParamLoader::is_exist_envpath()) {
            HakoControllerParamLoader loader;
            loader.loadParameters();
            initializeFromLoader(&loader);
        } else {
            initializeFromLoader(nullptr);
        }
    }

    HeadingController(const std::string& filename)
        : delta_time(SIMULATION_DELTA_TIME), 
          head_control_cycle(HEAD_CONTROL_CYCLE), 
          pid_param_max_rrate(PID_PARAM_MAX_RRATE), 
          heading_control(0, 0, 0, 0, SIMULATION_DELTA_TIME) {
        HakoControllerParamLoader loader(filename);
        loader.loadParameters();
        initializeFromLoader(&loader);
    }

    HeadingController(double dt, double kp, double ki, double kd, double hcc, double pmr)
        : delta_time(dt), head_control_cycle(hcc), pid_param_max_rrate(pmr), heading_control(kp, ki, kd, 0, dt) {}

    virtual ~HeadingController() {}

    HeadingControlPidControlOutputType run(HeadingControlInputType& in) {
        HeadingControlPidControlOutputType out = prev_out;
        if (simulation_time >= head_control_cycle) {
            simulation_time = 0;
            double current_angle = normalize_angle(RADIAN2DEGREE(in.euler.z));
            double target_angle = normalize_angle(in.target_angle_deg);
            // Calculate the shortest path to the target angle
            double shortest_diff = shortest_angle(current_angle, target_angle);

            out.angular_rate_r = heading_control.calculate(current_angle + shortest_diff, current_angle);
            out.angular_rate_r = flight_controller_get_limit_value(out.angular_rate_r, 0, -pid_param_max_rrate, pid_param_max_rrate);
        }
        simulation_time += delta_time;
        return out;
    }
};

#endif /* _HEADING_CONTROLLER_HPP_ */
