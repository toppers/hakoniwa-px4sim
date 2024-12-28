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
    setup_aircraft_inputs(index);
    uint64_t sitl_time_usec = 0;
    if (lock_step_) {
        advanceTimeStepLockStep(index, sitl_time_usec);
    }
    else {
        advanceTimeStepFreeRun(index, sitl_time_usec);
    }
    write_back_pdu(index);
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

void hako::service::impl::AircraftServiceContainer::setup_aircraft_inputs(uint32_t index)
{
    aircraft_inputs_[index].manual.control = false;
    if (aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().has_collision_detection()) {
        ServicePduDataType pdu_data = { SERVICE_PDU_DATA_ID_TYPE_COLLISION };
        pdu_synchers_[index]->load(index, pdu_data);
        aircraft_inputs_[index].collision.collision = pdu_data.pdu.collision.collision;
        if (aircraft_inputs_[index].collision.collision) {
            aircraft_inputs_[index].collision.contact_num = pdu_data.pdu.collision.contact_num;
            aircraft_inputs_[index].collision.relative_velocity.x = pdu_data.pdu.collision.relative_velocity.x;
            aircraft_inputs_[index].collision.relative_velocity.y = -pdu_data.pdu.collision.relative_velocity.y;
            aircraft_inputs_[index].collision.relative_velocity.z = -pdu_data.pdu.collision.relative_velocity.z;
            aircraft_inputs_[index].collision.restitution_coefficient = pdu_data.pdu.collision.restitution_coefficient;
            for (int i = 0; i < aircraft_inputs_[index].collision.contact_num; i++) {
                aircraft_inputs_[index].collision.contact_position[i].x = pdu_data.pdu.collision.contact_position[i].x;
                aircraft_inputs_[index].collision.contact_position[i].y = -pdu_data.pdu.collision.contact_position[i].y;
                aircraft_inputs_[index].collision.contact_position[i].z = -pdu_data.pdu.collision.contact_position[i].z;
            }
        }
    }
    if (aircraft_container_.getAllAirCrafts()[index]->is_enabled_disturbance()) {
        ServicePduDataType pdu_data = { SERVICE_PDU_DATA_ID_TYPE_DISTURBANCE };
        pdu_synchers_[index]->load(index, pdu_data);
        //temperature
        aircraft_inputs_[index].disturbance.values.d_temp.value = pdu_data.pdu.disturbance.d_temp.value;
        //wind
        aircraft_inputs_[index].disturbance.values.d_wind.x = pdu_data.pdu.disturbance.d_wind.value.x;
        aircraft_inputs_[index].disturbance.values.d_wind.y = pdu_data.pdu.disturbance.d_wind.value.y;
        aircraft_inputs_[index].disturbance.values.d_wind.z = pdu_data.pdu.disturbance.d_wind.value.z;
    }
}

void hako::service::impl::AircraftServiceContainer::write_back_pdu(uint32_t index)
{
    // collision write back
    ServicePduDataType col_pdu_data = { SERVICE_PDU_DATA_ID_TYPE_COLLISION };
    col_pdu_data.pdu.collision.collision = false;
    pdu_synchers_[index]->flush(index, col_pdu_data);

    // battery write back
    ServicePduDataType bat_pdu_data = { SERVICE_PDU_DATA_ID_TYPE_BATTERY_STATUS };
    auto battery = aircraft_container_.getAllAirCrafts()[index]->get_battery_dynamics();
    if (battery != nullptr) {
        auto status = battery->get_status();
        bat_pdu_data.pdu.battery_status.full_voltage = status.full_voltage;
        bat_pdu_data.pdu.battery_status.curr_voltage = status.curr_voltage;
        bat_pdu_data.pdu.battery_status.curr_temp = status.temperature;
        bat_pdu_data.pdu.battery_status.cycles = status.cycles;
        bat_pdu_data.pdu.battery_status.status = status.status;
    }
    else {
        bat_pdu_data.pdu.battery_status.full_voltage = 0;
        bat_pdu_data.pdu.battery_status.curr_voltage = 0;
        bat_pdu_data.pdu.battery_status.curr_temp = 0;
        bat_pdu_data.pdu.battery_status.cycles = 0;
        bat_pdu_data.pdu.battery_status.status = 0;
    }
    pdu_synchers_[index]->flush(index, bat_pdu_data);

    // control write back
    ServicePduDataType actuator_pdu_data = { SERVICE_PDU_DATA_ID_TYPE_ACTUATOR_CONTROLS };
    for (int i = 0; i < ROTOR_NUM; i++) {
        actuator_pdu_data.pdu.actuator_controls.controls[i] = aircraft_inputs_[index].controls[i];
    }
    pdu_synchers_[index]->flush(index, actuator_pdu_data);

    // position write back
    ServicePduDataType pos_pdu_data = { SERVICE_PDU_DATA_ID_TYPE_POSITION };
    DronePositionType dpos = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_pos();
    DroneEulerType dangle = aircraft_container_.getAllAirCrafts()[index]->get_drone_dynamics().get_angle();
    pos_pdu_data.pdu.position.linear.x = dpos.data.x;
    pos_pdu_data.pdu.position.linear.y = -dpos.data.y;
    pos_pdu_data.pdu.position.linear.z = -dpos.data.z;
    pos_pdu_data.pdu.position.angular.x = dangle.data.x;
    pos_pdu_data.pdu.position.angular.y = -dangle.data.y;
    pos_pdu_data.pdu.position.angular.z = -dangle.data.z;

    //std::cout << "INFO: AircraftServiceContainer::write_back_pdu: pos: (" << pos_pdu_data.pdu.position.linear.x << ", " << pos_pdu_data.pdu.position.linear.y << ", " << pos_pdu_data.pdu.position.linear.z << ")" << std::endl;
    pdu_synchers_[index]->flush(index, pos_pdu_data);

}