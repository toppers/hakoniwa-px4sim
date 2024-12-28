#include "service/drone/impl/drone_service.hpp"

using namespace hako::service::impl;

bool hako::service::drone_pdu_data_deep_copy(const ServicePduDataType& source, ServicePduDataType& dest)
{
    dest.id = source.id;
    switch (source.id) {
        case SERVICE_PDU_DATA_ID_TYPE_TAKEOFF:
            dest.pdu.takeoff = source.pdu.takeoff;
            break;
        case SERVICE_PDU_DATA_ID_TYPE_LAND:
            dest.pdu.land = source.pdu.land;
            break;
        case SERVICE_PDU_DATA_ID_TYPE_MOVE:
            dest.pdu.move = source.pdu.move;
            break;
        case SERVICE_PDU_DATA_ID_TYPE_GAME_CTRL:
            dest.pdu.game_ctrl = source.pdu.game_ctrl;
            break;
        case SERVICE_PDU_DATA_ID_TYPE_MAGNET:
            dest.pdu.magnet = source.pdu.magnet;
            break;
        case SERVICE_PDU_DATA_ID_TYPE_COLLISION:
            dest.pdu.collision = source.pdu.collision;
            break;
        case SERVICE_PDU_DATA_ID_TYPE_DISTURBANCE:
            dest.pdu.disturbance = source.pdu.disturbance;
            break;
        case SERVICE_PDU_DATA_ID_TYPE_BATTERY_STATUS:
            dest.pdu.battery_status = source.pdu.battery_status;
            break;
        case SERVICE_PDU_DATA_ID_TYPE_STATUS_MAGNET:
            dest.pdu.status_magnet = source.pdu.status_magnet;
            break;
        case SERVICE_PDU_DATA_ID_TYPE_POSITION:
            dest.pdu.position = source.pdu.position;
            break;
        case SERVICE_PDU_DATA_ID_TYPE_ACTUATOR_CONTROLS:
            dest.pdu.actuator_controls = source.pdu.actuator_controls;
            break;
        default:
            return false;
    }
    return true;
}


static void debug_print_drone_collision(DroneDynamicsCollisionType& drone_collision)
{
    std::cout << "Collision: " << (drone_collision.collision ? "Yes" : "No") << std::endl;
    std::cout << "Contact Number: " << drone_collision.contact_num << std::endl;
    std::cout << "Relative Velocity: (" 
                    << drone_collision.relative_velocity.x << ", " 
                    << drone_collision.relative_velocity.y << "," 
                    << drone_collision.relative_velocity.z << ")"
                    << std::endl;

    for (int i = 0; i < drone_collision.contact_num; ++i) {
        std::cout << "Contact Positions[" << i << "]: (" 
                    << drone_collision.contact_position[i].x << ", " 
                    << drone_collision.contact_position[i].y << "," 
                    << drone_collision.contact_position[i].z << ")"
                    << std::endl;
    }
    std::cout << "Restitution Coefficient: " << drone_collision.restitution_coefficient << std::endl;
}
void DroneService::advanceTimeStep() 
{
    //setup input data for controller
    setup_controller_inputs();

    //run controller
    if (drone_service_operation_->can_advanceTimeStep_for_controller()) {
        controller_outputs_ = controller_->run(controller_inputs_);

        //run mixer
        pwm_duty_ = controller_->mixer()->run(controller_outputs_);
    }

    //setup input data for aircraft
    setup_aircraft_inputs();

    //run aircraft
    aircraft_->run(aircraft_inputs_);
    
    //write pdu
    write_back_pdu();

    simulation_time_usec_ += delta_time_usec_;
    //std::cout << "DroneService::advanceTimeStep: " << simulation_time_usec_ << std::endl;
}

void DroneService::setup_controller_inputs()
{
    controller_outputs_ = {};
    controller_inputs_.mass = aircraft_->get_drone_dynamics().get_mass();
    controller_inputs_.drag = aircraft_->get_drone_dynamics().get_drag();

    DronePositionType pos = aircraft_->get_drone_dynamics().get_pos();
    DroneEulerType angle = aircraft_->get_drone_dynamics().get_angle();
    DroneVelocityBodyFrameType velocity = aircraft_->get_drone_dynamics().get_vel_body_frame();
    DroneAngularVelocityBodyFrameType angular_velocity = aircraft_->get_gyro().sensor_value();

    drone_service_operation_->setup_controller_inputs(controller_inputs_);
    controller_inputs_.max_rpm = aircraft_->get_rpm_max(0);
    controller_inputs_.pos_x = pos.data.x;
    controller_inputs_.pos_y = pos.data.y;
    controller_inputs_.pos_z = pos.data.z;
    controller_inputs_.euler_x = angle.data.x;
    controller_inputs_.euler_y = angle.data.y;
    controller_inputs_.euler_z = angle.data.z;
    controller_inputs_.u = velocity.data.x;
    controller_inputs_.v = velocity.data.y;
    controller_inputs_.w = velocity.data.z;
    controller_inputs_.p = angular_velocity.data.x;
    controller_inputs_.q = angular_velocity.data.y;
    controller_inputs_.r = angular_velocity.data.z;
}

void DroneService::setup_aircraft_inputs()
{
    for (int i = 0; i < ROTOR_NUM; i++) {
        aircraft_inputs_.controls[i] = pwm_duty_.d[i];
    }
    aircraft_inputs_.manual.control = false;
    if (aircraft_->get_drone_dynamics().has_collision_detection()) {
        ServicePduDataType pdu_data = { SERVICE_PDU_DATA_ID_TYPE_COLLISION };
        read_pdu(pdu_data);
        aircraft_inputs_.collision.collision = pdu_data.pdu.collision.collision;
        if (aircraft_inputs_.collision.collision) {
            aircraft_inputs_.collision.contact_num = pdu_data.pdu.collision.contact_num;
            aircraft_inputs_.collision.relative_velocity.x = pdu_data.pdu.collision.relative_velocity.x;
            aircraft_inputs_.collision.relative_velocity.y = -pdu_data.pdu.collision.relative_velocity.y;
            aircraft_inputs_.collision.relative_velocity.z = -pdu_data.pdu.collision.relative_velocity.z;
            aircraft_inputs_.collision.restitution_coefficient = pdu_data.pdu.collision.restitution_coefficient;
            for (int i = 0; i < aircraft_inputs_.collision.contact_num; i++) {
                aircraft_inputs_.collision.contact_position[i].x = pdu_data.pdu.collision.contact_position[i].x;
                aircraft_inputs_.collision.contact_position[i].y = -pdu_data.pdu.collision.contact_position[i].y;
                aircraft_inputs_.collision.contact_position[i].z = -pdu_data.pdu.collision.contact_position[i].z;
            }
            debug_print_drone_collision(aircraft_inputs_.collision);
        }
    }
    if (aircraft_->is_enabled_disturbance()) {
        ServicePduDataType pdu_data = { SERVICE_PDU_DATA_ID_TYPE_DISTURBANCE };
        read_pdu(pdu_data);
        //temperature
        aircraft_inputs_.disturbance.values.d_temp.value = pdu_data.pdu.disturbance.d_temp.value;
        //wind
        aircraft_inputs_.disturbance.values.d_wind.x = pdu_data.pdu.disturbance.d_wind.value.x;
        aircraft_inputs_.disturbance.values.d_wind.y = pdu_data.pdu.disturbance.d_wind.value.y;
        aircraft_inputs_.disturbance.values.d_wind.z = pdu_data.pdu.disturbance.d_wind.value.z;
    }
}

void DroneService::write_back_pdu()
{
    //write game control pdu
    if (drone_service_operation_->can_advanceTimeStep_for_controller()) {
        drone_service_operation_->write_controller_pdu();
    }
    
    // collision write back
    ServicePduDataType col_pdu_data = { SERVICE_PDU_DATA_ID_TYPE_COLLISION };
    col_pdu_data.pdu.collision.collision = false;
    write_pdu(col_pdu_data);

    // battery write back
    ServicePduDataType bat_pdu_data = { SERVICE_PDU_DATA_ID_TYPE_BATTERY_STATUS };
    auto battery = aircraft_->get_battery_dynamics();
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
    write_pdu(bat_pdu_data);
    
    // control write back
    ServicePduDataType actuator_pdu_data = { SERVICE_PDU_DATA_ID_TYPE_ACTUATOR_CONTROLS };
    for (int i = 0; i < ROTOR_NUM; i++) {
        actuator_pdu_data.pdu.actuator_controls.controls[i] = pwm_duty_.d[i];
    }
    write_pdu(actuator_pdu_data);

    // position write back
    ServicePduDataType pos_pdu_data = { SERVICE_PDU_DATA_ID_TYPE_POSITION };
    DronePositionType dpos = aircraft_->get_drone_dynamics().get_pos();
    DroneEulerType dangle = aircraft_->get_drone_dynamics().get_angle();
    pos_pdu_data.pdu.position.linear.x = dpos.data.x;
    pos_pdu_data.pdu.position.linear.y = -dpos.data.y;
    pos_pdu_data.pdu.position.linear.z = -dpos.data.z;
    pos_pdu_data.pdu.position.angular.x = dangle.data.x;
    pos_pdu_data.pdu.position.angular.y = -dangle.data.y;
    pos_pdu_data.pdu.position.angular.z = -dangle.data.z;

    //std::cout << "Drone Position: (" << dpos.data.x << ", " << dpos.data.y << ", " << dpos.data.z << ")" << std::endl;
    //std::cout << "Drone Angle: (" << dangle.data.x << ", " << dangle.data.y << ", " << dangle.data.z << ")" << std::endl;
    write_pdu(pos_pdu_data);

}
