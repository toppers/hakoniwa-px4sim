#include <iostream>
#include "comm/tcp_connector.hpp"
#include "hako_capi.h"
#include "modules/hako_bypass.hpp"
#include "modules/hako_phys.hpp"
#include "modules/hako_sim.hpp"
#include "utils/hako_params.hpp"
#include "config/drone_config.hpp"

class DroneConfig drone_config;

int main(int argc, char* argv[]) 
{
    if(argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port> <mode={sim|wsim|bypass|phys}> "  << std::endl;
        return -1;
    }
    const char* serverIp = argv[1];
    int serverPort = std::atoi(argv[2]);
    const char* arg_mode = argv[3];
    hako_param_env_init();
    std::string drone_config_path = hako_param_env_get_string(DRONE_CONFIG_PATH);
    if (drone_config.init(drone_config_path) == false)
    {
        std::cerr << "ERROR: can not find ../config/drone_config.json" << std::endl;
        return -1;
    }
    hako::px4::comm::IcommEndpointType serverEndpoint = { serverIp, serverPort };

    hako::px4::comm::ICommIO *comm_io  = nullptr;
    if (strcmp("bypass", arg_mode) == 0) {
        hako_bypass_main(serverIp, serverPort);
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
    else {
        std::cerr << "ERROR unknown mode = " << arg_mode << std::endl;
        return -1;
    }
    comm_io->close();
    return 0;
}
