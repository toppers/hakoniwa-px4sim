#ifndef HAKO_CONTROLLER_PARAM_LOADER_HPP
#define HAKO_CONTROLLER_PARAM_LOADER_HPP

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

class HakoControllerParamLoader {
public:
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
