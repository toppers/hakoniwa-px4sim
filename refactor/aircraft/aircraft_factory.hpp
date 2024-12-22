#ifndef _AIRCRAFT_FACTORY_HPP_
#define _AIRCRAFT_FACTORY_HPP_

#include "aircraft/interfaces/iaircraft.hpp"
#include "aircraft/interfaces/idrone_dynamics.hpp"
#include "aircraft/interfaces/irotor_dynamics.hpp"
#include "aircraft/interfaces/ithrust_dynamics.hpp"
#include "aircraft/interfaces/ibattery_dynamics.hpp"
#include "aircraft/interfaces/isensor_acceleration.hpp"
#include "aircraft/interfaces/isensor_baro.hpp"
#include "aircraft/interfaces/isensor_gps.hpp"
#include "aircraft/interfaces/isensor_gyro.hpp"
#include "aircraft/interfaces/isensor_mag.hpp"
#include "config/drone_config.hpp"
#include <vector>
#include <memory>

namespace hako::aircraft {

extern IAirCraft* create_aircraft(int index, const DroneConfig& drone_config);

class AirCraftContainer {
private:
    std::vector<std::unique_ptr<IAirCraft>> airCrafts;

public:
    AirCraftContainer() {}
    virtual ~AirCraftContainer() {}
    /*
     * create different aircrafts from config directory.
     */
    void createAirCrafts(DroneConfigManager& configManager) {
        size_t configCount = configManager.getConfigCount();
        for (size_t i = 0; i < configCount; ++i) {
            DroneConfig config;
            if (configManager.getConfig(i, config)) {
                std::unique_ptr<IAirCraft> airCraft(hako::aircraft::create_aircraft(i, config));
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
            std::unique_ptr<IAirCraft> airCraft(hako::aircraft::create_aircraft(i, config));
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


#endif /* _AIRCRAFT_FACTORY_HPP_ */