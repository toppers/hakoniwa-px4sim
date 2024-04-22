#include "hako_module_drone_controller_impl.h"
#include <algorithm>
#include <iostream>

static double get_limit_value(double input_value, double base_value, double min_value, double max_value)
{
    double limited_value = base_value + std::max(min_value, std::min(max_value, input_value));
    return limited_value;
}

const char* hako_module_drone_controller_impl_get_name(void)
{
    return "RadioController";
}

#define ALMOST_EQUAL(target, current, range) ( ( (current) >= ((target) - (range)) ) &&  ( (current) <= ((target) + (range)) ) )


void* hako_module_drone_controller_impl_create_context(void*)
{
    return nullptr;
}

int hako_module_drone_controller_impl_is_operation_doing(void *context)
{
    return true;
}

int hako_module_drone_controller_impl_init(void*)
{
    return 0;
}

mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    mi_drone_control_out_t out = {};
    //std::cout << "in->target.throttle.power: " << in->target.throttle.power << std::endl;
    return out;
}


