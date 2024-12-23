#include "service/aircraft/impl/aircraft_service.hpp"
#include "logger/impl/hako_logger.hpp"

using namespace hako::service;
using namespace hako::logger;


int main(int argc, const char* argv[])
{
    const char* server_ip = "127.0.0.1";
    const int server_port = 4560;

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
    MavLinkServiceContainer mavlink_service_container;
    for (int i = 0; i < aircraft_num; i++) {
        std::cout << "INFO: aircraft_num=" << i << std::endl;
        // MAVLINK サーバーの設定
        IcommEndpointType server_endpoint = {server_ip, server_port + i};
        auto mavlink_service = new MavLinkService(i, MAVLINK_SERVICE_IO_TYPE_TCP, &server_endpoint, nullptr);
        mavlink_service_container.addService(*mavlink_service);
    }

    auto aircraft_service = std::make_unique<hako::service::impl::AircraftService>(mavlink_service_container, aircraft_container);
    aircraft_service->startService(true, 3000);
    HakoLogger::enable();

    while (true) {
        HakoLogger::set_time_usec(aircraft_service->getSitlTimeUsec(0));
        //std::cout << "INFO: simulation time(usec)=" << time_usec << std::endl;
        for (int i = 0; i < aircraft_num; i++) {
            aircraft_service->advanceTimeStep(i);
        }
        //sleep for real time
        std::this_thread::sleep_for(std::chrono::milliseconds(real_sleep_msec));
    }
    return 0;
}