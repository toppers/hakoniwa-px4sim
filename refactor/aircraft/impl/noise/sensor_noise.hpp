#ifndef _SENSOR_NOISE_HPP_
#define _SENSOR_NOISE_HPP_

#include "aircraft/interfaces/isensor_noise.hpp"
#include <random>
#include <cmath>
#include <cstdlib>
#include <limits>

namespace hako::aircraft {

class SensorNoise : public ISensorNoise {
private:
    double stdDev;
    SensorNoise() {}
public:
    SensorNoise(double v) : stdDev(v) {}
    virtual ~SensorNoise() {}

    double add_random_noise(double data) override
    {

        double x0;
        double b0, b1;

        do {
            b0 = static_cast<double>(rand()) / RAND_MAX;
            b1 = static_cast<double>(rand()) / RAND_MAX;
        } while (b0 <= std::numeric_limits<float>::min());

        x0 = sqrt(-2.0 * log(b0)) * cos(M_PI * 2.0 * b1);

        if (std::isinf(x0) || std::isnan(x0)) {
            x0 = 0.0;
        }

        return data + (x0 * stdDev);
    }

};

}

#endif /* _SENSOR_NOISE_HPP_ */