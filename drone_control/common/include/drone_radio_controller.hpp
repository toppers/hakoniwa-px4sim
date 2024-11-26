#ifndef _DRONE_CONTROLLER_HPP_
#define _DRONE_CONTROLLER_HPP_

#include "drone_alt_controller.hpp"
#include "drone_pos_controller.hpp"
#include "drone_heading_controller.hpp"
#include "drone_angle_controller.hpp"
#include "hako_controller_param_loader.hpp"
#include <stdexcept>
#include <memory>

class DroneRadioController {
private:
    //parameters
    double delta_time;
    double alt_control_cycle;
    double head_control_cycle;
    double pos_control_cycle;
    double alt_delta_value_m;
    double pos_delta_value_m;
    double yaw_delta_value_deg;
    double pos_max_spd;
    bool   pos_control_enable = false;

    //altitude control
    bool r_altitude_initialized = false;
    double r_altitude = 0;
    double alt_time = 0;

    //position control
    double r_position_x = 0;
    double r_position_y = 0;
    double pos_time = 0;

    //yaw control
    double yaw_time = 0;
    double r_yaw = 0;

public:
    std::unique_ptr<DroneAltController> alt;
    std::unique_ptr<DronePosController> pos;
    std::unique_ptr<DroneHeadingController> head;
    std::unique_ptr<DroneAngleController> angle;
    HakoControllerParamLoader loader;
    
    DroneRadioController() : 
        loader() 
    {
        if (HakoControllerParamLoader::is_exist_envpath()) {
            loader.loadParameters();
        } else {
            throw std::runtime_error("Parameter file is not found on HAKO_CONTROLLER_PARAM_FILE");
        }
        alt = std::make_unique<DroneAltController>(loader);
        pos = std::make_unique<DronePosController>(loader);
        head = std::make_unique<DroneHeadingController>(loader);
        angle = std::make_unique<DroneAngleController>(loader);
        //params
        delta_time = loader.getParameter("SIMULATION_DELTA_TIME");
        head_control_cycle = loader.getParameter("HEAD_CONTROL_CYCLE");
        alt_control_cycle = loader.getParameter("PID_ALT_CONTROL_CYCLE");
        pos_control_cycle = loader.getParameter("POS_CONTROL_CYCLE");
        pos_delta_value_m = loader.getParameter("POS_DELTA_VALUE_M");
        int p_ctrl_enable = loader.getParameter("POS_CONTROL_ENABLE");
        if (p_ctrl_enable == 1) {
            std::cout << "Position control is enabled" << std::endl;
            pos_control_enable = true;
        }
        else {
            std::cout << "Position control is disabled" << std::endl;
        }
        alt_delta_value_m = loader.getParameter("ALT_DELTA_VALUE_M");
        yaw_delta_value_deg = loader.getParameter("YAW_DELTA_VALUE_DEG");
        pos_max_spd = loader.getParameter("PID_POS_MAX_SPD");
    }
    double get_pos_max_spd()
    {
        return pos_max_spd;
    }
    void save_for_initial_position(double altitude)
    {
        if (r_altitude_initialized == false) {
            r_altitude = altitude;
            r_altitude_initialized = true;
            std::cout << "r_altitude: " << r_altitude << std::endl;
        }
    }
    double update_target_altitude(double v) {
        if (alt_time >= alt_control_cycle) {
            alt_time = 0;
            r_altitude += v * alt_delta_value_m;
            //std::cout << "v: " << v << "r_altitude: " << r_altitude << " v: " << v << std::endl;
            if (r_altitude < 0) {
                r_altitude = 0;
            }
        }
        alt_time += this->delta_time;
        return r_altitude;
    }
    // almost same value of double data
    bool is_same_value(double a, double b) {
        return fabs(a - b) < 0.0001;
    }
    std::pair<double, double> update_target_position(double x, double y, double yaw_rad, double current_x, double current_y) {
        if (pos_time >= pos_control_cycle) {
            pos_time = 0;
            double delta_x = x * cos(yaw_rad) - y * sin(yaw_rad);
            double delta_y = x * sin(yaw_rad) + y * cos(yaw_rad);
            r_position_x += delta_x * pos_delta_value_m;
            r_position_y += delta_y * pos_delta_value_m;            
            if (is_same_value(x, 0.0) && is_same_value(y, 0.0)) {
                r_position_x = current_x;
                r_position_y = current_y;
            }
            //std::cout << "yaw_rad: " << yaw_rad << " delta_x: " << delta_x << " delta_y: " << delta_y << std::endl;
            //std::cout << "x: " << x << " y: " << y << " r_position_x: " << r_position_x << " r_position_y: " << r_position_y << std::endl;
        }
        pos_time += this->delta_time;
        return std::make_pair(r_position_x, r_position_y);
    }
    bool is_pos_control_enable()
    {
        return pos_control_enable;
    }
    // Function to update target yaw
    double update_target_yaw(double v) {
        if (yaw_time >= head_control_cycle) {
            yaw_time = 0;
            r_yaw += v * yaw_delta_value_deg;
        }
        //std::cout << "r_yaw: " << r_yaw << std::endl;
        yaw_time += this->delta_time;
        return r_yaw;
    }
};

#endif /* _DRONE_CONTROLLER_HPP_ */
