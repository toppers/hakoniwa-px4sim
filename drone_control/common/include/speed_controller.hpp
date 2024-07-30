#ifndef _SPEED_CONTROLLER_HPP_
#define _SPEED_CONTROLLER_HPP_

#include "flight_controller_types.hpp"
#include "hako_controller_param_loader.hpp"

struct SpeedControlInputType {
    FlightControllerInputVelocityType velocity;
    double target_vx;
    double target_vy;
    SpeedControlInputType() : velocity(), target_vx(0), target_vy(0) {}
    SpeedControlInputType(FlightControllerInputEulerType e, FlightControllerInputVelocityType v, double t_vx, double t_vy) 
        : velocity(v), target_vx(t_vx), target_vy(t_vy) {}    
};

struct SpeedControlPidControlOutputType {
    double roll;
    double pitch;
    SpeedControlPidControlOutputType() : roll(0), pitch(0) {}
    SpeedControlPidControlOutputType(double roll_val, double pitch_val) : roll(roll_val), pitch(pitch_val) {}
};

class SpeedController {
private:
    double delta_time;
    double spd_control_cycle;
    double max_roll_deg; // degree
    double max_pitch_deg; // degree
    PidControl speed_control_vx;
    PidControl speed_control_vy;
    SpeedControlPidControlOutputType prev_out = {};
    double simulation_time = 0;

    void initializeFromLoader(HakoControllerParamLoader* loader) {
        if (loader) {
            delta_time = loader->getParameter("SIMULATION_DELTA_TIME");
            spd_control_cycle = loader->getParameter("SPD_CONTROL_CYCLE");
            max_roll_deg = loader->getParameter("PID_PARAM_MAX_ROLL");
            max_pitch_deg = loader->getParameter("PID_PARAM_MAX_PITCH");
            speed_control_vx = PidControl(loader->getParameter("PID_PARM_VX_Kp"), loader->getParameter("PID_PARM_VX_Ki"), loader->getParameter("PID_PARM_VX_Kd"), 0, delta_time);
            speed_control_vy = PidControl(loader->getParameter("PID_PARM_VY_Kp"), loader->getParameter("PID_PARM_VY_Ki"), loader->getParameter("PID_PARM_VY_Kd"), 0, delta_time);
        } else {
            delta_time = SIMULATION_DELTA_TIME;
            spd_control_cycle = SPD_CONTROL_CYCLE;
            max_roll_deg = PID_PARAM_MAX_ROLL;
            max_pitch_deg = PID_PARAM_MAX_PITCH;
            speed_control_vx = PidControl(PID_PARM_VX_Kp, PID_PARM_VX_Ki, PID_PARM_VX_Kd, 0, delta_time);
            speed_control_vy = PidControl(PID_PARM_VY_Kp, PID_PARM_VY_Ki, PID_PARM_VY_Kd, 0, delta_time);
        }
    }

public:
    SpeedController() 
        : delta_time(SIMULATION_DELTA_TIME), 
          spd_control_cycle(SPD_CONTROL_CYCLE), 
          max_roll_deg(PID_PARAM_MAX_ROLL), 
          max_pitch_deg(PID_PARAM_MAX_PITCH), 
          speed_control_vx(PID_PARM_VX_Kp, PID_PARM_VX_Ki, PID_PARM_VX_Kd, 0, SIMULATION_DELTA_TIME), 
          speed_control_vy(PID_PARM_VY_Kp, PID_PARM_VY_Ki, PID_PARM_VY_Kd, 0, SIMULATION_DELTA_TIME) {
        if (HakoControllerParamLoader::is_exist_envpath()) {
            HakoControllerParamLoader loader;
            loader.loadParameters();
            initializeFromLoader(&loader);
        } else {
            initializeFromLoader(nullptr);
        }
    }

    SpeedController(const std::string& filename) 
        : delta_time(SIMULATION_DELTA_TIME), 
          spd_control_cycle(SPD_CONTROL_CYCLE), 
          max_roll_deg(PID_PARAM_MAX_ROLL), 
          max_pitch_deg(PID_PARAM_MAX_PITCH), 
          speed_control_vx(0, 0, 0, 0, SIMULATION_DELTA_TIME), 
          speed_control_vy(0, 0, 0, 0, SIMULATION_DELTA_TIME) {
        HakoControllerParamLoader loader(filename);
        loader.loadParameters();
        initializeFromLoader(&loader);
    }

    SpeedController(double dt, double max_pitch_val, double vx_kp, double vx_ki, double vx_kd, double max_roll_val, double vy_kp, double vy_ki, double vy_kd) 
        : delta_time(dt), spd_control_cycle(SPD_CONTROL_CYCLE), max_pitch_deg(max_pitch_val), max_roll_deg(max_roll_val), 
          speed_control_vx(vx_kp, vx_ki, vx_kd, 0, dt), speed_control_vy(vy_kp, vy_ki, vy_kd, 0, dt) {}

    virtual ~SpeedController() {}

    SpeedControlPidControlOutputType run(SpeedControlInputType& in) {
        SpeedControlPidControlOutputType out = prev_out;
        if (simulation_time >= spd_control_cycle) {
            simulation_time = 0;
            out.roll = speed_control_vy.calculate(in.target_vy, in.velocity.v);
            out.roll = flight_controller_get_limit_value(out.roll, 0, -max_roll_deg, max_roll_deg);
            out.pitch = -speed_control_vx.calculate(in.target_vx, in.velocity.u);
            out.pitch = flight_controller_get_limit_value(out.pitch, 0, -max_pitch_deg, max_pitch_deg);
        }
        simulation_time += delta_time;
        return out;
    }
};

#endif /* _SPEED_CONTROLLER_HPP_ */
