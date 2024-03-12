#include "hako_module_drone_sensor_gyro_impl.h"
#include "config_parser.hpp"
#include <iostream>

const char* hako_module_drone_sensor_gyro_impl_get_name(void)
{
    return "HakoniwaGyroSensor";
}

typedef struct {
    double temp_min;
    double temp_max;
    double temp_drift;
} SensorTempDriftParamType;

static double str2double(const std::string& str)
{
    try {
        double value = std::stod(str);
        return value;
    } catch (const std::invalid_argument& e) {
        std::cout << "ERROR: Invalid argument: " << e.what() << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "ERROR: Out of range: " << e.what() << std::endl;
    }
    return 0.0;
}

int hako_module_drone_sensor_gyro_impl_init(mi_drone_sensor_gyro_context_t* context)
{
    ConfigParser config_parser;
    SensorTempDriftParamType *sensor_temp_drift_param = new SensorTempDriftParamType();
    if (sensor_temp_drift_param == nullptr) {
        std::cout << "ERROR: no memory for SensorTempDriftParamType" << std::endl;
        return -1;
    }
    if (context->filepath == nullptr) {
        std::cout << "ERROR: not set filepath of sensor gyro" << std::endl;
        return -1;
    }
    std::string filepath((const char*)context->filepath);
    config_parser.parse_file(filepath);
    sensor_temp_drift_param->temp_min = str2double(config_parser.get("TEMP_MIN"));
    sensor_temp_drift_param->temp_max = str2double(config_parser.get("TEMP_MAX"));
    sensor_temp_drift_param->temp_drift = str2double(config_parser.get("TEMP_DRIFT"));
    std::cout << "TEMP_MIN: " << sensor_temp_drift_param->temp_min << std::endl;
    std::cout << "TEMP_MAX: " << sensor_temp_drift_param->temp_max << std::endl;
    std::cout << "TEMP_DRIFT: " << sensor_temp_drift_param->temp_drift << std::endl;
    context->opaque = (void*)sensor_temp_drift_param;
    return 0;
}

/*
 * data: rad/sec
 */
static double noise(SensorTempDriftParamType *sensor_temp_drift_param, double data, double temperature)
{
    if (temperature < sensor_temp_drift_param->temp_min) {
        return data +( (temperature - sensor_temp_drift_param->temp_min) * sensor_temp_drift_param->temp_drift);
    }
    else if (temperature > sensor_temp_drift_param->temp_max ) {
        return data +( (temperature - sensor_temp_drift_param->temp_max ) * sensor_temp_drift_param->temp_drift);
    }
    return data;
}

mi_drone_sensor_gyro_out_t hako_module_drone_sensor_gyro_impl_run(mi_drone_sensor_gyro_in_t *in)
{
    SensorTempDriftParamType *sensor_temp_drift_param = (SensorTempDriftParamType*)in->context->opaque;
    mi_drone_sensor_gyro_out_t out;
    double temperature = in->disturbance.d_temp.value;
    //std::cout << "temperature[" << in->context->index << "] : " << temperature  << "DRIFT: " << sensor_temp_drift_param->temp_drift << std::endl;
    out.angular_velocity_x = noise(sensor_temp_drift_param, in->angular_velocity_x, temperature);
    out.angular_velocity_y = noise(sensor_temp_drift_param, in->angular_velocity_y, temperature);
    out.angular_velocity_z = noise(sensor_temp_drift_param, in->angular_velocity_z, temperature);
    return out;
}
