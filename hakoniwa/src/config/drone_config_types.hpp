#ifndef _DRONE_CONFIG_TYPES_HPP_
#define _DRONE_CONFIG_TYPES_HPP_

#include <vector>
#include <optional>

namespace hako::assets::drone {
    struct OutOfBoundsReset {
        std::vector<bool> position; // X, Y, Z
        std::vector<bool> velocity; // X, Y, Z
        std::vector<bool> angular_velocity; // X, Y, Z
    };
    struct RotorBatteryModelConstants {
        double R;
        double Cq;
        double K;
        double D;
        double J;
    };
}

#endif /* _DRONE_CONFIG_TYPES_HPP_ */