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
    void advanceTimeStep(uint32_t index) override;
    void stopService() override
    {
        throw std::runtime_error("Not implemented");
    }
    void resetService() override;
    uint64_t getSimulationTimeUsec(uint32_t index) override;
    uint64_t getSitlTimeUsec(uint32_t index) override;

    virtual bool write_pdu(uint32_t index, ServicePduDataType& pdu) override;
    virtual bool read_pdu(uint32_t index, ServicePduDataType& message) override;

private:
    static const uint64_t gps_send_cycle = 10;
    bool lock_step_ = false;
    uint64_t delta_time_usec_ = 0;
    uint64_t activated_time_usec_ = 0;
    std::vector<uint64_t> send_count_;
    std::vector<uint64_t> sitl_simulation_time_usec_;
    MavLinkServiceContainer& mavlink_service_container_;
    AirCraftContainer& aircraft_container_;
    std::vector<AircraftInputType> aircraft_inputs_;

    void send_sensor_data(IAirCraft& aircraft, uint64_t activated_time_usec);
    void advanceTimeStepLockStep(uint32_t index, uint64_t& sitl_time_usec);
    void advanceTimeStepFreeRun(uint32_t index, uint64_t& sitl_time_usec);
};
} // namespace hako::service
#endif /* _AIRCRAFT_SERVICE_HPP_ */