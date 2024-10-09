#ifndef _DRONE_CONTROLLER_HPP_
#define _DRONE_CONTROLLER_HPP_

#include "drone_alt_controller.hpp"
#include "drone_pos_controller.hpp"
#include "drone_heading_controller.hpp"
#include "drone_angle_controller.hpp"
#include "hako_controller_param_loader.hpp"
#include <stdexcept>
#include <memory>

class DroneController {
public:
    std::unique_ptr<DroneAltController> alt;
    std::unique_ptr<DronePosController> pos;
    std::unique_ptr<DroneHeadingController> head;
    std::unique_ptr<DroneAngleController> angle;
    HakoControllerParamLoader loader;
    
    DroneController() : 
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
    }
};

#endif /* _DRONE_CONTROLLER_HPP_ */
