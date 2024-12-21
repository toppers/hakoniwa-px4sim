cmake_minimum_required(VERSION 3.10...3.25)

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

set(HAKO_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(MAVLINK_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../thirdparty/mavlink_c_library_v2")
set(ROS2PDU_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../thirdparty/hakoniwa-ros2pdu")

add_subdirectory(comm)
add_subdirectory(test)
add_subdirectory(mavlink)
#add_subdirectory(drone_physics)
#add_subdirectory(matlab-if)
#add_subdirectory(hakoniwa)
#add_subdirectory(drone_control)
#add_subdirectory(drone_sensors)