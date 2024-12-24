#include "controller/impl/radio_controller/radio_controller.hpp"
#include <algorithm>
#include <iostream>

using namespace hako::controller::impl;

RadioController::RadioController()
{
    ctrl_.reset();
}

void RadioController::reset()
{
    ctrl_.reset();
}

mi_aircraft_control_out_t RadioController::run(mi_aircraft_control_in_t& in)
{
    mi_aircraft_control_out_t out = {};
    /*
     * 入力
     * 補足：z軸は、わかりやすさを重視しして符号を反転する。
     */
    FlightControllerInputEulerType euler = {in.euler_x, in.euler_y, in.euler_z};
    FlightControllerInputPositionType pos = {in.pos_x, in.pos_y, -in.pos_z};
    FlightControllerInputVelocityType velocity = {in.u, in.v, -in.w};
    FlightControllerInputAngularRateType angular_rate = {in.p, in.q, in.r};

    /*
     * 目標値は、NED座標系で入る。
     * Z軸だけ、わかりやすさのため符号を反転している
     */
    ctrl_.save_for_initial_position(pos.z);
    double target_yaw      =  ctrl_.update_target_yaw(in.target.direction_velocity.r);
    double target_pos_z    =  ctrl_.update_target_altitude(-in.target.throttle.power);
    /*
     * 高度制御
     */
    DroneAltInputType alt_in(pos, velocity, target_pos_z);
    DroneAltOutputType alt_out = ctrl_.alt->run(alt_in);
    /*
     * 機首方向制御
     */
    DroneHeadingControlInputType head_in(euler, target_yaw);
    DroneHeadingControlOutputType head_out = ctrl_.head->run(head_in);

    DroneAngleOutputType angle_out = {};
    if (ctrl_.is_angle_control_enable() == false) {
        /*
        * 水平制御
        */
        DronePosOutputType pos_out = {};
        double target_vx       =  -in.target.attitude.pitch * ctrl_.get_pos_max_spd();
        double target_vy       =  in.target.attitude.roll  * ctrl_.get_pos_max_spd();
        DroneVelInputType spd_in(velocity, target_vx, target_vy);
        pos_out = ctrl_.pos->run_spd(spd_in);
        /*
        * 姿勢角度制御
        */
        DroneAngleInputType angle_in(euler, angular_rate, pos_out.target_roll, pos_out.target_pitch, head_out.target_yaw_rate);
        angle_out = ctrl_.angle->run(angle_in);
    }
    else {
        /*
         * 姿勢角速度制御
         */
        //std::cout << "roll: " << in.target.attitude.roll << " pitch: " << in.target.attitude.pitch << " yaw: " << head_out.target_yaw_rate << std::endl;
        DroneAngleInputType angle_in(euler, angular_rate, 
            in.target.attitude.roll * ctrl_.get_max_roll_deg(), 
            in.target.attitude.pitch * ctrl_.get_max_pitch_deg(), 
            head_out.target_yaw_rate);
        angle_out = ctrl_.angle->run(angle_in);
    }

    /*
     * 出力
     */
    out.thrust = alt_out.thrust;
    out.torque_x = angle_out.p;
    out.torque_y = angle_out.q;
    out.torque_z = angle_out.r;
    return out;
}


