#include "mavlink_service.hpp"
#include "comm/tcp_connector.hpp"
#include "comm/udp_connector.hpp"
#include "mavlink_comm_tcp.hpp"
#include "mavlink_comm_udp.hpp"
#include "mavlink_encoder.hpp"
#include "mavlink_decoder.hpp"
#include "mavlink_comm_buffer.hpp"

#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilSensor.hpp"
#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilGps.hpp"
#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilStateQuaternion.hpp"
#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilActuatorControls.hpp"

using namespace hako::comm;
using namespace hako::mavlink;

MavLinkService::MavLinkService(int index, MavlinkServiceIoType io_type, const IcommEndpointType* server_endpoint, const IcommEndpointType* client_endpoint)
    : comm_io_(nullptr), is_service_started_(false), index_(index), receiver_thread_(nullptr)
{
    index_ = index;
    is_service_started_ = false;
    if (server_endpoint == nullptr)
    {
        std::cerr << "Invalid endpoint" << std::endl;
        return;
    }
    if (client_endpoint != nullptr)
    {
        client_endpoint_ = std::make_unique<IcommEndpointType>();
    }
    else {
        client_endpoint_ = nullptr;
    }
    server_endpoint_ = *server_endpoint;
    io_type_ = io_type;
    MavlinkCommBuffer::init();
    switch (io_type)
    {
    case MAVLINK_SERVICE_IO_TYPE_TCP:
        hako::comm::comm_init();
        comm_server_ = std::make_unique<TcpServer>();
        mavlink_comm_ = std::make_unique<MavLinkCommTcp>();
        break;
    case MAVLINK_SERVICE_IO_TYPE_UDP:
        comm_server_ = std::make_unique<UdpServer>();
        mavlink_comm_ = std::make_unique<MavLinkCommUdp>();
        break;
    default:
        std::cerr << "Invalid IO type" << std::endl;
        break;
    }
}
MavLinkService::~MavLinkService() {
    stopService();
    if (receiver_thread_ && receiver_thread_->joinable()) {
        receiver_thread_->join();
    }
}
bool MavLinkService::sendMessage(MavlinkHakoMessage& message)
{
    if (!is_service_started_)
    {
        std::cerr << "Service is not started" << std::endl;
        return false;
    }
    if (comm_io_ == nullptr)
    {
        std::cerr << "Invalid comm io" << std::endl;
        return false;
    }
    MavlinkDecodedMessage decoded_message;
    switch (message.type) {
        case MAVLINK_MSG_TYPE_HIL_SENSOR:
        {
            hako_convert_pdu2mavlink_HakoHilSensor(message.data.hil_sensor, decoded_message.data.hil_sensor);
            break;
        }
        case MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS:
        {
            hako_convert_pdu2mavlink_HakoHilActuatorControls(message.data.hil_actuator_controls, decoded_message.data.hil_actuator_controls);
            break;
        }
        case MAVLINK_MSG_TYPE_HIL_STATE_QUATERNION:
        {
            hako_convert_pdu2mavlink_HakoHilStateQuaternion(message.data.hil_state_quaternion, decoded_message.data.hil_state_quaternion);
            break;
        }
        case MAVLINK_MSG_TYPE_HIL_GPS:
        {
            hako_convert_pdu2mavlink_HakoHilGps(message.data.hil_gps, decoded_message.data.hil_gps);
            break;
        }
        default:
            std::cerr << "Invalid message type" << std::endl;
            return false;
    }
    return sendMessage(decoded_message);
}
bool MavLinkService::sendMessage(MavlinkDecodedMessage& message)
{
    if (comm_io_ == nullptr)
    {
        std::cerr << "Invalid comm io" << std::endl;
        return false;
    }
    mavlink_message_t mavlinkMsg;
    if (mavlink_encode_message(&mavlinkMsg, &message)) 
    {
        char packet[MAVLINK_MAX_PACKET_LEN];
        int packetLen = mavlink_get_packet(packet, sizeof(packet), &mavlinkMsg);
        if (packetLen > 0) 
        {
            if (mavlink_comm_->sendMessage(comm_io_.get(), packet, packetLen))
            {
                //std::cout << "Sent MAVLink message with length: " << sentDataLen << std::endl;
            }
            else 
            {
                std::cerr << "Failed to send MAVLink message" << std::endl;
                return false;
            }
        }
        else {
            std::cerr << "Failed to get packet" << std::endl;
            return false;
        }
        return true;
    }
    else {
        std::cerr << "Failed to encode message" << std::endl;
        return false;
    }
}
bool MavLinkService::sendCommandLongAck()
{
    MavlinkDecodedMessage message;
    message.type = MAVLINK_MSG_TYPE_LONG;
    
    // Setting up the fields for COMMAND_LONG
    message.data.command_long.target_system = 1; // The system which should execute the command, for example, 1 for the first MAV
    message.data.command_long.target_component = 1; // The component which should execute the command, for example, 0 for a generic component
    message.data.command_long.command = 520;
    message.data.command_long.confirmation = 1; // 0: First transmission of this command. 1-255: Confirmation transmissions (e.g. for kill command)
    message.data.command_long.param1 = 0; // Parameter 1, as defined by MAV_CMD enum
    message.data.command_long.param2 = 0; // Parameter 2, as defined by MAV_CMD enum
    message.data.command_long.param3 = 0; // Parameter 3, as defined by MAV_CMD enum
    message.data.command_long.param4 = 0; // Parameter 4, as defined by MAV_CMD enum
    message.data.command_long.param5 = 0; // Parameter 5, as defined by MAV_CMD enum
    message.data.command_long.param6 = 0; // Parameter 6, as defined by MAV_CMD enum
    message.data.command_long.param7 = 0; // Parameter 7, as defined by MAV_CMD enum
    
    auto ret = sendMessage(message);
    std::cout << "INFO: COMMAND_LONG ack sended: ret = " << ret << std::endl;
    return ret;
}

bool MavLinkService::readMessage(MavlinkHakoMessage& message)
{
    return MavlinkCommBuffer::read(index_, message);
}

void MavLinkService::receiver() {
    try {
        if (!comm_io_) {
            throw std::runtime_error("Invalid comm io");
        }
        while (is_service_started_) {
            char recvBuffer[1024];
            int recvDataLen;
            if (mavlink_comm_->receiveMessage(comm_io_.get(), recvBuffer, sizeof(recvBuffer), &recvDataLen)) {
                mavlink_message_t msg;
                bool ret = mavlink_decode((uint8_t)index_, recvBuffer, recvDataLen, &msg);
                if (ret) {
                    MavlinkDecodedMessage message;
                    if (mavlink_get_message(&msg, &message)) {
                        if (message.type == MAVLINK_MSG_TYPE_LONG) {
                            sendCommandLongAck();
                        }
                    }
                    MavlinkCommBuffer::write(index_, message);
                }
            } else {
                std::cerr << "Failed to receive message" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in receiver thread: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in receiver thread" << std::endl;
    }
}

bool MavLinkService::startService() {
    if (is_service_started_) {
        std::cerr << "Service is already started" << std::endl;
        return false;
    }
    if (!comm_server_) {
        std::cerr << "Invalid comm server" << std::endl;
        return false;
    }

    auto io = comm_server_->server_open(&server_endpoint_);
    if (!io) {
        std::cerr << "Failed to open server" << std::endl;
        return false;
    }

    comm_io_ = std::unique_ptr<ICommIO>(io);
    is_service_started_ = true;

    receiver_thread_ = std::make_unique<std::thread>(&MavLinkService::receiver, this);
    if (!receiver_thread_ || !receiver_thread_->joinable()) {
        std::cerr << "Failed to create receiver thread" << std::endl;
        is_service_started_ = false;
        comm_io_->close();
        comm_io_ = nullptr;
        return false;
    }
    return true;
}
void MavLinkService::stopService() {
    if (!is_service_started_) {
        std::cerr << "Service is not started" << std::endl;
        return;
    }

    is_service_started_ = false;

    if (comm_io_) {
        comm_io_->close();
    }

    if (receiver_thread_ && receiver_thread_->joinable()) {
        receiver_thread_->join();
    }
}
