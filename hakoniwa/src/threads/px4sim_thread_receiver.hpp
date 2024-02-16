#ifndef _PX4SIM_THREAD_RECEIVER_HPP_
#define _PX4SIM_THREAD_RECEIVER_HPP_

#include "hako_capi.h"

extern hako_time_t hako_px4_asset_time;
extern hako_time_t hako_asset_time;

typedef struct {
    int index;
    void* comm_io;
} Px4simRcvArgType;
extern void *px4sim_thread_receiver(void *arg);

#endif /* _PX4SIM_THREAD_RECEIVER_HPP_ */