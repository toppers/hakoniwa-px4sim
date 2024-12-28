#include "hakoniwa/impl/hakoniwa_conductor.hpp"
#include "include/hako_conductor.h"
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

using namespace hako::drone::impl;

bool HakoniwaConductor::isStarted_ = false;
#define HAKO_CONFIG_DEFAULT_PATH "/etc/hakoniwa/cpp_core_config.json"

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
