#include "hako_module_drone_sensor_gyro_impl.h"

const char* hako_module_drone_sensor_gyro_impl_get_name(void)
{
    return "HakoniwaGyroSensor";
}


int hako_module_drone_sensor_gyro_impl_init(void*)
{
    return 0;
}
#define TEMP_MIN    10.0
#define TEMP_MAX    20.0
#define DRIFT       0.01
/*
 * data: rad/sec
 */
static double noise(double data, double temperature)
{
    if (temperature >= TEMP_MIN && temperature <= TEMP_MAX) {
        return data;
    }
    return data + (temperature * DRIFT);
}

mi_drone_sensor_gyro_out_t hako_module_drone_sensor_gyro_impl_run(mi_drone_sensor_gyro_in_t *in)
{
    mi_drone_sensor_gyro_out_t out;
    double temperature = in->disturbance.d_temp.value;
    out.angular_velocity_x = noise(in->angular_velocity_x, temperature);
    out.angular_velocity_y = noise(in->angular_velocity_y, temperature);
    out.angular_velocity_z = noise(in->angular_velocity_z, temperature);
    return out;
}
