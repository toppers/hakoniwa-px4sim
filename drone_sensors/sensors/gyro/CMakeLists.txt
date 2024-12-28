cmake_minimum_required(VERSION 3.10)
project(LoadableModuleProject)

set(CMAKE_CXX_STANDARD 17)


file(GLOB VENDOR_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/vendor ${CMAKE_CURRENT_SOURCE_DIR}/vendor/*)
foreach(VENDOR_DIR IN LISTS VENDOR_DIRS)
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/vendor/${VENDOR_DIR})
        add_subdirectory(vendor/${VENDOR_DIR})
    endif()
endforeach()
