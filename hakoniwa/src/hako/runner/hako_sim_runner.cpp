#include "hako_sim_runner.hpp"
#include "../pdu/hako_pdu_data.hpp"
#include "drone/drone_phys.hpp"
#include "drone/drone_phys_sensor.hpp"
#include "../../threads/px4sim_thread_sender.hpp"
#include "control/drone_control.hpp"
#include "drone/drone_phys_control.hpp"
#include <iostream>
#include <unistd.h>
#include <memory.h>
#include "hako_capi.h"

#define HAKO_AVATOR_CHANNLE_ID_MOTOR   0
#define HAKO_AVATOR_CHANNLE_ID_POS   1


typedef struct {
    HakoSimRunnerArgType *arg;
    hako_time_t asset_time;
} HakoSimControlType;
HakoSimControlType hako_sim_control;

static DroneControlType drone_ctrl;
static DronePhysType drone_phys;
static DronePropellerRotationRateType drone_propeller;

#define DRONE_PHYS_DELTA_TIME 0.001 /* 1msec */
static void my_setup()
{
    std::cout << "INFO: setup start" << std::endl;
    DronePhysParamType param;
    DronePhysStateType initial_value;
    param.m = DRONE_PARAM_M;
    param.l = DRONE_PARAM_L;
    param.gravity = DRONE_PARAM_GRAVITY;
    param.k = DRONE_PARAM_K;
    param.p = DRONE_PARAM_P;
    memset(&initial_value, 0, sizeof(initial_value));
    memset(&drone_propeller, 0, sizeof(drone_propeller));
    initial_value.pos.x = 0;
    initial_value.pos.y = 0;
    initial_value.pos.z = 0;
    initial_value.rot.z = 0;
    drone_init(DRONE_PHYS_DELTA_TIME, param, initial_value, drone_phys);
    drone_sensor_init(drone_phys);
    drone_control_init(drone_ctrl, DRONE_PHYS_DELTA_TIME);
    drone_ctrl.phys = &drone_phys;
    std::cout << "INFO: setup done" << std::endl;
    return;
}

static void do_io_write()
{
    Hako_HakoHilActuatorControls hil_actuator_controls;
    Hako_Twist pos;

    memset(&hil_actuator_controls, 0, sizeof(hil_actuator_controls));
    for (int i = 0; i < 4; i++) {
        hil_actuator_controls.controls[i] = drone_propeller.w[i];
    }
    if (!hako_asset_runner_pdu_write(hako_sim_control.arg->robo_name, HAKO_AVATOR_CHANNLE_ID_MOTOR, (const char*)&hil_actuator_controls, sizeof(hil_actuator_controls))) {
        std::cerr << "ERROR: can not write pdu data: hil_actuator_controls" << std::endl;
    }
#ifdef ENABLE_AIR_FRAME
    pos.linear.x = drone_phys.current.pos.x;
    pos.linear.y = -drone_phys.current.pos.y;
    pos.linear.z = -drone_phys.current.pos.z;
    pos.angular.x = drone_phys.current.rot.x * (180.0 / M_PI);
    pos.angular.y = -drone_phys.current.rot.y * (180.0 / M_PI);
    pos.angular.z = -drone_phys.current.rot.z * (180.0 / M_PI);
#else
    pos.linear.x = drone_phys.current.pos.x;
    pos.linear.y = drone_phys.current.pos.y;
    pos.linear.z = drone_phys.current.pos.z;
    pos.angular.x = drone_phys.current.rot.x * (180.0 / M_PI);
    pos.angular.y = drone_phys.current.rot.y * (180.0 / M_PI);
    pos.angular.z = drone_phys.current.rot.z * (180.0 / M_PI);
#endif
    if (!hako_asset_runner_pdu_write(hako_sim_control.arg->robo_name, HAKO_AVATOR_CHANNLE_ID_POS, (const char*)&pos, sizeof(pos))) {
        std::cerr << "ERROR: can not write pdu data: pos" << std::endl;
    }

    //TO sender shared data
    hako_write_hil_state_quaternion(drone_phys.sensor.hil_state_quaternion);
    hako_write_hil_sensor(drone_phys.sensor.hil_sensor);
    hako_write_hil_gps(drone_phys.sensor.hil_gps);
}
static void my_task()
{
#ifdef DRONE_PX4_CONTROL_ENABLE
    if (hako_read_hil_actuator_controls(drone_phys.actuator.hil_actuator_controls)) {
#ifndef DRONE_PX4_AIRFRAME_ADJUSTMENT_ENABLE
        drone_propeller.w[0] = drone_phys.actuator.hil_actuator_controls.controls[0] * KEISU;
        drone_propeller.w[1] = drone_phys.actuator.hil_actuator_controls.controls[1] * KEISU;
        drone_propeller.w[2] = drone_phys.actuator.hil_actuator_controls.controls[2] * KEISU;
        drone_propeller.w[3] = drone_phys.actuator.hil_actuator_controls.controls[3] * KEISU;
#else        
#define ttsqrt(a) (a)
        drone_propeller.w[0] = ttsqrt(KEISU * drone_phys.actuator.hil_actuator_controls.controls[2]);
        drone_propeller.w[1] = ttsqrt(KEISU * drone_phys.actuator.hil_actuator_controls.controls[0]);
        drone_propeller.w[2] = ttsqrt(KEISU * drone_phys.actuator.hil_actuator_controls.controls[3]);
        drone_propeller.w[3] = ttsqrt(KEISU * drone_phys.actuator.hil_actuator_controls.controls[1]);
#endif
    }
#endif
    drone_run(drone_propeller, drone_phys);
    //std::cout << "time: " << drone_phys.current_time << std::endl;
    //std::cout << "lat = " << drone_phys.sensor.hil_gps.lat << std::endl;
    //std::cout << "lon = " << drone_phys.sensor.hil_gps.lon << std::endl;
    //std::cout << "pos.z = " << drone_phys.current.pos.z << std::endl;
    drone_sensor_run(drone_phys);

    do_io_write();

#ifdef DRONE_PX4_ENABLE
    px4sim_sender_do_task();
#ifdef DRONE_PX4_CONTROL_ENABLE
    //nothing to do
#else
    drone_control_run(drone_ctrl);
    convert2RotationRate(drone_ctrl.signal, drone_phys, drone_propeller);
#endif /* DRONE_PX4_CONTROL_ENABLE */
#else
    drone_control_run(drone_ctrl);
    convert2RotationRate(drone_ctrl.signal, drone_phys, drone_propeller);
#endif /* DRONE_PX4_ENABLE */
    return;
}

static void my_reset()
{
    //nothing to do
}

static hako_asset_runner_callback_t my_callbacks = {
    my_setup,   // setup
    NULL,   // write_initial_pdu_data
    my_task,   // do_task
    my_reset    // reset
};

hako_time_t hako_asset_time = 0;

void *hako_sim_runner(void *argp)
{
    hako_asset_runner_register_callback(&my_callbacks);
    hako_sim_control.arg = static_cast<HakoSimRunnerArgType*>(argp);
    if (hako_asset_runner_init(hako_sim_control.arg->asset_name, hako_sim_control.arg->config_path, hako_sim_control.arg->delta_time_msec * 1000) == false) {
        std::cerr << "ERROR: " << "hako_asset_runner_init() error" << std::endl;
        return nullptr;
    }
    while (true) {
        hako_sim_control.asset_time = 0;
        std::cout << "INFO: start simulation" << std::endl;
        while (true) {
            if (hako_asset_runner_step(1) == false) {
                std::cout << "INFO: stopped simulation" << std::endl;
                break;
            }
            else {
                hako_sim_control.asset_time += HAKO_PX4_RUNNER_MASTER_DELTA_USEC;
                hako_asset_time = hako_sim_control.asset_time;
                usleep(hako_sim_control.arg->delta_time_msec * 200);
            }
            //std::cout << "STEP" << std::endl;
        }
        //リセット発生した場合は最初からやり直す。
    }
    std::cout << "INFO: end simulation" << std::endl;
    hako_asset_runner_fin();

    return nullptr;
}

