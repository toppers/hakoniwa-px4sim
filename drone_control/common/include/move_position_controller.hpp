#ifndef _MOVE_POSITION_CONTROLLER_HPP_
#define _MOVE_POSITION_CONTROLLER_HPP_

#include "heading_controller.hpp"
#include "position_controller.hpp"
#include "radio_controller.hpp"

class MovePositionController {
private:
    double delta_time;
    double simulation_time = 0;
public:
    HeadingController heading;
    double r_angle;
    double angle_time = 0;
    void update_target_angle(double v)
    {
        if (angle_time >= heading.head_control_cycle) {
            angle_time = 0;
            r_angle += v * HEAD_DELTA_VALUE_M;
        }
        angle_time += this->delta_time;
    }
    PositionController pos;
    RadioController *rc;
    double r_pos_x;
    double r_pos_y;
    double pos_time = 0;
    void update_target_pos(double x, double y)
    {
        if (pos_time >= rc->alt.alt_control_cycle) {
            pos_time = 0;
            r_pos_x += x * POS_DELTA_VALUE_M;
            if (r_pos_x >= POS_VALUE_MAX) {
                r_pos_x = POS_VALUE_MAX;
            }
            else if (r_pos_x < -POS_VALUE_MAX) {
                r_pos_x = -POS_VALUE_MAX;
            }
            r_pos_y += y * POS_DELTA_VALUE_M;
            if (r_pos_y >= POS_VALUE_MAX) {
                r_pos_y = POS_VALUE_MAX;
            }
            else if (r_pos_y < -POS_VALUE_MAX) {
                r_pos_y = -POS_VALUE_MAX;
            }
        }
        pos_time += this->delta_time;
    }

    MovePositionController() :delta_time(SIMULATION_DELTA_TIME)
    {
        RadioControllerParamType param = get_radio_control_default_parameters();
        this->rc = create_radio_controller(param);
        this->delta_time = rc->delta_time;
        if (rc == nullptr) {
            exit(1);
        }
    }
    virtual ~MovePositionController() {}

    FlightControllerOutputType run(double target_pos_x, double target_pos_y, double target_altitude, double target_angle, double target_speed,
        const FlightControllerInputEulerType& euler, const FlightControllerInputPositionType& pos, 
        const FlightControllerInputVelocityType& velocity, FlightControllerInputAngularRateType& angular_rate)
    {
        FlightControllerOutputType out = {};
        //Heading control;
        HeadingControlInputType h_in;
        h_in.euler = euler;
        h_in.target_angle_deg = target_angle;
        HeadingControlPidControlOutputType h_out = this->heading.run(h_in);

        //position control
        PositionControlInputType p_in;
        p_in.euler = euler;
        p_in.pos = pos;
        p_in.target_spd = target_speed;
        p_in.target_x = target_pos_x;
        p_in.target_y = target_pos_y;
        PositionControlPidControlOutputType p_out = this->pos.run(p_in);

        //altitude control
        AltitudeControlInputType a_in;
        a_in.pos = pos;
        a_in.target_altitude = target_altitude;
        AltitudeControlPidControlOutputType a_out = rc->alt.run(a_in);

        //speed control
        SpeedControlInputType s_in;
        s_in.target_vx = p_out.target_vx;
        s_in.target_vy = p_out.target_vy;
        s_in.velocity = velocity;
        SpeedControlPidControlOutputType s_out = rc->spd.run(s_in);

        //radio control
        RadioControlInputType rin;
        rin.euler = euler;
        rin.angular_rate = angular_rate;
        rin.target_thrust = a_out.throttle_power;
        rin.target_roll = s_out.roll;
        rin.target_pitch = s_out.pitch;
        rin.target_angular_rate_r = h_out.angular_rate_r;

        FlightControllerOutputType ret = rc->run(rin);
        out.thrust = ret.thrust;
        out.torque_x = ret.torque_x;
        out.torque_y = ret.torque_y;
        out.torque_z = ret.torque_z;
        return out;        
    }

};

#endif /* _MOVE_POSITION_CONTROLLER_HPP_ */