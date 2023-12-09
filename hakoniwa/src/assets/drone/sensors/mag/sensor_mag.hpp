#ifndef _SENSOR_MAG_HPP_
#define _SENSOR_MAG_HPP_

#include "isensor_mag.hpp"
#include "../../utils/sensor_data_assembler.hpp"

namespace hako::assets::drone {

class SensorMag : public hako::assets::drone::ISensorMag {
private:
    double delta_time_sec;
    hako::assets::drone::SensorDataAssembler mag_x;
    hako::assets::drone::SensorDataAssembler mag_y;
    hako::assets::drone::SensorDataAssembler mag_z;

public:
    SensorMag(double dt, int sample_num) : delta_time_sec(dt), mag_x(sample_num), mag_y(sample_num), mag_z(sample_num) 
    {
        this->noise = nullptr;
    }
    virtual ~SensorMag() {}
    void run(const DroneAngleType& angle) override
    {
        (void)this->delta_time_sec;

        double theta = angle.data.y + params_I;
        double psi = angle.data.z - params_D;
        //std::cout << "theta: " << theta << " psi: " << psi << std::endl;

        double x =   params_F * (cos(theta) * cos(psi));
        double y = - params_F * (cos(theta) * sin(psi));
        double z = - params_F * sin(theta);

        this->mag_x.add_data(x);
        this->mag_y.add_data(y);
        this->mag_z.add_data(z);
    }
    DroneMagDataType sensor_value() override
    {
        DroneMagDataType value;
        value.data.x = this->mag_x.get_calculated_value();
        value.data.y = this->mag_y.get_calculated_value();
        value.data.z = this->mag_z.get_calculated_value();
        if (this->noise != nullptr) {
            value.data.x = this->noise->add_noise(value.data.x);
            value.data.y = this->noise->add_noise(value.data.y);
            value.data.z = this->noise->add_noise(value.data.z);
        }
        return value;
    }
    void print() override
    {
        auto result = sensor_value();
        std::cout << "mag( "
                    << result.data.x
                    << ", "
                    << result.data.y
                    << ", "
                    << result.data.z
                    << " )" 
                    << std::endl;
    }

};

}


#endif /* _SENSOR_MAG_HPP_ */