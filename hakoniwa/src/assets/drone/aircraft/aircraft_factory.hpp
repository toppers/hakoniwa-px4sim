#ifndef _AIRCRAFT_FACTORY_HPP_
#define _AIRCRAFT_FACTORY_HPP_

#include "iaircraft.hpp"
#include "idrone_dynamics.hpp"
#include "isensor_acceleration.hpp"
#include "isensor_baro.hpp"
#include "isensor_gps.hpp"
#include "isensor_gyro.hpp"
#include "isensor_mag.hpp"
#include "irotor_dynamics.hpp"
#include "ithrust_dynamics.hpp"
#include "config/drone_config.hpp"

namespace hako::assets::drone {

extern IAirCraft* create_aircraft(int index, const DroneConfig& drone_config);
#include <vector>
#include <memory>

class AirCraftManager {
private:
    std::vector<std::unique_ptr<IAirCraft>> airCrafts;

public:
    AirCraftManager() {}
    virtual ~AirCraftManager() {}
    /*
     * create different aircrafts from config directory.
     */
    void createAirCrafts(DroneConfigManager& configManager) {
        size_t configCount = configManager.getConfigCount();
        for (size_t i = 0; i < configCount; ++i) {
            DroneConfig config;
            if (configManager.getConfig(i, config)) {
                std::unique_ptr<IAirCraft> airCraft(hako::assets::drone::create_aircraft(i, config));
                if (airCraft) {
                    airCrafts.push_back(std::move(airCraft));
                }
                else {
                    std::cerr << "ERROR: can not create airCraft: " << i << std::endl;
                }
            }
        }
    }
    /*
     * create same aircrafts from config 0.
     */
    bool createSameAirCrafts(DroneConfigManager& configManager, int create_num) {
        DroneConfig config;
        if (configManager.getConfig(0, config) == false) {
            std::cerr << "ERROR: can not create get config: 0" << std::endl;
            return false;
        }
        for (int i = 0; i < create_num; ++i) {
            std::unique_ptr<IAirCraft> airCraft(hako::assets::drone::create_aircraft(i, config));
            if (airCraft) {
                airCrafts.push_back(std::move(airCraft));
            }
            else {
                std::cerr << "ERROR: can not create airCraft: " << i << std::endl;
            }
        }
        return true;
    }
    IAirCraft* getAirCraft(size_t index) {
        if (index < airCrafts.size()) {
            return airCrafts[index].get();
        } else {
            return nullptr;
        }
    }
    std::vector<IAirCraft*> getAllAirCrafts() {
        std::vector<IAirCraft*> list;
        for (auto& airCraft : airCrafts) {
            list.push_back(airCraft.get());
        }
        return list;
    }

};

}

using hako::assets::drone::IAirCraft;
using hako::assets::drone::IDroneDynamics;
using hako::assets::drone::ISensorAcceleration;
using hako::assets::drone::ISensorBaro;
using hako::assets::drone::ISensorGps;
using hako::assets::drone::ISensorMag;
using hako::assets::drone::ISensorGyro;
using hako::assets::drone::IRotorDynamics;
using hako::assets::drone::IThrustDynamics;

using hako::assets::drone::DroneThrustType;
using hako::assets::drone::DroneTorqueType;
using hako::assets::drone::DronePositionType;
using hako::assets::drone::DroneEulerType;

#endif /* _AIRCRAFT_FACTORY_HPP_ */