#ifndef _MAVLINK_SERVICE_CONTAINER_HPP_
#define _MAVLINK_SERVICE_CONTAINER_HPP_

#include "mavlink_service.hpp"
#include <functional>

namespace hako::mavlink {
class MavLinkServiceContainer {
public:
    MavLinkServiceContainer() = default;
    ~MavLinkServiceContainer() = default;
    void addService(MavLinkService& service) {
        services_.push_back(std::ref(service));
    }
    std::vector<std::reference_wrapper<MavLinkService>>& getServices() {
        return services_;
    }
private:
    std::vector<std::reference_wrapper<MavLinkService>> services_;
};
} // namespace hako::mavlink

#endif /* _MAVLINK_SERVICE_CONTAINER_HPP_ */
