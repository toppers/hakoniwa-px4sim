#include "service/aircraft/impl/aircraft_service.hpp"

bool hako::service::impl::AircraftService::startService(bool lockStep, uint64_t deltaTimeUsec)
{
    lock_step_ = lockStep;
    delta_time_usec_ = deltaTimeUsec;
    std::cout << "INFO: lockStep=" << lockStep << std::endl;
    std::cout << "INFO: deltaTimeUsec=" << deltaTimeUsec << std::endl;
    aircraft_inputs_.resize(aircraft_container_.getAllAirCrafts().size());
    int index = 0;
    for (std::reference_wrapper<MavLinkService> mavlink_service: mavlink_service_container_.getServices()) {
        if (!mavlink_service.get().startService()) {
            throw std::runtime_error("Failed to start mavlink service");
        }
        aircraft_inputs_[index] = {};
        index++;
    }
    return true;
}

void hako::service::impl::AircraftService::advanceTimeStep()
{
    std::cout << "INFO: advanceTimeStep" << std::endl;
    for (IAirCraft* aircraft : aircraft_container_.getAllAirCrafts()) {
        AircraftInputType aircraft_input = {};
        MavlinkHakoMessage message;
        message.type = MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS;
        if (!mavlink_service_container_.getServices()[aircraft->get_index()].get().readMessage(message)) {
            throw std::runtime_error("Failed to read HIL_ACTUATOR_CONTROLS message on aircraft: " + std::to_string(aircraft->get_index()));
        }
        aircraft_input.no_use_actuator = false;
        aircraft_input.manual.control = false;
        for (int i = 0; i < hako::aircraft::ROTOR_NUM; i++) {
            aircraft_inputs_[aircraft->get_index()].controls[i] = message.data.hil_actuator_controls.controls[i];
        }
        std::cout << "INFO: advanceTimeStep: " << aircraft->get_index() << std::endl;
        aircraft->run(aircraft_inputs_[aircraft->get_index()]);
    }
}

void hako::service::impl::AircraftService::stopService()
{
    for (std::reference_wrapper<MavLinkService> mavlink_service: mavlink_service_container_.getServices()) {
        mavlink_service.get().stopService();
    }
}

void hako::service::impl::AircraftService::resetService()
{
    for (IAirCraft* aircraft : aircraft_container_.getAllAirCrafts()) {
        aircraft->reset();
    }
}

bool hako::service::impl::AircraftService::write_pdu(uint32_t index, HakoniwaPduDataType& pdu)
{
    if (index < 0 || index >= aircraft_inputs_.size()) {
        return false;
    }

    if (pdu.id == HAKONIWA_PDU_DATA_ID_TYPE_AIRCRAFT_DISTURBANCE) {
        aircraft_inputs_[index].disturbance.values.d_temp.value = pdu.data.disturbance.d_temp.value;
        aircraft_inputs_[index].disturbance.values.d_wind.x = pdu.data.disturbance.d_wind.value.x;
        aircraft_inputs_[index].disturbance.values.d_wind.y = pdu.data.disturbance.d_wind.value.y;
        aircraft_inputs_[index].disturbance.values.d_wind.z = pdu.data.disturbance.d_wind.value.z;
    }
    else if (pdu.id == HAKONIWA_PDU_DATA_ID_TYPE_AIRCRAFT_COLLISION) {
        aircraft_inputs_[index].collision.collision = pdu.data.collision.collision;
        aircraft_inputs_[index].collision.contact_num = pdu.data.collision.contact_num;
        aircraft_inputs_[index].collision.relative_velocity.x = pdu.data.collision.relative_velocity.x;
        aircraft_inputs_[index].collision.relative_velocity.y = pdu.data.collision.relative_velocity.y;
        aircraft_inputs_[index].collision.relative_velocity.z = pdu.data.collision.relative_velocity.z;
        aircraft_inputs_[index].collision.restitution_coefficient = pdu.data.collision.restitution_coefficient;
        for (int i = 0; i < MAX_CONTAT_NUM; i++) {
            aircraft_inputs_[index].collision.contact_position[i].x = pdu.data.collision.contact_position[i].x;
            aircraft_inputs_[index].collision.contact_position[i].y = pdu.data.collision.contact_position[i].y;
            aircraft_inputs_[index].collision.contact_position[i].z = pdu.data.collision.contact_position[i].z;
        }
    }
    else {
        std::cerr << "ERROR: write_pdu() Invalid PDU ID: " << pdu.id  << std::endl;
        return false;
    }
    return true;
}

bool hako::service::impl::AircraftService::read_pdu(uint32_t index, HakoniwaPduDataType& pdu)
{
    if (index < 0 || index >= aircraft_inputs_.size()) {
        return false;
    }
    switch (pdu.id) {
        case HAKONIWA_PDU_DATA_ID_TYPE_AIRCRAFT_CONTROLS:
            pdu.data.aircraft_controls.controls[0] = aircraft_inputs_[index].controls[0];
            pdu.data.aircraft_controls.controls[1] = aircraft_inputs_[index].controls[1];
            pdu.data.aircraft_controls.controls[2] = aircraft_inputs_[index].controls[2];
            pdu.data.aircraft_controls.controls[3] = aircraft_inputs_[index].controls[3];
            break;
        case HAKONIWA_PDU_DATA_ID_TYPE_AIRCRAFT_POSITION:
            pdu.data.aircraft_position.linear.x = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_pos().data[0];
            pdu.data.aircraft_position.linear.y = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_pos().data[1];
            pdu.data.aircraft_position.linear.z = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_pos().data[2];
            pdu.data.aircraft_position.angular.x = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_angle().data[0];
            pdu.data.aircraft_position.angular.y = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_angle().data[1];
            pdu.data.aircraft_position.angular.z = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_angle().data[2];
            break;
        default:
            std::cerr << "ERROR: read_pdu() Invalid PDU ID: " << pdu.id  << std::endl;
            return false;
    }
    return true;
}
