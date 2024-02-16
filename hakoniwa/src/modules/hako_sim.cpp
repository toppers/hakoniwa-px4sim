#include "hako_sim.hpp"
#include "hako_capi.h"
#include "assets/drone/mavlink/mavlink_io.hpp"
#include "assets/drone/aircraft/aircraft_factory.hpp"
#include "utils/hako_params.hpp"
#include "hako_asset_runner.h"
#include "hako/pdu/hako_pdu_data.hpp"
#include "hako/runner/hako_px4_master.hpp"
#include "threads/px4sim_thread_sender.hpp"
#include "threads/px4sim_thread_receiver.hpp"
#include "config/drone_config.hpp"

#include <unistd.h>
#include <memory.h>
#include <iostream>

#define HAKO_RUNNER_MASTER_MAX_DELAY_USEC       1000 /* usec*/
#define HAKO_AVATOR_CHANNLE_ID_MOTOR        0
#define HAKO_AVATOR_CHANNLE_ID_POS          1
#define HAKO_AVATOR_CHANNLE_ID_COLLISION    2
#define HAKO_AVATOR_CHANNLE_ID_MANUAL       3

#define HAKO_ASSET_NAME "px4sim"

static void* asset_runner(void*);

static hako::assets::drone::AirCraftManager drone_manager;

void hako_sim_main(bool master, hako::px4::comm::IcommEndpointType serverEndpoint)
{
    hako::px4::comm::TcpServer server;
    pthread_t thread;
    DroneConfig drone_config;
    if (drone_config_manager.getConfig(0, drone_config) == false) {
        std::cerr << "ERROR: " << "drone_config_manager.getConfig() error" << std::endl;
        return;
    }
    if (master) {
        if (!hako_master_init()) {
            std::cerr << "ERROR: " << "hako_master_init() error" << std::endl;
            return;
        }
        else {
            std::cout << "INFO: hako_master_init() success" << std::endl;
        }
        hako_master_set_config_simtime((drone_config.getSimTimeStep()*1000000), (drone_config.getSimTimeStep()*1000000));
        
        if (pthread_create(&thread, NULL, hako_px4_master_thread_run, nullptr) != 0) {
            std::cerr << "Failed to create hako_px4_master_thread_run thread!" << std::endl;
            return;
        }
    }
    if (pthread_create(&thread, NULL, asset_runner, nullptr) != 0) {
        std::cerr << "Failed to create asset_runner thread!" << std::endl;
        return;
    }
    size_t configCount = drone_config_manager.getConfigCount();
    std::vector<pthread_t> threads(configCount);
    std::vector<Px4simRcvArgType> rcv_arg(configCount);
    for (size_t i = 0; i < configCount; ++i) {
        hako::px4::comm::IcommEndpointType ep = serverEndpoint;
        ep.portno = serverEndpoint.portno + i;
        auto comm_io = server.server_open(&ep);
        if (comm_io == nullptr) 
        {
            std::cerr << "Failed to open TCP server" << std::endl;
            return;
        }
        px4sim_sender_init(comm_io);
        rcv_arg[i].index = i;
        rcv_arg[i].comm_io = comm_io;
        if (pthread_create(&threads[i], NULL, px4sim_thread_receiver, &rcv_arg[i]) != 0) {
            std::cerr << "Failed to create asset_runner thread!" << std::endl;
            return;
        }
    }
    for (size_t i = 0; i < configCount; ++i) {
        if (pthread_join(threads[i], nullptr) != 0) {
            std::cerr << "Failed to join thread " << i << std::endl;
        }
    }

    //not reached
    return;
}


static void my_setup()
{
    //nothing to do
}
static void debug_print(hako::assets::drone::DroneDynamicsCollisionType& drone_collision)
{
    std::cout << "Collision: " << (drone_collision.collision ? "Yes" : "No") << std::endl;
    std::cout << "Contact Number: " << drone_collision.contact_num << std::endl;
    std::cout << "Relative Velocity: (" 
                    << drone_collision.relative_velocity.x << ", " 
                    << drone_collision.relative_velocity.y << "," 
                    << drone_collision.relative_velocity.z << ")"
                    << std::endl;

    for (int i = 0; i < drone_collision.contact_num; ++i) {
        std::cout << "Contact Positions[" << i << "]: (" 
                    << drone_collision.contact_position[i].x << ", " 
                    << drone_collision.contact_position[i].y << "," 
                    << drone_collision.contact_position[i].z << ")"
                    << std::endl;
    }
    std::cout << "Restitution Coefficient: " << drone_collision.restitution_coefficient << std::endl;
}

static void do_io_read_collision(hako::assets::drone::IAirCraft *drone, hako::assets::drone::DroneDynamicsCollisionType& drone_collision)
{
    Hako_Collision hako_collision;
    memset(&drone_collision, 0, sizeof(drone_collision));
    if (!hako_asset_runner_pdu_read(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_COLLISION, (char*)&hako_collision, sizeof(hako_collision))) {
        std::cerr << "ERROR: can not read pdu data: Hako_Collision" << std::endl;
    }
    drone_collision.collision = hako_collision.collision;
    if (drone_collision.collision) {
        drone_collision.contact_num = hako_collision.contact_num;
        //ROS座標系 ==> 航空座標系
        drone_collision.relative_velocity.x = hako_collision.relative_velocity.x;
        drone_collision.relative_velocity.y = -hako_collision.relative_velocity.y;
        drone_collision.relative_velocity.z = -hako_collision.relative_velocity.z;
        drone_collision.restitution_coefficient = hako_collision.restitution_coefficient;
        for (int i = 0; i < drone_collision.contact_num; i++) {
            drone_collision.contact_position[i].x = hako_collision.contact_position[i].x;
            drone_collision.contact_position[i].y = -hako_collision.contact_position[i].y;
            drone_collision.contact_position[i].z = -hako_collision.contact_position[i].z;
        }
        debug_print(drone_collision);
        /*
         * Unityのシミュレーションは20msec周期で動作する。
         * 一方、こちらは 3msec周期で動作するので、衝突データを打ち消しておかないと、次のタイミングで拾ってしまう。
         */
        hako_collision.collision = false;
        if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_COLLISION, (const char*)&hako_collision, sizeof(hako_collision))) {
            std::cerr << "ERROR: can not write pdu data: Hako_Collision" << std::endl;
        }
    }
}
static void do_io_read_manual(hako::assets::drone::IAirCraft *drone, hako::assets::drone::DroneDynamicsManualControlType& drone_manual)
{
    Hako_ManualPosAttControl hako_manual;
    memset(&hako_manual, 0, sizeof(hako_manual));
    if (!hako_asset_runner_pdu_read(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_MANUAL, (char*)&hako_manual, sizeof(hako_manual))) {
        std::cerr << "ERROR: can not read pdu data: Hako_ManualPosAttControl" << std::endl;
    }
    drone_manual.control = hako_manual.do_operation;
    if (drone_manual.control) {
        std::cout << "manual set angle( " << hako_manual.posatt.angular.x << ", " << hako_manual.posatt.angular.y << ", " << hako_manual.posatt.angular.z << " )" << std::endl;
        hako_manual.do_operation = false;
        drone_manual.angle.data.x = hako_manual.posatt.angular.x;
        drone_manual.angle.data.y = hako_manual.posatt.angular.y;
        drone_manual.angle.data.z = hako_manual.posatt.angular.z;
        drone_manual.pos.data.x = hako_manual.posatt.linear.x;
        drone_manual.pos.data.y = hako_manual.posatt.linear.y;
        drone_manual.pos.data.z = hako_manual.posatt.linear.z;
        if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_MANUAL, (const char*)&hako_manual, sizeof(hako_manual))) {
            std::cerr << "ERROR: can not write pdu data: Hako_ManualPosAttControl" << std::endl;
        }
    }
}
static void do_io_write(hako::assets::drone::IAirCraft *drone, double controls[hako::assets::drone::ROTOR_NUM])
{
    Hako_HakoHilActuatorControls hil_actuator_controls;
    Hako_Twist pos;

    memset(&hil_actuator_controls, 0, sizeof(hil_actuator_controls));
    for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
        hil_actuator_controls.controls[i] = controls[i];
    }
    if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_MOTOR, (const char*)&hil_actuator_controls, sizeof(hil_actuator_controls))) {
        std::cerr << "ERROR: can not write pdu data: hil_actuator_controls" << std::endl;
    }

    DronePositionType dpos = drone->get_drone_dynamics().get_pos();
    DroneEulerType dangle = drone->get_drone_dynamics().get_angle();
    pos.linear.x = dpos.data.x;
    pos.linear.y = -dpos.data.y;
    pos.linear.z = -dpos.data.z;
    pos.angular.x = dangle.data.x;
    pos.angular.y = -dangle.data.y;
    pos.angular.z = -dangle.data.z;
    if (!hako_asset_runner_pdu_write(drone->get_name().c_str(), HAKO_AVATOR_CHANNLE_ID_POS, (const char*)&pos, sizeof(pos))) {
        std::cerr << "ERROR: can not write pdu data: pos" << std::endl;
    }
}
#include <chrono>
#include "utils/csv_logger.hpp"
bool CsvLogger::enable_flag = false;
uint64_t CsvLogger::time_usec = 0; 
class AircraftContainer
{
public:
    IAirCraft *drone;
    bool isRecvControl = false;
    int recv_count = 0;
    double controls[hako::assets::drone::ROTOR_NUM] = { 0, 0, 0, 0};
    hako::assets::drone::MavlinkIO mavlink_io;
    Hako_uint64 px4_time_usec;
};

class AircraftTaskManager
{
private:
    void clear_rcv_flags()
    {
        for (auto& container : aircraft_container) {
            container.isRecvControl = false;
        }
    }
    bool is_all_rcv_once()
    {
        for (auto& container : aircraft_container) {
            if (container.recv_count == 0) {
                return false;
            }
        }
        return true;
    }
    void send_sensor_data(Hako_uint64 hako_asset_time_usec, Hako_uint64 microseconds)
    {
        for (auto& container : aircraft_container) {
            container.mavlink_io.write_sensor_data(*container.drone);
            px4sim_send_sensor_data(container.drone->get_index(), hako_asset_time_usec, microseconds);
        }
    }
    bool recv_actuator_controls()
    {
        for (auto& container : aircraft_container) {
            if (container.mavlink_io.read_actuator_data(*container.drone, container.controls, container.px4_time_usec)) {
                container.isRecvControl = true;
                container.recv_count++;
            }
        }

        for (auto& container : aircraft_container) {
            if (container.isRecvControl == false) {
                return false;
            }
        }
        return true;
    }
    bool do_asset_task_nolockstep()
    {
        clear_rcv_flags();
        recv_actuator_controls();
        if (aircraft_container[0].isRecvControl) {
            CsvLogger::enable();
            CsvLogger::set_time_usec(aircraft_container[0].px4_time_usec);
            aircraft_container[0].isRecvControl = false;
        }
        if (hako_asset_runner_step(1) == false) {
            std::cout << "INFO: stopped simulation" << std::endl;
            return false;
        }
        hako_asset_time_usec += delta_time_usec;
        send_sensor_data(hako_asset_time_usec, activated_time_usec);
        return true;
    }

    bool do_asset_task_lockstep()
    {
        clear_rcv_flags();
        while (true) {
            if (recv_actuator_controls()) {
                break;
            }
            usleep(delta_time_usec); //1msec sleep
        }
        CsvLogger::enable();
        CsvLogger::set_time_usec(aircraft_container[0].px4_time_usec);
        if (hako_asset_runner_step(1) == false) {
            std::cout << "INFO: stopped simulation" << std::endl;
            return false;
        }
        hako_asset_time_usec += delta_time_usec;
        send_sensor_data(hako_asset_time_usec, activated_time_usec);
        return true;
    }
public:
    std::vector<AircraftContainer> aircraft_container;
    Hako_uint64 hako_asset_time_usec;
    Hako_uint64 delta_time_usec;
    Hako_uint64 activated_time_usec;
    void init(Hako_uint64 microseconds, Hako_uint64 dt_usec)
    {
        activated_time_usec = microseconds;
        hako_asset_time_usec = microseconds;
        delta_time_usec = dt_usec;
        drone_manager.createAirCrafts(drone_config_manager);
        for (auto* drone : drone_manager.getAllAirCrafts()) {
            AircraftContainer arg;
            arg.drone = drone;
            aircraft_container.push_back(arg);
        }
    }
    void do_task(bool lockStep)
    {
        std::cout << "INFO: lockStep=" << lockStep << std::endl;
        if (lockStep)
        {
            //prepare
            std::cout << "INFO: lock free mode for all instance received controls" << std::endl;
            while (is_all_rcv_once() == false)
            {
                if (do_asset_task_nolockstep() == false) {
                    return;
                }
            }
            std::cout << "INFO: move to lockstep mode" << std::endl;
            //do lockstep
            while (do_asset_task_lockstep() == true){};
        }
        else {
            std::cout << "INFO: lock free mode" << std::endl;
            while (do_asset_task_nolockstep() == true){};
        }
    }
};
static AircraftTaskManager task_manager;

static void my_task()
{
    for (auto& container : task_manager.aircraft_container) {
        hako::assets::drone::DroneDynamicsInputType drone_input;
        drone_input.no_use_actuator = false;
        drone_input.manual.control = false;
        if (container.drone->get_drone_dynamics().has_collision_detection()) {
            do_io_read_collision(container.drone, drone_input.collision);
        }
        if (container.drone->get_drone_dynamics().has_manual_control()) {
            do_io_read_manual(container.drone, drone_input.manual);
        }
        for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
            drone_input.controls[i] = container.controls[i];
        }
        container.drone->run(drone_input);
        do_io_write(container.drone, container.controls);
    }
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

static void* asset_runner(void*)
{
    std::cout << "INFO: setup start" << std::endl;
    DroneConfig drone_config;
    //TODO multi: インスタンスIDを引数でもらう
    if (drone_config_manager.getConfig(0, drone_config) == false) {
        std::cerr << "ERROR: " << "drone_config_manager.getConfig() error" << std::endl;
        return nullptr;
    }
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    Hako_uint64 delta_time_usec = static_cast<Hako_uint64>(drone_config.getSimTimeStep() * 1000000.0);
    px4sim_receiver_init(drone_config_manager);
    hako_pdu_data_init(drone_config_manager);
    task_manager.init(microseconds, delta_time_usec);
    bool lockstep = drone_config.getSimLockStep();
    hako_asset_runner_register_callback(&my_callbacks);
    const char* config_path = hako_param_env_get_string(HAKO_CUSTOM_JSON_PATH);
    if (hako_asset_runner_init(HAKO_ASSET_NAME, config_path, delta_time_usec) == false) {
        std::cerr << "ERROR: " << "hako_asset_runner_init() error" << std::endl;
        return nullptr;
    }
    while (true) {
        std::cout << "INFO: start simulation" << std::endl;
        task_manager.do_task(lockstep);
    }
    std::cout << "INFO: end simulation" << std::endl;
    hako_asset_runner_fin();

    return nullptr;
}

