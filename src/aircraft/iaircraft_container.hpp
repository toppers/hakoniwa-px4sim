#ifndef _IAIRCRAFT_CONTAINER_HPP_
#define _IAIRCRAFT_CONTAINER_HPP_

#include "aircraft/iaircraft.hpp"
#include "config/drone_config.hpp"

namespace hako::aircraft {
class IAirCraftContainer {
public:
    virtual ~IAirCraftContainer() {}
    /*
     * create different air crafts from config directory.
     */
    virtual void createAirCrafts(DroneConfigManager& configManager) = 0;
    /*
     * create same air crafts from config 0.
     */
    virtual bool createSameAirCrafts(DroneConfigManager& configManager, int create_num) = 0;
    virtual std::shared_ptr<IAirCraft> getAirCraft(size_t index) = 0;
    virtual std::vector<std::shared_ptr<IAirCraft>> getAllAirCrafts() = 0;
};

}


#endif /* _IAIRCRAFT_CONTAINER_HPP_ */