#ifndef _HAKO_MODULE_DRONE_SENSOR_H_
#define _HAKO_MODULE_DRONE_SENSOR_H_

typedef struct {
    double value;
} mi_drone_sensor_disturbance_temperature_t;
typedef struct {
    double x;
    double y;
    double z;
} mi_drone_sensor_disturbance_wind_t;

typedef struct {
    mi_drone_sensor_disturbance_temperature_t d_temp;
    mi_drone_sensor_disturbance_wind_t        d_wind;
} mi_drone_sensor_disturbance_t;

#endif /* _HAKO_MODULE_DRONE_SENSOR_H_ */
