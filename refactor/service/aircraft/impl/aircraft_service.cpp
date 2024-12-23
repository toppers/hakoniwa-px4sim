#include "service/aircraft/impl/aircraft_service.hpp"

void hako::service::impl::AircraftService::startService(bool lockStep, uint64_t deltaTimeUsec)
{
    std::cout << "INFO: lockStep=" << lockStep << std::endl;
    std::cout << "INFO: deltaTimeUsec=" << deltaTimeUsec << std::endl;
}
