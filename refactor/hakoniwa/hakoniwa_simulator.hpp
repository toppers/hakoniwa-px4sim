#ifndef _HAKONIWA_SIMULATOR_HPP_
#define _HAKONIWA_SIMULATOR_HPP_

#include <stdint.h>
#include <memory>
#include <mutex>
#include <string>
#include "service/iservice_container.hpp"

using namespace hako::service;

namespace hako::drone {
class HakoniwaSimulator {
public:
    virtual ~HakoniwaSimulator() {}

    static bool registerService(std::string& asset_name, std::string& config_path, uint64_t delta_time_usec, std::shared_ptr<IServiceContainer> service_container);
    static bool startService();
    static bool stopService();
    static bool isStarted() { return isStarted_; }
    static std::shared_ptr<IServiceContainer> service_container_;
private:
    static std::mutex mtx_;
    static bool isStarted_;
};
} // namespace hako::drone

#endif /* _HAKONIWA_SIMULATOR_HPP_ */