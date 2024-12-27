#include "service/drone/impl/drone_service_container.hpp"
#include "service/aircraft/impl/aircraft_service_container.hpp"
#include "hakoniwa/impl/hakoniwa_drone_service.hpp"
#include "logger/impl/hako_logger.hpp"
#include <iostream>
#include <thread>
#include <sstream>
#include <vector>
#include <string>

using namespace hako::aircraft;
using namespace hako::service;
using namespace hako::service::impl;
using namespace hako::logger;
using namespace hako::drone::impl;


int main(int argc, const char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << "<drone_config_dir_path> <custom_json_path>" << std::endl;
        return 1;
    }
    const char* drone_config_dir_path = argv[1];
    const char* custom_json_path = argv[2];
    // Aircraft サービスの設定
    DroneConfigManager configManager;
    configManager.loadConfigsFromDirectory(drone_config_dir_path);

    AirCraftContainer aircraft_container;
    aircraft_container.createAirCrafts(configManager);

    // AircrfaftControllerの設定
    AircraftControllerContainer controller_container;
    controller_container.createAircraftControllers(configManager);

    auto service_container = std::make_shared<DroneServiceContainer>(aircraft_container, controller_container);

    HakoLogger::enable();

    std::string asset_name = "drone";
    std::string config_path = custom_json_path;
    std::cout << "asset_name: " << asset_name << std::endl;
    auto hako_drone_service = HakoniwaDroneService::getInstance();    
    hako_drone_service->registerService(
        asset_name, 
        config_path, 
        1000, 20000, 
        service_container);
    std::cout << "HakoniwaDroneService::registerService() done" << std::endl;
    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_COLLISION, 2);
    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_DISTURBANCE, 4);
    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_TAKEOFF, 5);
    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_MOVE, 6);
    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_LAND, 7);
    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_GAME_CTRL, 8);

    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_ACTUATOR_CONTROLS, 0);
    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_POSITION, 1);
    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_BATTERY_STATUS, 9);


    hako_drone_service->startService();


    return 0;
}