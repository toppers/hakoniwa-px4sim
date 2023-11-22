#ifndef _PX4SIM_THREAD_SENDER_HPP_
#define _PX4SIM_THREAD_SENDER_HPP_

#include "../comm/icomm_connector.hpp"
#include "../mavlink/mavlink_msg_types.hpp"

extern void px4sim_sender_init(hako::px4::comm::ICommIO *comm_io);
extern void px4sim_sender_do_task(void);

extern void px4sim_send_message(hako::px4::comm::ICommIO &clientConnector, MavlinkDecodedMessage &message);
extern void px4sim_send_dummy_command_long(hako::px4::comm::ICommIO &clientConnector);
extern void px4sim_send_dummy_command_long_ack(hako::px4::comm::ICommIO &clientConnector);
extern void px4sim_send_dummy_heartbeat(hako::px4::comm::ICommIO &clientConnector);

#endif /* _PX4SIM_THREAD_SENDER_HPP_ */