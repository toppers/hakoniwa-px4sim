#include "hako_px4_master.hpp"
#include "hako_capi.h"
#include "hako_asset_runner.h"

void* hako_px4_master_thread_run(void* arg)
{
    if (arg) {
        //nothing to do
    }
    while (true) {
        (void)hako_master_execute();
    }
    return nullptr;
}

hako_time_t hako_get_current_time_usec()
{
    return hako_asset_runner_get_world_time();
}
