#include "hako_phys.hpp"
#include "hako_capi.h"
#include "../utils/hako_params.hpp"
#include "hako_asset_runner.h"
#include "../hako/pdu/hako_pdu_data.hpp"
#include "../hako/runner/hako_px4_master.hpp"
#include "../assets/drone/physics/body_frame/drone_dynamics_body_frame.hpp"
#include "../assets/drone/physics/ground_frame/drone_dynamics_ground_frame.hpp"
#include "../assets/drone/controller/drone_pid_control.hpp"
#include <unistd.h>
#include <memory.h>
#include <iostream>

#define HAKO_RUNNER_MASTER_MAX_DELAY_USEC       1000 /* usec*/
#define HAKO_RUNNER_MASTER_DELTA_USEC           1000 /* usec*/
#define HAKO_RUNNER_DELTA_TIME_SEC              0.001
#define HAKO_AVATOR_CHANNLE_ID_MOTOR    0
#define HAKO_AVATOR_CHANNLE_ID_POS      1
#define HAKO_AVATOR_CHANNLE_ID_CTRL     2
#define HAKO_PHYS_DRAG          0.05

#define HAKO_ROBO_NAME "px4sim"

static void asset_runner();
using hako::assets::drone::DroneDynamicsBodyFrame;
using hako::assets::drone::DroneDynamicsGroundFrame;
using hako::assets::drone::IDroneDynamics;
using hako::assets::drone::DronePositionType;
using hako::assets::drone::DroneAngleType;
using hako::assets::drone::DroneVelocityType;
using hako::assets::drone::DroneAngularVelocityType;
using hako::assets::drone::DroneTorqueType;
using hako::assets::drone::DroneThrustType;

void hako_phys_main()
{
    if (!hako_master_init()) {
        std::cerr << "ERROR: " << "hako_master_init() error" << std::endl;
        return;
    }
    else {
        std::cout << "INFO: hako_master_init() success" << std::endl;
    }
    hako_master_set_config_simtime(HAKO_RUNNER_MASTER_MAX_DELAY_USEC, HAKO_RUNNER_MASTER_DELTA_USEC);
    pthread_t thread;
    if (pthread_create(&thread, NULL, hako_px4_master_thread_run, nullptr) != 0) {
        std::cerr << "Failed to create hako_px4_master_thread_run thread!" << std::endl;
        return;
    }
    asset_runner();
    //not reached
    return;
}

static IDroneDynamics *drone_dynamics_ground = nullptr;
static IDroneDynamics *drone_dynamics_body = nullptr;

static void my_setup()
{
    drone_dynamics_ground = new DroneDynamicsGroundFrame(HAKO_RUNNER_DELTA_TIME_SEC);
    drone_dynamics_body = new DroneDynamicsBodyFrame(HAKO_RUNNER_DELTA_TIME_SEC);
    std::cout << "INFO: setup start" << std::endl;
    drone_dynamics_ground->set_drag(HAKO_PHYS_DRAG);
    drone_dynamics_body->set_drag(HAKO_PHYS_DRAG);
    drone_pid_control_init();
    std::cout << "INFO: setup done" << std::endl;
    return;
}

static void do_io_write()
{
    Hako_Twist pos;

    DronePositionType dpos = drone_dynamics_ground->get_pos();
    DroneAngleType dangle = drone_dynamics_ground->get_angle();
    pos.linear.x = dpos.data.x;
    pos.linear.y = dpos.data.y;
    pos.linear.z = dpos.data.z;
    pos.angular.x = dangle.data.x;
    pos.angular.y = dangle.data.y;
    pos.angular.z = dangle.data.z;
    if (!hako_asset_runner_pdu_write(HAKO_ROBO_NAME, HAKO_AVATOR_CHANNLE_ID_POS, (const char*)&pos, sizeof(pos))) {
        std::cerr << "ERROR: can not write pdu data: pos" << std::endl;
    }
}

#define POSITION_DIFF_POS_MAX   0.1
#define POSITION_DIFF_ANG_MAX   0.1
#define POSITION_DIFF_VEL_MAX   0.1
static void cross_check_position(void)
{
    const DronePositionType ground_pos = drone_dynamics_ground->get_pos();
    const DronePositionType body_pos = drone_dynamics_body->get_pos();
    if (std::abs(ground_pos.data.x - body_pos.data.x) > POSITION_DIFF_POS_MAX) {
        std::cerr << "WARNING: CROSS_CHECK ERROR<Pos> ground_x: " << ground_pos.data.x << " body_x: " << body_pos.data.x << std::endl;
    }
    if (std::abs(ground_pos.data.y - body_pos.data.y) > POSITION_DIFF_POS_MAX) {
        std::cerr << "WARNING: CROSS_CHECK ERROR<Pos> ground_y: " << ground_pos.data.y << " body_y: " << body_pos.data.y << std::endl;
    }
    if (std::abs(ground_pos.data.z - body_pos.data.z) > POSITION_DIFF_POS_MAX) {
        std::cerr << "WARNING: CROSS_CHECK ERROR<Pos> ground_z: " << ground_pos.data.z << " body_z: " << body_pos.data.z << std::endl;
    }
}
static void cross_check_velocity(void)
{
    const DroneVelocityType ground = drone_dynamics_ground->get_vel();
    const DroneVelocityType body = drone_dynamics_body->get_vel();
    if (std::abs(ground.data.x - body.data.x) > POSITION_DIFF_VEL_MAX) {
        std::cerr << "WARNING: CROSS_CHECK ERROR<Vel> ground_x: " << ground.data.x << " body_x: " << body.data.x << std::endl;
    }
    if (std::abs(ground.data.y - body.data.y) > POSITION_DIFF_VEL_MAX) {
        std::cerr << "WARNING: CROSS_CHECK ERROR<Vel> ground_y: " << ground.data.y << " body_y: " << body.data.y << std::endl;
    }
    if (std::abs(ground.data.z - body.data.z) > POSITION_DIFF_VEL_MAX) {
        std::cerr << "WARNING: CROSS_CHECK ERROR<Vel> ground_z: " << ground.data.z << " body_z: " << body.data.z << std::endl;
    }
}

static void cross_check_angle(void)
{
    const DroneAngleType ground_angle = drone_dynamics_ground->get_angle();
    const DroneAngleType body_angle = drone_dynamics_body->get_angle();
    if (std::abs(ground_angle.data.x - body_angle.data.x) > POSITION_DIFF_ANG_MAX) {
        std::cerr << "WARNING: CROSS_CHECK ERROR<Angle> ground_x: " << ground_angle.data.x << " body_x: " << body_angle.data.x << std::endl;
    }
    if (std::abs(ground_angle.data.y - body_angle.data.y) > POSITION_DIFF_ANG_MAX) {
        std::cerr << "WARNING: CROSS_CHECK ERROR<Angle> ground_y: " << ground_angle.data.y << " body_y: " << body_angle.data.y << std::endl;
    }
    if (std::abs(ground_angle.data.z - body_angle.data.z) > POSITION_DIFF_ANG_MAX) {
        std::cerr << "WARNING: CROSS_CHECK ERROR<Angle> ground_z: " << ground_angle.data.z << " body_z: " << body_angle.data.z << std::endl;
    }
}

static void cross_check_anglular_velocity(void)
{
    const DroneAngularVelocityType ground_angle = drone_dynamics_ground->get_angular_vel();
    const DroneAngularVelocityType body_angle = drone_dynamics_body->get_angular_vel();
    if (std::abs(ground_angle.data.x - body_angle.data.x) > POSITION_DIFF_VEL_MAX) {
        std::cerr << "WARNING: CROSS_CHECK ERROR<AngleVel> ground_x: " << ground_angle.data.x << " body_x: " << body_angle.data.x << std::endl;
    }
    if (std::abs(ground_angle.data.y - body_angle.data.y) > POSITION_DIFF_VEL_MAX) {
        std::cerr << "WARNING: CROSS_CHECK ERROR<AngleVel> ground_y: " << ground_angle.data.y << " body_y: " << body_angle.data.y << std::endl;
    }
    if (std::abs(ground_angle.data.z - body_angle.data.z) > POSITION_DIFF_VEL_MAX) {
        std::cerr << "WARNING: CROSS_CHECK ERROR<AngleVel> ground_z: " << ground_angle.data.z << " body_z: " << body_angle.data.z << std::endl;
    }
}

static void my_task()
{
    DroneThrustType thrust;
    DroneTorqueType torque;
    {
        Hako_Twist control;

        if (!hako_asset_runner_pdu_read(HAKO_ROBO_NAME, HAKO_AVATOR_CHANNLE_ID_CTRL, (char*)&control, sizeof(control))) {
            std::cerr << "ERROR: can not read pdu data: control" << std::endl;
            return;
        }
        thrust.data = control.linear.z;
        torque.data.x = control.angular.x;
        torque.data.y = control.angular.y;
        torque.data.z = control.angular.z;
    }
    hako::assets::drone::DroneDynamicsInputType input;
    input.thrust = thrust;
    input.torque = torque;
    drone_dynamics_ground->run(input);
    drone_dynamics_body->run(input);
#ifdef DRONE_PID_CONTROL_CPP
    drone_pid_control_run();
#endif
    cross_check_position();
    cross_check_velocity();
    cross_check_angle();
    cross_check_anglular_velocity();
    do_io_write();
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

static hako_time_t hako_asset_time = 0;
static void asset_runner()
{
    hako_asset_runner_register_callback(&my_callbacks);
    const char* config_path = hako_param_env_get_string(HAKO_CUSTOM_JSON_PATH);
    if (hako_asset_runner_init(HAKO_ROBO_NAME, config_path, HAKO_RUNNER_MASTER_DELTA_USEC) == false) {
        std::cerr << "ERROR: " << "hako_asset_runner_init() error" << std::endl;
        return;
    }
    while (true) {
        hako_asset_time = 0;
        std::cout << "INFO: start simulation" << std::endl;
        while (true) {
            if (hako_asset_runner_step(1) == false) {
                std::cout << "INFO: stopped simulation" << std::endl;
                break;
            }
            else {
                hako_asset_time += HAKO_RUNNER_MASTER_DELTA_USEC;
            }
        }
    }
    std::cout << "INFO: end simulation" << std::endl;
    hako_asset_runner_fin();

    return;
}

