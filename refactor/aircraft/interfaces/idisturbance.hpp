#ifndef _IDISTURBANCE_HPP_
#define _IDISTURBANCE_HPP_

namespace hako::aircraft {

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


typedef struct {
    mi_drone_sensor_disturbance_t disturbance;
    double angular_velocity_x;
    double angular_velocity_y;
    double angular_velocity_z;
} mi_drone_sensor_gyro_in_t;

}

#endif /* _IDISTURBANCE_HPP_ */