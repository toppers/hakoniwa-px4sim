#ifndef _DRONE_POS_CONTROLLER_HPP_
#define _DRONE_POS_CONTROLLER_HPP_

#include "drone_pid_control.hpp"
#include "frame_convertor.hpp"
#include "flight_controller_types.hpp"
#include "hako_controller_param_loader.hpp"
#include <memory>

struct DronePosInputType {
    FlightControllerInputPositionType pos;
    FlightControllerInputVelocityType velocity;
    FlightControllerInputEulerType euler;
    double target_x;
    double target_y;
    double target_spd;
    DronePosInputType() : pos(), velocity(), euler(), target_x(0), target_y(0), target_spd(0) {}
    DronePosInputType(FlightControllerInputPositionType p, FlightControllerInputVelocityType v, FlightControllerInputEulerType e, double t_x, double t_y, double spd)
        : pos(p), velocity(v), euler(e), target_x(t_x), target_y(t_y), target_spd(spd) {}
};

struct DronePosOutputType {
    double target_roll;
    double target_pitch;
    DronePosOutputType() : target_roll(0), target_pitch(0) {}
    DronePosOutputType(double roll_val, double pitch_val) : target_roll(roll_val), target_pitch(pitch_val) {}
};

struct DroneVelInputType {
    FlightControllerInputVelocityType velocity;
    double target_vx;
    double target_vy;
    DroneVelInputType() : velocity(), target_vx(0), target_vy(0) {}
    DroneVelInputType(FlightControllerInputVelocityType v, double t_vx, double t_vy) 
        : velocity(v), target_vx(t_vx), target_vy(t_vy) {}    
};


class DronePosController {
private:
    double delta_time;
    /*
     * position control
     */
    double pos_control_cycle;
    double max_spd;
    std::unique_ptr<DronePidControl> position_control_vx;
    std::unique_ptr<DronePidControl> position_control_vy;
    DronePosOutputType pos_prev_out = {};
    double pos_simulation_time = 0;

    /*
     * speed control
     */
    double spd_control_cycle;
    double max_roll_deg;
    double max_pitch_deg;
    std::unique_ptr<DronePidControl> speed_control_vx;
    std::unique_ptr<DronePidControl> speed_control_vy;
    DroneVelInputType spd_prev_out = {};
    double spd_simulation_time = 0;

    DroneVelInputType run_pos(DronePosInputType& in) {
        DroneVelInputType out = spd_prev_out;
        if (pos_simulation_time >= pos_control_cycle) {
            pos_simulation_time = 0;
            double target_vx = position_control_vx->calculate(in.target_x, in.pos.x);
            target_vx = flight_controller_get_limit_value(target_vx, 0, -max_spd, max_spd);
            double target_vy = position_control_vy->calculate(in.target_y, in.pos.y);
            target_vy = flight_controller_get_limit_value(target_vy, 0, -max_spd, max_spd);

            // convert ground frame to body frame
            VectorType val = body_vector_from_ground({ target_vx, target_vy, 0 }, { 0, 0, in.euler.z });

            // Normalize val.x and val.y with in.target_spd
            double magnitude = sqrt(val.x * val.x + val.y * val.y);
            if (magnitude > in.target_spd) {
                val.x = (val.x / magnitude) * in.target_spd;
                val.y = (val.y / magnitude) * in.target_spd;
            }
            out.velocity = in.velocity;
            out.target_vx = val.x;
            out.target_vy = val.y;
            spd_prev_out = out;
        }
        pos_simulation_time += delta_time;
        return out;
    }


public:
    DronePosController(const HakoControllerParamLoader& loader) {
        delta_time = loader.getParameter("SIMULATION_DELTA_TIME");
        /*
         * position control
         */
        pos_control_cycle = loader.getParameter("POS_CONTROL_CYCLE");
        max_spd = loader.getParameter("PID_POS_MAX_SPD");
        position_control_vx = std::make_unique<DronePidControl>(
            loader.getParameter("PID_POS_X_Kp"), 
            loader.getParameter("PID_POS_X_Ki"), 
            loader.getParameter("PID_POS_X_Kd"), 
            0, delta_time);
        position_control_vy = std::make_unique<DronePidControl>(
            loader.getParameter("PID_POS_Y_Kp"), 
            loader.getParameter("PID_POS_Y_Ki"), 
            loader.getParameter("PID_POS_Y_Kd"), 
            0, delta_time);

        /*
         * speed control
         */
        spd_control_cycle = loader.getParameter("SPD_CONTROL_CYCLE");
        max_roll_deg = loader.getParameter("PID_POS_MAX_ROLL");
        max_pitch_deg = loader.getParameter("PID_POS_MAX_PITCH");
        speed_control_vx = std::make_unique<DronePidControl>(
            loader.getParameter("PID_POS_VX_Kp"), 
            loader.getParameter("PID_POS_VX_Ki"), 
            loader.getParameter("PID_POS_VX_Kd"), 
            0, delta_time);
        speed_control_vy = std::make_unique<DronePidControl>(
            loader.getParameter("PID_POS_VY_Kp"), 
            loader.getParameter("PID_POS_VY_Ki"), 
            loader.getParameter("PID_POS_VY_Kd"), 
            0, delta_time);
    }

    virtual ~DronePosController() {}
    DronePosOutputType run_spd(DroneVelInputType& in) {
        DronePosOutputType out = pos_prev_out;
        if (spd_simulation_time >= spd_control_cycle) {
            spd_simulation_time = 0;
            //左右の場合は、ロール角を回転させる必要がある
            out.target_roll = speed_control_vy->calculate(in.target_vy, in.velocity.v);
            out.target_roll = flight_controller_get_limit_value(out.target_roll, 0, -max_roll_deg, max_roll_deg);
            //前後の場合は、ピッチ角を回転させる必要がある。ただし、回転方向は逆にする必要がある
            out.target_pitch = -speed_control_vx->calculate(in.target_vx, in.velocity.u);
            out.target_pitch = flight_controller_get_limit_value(out.target_pitch, 0, -max_pitch_deg, max_pitch_deg);
            pos_prev_out = out;
        }
        spd_simulation_time += delta_time;
        return out;
    }
    DronePosOutputType run(DronePosInputType& in)
    {
        DroneVelInputType vel = run_pos(in);
        return run_spd(vel); 
    }
};

#endif /* _DRONE_POS_CONTROLLER_HPP_ */
