set(HAKO_CMAKE_VERSION 3.5.1)
cmake_minimum_required(VERSION ${HAKO_CMAKE_VERSION})

project(hakoniwa-px4sim
    VERSION 1.0.0
    DESCRIPTION "hakoniwa-px4sim runner project"
    LANGUAGES C CXX
)

if (DEFINED HAKO_CLIENT_OPTION_FILEPATH)
    include(${HAKO_CLIENT_OPTION_FILEPATH})
else()
    include(../cmake-options/default-cmake-options.cmake)
endif()

set(GLM_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third-party/glm")
set(PHYSICS_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../drone_physics")
set(CONTROL_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../drone_control")
set(SENSOR_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../drone_sensors")
set(MATLAB_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../matlab-if")
set(MAVLINK_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third-party/c_library_v2")
set(HAKONIWA_CORE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third-party/hakoniwa-core-cpp-client/core/src")
set(HAKONIWA_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third-party/hakoniwa-core-cpp-client/src/hakoc/src")
set(HAKONIWA_ASSET_DIR "${PROJECT_SOURCE_DIR}/third-party/hakoniwa-core-cpp-client/src/include")
set(HAKONIWA_PDU_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third-party/hakoniwa-ros2pdu/pdu/types")

if (DO_TEST)
    add_subdirectory(test)
endif()

add_subdirectory(third-party/hakoniwa-core-cpp-client)
if (HAKONIWA_BUILD)
    add_subdirectory(${MATLAB_SOURCE_DIR} matlab-if)
endif()
add_subdirectory(src)
