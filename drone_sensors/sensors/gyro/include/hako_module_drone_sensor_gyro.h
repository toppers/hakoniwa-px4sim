#ifndef _HAKO_MODULE_DRONE_SENSOR_GYRO_H_
#define _HAKO_MODULE_DRONE_SENSOR_GYRO_H_

#include "hako_module_drone_sensor.h"

typedef struct {
    mi_drone_sensor_disturbance_t disturbance;
    double angular_velocity_x;
    double angular_velocity_y;
    double angular_velocity_z;
} mi_drone_sensor_gyro_in_t;

typedef struct {
    double angular_velocity_x;
    double angular_velocity_y;
    double angular_velocity_z;
} mi_drone_sensor_gyro_out_t;

#define HAKO_MODULE_DRONE_SENSOR_GYRO_SYMBOLE_NAME   "hako_module_drone_sensor_gyro"
typedef struct {
    int (*init) (void* context);
    mi_drone_sensor_gyro_out_t (*run) (mi_drone_sensor_gyro_in_t *in);
} HakoModuleDroneSensorGyroType;

#endif /* _HAKO_MODULE_DRONE_SENSOR_GYRO_H_ */
