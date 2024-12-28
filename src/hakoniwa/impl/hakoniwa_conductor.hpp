#ifndef _HAKONIWA_CONDUCTOR_HPP_
#define _HAKONIWA_CONDUCTOR_HPP_

#include <stdint.h>

namespace hako::drone::impl {
class HakoniwaConductor {
public:
    virtual ~HakoniwaConductor() {}

    static bool startService(uint64_t delta_time_usec, uint64_t max_delay_usec);
    static bool stopService();
    static bool isStarted() { return isStarted_; }
private:
    HakoniwaConductor() {}
    static bool isStarted_;
};

} // namespace hako::drone

#endif /* _HAKONIWA_CONDUCTOR_HPP_ */