#ifndef _AIRCRAFT_SERVICE_CONTAINER_HPP_
#define _AIRCRAFT_SERVICE_CONTAINER_HPP_

#include "service/aircraft/iaircraft_service_container.hpp"
#include "aircraft/impl/aircraft_container.hpp"
#include "mavlink/mavlink_service.hpp"
#include "mavlink/mavlink_service_container.hpp"
#include "service/impl/service_pdu_syncher.hpp"

using namespace hako::mavlink;
using namespace hako::aircraft;

namespace hako::service::impl {

class AircraftServiceContainer : public IAircraftServiceContainer {
public:
    AircraftServiceContainer(MavLinkServiceContainer& mavlink_service_container, AirCraftContainer& aircraft_container):
        mavlink_service_container_(mavlink_service_container), aircraft_container_(aircraft_container) 
        {
            if (mavlink_service_container.getServices().size() != aircraft_container.getAllAirCrafts().size()) {
                throw std::runtime_error("MavLinkServiceContainer size is not equal to AirCraftContainer size");
            }
            for (auto& aircraft : aircraft_container_.getAllAirCrafts()) {
                (void)aircraft;
                pdu_synchers_.push_back(std::make_shared<ServicePduSyncher>());
            }
        }
    ~AircraftServiceContainer() = default;

    bool startService(bool lockStep, uint64_t deltaTimeUsec) override;
    bool startService(uint64_t deltaTimeUsec) override
    {
        return startService(true, deltaTimeUsec);
    }
    bool setRealTimeStepUsec(uint64_t deltaTimeUsec) override
    {
        //std::cout << "INFO: AircraftServiceContainer::setRealTimeStepUsec: deltaTimeUsec=" << deltaTimeUsec << std::endl;
        delta_real_time_usec_ = deltaTimeUsec;
        return true;
    }
    void advanceTimeStep(uint32_t index) override;
    void advanceTimeStep() override
    {
        for (uint32_t i = 0; i < aircraft_container_.getAllAirCrafts().size(); i++) {
            advanceTimeStep(i);
        }
        if (delta_real_time_usec_ > 0) {
            std::this_thread::sleep_for(std::chrono::microseconds(delta_real_time_usec_));
        }
    }
    void stopService() override
    {
        throw std::runtime_error("Not implemented");
    }
    void resetService() override;
    uint64_t getSimulationTimeUsec(uint32_t index) override;
    uint64_t getSitlTimeUsec(uint32_t index) override;

    virtual bool write_pdu(uint32_t index, ServicePduDataType& pdu) override
    {
        if (index >= pdu_synchers_.size()) {
            throw std::runtime_error("write_pdu index out of range");
        }
        return pdu_synchers_[index]->flush(index, pdu);
    }
    virtual bool read_pdu(uint32_t index, ServicePduDataType& message) override
    {
        if (index >= pdu_synchers_.size()) {
            throw std::runtime_error("read_pdu index out of range");
        }
        return pdu_synchers_[index]->load(index, message);

    }
    virtual void peek_pdu(uint32_t index, ServicePduDataType& message) override
    {
        read_pdu(index, message);
    }

    uint32_t getNumServices() override
    {
        return aircraft_container_.getAllAirCrafts().size();
    }
    std::string getRobotName(uint32_t index) override
    {
        return aircraft_container_.getAirCraft(index)->get_name();
    }
    void setPduSyncher(std::shared_ptr<IServicePduSyncher> pdu_syncher) override
    {
        for (auto& aircraft : aircraft_container_.getAllAirCrafts()) {
            pdu_synchers_[aircraft->get_index()] = pdu_syncher;
        }
    }
private:
    static const uint64_t gps_send_cycle = 10;
    bool lock_step_ = false;
    uint64_t delta_time_usec_ = 0;
    uint64_t activated_time_usec_ = 0;
    uint64_t delta_real_time_usec_ = 0;
    std::vector<uint64_t> send_count_;
    std::vector<uint64_t> sitl_simulation_time_usec_;
    MavLinkServiceContainer& mavlink_service_container_;
    AirCraftContainer& aircraft_container_;
    std::vector<AircraftInputType> aircraft_inputs_;
    std::vector<std::shared_ptr<IServicePduSyncher>> pdu_synchers_;

    void send_sensor_data(IAirCraft& aircraft, uint64_t activated_time_usec);
    void advanceTimeStepLockStep(uint32_t index, uint64_t& sitl_time_usec);
    void advanceTimeStepFreeRun(uint32_t index, uint64_t& sitl_time_usec);

    void setup_aircraft_inputs(uint32_t index);
    void write_back_pdu(uint32_t index);

};
} // namespace hako::service
#endif /* _AIRCRAFT_SERVICE_CONTAINER_HPP_ */