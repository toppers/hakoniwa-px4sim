#ifndef _DRONE_CONFIG_HPP_
#define _DRONE_CONFIG_HPP_

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <optional>
#include "drone_config_types.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;
#ifdef __APPLE__
#define SHARED_LIB_EXT  ".so"
#elif __linux__
#define SHARED_LIB_EXT  ".so"
#else
#define SHARED_LIB_EXT  ".dll"
#endif

//#define DRONE_PX4_RX_DEBUG_ENABLE
//DRONE_PX4_TX_DEBUG_ENABLE
//DRONE_PID_CONTROL_CPP
struct RotorPosition {
    std::vector<double> position;
    double rotationDirection;
};
#define DEGREE2RADIAN(v)    ( (v) * M_PI / (180.0) )
#define RADIAN2DEGREE(v)    ( (180.0 * (v)) / M_PI )

class DroneConfig {
private:
    json configJson;
    std::string config_filepath;
public:
    DroneConfig() {}
    bool init(const std::string& configFilePath) {
        config_filepath = configFilePath;
        std::ifstream configFile(config_filepath);
        if (configFile.is_open()) {
            try {
                configFile >> configJson;
            } catch (json::parse_error& e) {
                std::cerr << "JSON parsing error: " << e.what() << std::endl;
                return false;
            }
        } else {
            std::cerr << "Unable to open config file: " << config_filepath << std::endl;
            return false;
        }
        return true;
    }

    // Simulation parameters
    double getSimTimeStep() const {
        return configJson["simulation"]["timeStep"].get<double>();
    }
    bool getSimLockStep() const {
        return configJson["simulation"]["lockstep"].get<bool>();
    }
    std::string getSimLogOutputDirectory() const 
    {
        std::string directory = configJson["simulation"]["logOutputDirectory"].get<std::string>();
        if (!std::filesystem::exists(directory)) {
            std::cerr << "Error: Log output directory '" << directory << "' does not exist." << std::endl;
            return "./";
        }
        return directory;
    }
    std::string getRoboName() const
    {
        return configJson["name"].get<std::string>();
    }
    std::string getSimLogFullPath(const std::string& filename) const
    {
        std::string logDirectory = getSimLogOutputDirectory();

        if (logDirectory.back() != '/' && logDirectory.back() != '\\') {
            logDirectory += "/";
        }

        return logDirectory + filename;
    }
    void createDirectory(const std::string dirpath) const
    {
        if (!std::filesystem::exists(dirpath)) {
            if (std::filesystem::create_directory(dirpath)) {
                std::cout << "INFO: create directory " << dirpath << std::endl;
            } else {
                std::cout << "ERROR: can not create directory: " << dirpath << std::endl;
            }
        } else {
            //std::cout << "INFO: directory is already exist: " << dirpath << std::endl;
        }
    }
    std::string getSimLogFullPathFromIndex(int index, const std::string& name) const
    {
        std::string logDirectory = getSimLogDirectoryPathFromIndex(index);
        if (logDirectory.back() != '/' && logDirectory.back() != '\\') {
            logDirectory += "/";
        }
        return logDirectory + name;
    }
    std::string getSimLogDirectoryPathFromIndex(int index) const
    {
        std::string logDirectory = getSimLogOutputDirectory();
        if (logDirectory.back() != '/' && logDirectory.back() != '\\') {
            logDirectory += "/";
        }
        std::string path = logDirectory + "drone_log" + std::to_string(index);
        createDirectory(path);
        return path;
    }

    // Log Output for Sensors
    bool isSimSensorLogEnabled(const std::string& sensorName) const {
        return configJson["simulation"]["logOutput"]["sensors"][sensorName].get<bool>();
    }

    // Log Output for MAVLINK
    bool isMSimavlinkLogEnabled(const std::string& mavlinkMessage) const {
        return configJson["simulation"]["logOutput"]["mavlink"][mavlinkMessage].get<bool>();
    }

    // MAVLINK Transmission Period
    int getSimMavlinkTransmissionPeriod(const std::string& mavlinkMessage) const {
        return configJson["simulation"]["mavlink_tx_period_msec"][mavlinkMessage].get<int>();
    }

    // Location parameters
    double getSimLatitude() const {
        return configJson["simulation"]["location"]["latitude"].get<double>();
    }

    double getSimLongitude() const {
        return configJson["simulation"]["location"]["longitude"].get<double>();
    }

    double getSimAltitude() const {
        return configJson["simulation"]["location"]["altitude"].get<double>();
    }

    struct MagneticField {
        double intensity_nT;
        double declination_deg;
        double inclination_deg;
    };

    MagneticField getSimMagneticField() const {
        MagneticField field;
        field.intensity_nT = configJson["simulation"]["location"]["magneticField"]["intensity_nT"].get<double>();
        field.declination_deg = configJson["simulation"]["location"]["magneticField"]["declination_deg"].get<double>();
        field.inclination_deg = configJson["simulation"]["location"]["magneticField"]["inclination_deg"].get<double>();
        return field;
    }

    // Drone Dynamics parameters
    std::string getCompDroneDynamicsPhysicsEquation() const {
        return configJson["components"]["droneDynamics"]["physicsEquation"].get<std::string>();
    }

    std::vector<double> getCompDroneDynamicsAirFrictionCoefficient() const {
        std::vector<double> frictions;
        for (const auto& item : configJson["components"]["droneDynamics"]["airFrictionCoefficient"]) {
            frictions.push_back(item);
        }
        return frictions;
    }
    bool getCompDroneDynamicsCollisionDetection() const {
        return configJson["components"]["droneDynamics"]["collision_detection"].get<bool>();
    }
    bool getCompDroneDynamicsManualControl() const {
        return configJson["components"]["droneDynamics"]["manual_control"].get<bool>();
    }
    std::vector<double> getCompDroneDynamicsBodySize() const {
        std::vector<double> body_size;
        for (const auto& item : configJson["components"]["droneDynamics"]["body_size"]) {
            body_size.push_back(item);
        }
        return body_size;
    }
    std::vector<double> getCompDroneDynamicsInertia() const {
        std::vector<double> inertia;
        for (const auto& item : configJson["components"]["droneDynamics"]["inertia"]) {
            inertia.push_back(item);
        }
        return inertia;
    }
    std::vector<double> getCompDroneDynamicsPosition() const {
        std::vector<double> inertia;
        for (const auto& item : configJson["components"]["droneDynamics"]["position_meter"]) {
            inertia.push_back(item);
        }
        return inertia;
    }
    std::vector<double> getCompDroneDynamicsAngle() const {
        std::vector<double> inertia;
        for (const auto& item : configJson["components"]["droneDynamics"]["angle_degree"]) {
            inertia.push_back(item);
        }
        return inertia;
    }
    double getCompDroneDynamicsMass() const {
        return configJson["components"]["droneDynamics"]["mass_kg"].get<double>();
    }
    std::optional<hako::assets::drone::OutOfBoundsReset> getCompDroneDynamicsOutOfBoundsReset() const {
        if (configJson["components"]["droneDynamics"].contains("out_of_bounds_reset")) {
            std::cout << "out_of_bounds_reset values are enabled." << std::endl;
            hako::assets::drone::OutOfBoundsReset out_of_bounds_reset;
            out_of_bounds_reset.position = configJson["components"]["droneDynamics"]["out_of_bounds_reset"]["position"].get<std::vector<bool>>();
            out_of_bounds_reset.velocity = configJson["components"]["droneDynamics"]["out_of_bounds_reset"]["velocity"].get<std::vector<bool>>();
            out_of_bounds_reset.angular_velocity = configJson["components"]["droneDynamics"]["out_of_bounds_reset"]["angular_velocity"].get<std::vector<bool>>();
            return out_of_bounds_reset;
        }
        return std::nullopt;
    }

    std::string getCompRotorVendor() const {
        if (configJson["components"]["rotor"].contains("vendor")) {
            return configJson["components"]["rotor"]["vendor"].get<std::string>();
        } else {
            return "None";
        }
    }
    double getCompRotorTr() const {
        return configJson["components"]["rotor"]["Tr"].get<double>();
    }

    double getCompRotorKr() const {
        return configJson["components"]["rotor"]["Kr"].get<double>();
    }

    int getCompRotorRpmMax() const {
        return configJson["components"]["rotor"]["rpmMax"].get<int>();
    }

    // Thruster parameters
    std::vector<RotorPosition> getCompThrusterRotorPositions() const {
        std::vector<RotorPosition> positions;
        for (const auto& item : configJson["components"]["thruster"]["rotorPositions"]) {
            RotorPosition pos;
            pos.position = item["position"].get<std::vector<double>>();
            pos.rotationDirection = item["rotationDirection"].get<double>();
            positions.push_back(pos);
        }
        return positions;
    }
    double getCompThrusterParameter(const std::string& param_name) const {
        if (configJson["components"]["thruster"].contains(param_name)) {
            return configJson["components"]["thruster"][param_name].get<double>();
        } else {
            return 0.0;
        }
    }
    std::string getCompThrusterVendor() const {
        if (configJson["components"]["thruster"].contains("vendor")) {
            return configJson["components"]["thruster"]["vendor"].get<std::string>();
        } else {
            return "None";
        }
    }
    std::string getLastDirectoryName(const std::string& pathStr) const {
        fs::path pathObj(pathStr);

        if (!pathObj.empty()) {
            if (pathObj.filename() == "." || pathObj.filename() == ".." || pathObj.filename() == fs::path()) {
                pathObj = pathObj.parent_path();
            }
            return pathObj.filename().string();
        } else {
            return "";
        }
    }
    std::string getCompSensorVendor(const std::string& sensor_name) const {
        if (configJson["components"]["sensors"][sensor_name].contains("vendor")) {
            std::string moduleDirectory = configJson["components"]["sensors"][sensor_name]["vendor"].get<std::string>();
            if (moduleDirectory.back() != '/' && moduleDirectory.back() != '\\') {
                moduleDirectory += "/";
            }
            std::string moduleName = getCompSensorContextModuleName(sensor_name);
            if (moduleName.empty()) {
                moduleName = getLastDirectoryName(moduleDirectory);
            }
#if WIN32
            return moduleDirectory + moduleName + SHARED_LIB_EXT;
#else
            return moduleDirectory + "lib" + moduleName + SHARED_LIB_EXT;
#endif
        } else {
            return "";
        }
    }
    std::string getCompSensorContextModuleName(const std::string& sensor_name) const
    {
        return getCompSensorContext(sensor_name, "moduleName");
    }

    std::string getCompSensorContext(const std::string& sensor_name, const std::string& param) const
    {
         if (configJson["components"]["sensors"][sensor_name].contains("context")) {
            if (configJson["components"]["sensors"][sensor_name]["context"].contains(param)) {
                return configJson["components"]["sensors"][sensor_name]["context"][param];
            }
        }
        return "";
    }

    double getCompSensorSampleCount(const std::string& sensor_name) const {
        return configJson["components"]["sensors"][sensor_name]["sampleCount"].get<double>();
    }
    double getCompSensorNoise(const std::string& sensor_name) const {
        return configJson["components"]["sensors"][sensor_name]["noise"].get<double>();
    }
    bool isExistController(const std::string& param)
    {
        if (!configJson.contains("controller")) {
            std::cerr << "WARING: can not find controller on drone_config" << std::endl;
            return false;
        }
        if (!configJson["controller"].contains(param)) {
            std::cerr << "WARING: can not find controller[ " << param << " ] on drone_config" << std::endl;
            return false;
        }
        return true;
    }
    double getControllerPid(const std::string& param1, const std::string& param2, const std::string& param3)
    {
        return configJson["controller"]["pid"][param1][param2][param3].get<double>();
    }
    std::string getControllerModuleName() const
    {
        if (configJson["controller"].contains("moduleName")) {
            return configJson["controller"]["moduleName"];
        }
        else {
            return getRoboName();
        }
    }
    std::string getControllerContext(const std::string& param) const
    {
        if (configJson["controller"].contains("context")) {
            if (configJson["controller"]["context"].contains(param)) {
                return configJson["controller"]["context"][param];
            }
        }
        return "";
    }
    std::string getControllerModuleFilePath() const
    {
        if (configJson["controller"].contains("moduleDirectory")) {
            std::string moduleDirectory = configJson["controller"]["moduleDirectory"].get<std::string>();
            if (moduleDirectory.back() != '/' && moduleDirectory.back() != '\\') {
                moduleDirectory += "/";
            }
#if WIN32
            return moduleDirectory + getControllerModuleName() + SHARED_LIB_EXT;
#else
            return moduleDirectory + "lib" + getControllerModuleName() + SHARED_LIB_EXT;
#endif
        }
        else {
            return "";
        }
    }
    struct MixerInfo {
        bool enable;
        std::string vendor;
        bool enableDebugLog;
        bool enableErrorLog;
    };
    bool getControllerMixerInfo(MixerInfo& info) const
    {
        if (configJson["controller"].contains("mixer")) {
            info.vendor = configJson["controller"]["mixer"]["vendor"];
            info.enableDebugLog = configJson["controller"]["mixer"]["enableDebugLog"];
            info.enableErrorLog = configJson["controller"]["mixer"]["enableErrorLog"];
            info.enable = true;
            return true;
        }
        else {
            return false;
        }
    }

};

#include <filesystem>
#include <regex>
#include <iostream>
#include <algorithm> 

class DroneConfigManager {
private:
    std::vector<DroneConfig> configs;

public:
    DroneConfigManager() {}

    int loadConfigsFromDirectory(const std::string& directoryPath) {
        namespace fs = std::filesystem;
        std::regex pattern("drone_config_(\\d+)\\.json");

        std::vector<std::pair<int, std::string>> filesWithIndex;

        try {
            for (const auto& entry : fs::directory_iterator(directoryPath)) {
                if (entry.is_regular_file()) {
                    std::smatch matches;
                    std::string filename = entry.path().filename().string();

                    if (std::regex_match(filename, matches, pattern) && matches.size() == 2) {
                        int index = std::stoi(matches[1].str());
                        filesWithIndex.emplace_back(index, entry.path().string());
                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "Standard exception: " << e.what() << std::endl;
            return 0;
        } catch (...) {
            std::cerr << "Unknown error occurred during directory traversal." << std::endl;
            return 0;
        }

        std::sort(filesWithIndex.begin(), filesWithIndex.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        int loadedCount = 0;
        for (const auto& [index, filePath] : filesWithIndex) {
            if (loadConfigFromFile(filePath)) {
                loadedCount++;
            } else {
                std::cerr << "Failed to load config from: " << filePath << std::endl;
            }
        }

        return loadedCount;
    }
    bool loadConfigFromFile(const std::string& drone_config_path) {
        DroneConfig drone_config;
        if (!drone_config.init(drone_config_path)) {
            std::cerr << "ERROR: can not find drone config path: " << drone_config_path << std::endl;
            return false;
        }
        std::cout << "INFO: LOADED drone config file: " << drone_config_path << std::endl;
        configs.push_back(drone_config);
        return true;
    }


    bool getConfig(size_t index, DroneConfig& config) {
        if (index >= configs.size()) {
            return false;
        }
        config = configs[index];
        return true;
    }
    int getConfigCount() {
        return configs.size();
    }
};

extern class DroneConfigManager drone_config_manager;

#endif /* _DRONE_CONFIG_HPP_ */