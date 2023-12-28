#ifndef _SENSOR_MAG_HPP_
#define _SENSOR_MAG_HPP_

#include "isensor_mag.hpp"
#include "utils/icsv_log.hpp"
#include "../../utils/sensor_data_assembler.hpp"
#include "utils/csv_logger.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace hako::assets::drone {

class SensorMag : public hako::assets::drone::ISensorMag, public ICsvLog {
private:
    double delta_time_sec;
    double total_time_sec;
    hako::assets::drone::SensorDataAssembler mag_x;
    hako::assets::drone::SensorDataAssembler mag_y;
    hako::assets::drone::SensorDataAssembler mag_z;

    glm::dvec3 get_mag_field() const
    {
        glm::dvec3 m;
        m.x = cos(params_D) * cos(params_I);
        m.y = sin(params_D) * cos(params_I);
        m.z = sin(params_I);
        return m;
    }
    glm::mat3 getRotationMatrix(const glm::dvec3& angles_rad) {
        // オイラー角からクォータニオンを生成
        glm::quat quaternion = glm::quat(angles_rad);

        // クォータニオンから回転行列を生成
        glm::mat3 rot = glm::mat3_cast(quaternion);

        return rot;
    }
    void run_old(const DroneAngleType& angle)
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
    void run_fix(const DroneAngleType& angle)
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
    void run_new(const DroneAngleType& angle)
    {
        glm::dvec3 mag = get_mag_field(); // 磁場ベクトルを取得
        glm::mat3 rot = getRotationMatrix(angle.data); // 回転行列を取得

        // 行列の転置
        rot = glm::transpose(rot);

        // ベクトルに行列を適用（キャストが必要）
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

    void run(const DroneAngleType& angle) override
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
    const std::vector<std::string> log_head() override
    {
        return { "timestamp", "X", "Y", "Z" };
    }
    const std::vector<std::string> log_data() override
    {
        auto v = sensor_value();

        return {std::to_string(CsvLogger::get_time_usec()), std::to_string(v.data.x), std::to_string(v.data.y), std::to_string(v.data.z)};
    }

};

}


#endif /* _SENSOR_MAG_HPP_ */