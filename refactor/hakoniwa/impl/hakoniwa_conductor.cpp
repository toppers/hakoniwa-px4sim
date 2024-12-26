#include "hakoniwa/hakoniwa_conductor.hpp"
#include "include/hako_conductor.h"

using namespace hako::drone;

bool HakoniwaConductor::isStarted_ = false;

bool HakoniwaConductor::startService(uint64_t delta_time_usec, uint64_t max_delay_usec)
{
    if (isStarted_) {
        return false;
    }
    if (hako_conductor_start(delta_time_usec, max_delay_usec) != 0) {
        isStarted_ = true;
        return true;
    }
    else {
        return false;
    }
}

bool HakoniwaConductor::stopService()
{
    if (!isStarted_) {
        return false;
    }
    hako_conductor_stop();
    isStarted_ = false;
    return true;
}
