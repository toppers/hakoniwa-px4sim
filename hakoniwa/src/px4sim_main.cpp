#include <iostream>
#include <cstdlib>
#include <string>
#include "comm/tcp_connector.hpp"
#include "hako_capi.h"
#include "modules/hako_phys.hpp"
#include "modules/hako_sim.hpp"
#include "modules/hako_pid.hpp"
#include "modules/hako_ext.hpp"
#include "utils/hako_params.hpp"
#include "config/drone_config.hpp"
#ifndef WIN32
#include "modules/hako_bypass.hpp"
#endif
class DroneConfigManager drone_config_manager;

int main(int argc, char* argv[]) 
{
    if(argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port> <mode={sim|wsim|bypass|phys|pid|ext}> " << std::endl;
        return -1;
    }
    const char* value = std::getenv("HAKO_MASTER_DISABLE");
    bool enable_master = true;
    if (value != nullptr) {
        std::string valueStr(value);
        if (valueStr == "true") {
            std::cout << "hakoniwa master is disabled" << std::endl;
            enable_master = false;
        } else {
            std::cout << "hakoniwa master is enabled" << std::endl;
        }
    } else {
        std::cout << "hakoniwa master is enabled" << std::endl;
    }
    const char* serverIp = argv[1];
    int serverPort = std::atoi(argv[2]);
    const char* arg_mode = argv[3];
    hako_param_env_init();
    hako::px4::comm::IcommEndpointType serverEndpoint = { serverIp, serverPort };

    hako::px4::comm::ICommIO *comm_io  = nullptr;
    if (strcmp("pid", arg_mode) == 0) {
        hako_pid_main(enable_master);
        //not returned function.
        //do not pass
    }
    if (strcmp("ext", arg_mode) == 0) {
        hako_ext_main(enable_master);
        //not returned function.
        //do not pass
    }
    else if (strcmp("phys", arg_mode) == 0) {
        hako_phys_main();
        //not returned function.
        //do not pass
    }
    else if ((strcmp("sim", arg_mode) == 0) ||  (strcmp("wsim", arg_mode) == 0)) {
        bool master = true;
        if  (strcmp("wsim", arg_mode) == 0) {
            master = false;
        }
        hako_sim_main(master, serverEndpoint);
        //not returned function.
        //do not pass
    }
#ifndef WIN32
    else if (strcmp("bypass", arg_mode) == 0) {
        hako_bypass_main(serverIp, serverPort);
        //not returned function.
        //do not pass
    }
#endif
    else {
        std::cerr << "ERROR unknown mode = " << arg_mode << std::endl;
        return -1;
    }
    comm_io->close();
    return 0;
}
