#ifndef _DRONE_HEADING_CONTROLLER_HPP_
#define _DRONE_HEADING_CONTROLLER_HPP_

#include "drone_pid_control.hpp"
#include "flight_controller_types.hpp"
#include "hako_controller_param_loader.hpp"

struct DroneHeadingControlInputType {
    FlightControllerInputEulerType euler;
    double target_angle_deg;
    DroneHeadingControlInputType() : target_angle_deg(0) {}
    DroneHeadingControlInputType(FlightControllerInputEulerType e, double t_e) : euler(e), target_angle_deg(t_e) {}    
};

struct DroneHeadingControlOutputType {
    double target_yaw_rate;
    DroneHeadingControlOutputType() : target_yaw_rate(0) {}
    DroneHeadingControlOutputType(double angular_rate_r_val) : target_yaw_rate(angular_rate_r_val) {}
};

class DroneHeadingController {
private:
    double delta_time;
    double yaw_rate_max;
    std::unique_ptr<DronePidControl> heading_control;
    DroneHeadingControlOutputType prev_out = {};
    double simulation_time = 0;

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
    DroneHeadingController(const HakoControllerParamLoader& loader)
    {
        delta_time = loader.getParameter("SIMULATION_DELTA_TIME");
        head_control_cycle = loader.getParameter("HEAD_CONTROL_CYCLE");
        yaw_rate_max = RPM2EULER_RATE(loader.getParameter("PID_YAW_RPM_MAX"));
        heading_control = std::make_unique<DronePidControl>(
            loader.getParameter("PID_YAW_Kp"), 
            loader.getParameter("PID_YAW_Ki"), 
            loader.getParameter("PID_YAW_Kd"), 
            0, delta_time);
    }

    virtual ~DroneHeadingController() {}

    DroneHeadingControlOutputType run(DroneHeadingControlInputType& in) {
        DroneHeadingControlOutputType out = prev_out;
        if (simulation_time >= head_control_cycle) {
            simulation_time = 0;
            double current_angle = normalize_angle(RADIAN2DEGREE(in.euler.z));
            double target_angle = normalize_angle(in.target_angle_deg);
            // Calculate the shortest path to the target angle
            double shortest_diff = shortest_angle(current_angle, target_angle);

            out.target_yaw_rate = heading_control->calculate(current_angle + shortest_diff, current_angle);
            out.target_yaw_rate = flight_controller_get_limit_value(out.target_yaw_rate, 0, -yaw_rate_max, yaw_rate_max);
            prev_out = out;
        }
        simulation_time += delta_time;
        return out;
    }
};

#endif /* _DRONE_HEADING_CONTROLLER_HPP_ */
