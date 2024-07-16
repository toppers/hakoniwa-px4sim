#include "hako_sim.hpp"
#include "hako_capi.h"
#include "assets/drone/mavlink/mavlink_io.hpp"
#include "assets/drone/aircraft/aircraft_factory.hpp"
#include "utils/hako_params.hpp"
#include "hako_asset_runner.h"
#include "hako/runner/hako_px4_master.hpp"
#include "threads/px4sim_thread_sender.hpp"
#include "threads/px4sim_thread_receiver.hpp"
#include "config/drone_config.hpp"
#include "hako/pdu/hako_pdu_accessor.hpp"
#include "comm/tcp_connector.hpp"
#include "utils/hako_osdep.h"
#include <memory.h>
#include <iostream>
#include <thread>

#define HAKO_RUNNER_MASTER_MAX_DELAY_USEC       1000 /* usec*/

#define HAKO_ASSET_NAME "px4sim"

static void* asset_runner(void*);

static hako::assets::drone::AirCraftManager drone_manager;

void hako_sim_main(bool master, hako::px4::comm::IcommEndpointType serverEndpoint)
{
    if (hako::px4::comm::comm_init() != 0) {
        std::cerr << "ERROR: can not init tcp comm on " << std::endl;
        return;
    }
    std::string drone_config_directory = hako_param_env_get_string(DRONE_CONFIG_PATH);
    if (drone_config_manager.loadConfigsFromDirectory(drone_config_directory) == 0)
    {
        std::cerr << "ERROR: can not find drone config file on " << drone_config_directory << std::endl;
        return;
    }
    hako::px4::comm::TcpServer server;
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
        
        try {
            std::thread thread(hako_px4_master_thread_run, (void*)nullptr);
            thread.detach();
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to create hako_px4_master_thread_run thread: " << e.what() << std::endl;
            return;
        }

    }
    try {
        std::thread thread(asset_runner, (void*)nullptr);
        thread.detach();
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to create asset_runner thread: " << e.what() << std::endl;
        return;
    }
    size_t configCount = drone_config_manager.getConfigCount();
    std::vector<std::thread> threads;
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
        try {
            threads.emplace_back(px4sim_thread_receiver, (void*)&rcv_arg[i]);
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to create px4sim_thread_receiver thread: " << e.what() << std::endl;
            return;
        }
    }
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    //not reached
    return;
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
    void reset() {
        drone->reset();
    }
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
    void send_sensor_data(Hako_uint64 _hako_asset_time_usec, Hako_uint64 microseconds)
    {
        for (auto& container : aircraft_container) {
            container.mavlink_io.write_sensor_data(*container.drone);
            px4sim_send_sensor_data(container.drone->get_index(), _hako_asset_time_usec, microseconds);
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
        hako::assets::drone::DroneDynamicsInputType drone_input = {};
        drone_input.no_use_actuator = false;
        drone_input.manual.control = false;
        if (container.drone->get_drone_dynamics().has_collision_detection()) {
            do_io_read_collision(container.drone, drone_input.collision);
        }
        if (container.drone->get_drone_dynamics().has_manual_control()) {
            do_io_read_manual(container.drone, drone_input.manual);
        }
        if (container.drone->is_enabled_disturbance()) {
            do_io_read_disturb(container.drone, drone_input.disturbance);
        }
        for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
            drone_input.controls[i] = container.controls[i];
        }
        container.drone->run(drone_input);
        do_io_write(container.drone, container.controls);
    }
    return;
}

static void my_setup()
{
    for (auto& container : task_manager.aircraft_container) {
        std::cout << "INFO: Setup pdu data : " << container.drone->get_name() << std::endl;
        Hako_GameControllerOperation cmd_game = {};
        do_io_write_cmd(container.drone, HAKO_AVATOR_CHANNEL_ID_GAME_CTRL, cmd_game);
    }
}
static void my_reset()
{
    for (auto& container : task_manager.aircraft_container) {
        container.reset();
    }
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

