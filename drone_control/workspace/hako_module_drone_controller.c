#include "hako_module_header.h"
#include "hako_module_drone_controller.h"
#include "hako_module_drone_controller_impl.h"


static const char* hako_module_drone_controller_impl_get_type(void);

HakoModuleHeaderType hako_module_header = {
    .magicno = HAKO_MODULE_MAGICNO,
    .version = HAKO_MODULE_VERSION,
    .get_type = hako_module_drone_controller_impl_get_type,
    .get_name = hako_module_drone_controller_impl_get_name
};

HakoModuleDroneControllerType hako_module_drone_controller = {
    .init = hako_module_drone_controller_impl_init,
    .run = hako_module_drone_controller_impl_run,
};

static const char* hako_module_drone_controller_impl_get_type(void)
{
    return "hako_module_drone_controller";
}
