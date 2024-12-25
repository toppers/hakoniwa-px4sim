#include <gtest/gtest.h>
#include <iostream>
#include "aircraft/impl/aircraft.hpp"
#include "aircraft/impl/aircraft_container.hpp"

using namespace hako::aircraft;

class UtilsTest : public ::testing::Test {
protected:
    static void SetUpTestCase()
    {
    }
    static void TearDownTestCase()
    {
    }
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }

};

TEST_F(UtilsTest, CreateAirCrafts1) 
{
    AirCraftContainer container;
    DroneConfigManager configManager;
    //環境変数DRONE_CONFIG1_DIRから対象ディレクトリパスを取得
    const char* config1_dir = std::getenv("DRONE_CONFIG1_DIR");
    if (config1_dir == nullptr) {
        std::cerr << "Error: Environment variable DRONE_CONFIG1_DIR is not set." << std::endl;
        return;
    }
    std::string config1_dir_path = std::string(config1_dir);
    int num = configManager.loadConfigsFromDirectory(config1_dir_path);
    ASSERT_EQ(num, 1);
    container.createAirCrafts(configManager);
    ASSERT_EQ(container.getAllAirCrafts().size(), 1);

    for (int i = 0; i < 10; i++) {
        AircraftInputType aircraft_input = {};
        aircraft_input.manual.control = false;
        for (auto aircraft : container.getAllAirCrafts()) {
            aircraft->run(aircraft_input);
        }
    }
}
TEST_F(UtilsTest, CreateAirCrafts2) 
{
    AirCraftContainer container;
    DroneConfigManager configManager;
    //環境変数DRONE_CONFIG1_DIRから対象ディレクトリパスを取得
    const char* config2_dir = std::getenv("DRONE_CONFIG2_DIR");
    if (config2_dir == nullptr) {
        std::cerr << "Error: Environment variable DRONE_CONFIG2_DIR is not set." << std::endl;
        return;
    }
    std::string config2_dir_path = std::string(config2_dir);
    int num = configManager.loadConfigsFromDirectory(config2_dir_path);
    ASSERT_EQ(num, 2);
    container.createAirCrafts(configManager);
    ASSERT_EQ(container.getAllAirCrafts().size(), 2);

    for (int i = 0; i < 10; i++) {
        AircraftInputType aircraft_input = {};
        aircraft_input.manual.control = false;
        for (auto aircraft : container.getAllAirCrafts()) {
            aircraft->run(aircraft_input);
        }
    }
}
