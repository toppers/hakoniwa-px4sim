#ifndef _MAVLINK_SERVICE_HPP_
#define _MAVLINK_SERVICE_HPP_

#include "imavlink_comm.hpp"
#include "mavlink_msg_types.hpp"
#include <iostream>
#include <memory>

namespace hako::mavlink {

typedef enum {
    MAVLINK_SERVICE_IO_TYPE_TCP,
    MAVLINK_SERVICE_IO_TYPE_UDP,
    MAVLINK_SERVICE_IO_TYPE_NUM,
} MavlinkServiceIoType;

class MavLinkService {
public:
    explicit MavLinkService(MavlinkServiceIoType io_type, const IcommEndpointType *server_endpoint, const IcommEndpointType *client_endpoint);
    ~MavLinkService() = default;

    bool sendMessage(MavlinkHakoMessage& message);
    bool readMessage(MavlinkHakoMessage& message);
    bool start_Service();
    void stopService();

private:
    bool sendMessage(MavlinkDecodedMessage &message);
    std::unique_ptr<hako::comm::ICommClient> comm_client_;
    std::unique_ptr<hako::comm::ICommServer> comm_server_;
    std::unique_ptr<hako::mavlink::IMavLinkComm> mavlink_comm_;
    std::unique_ptr<IcommEndpointType> client_endpoint_;
    std::unique_ptr<ICommIO> comm_io_;
    IcommEndpointType server_endpoint_;
    MavlinkServiceIoType io_type_;
    bool is_service_started_;
};

} // namespace hako::mavlink

#endif /* _MAVLINK_SERVICE_HPP_ */
