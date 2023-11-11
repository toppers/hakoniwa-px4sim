#ifndef _HAKO_SIM_RUNNER_HPP_
#define _HAKO_SIM_RUNNER_HPP_

#include "hako_asset_runner.h"
#include "hako_px4_runner_config.hpp"

typedef struct {
    const char* asset_name;
    const char* robo_name;
    const char* config_path;
    int delta_time_msec;
} HakoSimRunnerArgType;

extern void *hako_sim_runner(void *argp);


#endif /* _HAKO_SIM_RUNNER_HPP_ */