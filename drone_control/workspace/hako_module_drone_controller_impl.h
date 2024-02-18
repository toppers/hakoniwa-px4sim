#ifndef _HAKO_MODULE_DRONE_CONTROLLER_IMPL_H_
#define _HAKO_MODULE_DRONE_CONTROLLER_IMPL_H_

#include "hako_module_drone_controller.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* hako_module_drone_controller_impl_get_name(void);
extern int hako_module_drone_controller_impl_init(void* context);
extern mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in);

#ifdef __cplusplus
}
#endif

#endif /* _HAKO_MODULE_DRONE_CONTROLLER_IMPL_H_ */
