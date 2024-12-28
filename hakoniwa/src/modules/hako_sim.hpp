#ifndef _HAKO_SIM_HPP_
#define _HAKO_SIM_HPP_

#include "comm/tcp_connector.hpp"

extern void hako_sim_main(bool master, hako::px4::comm::IcommEndpointType serverEndpoint);

#endif /* _HAKO_SIM_HPP_ */