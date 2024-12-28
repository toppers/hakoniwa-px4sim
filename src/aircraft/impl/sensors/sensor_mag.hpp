#ifndef _SENSOR_MAG_HPP_
#define _SENSOR_MAG_HPP_

#include "aircraft/interfaces/isensor_mag.hpp"
#include "aircraft/impl/sensors/sensor_mag.hpp"
#include "aircraft/impl/noise/sensor_data_assembler.hpp"
#include "logger/ilog.hpp"
#include "logger/impl/hako_logger.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace hako::logger;

namespace hako::aircraft {

class SensorMag : public ISensorMag, public ILog {
private:
    double delta_time_sec;
    double total_time_sec;
    SensorDataAssembler mag_x;
    SensorDataAssembler mag_y;
    SensorDataAssembler mag_z;

    glm::dvec3 get_mag_field() const
    {
        glm::dvec3 m;
        m.x = cos(params_D) * cos(params_I);
        m.y = sin(params_D) * cos(params_I);
        m.z = sin(params_I);
        return m;
    }
    glm::mat3 getRotationMatrix(const glm::dvec3& angles_rad) {
        glm::quat quaternion = glm::quat(angles_rad);

        glm::mat3 rot = glm::mat3_cast(quaternion);

        return rot;
    }
    void run_old(const DroneEulerType& angle)
    {
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
    void run_fix(const DroneEulerType& angle)
    {
        double x =   (cos(params_I) * cos(params_D));
        double y =   (cos(params_I) * sin(params_D));
        double z =   sin(params_I);

        x += angle.data.x;
        y += angle.data.y;
        z += angle.data.z;

        this->mag_x.add_data(x * params_F);
        this->mag_y.add_data(y * params_F);
        this->mag_z.add_data(z * params_F);
    }
    void run_new(const DroneEulerType& angle)
    {
        glm::dvec3 mag = get_mag_field();
        glm::mat3 rot = getRotationMatrix(angle.data);

        rot = glm::transpose(rot);

        mag = glm::dvec3(rot * glm::vec3(mag));
        double x = mag.x * params_F;
        double y = mag.y * params_F;
        double z = mag.z * params_F;

        this->mag_x.add_data(x);
        this->mag_y.add_data(y);
        this->mag_z.add_data(z);
    }
public:
    SensorMag(double dt, int sample_num) : delta_time_sec(dt), mag_x(sample_num), mag_y(sample_num), mag_z(sample_num) 
    {
        this->noise = nullptr;
    }
    virtual ~SensorMag() {}

    void run(const DroneEulerType& angle) override
    {
        run_new(angle);
        total_time_sec += delta_time_sec;
    }
    DroneMagDataType sensor_value() override
    {
        DroneMagDataType value;
        value.data.x = this->mag_x.get_calculated_value();
        value.data.y = this->mag_y.get_calculated_value();
        value.data.z = this->mag_z.get_calculated_value();
        if (this->noise != nullptr) {
            value.data.x = this->noise->add_random_noise(value.data.x);
            value.data.y = this->noise->add_random_noise(value.data.y);
            value.data.z = this->noise->add_random_noise(value.data.z);
        }
        return value;
    }
    const std::vector<LogHeaderType>& log_head() override
    {
        static const std::vector<LogHeaderType> headers = {
            {"timestamp", LOG_TYPE_UINT64}, // timestamp: unsigned 64-bit integer
            {"X", LOG_TYPE_DOUBLE},         // X: double
            {"Y", LOG_TYPE_DOUBLE},         // Y: double
            {"Z", LOG_TYPE_DOUBLE}          // Z: double
        };
        return headers;
    }
    const std::vector<LogDataType>& log_data() override
    {
        auto v = sensor_value();
        static std::vector<LogDataType> data;
        data.clear();
        data.push_back(HakoLogger::get_time_usec()); // timestamp (uint64_t)
        data.push_back(v.data.x);                  // X (double)
        data.push_back(v.data.y);                  // Y (double)
        data.push_back(v.data.z);                  // Z (double)
        return data;
    }

};

}


#endif /* _SENSOR_MAG_HPP_ */