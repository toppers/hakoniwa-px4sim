#include "service/drone/impl/drone_service_container.hpp"
#include "service/aircraft/impl/aircraft_service.hpp"
#include "service/drone/drone_service_api_protocol.hpp"
#include "logger/impl/hako_logger.hpp"
#include <iostream>
#include <thread>
using namespace hako::aircraft;
using namespace hako::service;
using namespace hako::service::impl;
using namespace hako::logger;


int main(int argc, const char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << "<real_sleep_msec> <drone_config_dir_path>" << std::endl;
        return 1;
    }
    int real_sleep_msec = std::stoi(argv[1]);
    const char* drone_config_dir_path = argv[2];
    // Aircraft サービスの設定
    DroneConfigManager configManager;
    configManager.loadConfigsFromDirectory(drone_config_dir_path);

    AirCraftContainer aircraft_container;
    aircraft_container.createAirCrafts(configManager);

    // AircrfaftControllerの設定
    AircraftControllerContainer controller_container;
    controller_container.createAircraftControllers(configManager);

    DroneServiceContainer service_container(aircraft_container, controller_container);

    auto ret = service_container.startService(1000);
    if (!ret) {
        std::cerr << "Failed to start service" << std::endl;
        return 1;
    }
    HakoLogger::enable();
    //以下の処理をスレッドで実行する
    std::thread th([&service_container, real_sleep_msec]() {
        while (true) {
            HakoLogger::set_time_usec(service_container.getSimulationTimeUsec(0));
            service_container.advanceTimeStep();
            std::this_thread::sleep_for(std::chrono::milliseconds(real_sleep_msec * 1000));
        }
    });
    DroneServiceApiProtocol api(service_container);
    std::cout << "INFO: takeoff start" << std::endl;
    api.takeoff(0, 10.0);
    std::cout << "INFO: takeoff done" << std::endl;
    auto pos = api.get_position(0);
    std::cout << "INFO: position=(" << pos.x << "," << pos.y << "," << pos.z << ")" << std::endl;
    th.join();
    return 0;
}