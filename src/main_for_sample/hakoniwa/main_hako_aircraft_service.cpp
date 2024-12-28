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
    const char* server_ip = "127.0.0.1";
    const int server_port = 4560;

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
    int aircraft_num = configManager.getConfigCount();

    // MavlinkContainerの設定
    MavLinkServiceContainer mavlink_service_container;
    for (int i = 0; i < aircraft_num; i++) {
        std::cout << "INFO: aircraft_num=" << i << std::endl;
        // MAVLINK サーバーの設定
        IcommEndpointType server_endpoint = {server_ip, server_port + i};
        auto mavlink_service = new MavLinkService(i, MAVLINK_SERVICE_IO_TYPE_TCP, &server_endpoint, nullptr);
        mavlink_service_container.addService(*mavlink_service);
    }

    std::shared_ptr<IServiceContainer> aircraft_service = std::make_unique<hako::service::impl::AircraftServiceContainer>(mavlink_service_container, aircraft_container);
    aircraft_service->setRealTimeStepUsec(1000); //1msec

    HakoLogger::enable();

    std::string asset_name = "drone";
    std::string config_path = custom_json_path;
    std::cout << "asset_name: " << asset_name << std::endl;
    auto hako_drone_service = HakoniwaDroneService::getInstance();    
    hako_drone_service->registerService(
        asset_name, 
        config_path, 
        3000, 20000, 
        aircraft_service);
    std::cout << "HakoniwaDroneService::registerService() done" << std::endl;
    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_COLLISION, 2);
    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_DISTURBANCE, 4);

    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_ACTUATOR_CONTROLS, 0);
    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_POSITION, 1);
    hako_drone_service->setPduIdMap(SERVICE_PDU_DATA_ID_TYPE_BATTERY_STATUS, 9);

    hako_drone_service->startService();


    return 0;
}