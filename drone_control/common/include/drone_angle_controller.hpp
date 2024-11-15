#ifndef _DRONE_ANGLE_CONTROLLER_HPP_
#define _DRONE_ANGLE_CONTROLLER_HPP_

#include "drone_pid_control.hpp"
#include "frame_convertor.hpp"
#include "flight_controller_types.hpp"
#include "hako_controller_param_loader.hpp"
#include <memory>


struct DroneAngleInputType {
    FlightControllerInputEulerType euler;
    FlightControllerInputAngularRateType angular_rate;
    double target_roll;
    double target_pitch;
    double target_yaw_rate;
    DroneAngleInputType() : euler(), angular_rate(), target_roll(0), target_pitch(0), target_yaw_rate(0) {}
    DroneAngleInputType(
        FlightControllerInputEulerType e, FlightControllerInputAngularRateType rate, 
        double t_r, double t_p, double t_arr) : 
        euler(e), angular_rate(rate), 
        target_roll(t_r), target_pitch(t_p), target_yaw_rate(t_arr) {}    
};

struct DroneAngleOutputType {
    double p;
    double q;
    double r;
    DroneAngleOutputType() : p(0), q(0), r(0) {}
    DroneAngleOutputType(double p_val, double q_val, double r_val)
        : p(p_val), q(q_val), r(r_val) {}
};

struct DroneAngleRateInputType {
    FlightControllerInputAngularRateType angular_rate;
    double target_roll_rate;
    double target_pitch_rate;
    double target_yaw_rate;
    DroneAngleRateInputType() : target_roll_rate(0), target_pitch_rate(0), target_yaw_rate(0) {}
    DroneAngleRateInputType(double roll_rate_val, double pitch_rate_val, double yaw_rate_val)
        : target_roll_rate(roll_rate_val), target_pitch_rate(pitch_rate_val), target_yaw_rate(yaw_rate_val) {}
};

class DroneAngleController {
private:
    double delta_time;
    /*
     * angle control
     */
    double angular_simulation_time;
    double angular_control_cycle;
    double roll_rate_max;
    double pitch_rate_max;
    std::unique_ptr<DronePidControl> roll_control;
    std::unique_ptr<DronePidControl> pitch_control;
    DroneAngleRateInputType angle_prev_out = {};

    /*
     * rate control
     */
    double angular_rate_simulation_time = 0;
    double angular_rate_control_cycle;
    double roll_torque_max;
    double pitch_torque_max;
    double yaw_torque_max;
    std::unique_ptr<DronePidControl> roll_rate_control;
    std::unique_ptr<DronePidControl> pitch_rate_control;
    std::unique_ptr<DronePidControl> yaw_rate_control;
    DroneAngleOutputType rate_prev_out = {};

    DroneAngleRateInputType run_angle(DroneAngleInputType& in) {
        DroneAngleRateInputType out = angle_prev_out;
        if (angular_simulation_time >= angular_control_cycle) {
            //roll control
            out.target_roll_rate = roll_control->calculate(DEGREE2RADIAN(in.target_roll), NORMALIZE_RADIAN(in.euler.x));
            out.target_roll_rate = flight_controller_get_limit_value(out.target_roll_rate, 0, -roll_rate_max, roll_rate_max);

            //pitch control
            out.target_pitch_rate = roll_control->calculate(DEGREE2RADIAN(in.target_pitch), NORMALIZE_RADIAN(in.euler.y));
            out.target_pitch_rate = flight_controller_get_limit_value(out.target_pitch_rate, 0, -pitch_rate_max, pitch_rate_max);

            //yaw control
            out.target_yaw_rate = in.target_yaw_rate;

            //copy current angular rate for next control
            out.angular_rate = in.angular_rate;
            angular_simulation_time = 0;
            angle_prev_out = out;
        }
        this->angular_simulation_time += this->delta_time;
        return out;
    }

    DroneAngleOutputType run_rate(DroneAngleRateInputType& in) {
        DroneAngleOutputType out = rate_prev_out;
        if (angular_rate_simulation_time >= angular_rate_control_cycle) {
            //roll rate control
            out.p = roll_rate_control->calculate(in.target_roll_rate, NORMALIZE_RADIAN(in.angular_rate.p));
            out.p = flight_controller_get_limit_value(out.p, 0, -roll_torque_max, roll_torque_max);

            //pitch rate control
            out.q = pitch_rate_control->calculate(in.target_pitch_rate, NORMALIZE_RADIAN(in.angular_rate.q));
            out.q = flight_controller_get_limit_value(out.q, 0, -pitch_torque_max, pitch_torque_max);

            //yaw rate control
            out.r = roll_rate_control->calculate(in.target_yaw_rate, NORMALIZE_RADIAN(in.angular_rate.r));
            out.r = flight_controller_get_limit_value(out.r, 0, -yaw_torque_max, yaw_torque_max);

            angular_rate_simulation_time = 0;
            rate_prev_out = out;
        }
        this->angular_rate_simulation_time += this->delta_time;
        return out;
    }
public:
    DroneAngleController(const HakoControllerParamLoader& loader) {
        delta_time = loader.getParameter("SIMULATION_DELTA_TIME");
        /*
         * angle control
         */
        angular_control_cycle = loader.getParameter("ANGULAR_CONTROL_CYCLE");
        roll_rate_max = RPM2EULER_RATE(loader.getParameter("PID_ROLL_RPM_MAX"));
        pitch_rate_max = RPM2EULER_RATE(loader.getParameter("PID_PITCH_RPM_MAX"));
        roll_control = std::make_unique<DronePidControl>(
            loader.getParameter("PID_ROLL_Kp"), 
            loader.getParameter("PID_ROLL_Ki"), 
            loader.getParameter("PID_ROLL_Kd"), 
            0, delta_time);
        pitch_control = std::make_unique<DronePidControl>(
            loader.getParameter("PID_PITCH_Kp"), 
            loader.getParameter("PID_PITCH_Ki"), 
            loader.getParameter("PID_PITCH_Kd"), 
            0, delta_time);

        /*
         * rate control
         */
        angular_rate_control_cycle = loader.getParameter("ANGULAR_RATE_CONTROL_CYCLE");
        roll_torque_max = loader.getParameter("PID_ROLL_TORQUE_MAX");
        pitch_torque_max = loader.getParameter("PID_PITCH_TORQUE_MAX");
        yaw_torque_max = loader.getParameter("PID_YAW_TORQUE_MAX");
        roll_rate_control = std::make_unique<DronePidControl>(
            loader.getParameter("PID_ROLL_RATE_Kp"), 
            loader.getParameter("PID_ROLL_RATE_Ki"), 
            loader.getParameter("PID_ROLL_RATE_Kd"), 
            0, delta_time);
        pitch_rate_control = std::make_unique<DronePidControl>(
            loader.getParameter("PID_PITCH_RATE_Kp"), 
            loader.getParameter("PID_PITCH_RATE_Ki"), 
            loader.getParameter("PID_PITCH_RATE_Kd"), 
            0, delta_time);
        yaw_rate_control = std::make_unique<DronePidControl>(
            loader.getParameter("PID_YAW_RATE_Kp"), 
            loader.getParameter("PID_YAW_RATE_Ki"), 
            loader.getParameter("PID_YAW_RATE_Kd"), 
            0, delta_time);
    }

    virtual ~DroneAngleController() {}
    DroneAngleOutputType run(DroneAngleInputType& in)
    {
        DroneAngleRateInputType vel = run_angle(in);
        return run_rate(vel);
    }
};
#endif /* _DRONE_ANGLE_CONTROLLER_HPP_ */