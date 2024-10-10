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
    double alt_delta_value_m;
    double yaw_delta_value_deg;
    double pos_max_spd;

    //altitude control
    bool r_altitude_initialized = false;
    double r_altitude = 0;
    double alt_time = 0;

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
