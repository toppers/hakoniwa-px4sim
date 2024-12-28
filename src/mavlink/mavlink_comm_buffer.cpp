#include "mavlink_comm_buffer.hpp"
#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilActuatorControls.hpp"
#include <thread>
#include <atomic>
#include <iostream>

using namespace hako::mavlink;

std::unordered_map<std::pair<int, MavlinkMsgType>, std::unique_ptr<MavlinkHakoMessage>, MavlinkCommBuffer::PairHash> MavlinkCommBuffer::cache_;
std::atomic<bool> MavlinkCommBuffer::is_busy_{false};
std::atomic<bool> MavlinkCommBuffer::is_dirty_[MAVLINK_INSTNACE_MAX_NUM][MAVLINK_MSG_TYPE_NUM]{{false}};

void MavlinkCommBuffer::init() {
    set_busy();
    cache_.clear();
    unset_busy();
}

void MavlinkCommBuffer::set_busy() {
    while (is_busy_.exchange(true)) {
        std::this_thread::yield();
    }
}

void MavlinkCommBuffer::unset_busy() {
    is_busy_.store(false);
}

bool MavlinkCommBuffer::write(int index, MavlinkDecodedMessage &message) {
    MavlinkHakoMessage out;
    out.type = message.type;

    switch (message.type) {
        case MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS:
            hako_convert_mavlink2pdu_HakoHilActuatorControls(
                message.data.hil_actuator_controls, 
                out.data.hil_actuator_controls
            );
            //std::cout << "Converted message type: " << message.type << std::endl;
            break;
        default:
            //std::cerr << "Unsupported message type: " << message.type << std::endl;
            return false;
    }

    auto key = std::make_pair(index, message.type);

    set_busy();
    auto &entry = cache_[key];
    if (!entry) {
        entry = std::make_unique<MavlinkHakoMessage>();
    }
    *entry = out;
    is_dirty_[index][message.type].store(true);
    unset_busy();

    return true;
}

bool MavlinkCommBuffer::read(int index, MavlinkHakoMessage &message, bool &is_dirty) {
    auto key = std::make_pair(index, message.type);
    is_dirty = false;
    set_busy();
    auto it = cache_.find(key);
    if (it == cache_.end()) {
        unset_busy();
        return false;
    }
    message = *(it->second);
    is_dirty = is_dirty_[index][message.type].load();
    is_dirty_[index][message.type].store(false);
    unset_busy();

    return true;
}
