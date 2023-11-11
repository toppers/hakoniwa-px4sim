#ifndef _DRONE_CONTROL_HPP_
#define _DRONE_CONTROL_HPP_

#include "../common/drone_types.hpp"
#include "drone_control_pid.hpp"

typedef struct {
    double delta_t;
    struct {
        Vector3Type target;         // 位置の目標値(HIL_GPSの単位)
        PIDController pid_x;        // X軸制御のためのPID
        PIDController pid_y;        // Y軸制御のためのPID
        PIDController pid_z;        // Z軸制御のためのPID
    } target_pos;
    struct {
        Vector3Type target;         // 姿勢の目標値
        PIDController pid_roll;     // Roll軸制御のためのPID
        PIDController pid_pitch;    // Pitch軸制御のためのPID
        PIDController pid_yaw;      // Yaw軸制御のためのPID
    } target_rot;

    /*
     * 制御信号
     */
    DroneControlSignalType signal;
} DroneControlType;

extern void drone_control_init(DroneControlType& ctrl, double delta_t);
extern void drone_control_run(DroneControlType& ctrl, Vector3Type& current_pos, double delta_t);

#endif /* _DRONE_CONTROL_HPP_ */
