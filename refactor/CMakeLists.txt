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
    set(HAKO_CLIENT_OPTION_FILEPATH "${PROJECT_SOURCE_DIR}/cmake-options/default-cmake-options.cmake")
    include(cmake-options/default-cmake-options.cmake)
endif()

set(HAKO_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(MAVLINK_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../thirdparty/mavlink_c_library_v2")
set(ROS2PDU_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../thirdparty/hakoniwa-ros2pdu")
set(GLM_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../thirdparty/glm")
set(NOLMAN_JSON_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../thirdparty/nolman/single_include")

add_subdirectory(comm)
add_subdirectory(mavlink)
add_subdirectory(logger)
add_subdirectory(physics)
add_subdirectory(aircraft)
add_subdirectory(service)

add_subdirectory(test)

add_subdirectory(main_for_sample)
