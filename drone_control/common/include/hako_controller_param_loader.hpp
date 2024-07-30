#ifndef HAKO_CONTROLLER_PARAM_LOADER_HPP
#define HAKO_CONTROLLER_PARAM_LOADER_HPP

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib> // for std::getenv
#include "flight_controller_types.hpp"

class HakoControllerParamLoader {
public:
    static bool is_exist_envpath() {
        const char* env_p = std::getenv("HAKO_CONTROLLER_PARAM_FILE");
        return env_p != nullptr && env_p[0] != '\0';
    }

    HakoControllerParamLoader() {
        const char* env_p = std::getenv("HAKO_CONTROLLER_PARAM_FILE");
        if (env_p == nullptr || env_p[0] == '\0') {
            throw std::runtime_error("Environment variable HAKO_CONTROLLER_PARAM_FILE is not set or is empty");
        }
        std::cout << "controller param path: " << env_p << std::endl;
        filename = std::string(env_p);
    }

    HakoControllerParamLoader(const std::string& filename) : filename(filename) {}

    void loadParameters() {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        std::string line;
        while (std::getline(file, line)) {
            // コメント行をスキップ
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

        file.close();
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
    std::string filename;
    std::unordered_map<std::string, double> parameters;
};

#endif // HAKO_CONTROLLER_PARAM_LOADER_HPP
