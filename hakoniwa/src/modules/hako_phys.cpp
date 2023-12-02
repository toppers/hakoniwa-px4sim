#include "hako_phys.hpp"
#include "hako_capi.h"
#include "../utils/hako_params.hpp"
#include "hako_asset_runner.h"
#include "../hako/pdu/hako_pdu_data.hpp"
#include "../hako/runner/hako_px4_master.hpp"
#include "../assets/drone/physics/drone_dynamics.hpp"
#include <unistd.h>
#include <memory.h>
#include <iostream>

#define HAKO_RUNNER_MASTER_MAX_DELAY_USEC       1000 /* usec*/
#define HAKO_RUNNER_MASTER_DELTA_USEC           1000 /* usec*/
#define HAKO_RUNNER_DELTA_TIME_SEC              0.001
#define HAKO_AVATOR_CHANNLE_ID_MOTOR    0
#define HAKO_AVATOR_CHANNLE_ID_POS      1
#define HAKO_AVATOR_CHANNLE_ID_CTRL     2
#define HAKO_PHYS_DRAG          0.001

#define HAKO_ROBO_NAME "px4sim"

static void asset_runner();
using hako::assets::drone::DroneDynamics;

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

static DroneDynamics *drone_dynamics = nullptr;

static void my_setup()
{
    drone_dynamics = new DroneDynamics(HAKO_RUNNER_DELTA_TIME_SEC);
    std::cout << "INFO: setup start" << std::endl;
    drone_dynamics->set_drag(HAKO_PHYS_DRAG);

    std::cout << "INFO: setup done" << std::endl;
    return;
}

static void do_io_write()
{
    Hako_Twist pos;

    DronePositionType dpos = drone_dynamics->get_pos();
    DroneAngleType dangle = drone_dynamics->get_angle();
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

static void my_task()
{
    DroneThrustype thrust;
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
    drone_dynamics->run(thrust, torque);
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

