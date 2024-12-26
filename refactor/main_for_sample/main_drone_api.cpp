#include "service/drone/impl/drone_service_container.hpp"
#include "service/aircraft/impl/aircraft_service.hpp"
#include "service/drone/drone_service_api_protocol.hpp"
#include "logger/impl/hako_logger.hpp"
#include <iostream>
#include <thread>
#include <sstream>
#include <vector>
#include <string>

using namespace hako::aircraft;
using namespace hako::service;
using namespace hako::service::impl;
using namespace hako::logger;


static std::vector<std::string> split_by_space(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> result;
    std::string word;

    while (iss >> word) {
        result.push_back(word);
    }

    return result;
}

int main(int argc, const char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << "<real_sleep_msec> <drone_config_dir_path>" << std::endl;
        return 1;
    }
    int real_sleep_msec = std::stoi(argv[1]);
    const char* drone_config_dir_path = argv[2];
    // Aircraft サービスの設定
    DroneConfigManager configManager;
    configManager.loadConfigsFromDirectory(drone_config_dir_path);

    AirCraftContainer aircraft_container;
    aircraft_container.createAirCrafts(configManager);

    // AircrfaftControllerの設定
    AircraftControllerContainer controller_container;
    controller_container.createAircraftControllers(configManager);

    DroneServiceContainer service_container(aircraft_container, controller_container);

    auto ret = service_container.startService(1000);
    if (!ret) {
        std::cerr << "Failed to start service" << std::endl;
        return 1;
    }
    HakoLogger::enable();
    //以下の処理をスレッドで実行する
    std::thread th([&service_container, real_sleep_msec]() {
        std::cout << "Start service" << std::endl;
        while (service_container.isServiceAvailable()) {
            HakoLogger::set_time_usec(service_container.getSimulationTimeUsec(0));
            service_container.advanceTimeStep();
            std::this_thread::sleep_for(std::chrono::milliseconds(real_sleep_msec));
        }
        std::cout << "Finish service" << std::endl;
    });
    DroneServiceApiProtocol api(service_container);

    // ドローンの操縦操作をキーボードから取得する
    while (true) {
        std::cout << "> ";

        //改行までの文字列を取得する
        std::string line;
        std::getline(std::cin, line);
        std::vector<std::string> words = split_by_space(line);
        // takeoff は引数を１つ取り、高度を指定する
        if (line.find("takeoff") == 0) {
            if (words.size() < 2) {
                std::cout << "Usage: takeoff <height>" << std::endl;
                continue;
            }
            float height = std::stof(words[1]);
            api.takeoff(0, height);
        }
        // land は引数を取らない
        else if (line.find("land") == 0) {
            api.land(0);
        }
        // move は引数を３つ取り、x, y, z を指定する
        else if (line.find("move") == 0) {
            float x, y, z;
            if (words.size() < 4) {
                std::cout << "Usage: move <x> <y> <z>" << std::endl;
                continue;
            }
            std::sscanf(line.c_str(), "move %f %f %f", &x, &y, &z);
            api.move(0, x, y, z);
        }
        else if (line.find("pos") == 0) {
            auto pos = api.get_position(0);
            std::cout << "position x=" << std::fixed << std::setprecision(1) << pos.x << " y=" << pos.y << " z=" << pos.z << std::endl;
        }
        // quit はプログラムを終了する
        else if (line.find("quit") == 0) {
            break;
        }
        // それ以外は、コマンド仕様を表示する
        else {
            std::cout << "Usage: takeoff <height> | land | move <x> <y> <z> | quit" << std::endl;
        }
    }
    service_container.stopService();
    th.join();
    return 0;
}