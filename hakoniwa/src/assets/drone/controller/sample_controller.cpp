#include "sample_controller.hpp"
#include "config/drone_config.hpp"

hako::assets::drone::SampleController::SampleController(int index) 
{
    DroneConfig drone_config;
    this->set_index(index);
    if (drone_config_manager.getConfig(index, drone_config) == false) {
        std::cerr << "ERROR: " << "drone_config_manager.getConfig() error" << std::endl;
        return;
    }
    double mass = drone_config.getCompDroneDynamicsMass();
    hovering_thrust = mass * 9.81;
    hovering_thrust_range = hovering_thrust / 2;

    pid_pos_x = new PID(
        drone_config.getControllerPid("position", "x", "Kp"), 
        drone_config.getControllerPid("position", "x", "Ki"), 
        drone_config.getControllerPid("position", "x", "Kd"), 
        drone_config.getControllerPid("position", "x", "setpoint"));
    pid_pos_y = new PID(
        drone_config.getControllerPid("position", "y", "Kp"), 
        drone_config.getControllerPid("position", "y", "Ki"), 
        drone_config.getControllerPid("position", "y", "Kd"), 
        drone_config.getControllerPid("position", "y", "setpoint"));
    pid_pos_z = new PID(
        drone_config.getControllerPid("position", "z", "Kp"), 
        drone_config.getControllerPid("position", "z", "Ki"), 
        drone_config.getControllerPid("position", "z", "Kd"), 
        drone_config.getControllerPid("position", "z", "setpoint"));
    pid_euler_x = new PID(
        drone_config.getControllerPid("angle", "phi", "Kp"), 
        drone_config.getControllerPid("angle", "phi", "Ki"), 
        drone_config.getControllerPid("angle", "phi", "Kd"), 
        drone_config.getControllerPid("angle", "phi", "setpoint"));
    pid_euler_y = new PID(
        drone_config.getControllerPid("angle", "theta", "Kp"), 
        drone_config.getControllerPid("angle", "theta", "Ki"), 
        drone_config.getControllerPid("angle", "theta", "Kd"), 
        drone_config.getControllerPid("angle", "theta", "setpoint"));
    pid_euler_z = new PID(
        drone_config.getControllerPid("angle", "psi", "Kp"), 
        drone_config.getControllerPid("angle", "psi", "Ki"), 
        drone_config.getControllerPid("angle", "psi", "Kd"), 
        drone_config.getControllerPid("angle", "psi", "setpoint"));
}

hako::assets::drone::mi_drone_control_out_t hako::assets::drone::SampleController::run(hako::assets::drone::mi_drone_control_in_t &in) 
{
    mi_drone_control_out_t out;

    double z = pid_pos_z->calculate(-in.pos_z);
    out.thrust = get_limit_value(z, hovering_thrust, -hovering_thrust_range, hovering_thrust_range);
    //std::cout << "in.pos_z : " << in.pos_z << std::endl;
    //std::cout << "in.z : " << z << std::endl;
    //std::cout << "thrust : " << out.thrust << std::endl;
    out.torque_x = 0;
    out.torque_y = 0;
    out.torque_z = 0;

    return out;
}
