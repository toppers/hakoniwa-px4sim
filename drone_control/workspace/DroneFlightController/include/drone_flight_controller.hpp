#ifndef _DRONE_FLIGHT_CONTROLLER_HPP_
#define _DRONE_FLIGHT_CONTROLLER_HPP_

#include "control/position/pid_ctrl_vertical_pos.hpp"
#include "control/position/pid_ctrl_vertical_vel.hpp"
#include "control/position/pid_ctrl_forward_pos.hpp"
#include "control/position/pid_ctrl_forward_vel.hpp"
#include "control/angle/pid_ctrl_yaw_angle.hpp"
#include "control/angle/pid_ctrl_yaw_rate.hpp"
#include "control/angle/pid_ctrl_roll_angle.hpp"
#include "control/angle/pid_ctrl_roll_rate.hpp"
#include "control/angle/pid_ctrl_pitch_angle.hpp"
#include "control/angle/pid_ctrl_pitch_rate.hpp"
#include "flight_plan.hpp"
typedef enum {
    DRONE_CONTROL_MODE_NONE = 0,
    DRONE_CONTROL_MODE_TAKEOFF,
    DRONE_CONTROL_MODE_YAW,
    DRONE_CONTROL_MODE_MOVE,
    DRONE_CONTROL_MODE_NUM
} DroneControlModeType;
typedef struct {
    PidCtrlVerticalPos pid_ctrl_vertical_pos;
    PidCtrlVerticalVel pid_ctrl_vertical_vel;
    PidCtrlForwardPos  pid_ctrl_forward_pos;
    PidCtrlForwardVel  pid_ctrl_forward_vel;
    PidCtrlYawAngle    pid_ctrl_yaw_angle;
    PidCtrlYawRate     pid_ctrl_yaw_rate;
    PidCtrlRollAngle   pid_ctrl_roll_angle;
    PidCtrlRollRate    pid_ctrl_roll_rate;
    PidCtrlPitchAngle  pid_ctrl_pitch_angle;
    PidCtrlPitchRate   pid_ctrl_pitch_rate;
    DroneControlModeType drone_control_mode;
    double target_yaw;
    PositionType target_pos;
    int count[DRONE_CONTROL_MODE_NUM];
    const char* plan_filepath;
    MovePlan move_plan;
} DroneFlightControllerContextType;

#endif /* _DRONE_FLIGHT_CONTROLLER_HPP_ */