#ifndef _MAVLINK_SERVICE_HPP_
#define _MAVLINK_SERVICE_HPP_

#include "imavlink_comm.hpp"
#include "mavlink_msg_types.hpp"
#include <iostream>
#include <memory>
#include <atomic>

namespace hako::mavlink {

typedef enum {
    MAVLINK_SERVICE_IO_TYPE_TCP,
    MAVLINK_SERVICE_IO_TYPE_UDP,
    MAVLINK_SERVICE_IO_TYPE_NUM,
} MavlinkServiceIoType;

class MavLinkService {
public:
    explicit MavLinkService(int index, MavlinkServiceIoType io_type, const IcommEndpointType *server_endpoint, const IcommEndpointType *client_endpoint);
    ~MavLinkService();

    bool sendMessage(MavlinkHakoMessage& message);
    bool readMessage(MavlinkHakoMessage& message);
    bool startService();
    void stopService();

private:
    bool sendMessage(MavlinkDecodedMessage &message);
    bool sendCommandLongAck();
    void receiver();
    std::unique_ptr<hako::comm::ICommClient> comm_client_;
    std::unique_ptr<hako::comm::ICommServer> comm_server_;
    std::unique_ptr<hako::mavlink::IMavLinkComm> mavlink_comm_;
    std::unique_ptr<IcommEndpointType> client_endpoint_;
    std::unique_ptr<ICommIO> comm_io_;
    IcommEndpointType server_endpoint_;
    MavlinkServiceIoType io_type_;
    std::atomic<bool> is_service_started_;
    int index_;
    std::unique_ptr<std::thread> receiver_thread_;
};

} // namespace hako::mavlink

#endif /* _MAVLINK_SERVICE_HPP_ */
