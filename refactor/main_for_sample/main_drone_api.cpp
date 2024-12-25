#include "service/drone/impl/drone_service_container.hpp"
#include "service/aircraft/impl/aircraft_service.hpp"
#include "logger/impl/hako_logger.hpp"
#include <iostream>

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
    int aircraft_num = configManager.getConfigCount();

    AirCraftContainer aircraft_container;
    aircraft_container.createAirCrafts(configManager);

    // AircrfaftControllerの設定
    AircraftControllerContainer controller_container;
    controller_container.createAircraftControllers(configManager);

    DroneServiceContainer service_container(aircraft_container, controller_container);
    return 0;
}