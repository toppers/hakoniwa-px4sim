#ifndef _AIRCRAFT_SERVICE_HPP_
#define _AIRCRAFT_SERVICE_HPP_

#include "service/aircraft/iaircraft_service.hpp"
#include "aircraft/impl/aircraft_container.hpp"
#include "mavlink/mavlink_service.hpp"
#include "mavlink/mavlink_service_container.hpp"

using namespace hako::mavlink;
using namespace hako::aircraft;

namespace hako::service::impl {

class AircraftService : public IAircraftService {
public:
    AircraftService(MavLinkServiceContainer& mavlink_service_container, AirCraftContainer& aircraft_container):
        mavlink_service_container_(mavlink_service_container), aircraft_container_(aircraft_container) 
        {
            if (mavlink_service_container.getServices().size() != aircraft_container.getAllAirCrafts().size()) {
                throw std::runtime_error("MavLinkServiceContainer size is not equal to AirCraftContainer size");
            }
        }
    ~AircraftService() = default;

    bool startService(bool lockStep, uint64_t deltaTimeUsec) override;
    void advanceTimeStep() override;
    void stopService() override;
    void resetService() override;

    virtual bool write_pdu(uint32_t index, HakoniwaPduDataType& pdu) override;
    virtual bool read_pdu(uint32_t index, HakoniwaPduDataType& message) override;

private:
    bool lock_step_ = false;
    uint64_t delta_time_usec_ = 0;
    MavLinkServiceContainer& mavlink_service_container_;
    AirCraftContainer& aircraft_container_;
    std::vector<AircraftInputType> aircraft_inputs_;
};
} // namespace hako::service
#endif /* _AIRCRAFT_SERVICE_HPP_ */