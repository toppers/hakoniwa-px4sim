#include "hako_px4_master.hpp"
#include "hako_capi.h"

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
