#include "hako_module_header.h"
#include "hako_module_drone_controller.h"
#include "hako_module_drone_controller_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

    static const char* hako_module_drone_controller_impl_get_type(void);

#ifdef WIN32
#define HAKO_MODULE_EXPORT __declspec(dllexport)
#else
#define HAKO_MODULE_EXPORT
#endif

    HAKO_MODULE_EXPORT HakoModuleHeaderType hako_module_header = {
        .magicno = HAKO_MODULE_MAGICNO,
        .version = HAKO_MODULE_VERSION,
        .get_type = hako_module_drone_controller_impl_get_type,
        .get_name = hako_module_drone_controller_impl_get_name
    };

    HAKO_MODULE_EXPORT HakoModuleDroneControllerType hako_module_drone_controller = {
        .create_context = hako_module_drone_controller_impl_create_context,
        .is_operation_doing = hako_module_drone_controller_impl_is_operation_doing,
        .init = hako_module_drone_controller_impl_init,
        .run = hako_module_drone_controller_impl_run,
    };

    static const char* hako_module_drone_controller_impl_get_type(void)
    {
        return "hako_module_drone_controller";
    }

#ifdef __cplusplus
}
#endif
