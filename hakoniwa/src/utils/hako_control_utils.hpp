#ifndef _HAKO_CONTROL_UTILS_HPP_
#define _HAKO_CONTROL_UTILS_HPP_

#include "drone_flight_controller.hpp"
#include "utils/hako_module_loader.hpp"
#include "hako_module_drone_controller.h"

typedef struct {
    void *handle;
    HakoModuleHeaderType *header;
    HakoModuleDroneControllerType *controller;
} AircraftControlModuleType;

class AirCraftModule
{
public:
    DroneFlightControllerContextType context;
    AircraftControlModuleType control_module;
    hako::assets::drone::IController *controller;
    IAirCraft *drone;
    double controls[hako::assets::drone::ROTOR_NUM] = { 0, 0, 0, 0};

    bool load_controller(const char* filepath, void* context) 
    {
        control_module.header = nullptr;
        control_module.controller = nullptr;
        control_module.handle = hako_module_handle(filepath, &control_module.header);
        if (control_module.handle == nullptr) {
            return false;
        }
        control_module.controller = (HakoModuleDroneControllerType*)hako_module_load_symbol(control_module.handle, HAKO_MODULE_DRONE_CONTROLLER_SYMBOLE_NAME);
        if (control_module.controller == nullptr) {
            return false;
        }
        std::cout << "SUCCESS: Loaded module name: " << control_module.header->get_name() << std::endl;
        return (control_module.controller->init(context) == 0);
    }

};

class AirCraftModuleSimulator
{
private:
    hako::assets::drone::AirCraftManager drone_manager;

    bool do_asset_task()
    {
        CsvLogger::set_time_usec(hako_asset_time_usec);
        if (hako_asset_runner_step(1) == false) {
            std::cout << "INFO: stopped simulation" << std::endl;
            return false;
        }
        hako_asset_time_usec += delta_time_usec;
        return true;
    }
    std::vector<AirCraftModule> aircraft_modules;

    Hako_uint64 hako_asset_time_usec;
    Hako_uint64 delta_time_usec;
public:
    std::vector<AirCraftModule> get_modules() const
    {
        return this->aircraft_modules;
    }
    void init(Hako_uint64 microseconds, Hako_uint64 dt_usec)
    {
        hako_asset_time_usec = microseconds;
        delta_time_usec = dt_usec;
        drone_manager.createAirCrafts(drone_config_manager);
        for (auto* drone : drone_manager.getAllAirCrafts()) {
            std::cout << "INFO: loading drone & controller: " << drone->get_index() << std::endl;
            AirCraftModule arg;
            arg.drone = drone;
            DroneConfig drone_config;
            drone_config_manager.getConfig(drone->get_index(), drone_config);
            arg.control_module.controller = nullptr;
            std::string filepath = drone_config.getControllerModuleFilePath();
            if (!filepath.empty()) {
                std::string file = drone_config.getControllerContext("file");
                if (file.empty()) {
                    arg.context.plan_filepath = nullptr;
                }
                else {
                    arg.context.plan_filepath = file.c_str();
                }
                arg.load_controller(filepath.c_str(), &arg.context);
            }
            else {
                std::cerr << "WARNING: can not find module for " << drone->get_name() << std::endl;
            }
            if (arg.control_module.controller == nullptr) {
                HAKO_ASSERT(drone_config.isExistController("pid"));
                arg.controller = new hako::assets::drone::SampleController(drone->get_index());
                if (arg.controller == nullptr) {
                    std::cerr << "ERROR: can not create Controller: " << drone->get_index() << std::endl;
                    HAKO_ASSERT(arg.controller != nullptr);
                    return;
                }
            }
            aircraft_modules.push_back(arg);
        }
    }
    void do_task()
    {
        while (do_asset_task() == true){};
    }
};
static inline void calculate_simple_controls(AirCraftModule& module, const DroneThrustType& thrust)
{
    double hovering_force = module.drone->get_drone_dynamics().get_mass() * hako::assets::drone::GRAVITY;
    double max_hovering_force = 2.0 * hovering_force;
    double control = 0;
    if (thrust.data > max_hovering_force) {
        control = 1.0;
    }
    else {
        control = thrust.data / max_hovering_force;
    }
    for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++) {
        module.controls[i] = control;
    }
    return;
}
#endif /* _HAKO_CONTROL_UTILS_HPP_ */