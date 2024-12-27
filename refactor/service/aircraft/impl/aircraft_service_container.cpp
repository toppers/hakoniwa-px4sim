#include "service/aircraft/impl/aircraft_service_container.hpp"
#include "service/aircraft/impl/aricraft_mavlink_message_buider.hpp"

bool hako::service::impl::AircraftServiceContainer::startService(bool lockStep, uint64_t deltaTimeUsec)
{
    lock_step_ = lockStep;
    delta_time_usec_ = deltaTimeUsec;
    send_count_.resize(aircraft_container_.getAllAirCrafts().size());
    sitl_simulation_time_usec_.resize(aircraft_container_.getAllAirCrafts().size());
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    activated_time_usec_ = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    std::cout << "INFO: lockStep=" << lockStep << std::endl;
    std::cout << "INFO: deltaTimeUsec=" << deltaTimeUsec << std::endl;
    aircraft_inputs_.resize(aircraft_container_.getAllAirCrafts().size());
    int index = 0;
    for (std::reference_wrapper<MavLinkService> mavlink_service: mavlink_service_container_.getServices()) {
        std::cout << "INFO: AircraftService startService wait for connection : " << index << std::endl;
        if (!mavlink_service.get().startService()) {
            throw std::runtime_error("Failed to start mavlink service");
        }
        std::cout << "INFO: AircraftService startService is returned : " << index << std::endl;
        aircraft_inputs_[index] = {};
        aircraft_container_.getAllAirCrafts()[index]->set_delta_time_usec(deltaTimeUsec);
        aircraft_container_.getAllAirCrafts()[index]->reset();
        index++;
    }
    std::cout << "INFO: AircraftService started" << std::endl;
    return true;
}

void hako::service::impl::AircraftServiceContainer::advanceTimeStep(uint32_t index)
{
    if (index >= static_cast<uint32_t>(aircraft_container_.getAllAirCrafts().size())) {
        throw std::runtime_error("Invalid index for advanceTimeStep : " + std::to_string(index));
    }
    uint64_t sitl_time_usec = 0;
    if (lock_step_) {
        advanceTimeStepLockStep(index, sitl_time_usec);
    }
    else {
        advanceTimeStepFreeRun(index, sitl_time_usec);
    }
    if (sitl_time_usec > sitl_simulation_time_usec_[index]) {
        sitl_simulation_time_usec_[index] = sitl_time_usec;
    }
}
void hako::service::impl::AircraftServiceContainer::advanceTimeStepLockStep(uint32_t index, uint64_t& sitl_time_usec)
{
    /*
     * Setup input data for each aircraft
     */
    auto aircraft = aircraft_container_.getAllAirCrafts()[index];
    MavlinkHakoMessage message;
    message.type = MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS;
    bool is_dirty = false;
    if (mavlink_service_container_.getServices()[index].get().readMessage(message, is_dirty)) {
        sitl_time_usec = message.data.hil_actuator_controls.time_usec;
        aircraft_inputs_[index].manual.control = false;
        for (int i = 0; i < hako::aircraft::ROTOR_NUM; i++) {
            aircraft_inputs_[index].controls[i] = message.data.hil_actuator_controls.controls[i];
        }
        if (is_dirty) {
            //lock_step and dirty
            /*
             * Run the aircraft
             */
            aircraft->run(aircraft_inputs_[aircraft->get_index()]);
            send_sensor_data(*aircraft, activated_time_usec_);
        }
    }
    else {
        sitl_time_usec = 0;
        aircraft->run(aircraft_inputs_[aircraft->get_index()]);
        send_sensor_data(*aircraft, activated_time_usec_);
    }
}

void hako::service::impl::AircraftServiceContainer::advanceTimeStepFreeRun(uint32_t index, uint64_t& sitl_time_usec)
{
    /*
     * Setup input data for each aircraft
     */
    auto aircraft = aircraft_container_.getAllAirCrafts()[index];
    MavlinkHakoMessage message;
    message.type = MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS;
    bool is_dirty = false;
    if (mavlink_service_container_.getServices()[index].get().readMessage(message, is_dirty)) {
        sitl_time_usec = message.data.hil_actuator_controls.time_usec;
        aircraft_inputs_[index].manual.control = false;
        for (int i = 0; i < hako::aircraft::ROTOR_NUM; i++) {
            aircraft_inputs_[index].controls[i] = message.data.hil_actuator_controls.controls[i];
        }
        if (!lock_step_ || is_dirty) {
            //free_run or lock_step and dirty
            /*
             * Run the aircraft
             */
            aircraft->run(aircraft_inputs_[aircraft->get_index()]);
        }
        send_sensor_data(*aircraft, activated_time_usec_);
    }
    else {
        sitl_time_usec = 0;
        aircraft->run(aircraft_inputs_[aircraft->get_index()]);
        send_sensor_data(*aircraft, activated_time_usec_);
    }
}

uint64_t hako::service::impl::AircraftServiceContainer::getSimulationTimeUsec(uint32_t index)
{
    if (index >= static_cast<uint32_t>(aircraft_container_.getAllAirCrafts().size())) {
        throw std::runtime_error("Invalid index for getSimulationTimeUsec : " + std::to_string(index));
    }
    return aircraft_container_.getAllAirCrafts()[index]->get_simulation_time_usec();
}

uint64_t hako::service::impl::AircraftServiceContainer::getSitlTimeUsec(uint32_t index)
{
    if (index >= static_cast<uint32_t>(aircraft_container_.getAllAirCrafts().size())) {
        throw std::runtime_error("Invalid index for getSitlTimeUsec : " + std::to_string(index));
    }
    return sitl_simulation_time_usec_[index];
}

void hako::service::impl::AircraftServiceContainer::send_sensor_data(IAirCraft& aircraft, uint64_t activated_time_usec)
{
    uint64_t time_usec = aircraft.get_simulation_time_usec() + activated_time_usec;
    int index = aircraft.get_index();
    MavlinkHakoMessage sensor_message;
    sensor_message.type = MAVLINK_MSG_TYPE_HIL_SENSOR;
    AircraftMavlinkMessageBuilder::build_hil_sensor(aircraft, sensor_message.data.hil_sensor, time_usec);
    bool ret = mavlink_service_container_.getServices()[index].get().sendMessage(sensor_message);
    if (!ret) {
        std::cerr << "ERROR: Failed to send HIL_SENSOR message on aircraft: " << aircraft.get_index() << std::endl;
    }
    if (send_count_[index] % gps_send_cycle == 0) {
        MavlinkHakoMessage gps_message;
        gps_message.type = MAVLINK_MSG_TYPE_HIL_GPS;
        AircraftMavlinkMessageBuilder::build_hil_gps(aircraft, gps_message.data.hil_gps, time_usec);
        ret = mavlink_service_container_.getServices()[aircraft.get_index()].get().sendMessage(gps_message);
        if (!ret) {
            std::cerr << "ERROR: Failed to send HIL_GPS message on aircraft: " << index << std::endl;
        }
    }
    send_count_[index]++;
}

void hako::service::impl::AircraftServiceContainer::resetService()
{
    for (auto aircraft : aircraft_container_.getAllAirCrafts()) {
        aircraft->reset();
    }
}

bool hako::service::impl::AircraftServiceContainer::write_pdu(uint32_t index, ServicePduDataType& pdu)
{
    //TODO synchronize with pdu
    if (index >= static_cast<uint32_t>(aircraft_inputs_.size())) {
        return false;
    }

    if (pdu.id == SERVICE_PDU_DATA_ID_TYPE_DISTURBANCE) {
        aircraft_inputs_[index].disturbance.values.d_temp.value = pdu.pdu.disturbance.d_temp.value;
        aircraft_inputs_[index].disturbance.values.d_wind.x = pdu.pdu.disturbance.d_wind.value.x;
        aircraft_inputs_[index].disturbance.values.d_wind.y = pdu.pdu.disturbance.d_wind.value.y;
        aircraft_inputs_[index].disturbance.values.d_wind.z = pdu.pdu.disturbance.d_wind.value.z;
    }
    else if (pdu.id == SERVICE_PDU_DATA_ID_TYPE_COLLISION) {
        aircraft_inputs_[index].collision.collision = pdu.pdu.collision.collision;
        aircraft_inputs_[index].collision.contact_num = pdu.pdu.collision.contact_num;
        aircraft_inputs_[index].collision.relative_velocity.x = pdu.pdu.collision.relative_velocity.x;
        aircraft_inputs_[index].collision.relative_velocity.y = pdu.pdu.collision.relative_velocity.y;
        aircraft_inputs_[index].collision.relative_velocity.z = pdu.pdu.collision.relative_velocity.z;
        aircraft_inputs_[index].collision.restitution_coefficient = pdu.pdu.collision.restitution_coefficient;
        for (int i = 0; i < MAX_CONTAT_NUM; i++) {
            aircraft_inputs_[index].collision.contact_position[i].x = pdu.pdu.collision.contact_position[i].x;
            aircraft_inputs_[index].collision.contact_position[i].y = pdu.pdu.collision.contact_position[i].y;
            aircraft_inputs_[index].collision.contact_position[i].z = pdu.pdu.collision.contact_position[i].z;
        }
    }
    else {
        std::cerr << "ERROR: write_pdu() Invalid PDU ID: " << pdu.id  << std::endl;
        return false;
    }
    return true;
}

bool hako::service::impl::AircraftServiceContainer::read_pdu(uint32_t index, ServicePduDataType& pdu)
{
    //TODO synchronize with pdu
    if (index >= static_cast<uint32_t>(aircraft_inputs_.size())) {
        return false;
    }
    switch (pdu.id) {
        case SERVICE_PDU_DATA_ID_TYPE_ACTUATOR_CONTROLS:
            pdu.pdu.actuator_controls.controls[0] = aircraft_inputs_[index].controls[0];
            pdu.pdu.actuator_controls.controls[1] = aircraft_inputs_[index].controls[1];
            pdu.pdu.actuator_controls.controls[2] = aircraft_inputs_[index].controls[2];
            pdu.pdu.actuator_controls.controls[3] = aircraft_inputs_[index].controls[3];
            break;
        case SERVICE_PDU_DATA_ID_TYPE_POSITION:
            pdu.pdu.position.linear.x = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_pos().data[0];
            pdu.pdu.position.linear.y = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_pos().data[1];
            pdu.pdu.position.linear.z = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_pos().data[2];
            pdu.pdu.position.angular.x = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_angle().data[0];
            pdu.pdu.position.angular.y = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_angle().data[1];
            pdu.pdu.position.angular.z = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_angle().data[2];
            break;
        default:
            std::cerr << "ERROR: read_pdu() Invalid PDU ID: " << pdu.id  << std::endl;
            return false;
    }
    return true;
}
