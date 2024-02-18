#ifndef _HAKO_MODULE_LOADER_HPP_
#define _HAKO_MODULE_LOADER_HPP_

#include "hako_module_header.hpp"

/*
 * return value: loaded handle
 */
extern void *hako_module_handle(const char* filepath, HakoModuleHeaderType* header);
/*
 * return value: symbol
 */
extern void *hako_module_load_symbol(void *handle, const char* symbole_name);

#endif /* _HAKO_MODULE_LOADER_HPP_ */