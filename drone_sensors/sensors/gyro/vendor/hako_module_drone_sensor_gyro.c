#include "hako_module_header.h"
#include "hako_module_drone_sensor_gyro.h"
#include "hako_module_drone_sensor_gyro_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

    static const char* hako_module_drone_sensor_gyro_impl_get_type(void);

#ifdef WIN32
#define HAKO_MODULE_EXPORT __declspec(dllexport)
#else
#define HAKO_MODULE_EXPORT
#endif

    HAKO_MODULE_EXPORT HakoModuleHeaderType hako_module_header = {
        .magicno = HAKO_MODULE_MAGICNO,
        .version = HAKO_MODULE_VERSION,
        .get_type = hako_module_drone_sensor_gyro_impl_get_type,
        .get_name = hako_module_drone_sensor_gyro_impl_get_name
    };

    HAKO_MODULE_EXPORT HakoModuleDroneSensorGyroType hako_module_drone_sensor_gyro = {
        .init = hako_module_drone_sensor_gyro_impl_init,
        .run = hako_module_drone_sensor_gyro_impl_run,
    };

    static const char* hako_module_drone_sensor_gyro_impl_get_type(void)
    {
        return "hako_module_drone_sensor_gyro";
    }

#ifdef __cplusplus
}
#endif
