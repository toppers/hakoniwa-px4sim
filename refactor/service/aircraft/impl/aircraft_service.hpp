#ifndef _AIRCRAFT_SERVICE_HPP_
#define _AIRCRAFT_SERVICE_HPP_

#include "service/aircraft/iaircraft_service.hpp"
#include "aircraft/impl/aircraft_container.hpp"
#include "mavlink/mavlink_service.hpp"

using namespace hako::mavlink;
using namespace hako::aircraft;

namespace hako::service::impl {

class AircraftService : public IAircraftService {
public:
    AircraftService() = default;
    ~AircraftService() = default;

    void startService(bool lockStep, uint64_t deltaTimeUsec) override;
    void advanceTimeStep() override {}
    void stopService() override {}
    void resetService() override {}

    bool write() override { return true; }
    bool read() override { return true; }
private:

};
} // namespace hako::service
#endif /* _AIRCRAFT_SERVICE_HPP_ */