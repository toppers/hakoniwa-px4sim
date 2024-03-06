#ifndef _FLIGHT_PLAN_HPP_
#define _FLIGHT_PLAN_HPP_
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

class Pos {
public:
    double x, y;

    Pos(double x = 0.0, double y = 0.0) : x(x), y(y) {}
};

class MovePlan {
public:
    std::vector<Pos> positions;

    bool load_plan_from_file(const std::string& file_path) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << file_path << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::istringstream lineStream(line);
            double x, y;
            if (lineStream >> x >> y) {
                positions.emplace_back(x, y);
            }
        }

        file.close();
        return true;
    }

    size_t get_plan_num() const {
        return positions.size();
    }
};


#endif /* _FLIGHT_PLAN_HPP_ */