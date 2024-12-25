//#include "service/drone/impl/drone_service_container.hpp"
#include "logger/impl/hako_logger.hpp"
#include <iostream>

//using namespace hako::service;
using namespace hako::logger;


int main(int argc, const char* argv[])
{

    const char* server_ip = "127.0.0.1";
    const int server_port = 4560;

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << "<real_sleep_msec> <drone_config_dir_path>" << std::endl;
        return 1;
    }
    return 0;
}