#ifndef _HAKO_PX4_MASETER_HPP_
#define _HAKO_PX4_MASETER_HPP_

#include "hako_capi_types.h"

extern void* hako_px4_master_thread_run(void* arg);
extern hako_time_t hako_get_current_time_usec();

#endif /* _HAKO_PX4_MASETER_HPP_ */