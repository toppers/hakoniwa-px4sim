#ifndef _HAKO_MODULE_DRONE_CONTROLLER_IMPL_H_
#define _HAKO_MODULE_DRONE_CONTROLLER_IMPL_H_

#include "hako_module_drone_controller.h"

#ifdef __cplusplus
extern "C" {
#endif
#define HAKO_MODULE_DRONE_MASS  0.1 /* kg */

extern const char* hako_module_drone_controller_impl_get_name(void);
extern void* hako_module_drone_controller_impl_create_context(void* arguments);
extern int hako_module_drone_controller_impl_is_operation_doing(void* context);
extern int hako_module_drone_controller_impl_init(void* context);
extern mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in);

#ifdef __cplusplus
}
#endif

#endif /* _HAKO_MODULE_DRONE_CONTROLLER_IMPL_H_ */
