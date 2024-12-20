set(HAKO_CMAKE_VERSION 3.5.1)
cmake_minimum_required(VERSION ${HAKO_CMAKE_VERSION})

project(hakoniwa-px4sim-root
    VERSION 1.0.0
    DESCRIPTION "hakoniwa-drone-core project"
    LANGUAGES C CXX
)
if (DEFINED HAKO_CLIENT_OPTION_FILEPATH)
    message(STATUS "HAKO_CLIENT_OPTION_FILEPATH=${HAKO_CLIENT_OPTION_FILEPATH}")
    include(${HAKO_CLIENT_OPTION_FILEPATH})
else()
    message(STATUS "HAKO_CLIENT_OPTION_FILEPATH is not defined. Use default options.")
    include(cmake-options/default-cmake-options.cmake)
endif()

add_subdirectory(comm)
#add_subdirectory(drone_physics)
#add_subdirectory(matlab-if)
#add_subdirectory(hakoniwa)
#add_subdirectory(drone_control)
#add_subdirectory(drone_sensors)