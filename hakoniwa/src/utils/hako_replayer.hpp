#ifndef _HAKO_REPLAYER_HPP_
#define _HAKO_REPLAYER_HPP_

#include "csv_data.hpp"
#include <map>
#include <string>
#include <vector>
#include <iostream>

class HakoReplayer {
private:
    std::map<std::string, CsvData*> vehicle_replayers;
    std::map<std::string, double> mass;
public:
    HakoReplayer() {}

    ~HakoReplayer() {
        close_all();
    }
    void set_mass(const std::string& vehicle_id, double m)
    {
        if (vehicle_replayers.find(vehicle_id) == vehicle_replayers.end()) {
            mass[vehicle_id] = m;
        }
    }
    double get_mass(const std::string& vehicle_id)
    {
        return mass[vehicle_id];
    }
    void add_vehicle(const std::string& vehicle_id, const std::string& file_name) {
        std::cout << "vehicle: " << vehicle_id << " logpath: " << file_name << std::endl;
        if (vehicle_replayers.find(vehicle_id) == vehicle_replayers.end()) {
            vehicle_replayers[vehicle_id] = new CsvData(file_name);
            // skip head
            std::vector<std::string> values;
            vehicle_replayers[vehicle_id]->read(values);
            // Remove the last row
            vehicle_replayers[vehicle_id]->remove_last();
        }
    }

    bool next(const std::string& vehicle_id, std::vector<std::string>& data) {
        if (vehicle_replayers.find(vehicle_id) != vehicle_replayers.end()) {
            return vehicle_replayers[vehicle_id]->read(data);
        }
        return false;
    }

    void close_all() {
        for (auto& [vehicle_id, replayer] : vehicle_replayers) {
            delete replayer;
        }
        vehicle_replayers.clear();
    }

    std::vector<std::string> get_vehicle_ids() const {
        std::vector<std::string> vehicle_ids;
        for (const auto& [vehicle_id, _] : vehicle_replayers) {
            vehicle_ids.push_back(vehicle_id);
        }
        return vehicle_ids;
    }
};

#endif /* _HAKO_REPLAYER_HPP_ */
