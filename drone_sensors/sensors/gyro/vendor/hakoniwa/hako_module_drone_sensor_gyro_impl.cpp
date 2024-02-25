#include "hako_module_drone_sensor_gyro_impl.h"

const char* hako_module_drone_sensor_gyro_impl_get_name(void)
{
    return "HakoniwaGyroSensor";
}

int hako_module_drone_sensor_gyro_impl_init(void* context)
{
    //TODO
    return 0;
}

mi_drone_sensor_gyro_out_t hako_module_drone_sensor_gyro_impl_run(mi_drone_sensor_gyro_in_t *in)
{
    mi_drone_sensor_gyro_out_t sensor_gyro_output;
    return sensor_gyro_output;
}
