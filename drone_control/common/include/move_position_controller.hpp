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
        if (angle_time >= HEAD_CONTROL_CYCLE) {
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
        if (pos_time >= ALT_CONTROL_CYCLE) {
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
        if (rc == nullptr) {
            exit(1);
        }
    }
    virtual ~MovePositionController() {}

};

#endif /* _MOVE_POSITION_CONTROLLER_HPP_ */