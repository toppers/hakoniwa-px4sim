#ifndef _ISENSOR_HPP_
#define _ISENSOR_HPP_

#include "primitive_types.hpp"

#include "isensor_noise.hpp"
#include "isensor_data_assembler.hpp"

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
    int index;
    void* opaque;
    char* filepath;
} mi_drone_sensor_gyro_context_t;

typedef struct {
    mi_drone_sensor_gyro_context_t *context;
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
    int (*init) (mi_drone_sensor_gyro_context_t* context);
    mi_drone_sensor_gyro_out_t (*run) (mi_drone_sensor_gyro_in_t *in);
} HakoModuleDroneSensorGyroType;

class ISensor {
protected:
    ISensorNoise *noise;
    void *vendor_model;
    void *context;
public:
    virtual ~ISensor() {}
    virtual void set_vendor(void *vendor, void *context)
    {
        this->vendor_model = vendor;
        this->context = context;
    }
    virtual void set_noise(ISensorNoise *n)
    {
        this->noise = n;
    }
};

}

#endif /* _ISENSOR_HPP_ */