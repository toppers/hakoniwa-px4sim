#ifndef _DRONE_CONFIG_TYPES_HPP_
#define _DRONE_CONFIG_TYPES_HPP_

#include <vector>
#include <optional>

namespace hako::assets::drone {
    struct OutOfBoundsReset {
        std::vector<bool> position; /* X, Y, Z */
        std::vector<bool> velocity; /* X, Y, Z */
        std::vector<bool> angular_velocity; /* X, Y, Z */
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
        std::string BatteryModelCsvFilePath;
        /* constant: バッテリ電圧が一定のモデル */
        /* linear: リニアモデル */
        /* custom: CSVファイルで指定するモデル */
        std::string model;
        /* 公称容量: [Ah] */
        /* バッテリーが供給できる理論上の電気量 */
        double NominalCapacity;
        /* 終了電圧: [V] */
        double EODVoltage;
        /* 公称電圧: [V] */
        /* バッテリーの標準的な動作電圧 */
        double NominalVoltage;
        /* 電圧 Green レベルのミニマム値： [V] */
        double VoltageLevelGreen;
        /* 電圧 Yellow レベルのミニマム値： [V] */
        double VoltageLevelYellow;
        /* 容量 Yellow レベルのミニマム値： [Ah] */
        double CapacityLevelYellow;
    };
}

#endif /* _DRONE_CONFIG_TYPES_HPP_ */
