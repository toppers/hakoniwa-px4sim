#ifndef _DRONE_CONFIG_TYPES_HPP_
#define _DRONE_CONFIG_TYPES_HPP_

#include <vector>

namespace hako::assets::drone {
    struct OutOfBoundsReset {
        std::vector<bool> position; // X, Y, Z
        std::vector<bool> velocity; // X, Y, Z
        std::vector<bool> angular_velocity; // X, Y, Z
    };
}

#endif /* _DRONE_CONFIG_TYPES_HPP_ */