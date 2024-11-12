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
    struct BatteryModelParameters {
        std::string vendor;
        /*
         * 公称容量: [Ah]
         * バッテリーが供給できる理論上の電気量
         */
        double NominalCapacity;
        /*
         * 実容量: [Ah]
         * 実際に使用可能な電気容量
         */
        double ActualCapacity;
        /*
         * 定格電圧: [V]
         * バッテリーの公称電圧
         */
        double RatedVoltage;
        /*
         * 公称電圧: [V]
         * バッテリーの標準的な動作電圧
         */
        double NominalVoltage;
        /*
         * 最大放電電流: [A]
         * 安全に放電できる最大電流値
         */
        double MaximumDischargeCurrent;
        /*
         * 定格容量: [Ah]
         * メーカーが保証する容量
         */
        double RatedCapacity;

        /*
         * 電圧 Green レベルのミニマム値： [V]
         */
        double VoltageLevelGreen;
        /*
         * 電圧 Yellow レベルのミニマム値： [V]
         */
        double VoltageLevelYellow;
    };
}

#endif /* _DRONE_CONFIG_TYPES_HPP_ */