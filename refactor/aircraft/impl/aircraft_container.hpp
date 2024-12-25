#ifndef _AIRCRAFT_CONTAINER_HPP_
#define _AIRCRAFT_CONTAINER_HPP_

#include "aircraft/iaircraft.hpp"
#include "aircraft/iaircraft_container.hpp"
#include "aircraft/impl/aircraft_factory.hpp"
#include "config/drone_config.hpp"
#include <vector>

namespace hako::aircraft {

class AirCraftContainer : public IAirCraftContainer {
private:
    std::vector<std::shared_ptr<IAirCraft>> airCrafts;
public:
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
    std::shared_ptr<IAirCraft> getAirCraft(size_t index) {
        if (index < airCrafts.size()) {
            return airCrafts[index];
        } else {
            return nullptr;
        }
    }
    std::vector<std::shared_ptr<IAirCraft>> getAllAirCrafts() {
        return airCrafts;
    }

};
}

#endif /* _AIRCRAFT_CONTAINER_HPP_ */