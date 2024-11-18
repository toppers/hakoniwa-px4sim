#ifndef _DRONE_ALT_CONTROLLER_HPP_
#define _DRONE_ALT_CONTROLLER_HPP_

#include "drone_pid_control.hpp"
#include "flight_controller_types.hpp"
#include "frame_convertor.hpp"
#include "hako_controller_param_loader.hpp"
#include <memory>

struct DroneAltInputType {
    FlightControllerInputEulerType euler;
    FlightControllerInputPositionType pos;
    FlightControllerInputVelocityType spd;
    double target_altitude;
    DroneAltInputType() : euler(), pos(), spd(), target_altitude() {}
    DroneAltInputType(FlightControllerInputPositionType p, FlightControllerInputVelocityType s, double t_a) : pos(p), spd(s), target_altitude(t_a) {}    
};
struct DroneAltSpdInputType {
    FlightControllerInputEulerType euler;
    FlightControllerInputVelocityType spd;
    double target_spd;
    DroneAltSpdInputType() : euler(), spd(), target_spd() {}
    DroneAltSpdInputType(FlightControllerInputEulerType e, FlightControllerInputVelocityType s, double t_s) : euler(e), spd(s), target_spd(t_s) {}    
};

struct DroneAltOutputType {
    double thrust;
    DroneAltOutputType() : thrust(0) {}
    DroneAltOutputType(double thrust_val) : thrust(thrust_val) {}
};

class DroneAltController {
private:
    std::unique_ptr<DronePidControl> pos_control;
    std::unique_ptr<DronePidControl> spd_control;

    double delta_time;
    double mass;
    double gravity;
    double throttle_gain;

    double max_power;
    double max_spd;
    double pos_simulation_time = 0;
    double spd_simulation_time = 0;
    double control_cycle;

    DroneAltSpdInputType pos_prev_out = {};
    DroneAltOutputType   spd_prev_out = {};
    DroneAltSpdInputType run_pos(DroneAltInputType &in)
    {
        DroneAltSpdInputType out = pos_prev_out;
        if (pos_simulation_time >= control_cycle) {
            pos_simulation_time = 0;
            /*
             * position control
             */
            double target_spd = pos_control->calculate(in.target_altitude, in.pos.z);
            out.target_spd = flight_controller_get_limit_value(target_spd, 0, -max_spd, max_spd);
            pos_prev_out = out;
        }
        out.euler = in.euler;
        out.spd = in.spd;
        pos_simulation_time += delta_time;
        return out;
    }
public:
    DroneAltController(const HakoControllerParamLoader& loader) {
        delta_time = loader.getParameter("SIMULATION_DELTA_TIME");
        control_cycle = loader.getParameter("PID_ALT_CONTROL_CYCLE");
        max_power = loader.getParameter("PID_ALT_MAX_POWER");
        max_spd = loader.getParameter("PID_ALT_MAX_SPD");
        throttle_gain = loader.getParameter("PID_ALT_THROTTLE_GAIN");
        mass = loader.getParameter("MASS");
        gravity = loader.getParameter("GRAVITY");
        pos_control = std::make_unique<DronePidControl>(
            loader.getParameter("PID_ALT_Kp"), 
            loader.getParameter("PID_ALT_Ki"), 
            loader.getParameter("PID_ALT_Kd"), 
            0, delta_time);
        spd_control = std::make_unique<DronePidControl>(
            loader.getParameter("PID_ALT_SPD_Kp"), 
            loader.getParameter("PID_ALT_SPD_Ki"), 
            loader.getParameter("PID_ALT_SPD_Kd"), 
            0, delta_time);
    }
    ~DroneAltController() {}
    DroneAltOutputType run_spd(DroneAltSpdInputType &in)
    {
        DroneAltOutputType out = spd_prev_out;
        if (spd_simulation_time >= control_cycle) {
            spd_simulation_time = 0;
            /*
             * speed control
             */
            //機体座標系の速度を地上座標系に変換
            EulerType  e = {in.euler.x, in.euler.y, in.euler.z};
            VectorType v = {in.spd.u, in.spd.v, in.spd.w};
            VectorType g_v = ground_vector_from_body(v, e);
            //地上座標系の速度でPID制御
            double throttle_power = spd_control->calculate(in.target_spd, g_v.z);
            throttle_power = flight_controller_get_limit_value(throttle_power, 0, -max_power, max_power);
            /*
             * thrust
             */
            out.thrust = (mass * gravity) + (throttle_gain * throttle_power);
            //out.thrust = (throttle_gain * throttle_power);
            spd_prev_out = out;
        }
        spd_simulation_time += delta_time;
        return out;
    }
    DroneAltOutputType run(DroneAltInputType &in)
    {
        auto out_pos = run_pos(in);
        return run_spd(out_pos);
    }
};

#endif /* _DRONE_ALT_CONTROLLER_HPP_ */
