#ifndef _SPEED_CONTROLLER_HPP_
#define _SPEED_CONTROLLER_HPP_

#include "flight_controller_types.hpp"

struct SpeedControlInputType {
    FlightControllerInputVelocityType velocity;
    double target_vx;
    double target_vy;
    SpeedControlInputType() : velocity(), target_vx(0), target_vy(0) {}
    SpeedControlInputType(
        FlightControllerInputEulerType e, FlightControllerInputVelocityType v, double t_vx, double t_vy) : 
        velocity(v), 
        target_vx(t_vx),target_vy(t_vy) {}    
};

struct SpeedControlPidControlOutputType {
    double roll;
    double pitch;
    SpeedControlPidControlOutputType() : roll(0), pitch(0) {}
    SpeedControlPidControlOutputType(double roll_val, double pitch_val)
        : roll(roll_val), pitch(pitch_val) {}
};

class SpeedController {
private:
    double delta_time;
    double max_roll_deg;//degree
    double max_pitch_deg;//degree
    PidControl speed_control_vx;
    PidControl speed_control_vy;
    SpeedControlPidControlOutputType prev_out = {};
    double simulation_time = 0;
public:
    SpeedController() :
        delta_time(SIMULATION_DELTA_TIME), 
        max_pitch_deg(PID_PARAM_MAX_PITCH), speed_control_vx(PID_PARM_VX_Kp, PID_PARM_VX_Ki, PID_PARM_VX_Kd, 0, SIMULATION_DELTA_TIME), 
        max_roll_deg(PID_PARAM_MAX_ROLL), speed_control_vy(PID_PARM_VY_Kp, PID_PARM_VY_Ki, PID_PARM_VY_Kd, 0, SIMULATION_DELTA_TIME) {}
    SpeedController(double dt, double max_pitch_val, double vx_kp, double vx_ki, double vx_kd, double max_roll_val, double vy_kp, double vy_ki, double vy_kd) :
        delta_time(dt), max_pitch_deg(max_pitch_val), speed_control_vx(vx_kp, vx_ki, vx_kd, 0, dt), max_roll_deg(max_roll_val), speed_control_vy(vy_kp, vy_ki, vy_kd, 0, dt) {}
    virtual ~SpeedController() {}

    SpeedControlPidControlOutputType run(SpeedControlInputType& in)
    {
        SpeedControlPidControlOutputType out = prev_out;
        if (simulation_time >= SPD_CONTROL_CYCLE) {
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