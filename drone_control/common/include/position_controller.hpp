#ifndef _POSITION_CONTROLLER_HPP_
#define _POSITION_CONTROLLER_HPP_

#include "flight_controller_types.hpp"
#include "frame_convertor.hpp"

struct PositionControlInputType {
    FlightControllerInputPositionType pos;
    FlightControllerInputEulerType euler;
    double target_x;
    double target_y;
    double target_spd;
    PositionControlInputType() : pos(), euler(), target_x(0), target_y(0), target_spd(0) {}
    PositionControlInputType(
        FlightControllerInputPositionType p, FlightControllerInputEulerType e, double t_x, double t_y, double spd) : 
        pos(p), euler(e),
        target_x(t_x), target_y(t_y), target_spd(spd) {}    
};

struct PositionControlPidControlOutputType {
    double target_vx;
    double target_vy;
    PositionControlPidControlOutputType() : target_vx(0), target_vy(0) {}
    PositionControlPidControlOutputType(double vx, double vy)
        : target_vx(vx), target_vy(vy) {}
};

class PositionController {
private:
    double delta_time;
    double max_spd;
    PidControl position_control_vx;
    PidControl position_control_vy;
    PositionControlPidControlOutputType prev_out = {};
    double simulation_time = 0;
public:
    PositionController() :
        delta_time(SIMULATION_DELTA_TIME), 
        max_spd(PID_PARAM_MAX_SPD), position_control_vx(PID_PARM_POSX_Kp, PID_PARM_POSX_Ki, PID_PARM_POSX_Kd, 0, SIMULATION_DELTA_TIME),
        position_control_vy(PID_PARM_POSY_Kp, PID_PARM_POSY_Ki, PID_PARM_POSY_Kd, 0, SIMULATION_DELTA_TIME) {}
    PositionController(double dt, double max_spd_val, double x_kp, double x_ki, double x_kd, double y_kp, double y_ki, double y_kd) :
        delta_time(dt), max_spd(max_spd_val), position_control_vx(x_kp, x_ki, x_kd, 0, dt), position_control_vy(y_kp, y_ki, y_kd, 0, dt) {}
    virtual ~PositionController() {}

    PositionControlPidControlOutputType run(PositionControlInputType& in)
    {
        PositionControlPidControlOutputType out = prev_out;
        if (simulation_time >= POS_CONTROL_CYCLE) {
            simulation_time = 0;
            double target_vx = position_control_vx.calculate(in.target_x, in.pos.x);
            target_vx = flight_controller_get_limit_value(target_vx, 0, -max_spd, max_spd);
            double target_vy = position_control_vy.calculate(in.target_y, in.pos.y);
            target_vy = flight_controller_get_limit_value(target_vy, 0, -max_spd, max_spd);

            //convert ground frame to body frame
            VectorType val = body_vector_from_ground({ target_vx, target_vy, 0 }, { 0, 0, in.euler.z });
            out.target_vx = val.x;
            out.target_vy = val.y;
        }
        simulation_time += delta_time;
        return out;
    }
};

#endif /* _POSITION_CONTROLLER_HPP_ */
