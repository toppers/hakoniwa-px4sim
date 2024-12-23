#include "service/aircraft/impl/aircraft_service.hpp"

using namespace hako::service;

int main(int argc, const char* argv[])
{
    const char* server_ip = "127.0.0.1";
    const int server_port = 4560;

    // MAVLINK サーバーの設定
    IcommEndpointType server_endpoint = {server_ip, server_port};
    auto mavlink_service = MavLinkService(0, MAVLINK_SERVICE_IO_TYPE_TCP, &server_endpoint, nullptr);
    MavLinkServiceContainer mavlink_service_container;
    mavlink_service_container.addService(mavlink_service);

    // Aircraft サービスの設定
    DroneConfigManager configManager;
    configManager.loadConfigsFromDirectory("./drone_config");

    AirCraftContainer aircraft_container;
    aircraft_container.createAirCrafts(configManager);

    std::unique_ptr<IAircraftService> aircraft_service = std::make_unique<hako::service::impl::AircraftService>(mavlink_service_container, aircraft_container);
    aircraft_service->startService(true, 3000);

    while (true) {
        aircraft_service->advanceTimeStep(0);

        uint64_t time_usec = aircraft_service->getSimulationTimeUsec(0);
        std::cout << "INFO: simulation time(usec)=" << time_usec << std::endl;
        //sleep 3msec
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    return 0;
}