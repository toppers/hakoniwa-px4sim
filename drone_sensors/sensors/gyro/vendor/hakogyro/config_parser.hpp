#ifndef _CONFIG_PARSER_HPP_
#define _CONFIG_PARSER_HPP_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

class ConfigParser {
private:
    std::unordered_map<std::string, std::string> data;

public:
    bool parse_file(const std::string& file_path) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << file_path << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            auto pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                trim(key);
                trim(value);
                data[key] = value;
            }
        }

        file.close();
        return true;
    }

    std::string get(const std::string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
        return "";
    }

    void trim(std::string& str) const {
        const auto strBegin = str.find_first_not_of(" \t");
        if (strBegin == std::string::npos) return;

        const auto strEnd = str.find_last_not_of(" \t");
        const auto strRange = strEnd - strBegin + 1;

        str = str.substr(strBegin, strRange);
    }
};

#endif /* _CONFIG_PARSER_HPP_ */