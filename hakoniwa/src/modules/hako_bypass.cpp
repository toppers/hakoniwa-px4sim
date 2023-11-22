#include "hako_bypass.hpp"
#include "../comm/tcp_connector.hpp"
#include "../utils/hako_params.hpp"
#include "../hako/runner/hako_px4_master.hpp"
#include "hako_capi.h"
#include <stdlib.h>
#include <iostream>

static hako::px4::comm::ICommIO *phys_comm  = nullptr;
static hako::px4::comm::ICommIO *ctrl_comm  = nullptr;
static inline void HAKO_ABORT(const char* errmsg)
{
    std::cerr << "ERROR: " << errmsg << std::endl;
    exit(1);
}

void hako_bypass_main(const char* sever_ipaddr, int server_portno)
{
    if (!hako_master_init()) {
        HAKO_ABORT("ERROR: hako_master_init() error" );
    }
    {
        hako::px4::comm::TcpClient client;
        std::cout << "INFO: connecting phys server" << std::endl;
        const char* srv_ip = hako_param_env_get_string(HAKO_BYPASS_IPADDR);
        if (srv_ip == NULL) {
            HAKO_ABORT("Failed to get HAKO_BYPASS_IPADDR");
        }
        int portno;
        if (hako_param_env_get_integer(HAKO_BYPASS_PORTNO, &portno) == false) {
            HAKO_ABORT("Failed to get HAKO_BYPASS_PORTNO");
        }
        hako::px4::comm::IcommEndpointType physEndpoint = { srv_ip, portno };
        phys_comm = client.client_open(nullptr, &physEndpoint);
        if (phys_comm == nullptr) 
        {
            HAKO_ABORT("Failed to connect phys");
        }
        std::cout << "INFO: connected phys server" << std::endl;
    }
    {
        hako::px4::comm::TcpServer srv_comm;
        std::cout << "INFO: waiting for controller connection" << std::endl;
        hako::px4::comm::IcommEndpointType ctrlEndpoint = { sever_ipaddr, server_portno };
        ctrl_comm = srv_comm.server_open(&ctrlEndpoint);
        if (ctrl_comm == nullptr) 
        {
            HAKO_ABORT("Failed to connect controller");
        }
        std::cout << "INFO: connected to controller" << std::endl;
    }

    return;
}