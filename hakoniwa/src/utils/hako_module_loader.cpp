#include "hako_module_loader.hpp"
#include <dlfcn.h>
#include <iostream>

void *hako_module_handle(const char* filepath, HakoModuleHeaderType** header)
{
    void *handle = dlopen(filepath, RTLD_NOW);
    if (handle == nullptr) {
        std::cerr << "ERROR: can not load module filepath: " << filepath << std::endl;
        return nullptr;
    }
    auto* varp = dlsym(handle, HAOKO_MODULE_HEADER_NAME);
    if (varp == nullptr) {
        std::cerr << "ERROR: can not find symbol: " << HAOKO_MODULE_HEADER_NAME << " on module filepath: " << filepath << std::endl;
        return nullptr;
    }
    *header = static_cast<HakoModuleHeaderType*>(varp);
    if ((*header)->magicno != HAKO_MODULE_MAGICNO) {
        std::cerr << "ERROR: invalid magicno: " << (*header)->magicno << " on module filepath: " << filepath << std::endl;
        return nullptr;
    }
    if ((*header)->version != HAKO_MODULE_VERSION) {
        std::cerr << "ERROR: invalid version: " << (*header)->version << " on module filepath: " << filepath << std::endl;
        return nullptr;
    }
    return handle;
}

void *hako_module_load_symbol(void *handle, const char* symbole_name)
{
    auto* varp = dlsym(handle, symbole_name);
    if (varp == nullptr) {
        std::cerr << "ERROR: can not find symbol: " << symbole_name  << std::endl;
        return nullptr;
    }
    return varp;
}
