#ifndef HAKO_CONTROLLER_PARAM_LOADER_HPP
#define HAKO_CONTROLLER_PARAM_LOADER_HPP

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "flight_controller_types.hpp"

class HakoControllerParamLoader {
public:
    static bool is_exist_envpath() {
        const char* env_p = std::getenv("HAKO_CONTROLLER_PARAM_FILE");
        return env_p != nullptr && env_p[0] != '\0';
    }
    static std::string get_controller_param_filedata()
    {
        const char* env_p = std::getenv("HAKO_CONTROLLER_PARAM_FILE");
        if (env_p == nullptr || env_p[0] == '\0') {
            throw std::runtime_error("Environment variable HAKO_CONTROLLER_PARAM_FILE is not set or is empty");
        }
        std::ifstream file(env_p);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + std::string(env_p));
        }
        std::string filedata((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return filedata;
    }
    HakoControllerParamLoader() {
    }
    HakoControllerParamLoader(const std::string& input) {
        loadParametersFromString(input);
    }

    void reload(const std::string &input) {
        std::cout << "Reloading parameters from string" << std::endl;
        loadParametersFromString(input);
    }
    double getParameter(const std::string& paramName) const {
        auto it = parameters.find(paramName);
        if (it != parameters.end()) {
            std::cout << paramName << ": " << it->second << std::endl;
            return it->second;
        } else {
            throw std::runtime_error("Parameter not found: " + paramName);
        }
    }

private:
    std::unordered_map<std::string, double> parameters;

    void parseStream(std::istream& stream) {
        validateAndParseStream(stream);
        std::string line;
        while (std::getline(stream, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::istringstream iss(line);
            std::string param;
            double value;
            if (iss >> param >> value) {
                parameters[param] = value;
            }
        }
    }

    void loadParametersFromString(const std::string& input) {
        std::istringstream iss(input);
        parameters.clear();
        parseStream(iss);
    }    
    void validateAndParseStream(std::istream& stream) {
        std::string line;
        int lineNumber = 0;

        while (std::getline(stream, line)) {
            lineNumber++;

            if (line.empty() || line[0] == '#') {
                continue;
            }

            if (!validateLine(line)) {
                throw std::runtime_error("Invalid parameter format at line " + std::to_string(lineNumber) + ": " + line);
            }

            std::istringstream iss(line);
            std::string param;
            double value;
            iss >> param >> value;
            parameters[param] = value;
        }
    }

    bool validateLine(const std::string& line) const {
        std::istringstream iss(line);
        std::string param;
        double value;

        if (!(iss >> param >> value)) {
            return false;
        }

        std::string extra;
        if (iss >> extra) {
            return false;
        }

        return true;
    }

};

#endif // HAKO_CONTROLLER_PARAM_LOADER_HPP
