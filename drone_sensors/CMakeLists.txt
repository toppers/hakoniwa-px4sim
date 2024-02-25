cmake_minimum_required(VERSION 3.10)
project(LoadableModuleProject)

# C++標準の指定
set(CMAKE_CXX_STANDARD 17)

set(HAKONIWA_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../hakoniwa")
set(HAKONIWA_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

include_directories(
    include
    ${HAKONIWA_SOURCE_DIR}/src/utils
)

file(GLOB SENSOR_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/sensors ${CMAKE_CURRENT_SOURCE_DIR}/sensors/*)
foreach(SENSOR_DIR IN LISTS SENSOR_DIRS)
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sensors/${SENSOR_DIR})
        add_subdirectory(sensors/${SENSOR_DIR})
    endif()
endforeach()
