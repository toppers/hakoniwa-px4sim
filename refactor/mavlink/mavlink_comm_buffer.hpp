#ifndef MAVLINK_COMM_BUFFER_HPP
#define MAVLINK_COMM_BUFFER_HPP

#include <unordered_map>
#include <vector>
#include <memory>
#include <utility>
#include <iostream>
#include <atomic>
#include "mavlink_msg_types.hpp"

namespace hako::mavlink {

class MavlinkCommBuffer {
public:
    static void init();
    static bool write(int index, MavlinkDecodedMessage &message);
    static bool read(int index, MavlinkHakoMessage &message);

private:
    struct PairHash {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            auto hash1 = std::hash<T1>{}(p.first);
            auto hash2 = std::hash<T2>{}(p.second);
            return hash1 ^ (hash2 << 1);
        }
    };
    static void set_busy();
    static void unset_busy();
    static std::atomic<bool>  is_busy_;
    static std::unordered_map<std::pair<int, MavlinkMsgType>, std::unique_ptr<MavlinkHakoMessage>, PairHash> cache_;
};

} // namespace hako::mavlink

#endif // MAVLINK_COMM_BUFFER_HPP
