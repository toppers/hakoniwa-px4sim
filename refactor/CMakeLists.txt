cmake_minimum_required(VERSION 3.10...3.25)

project(hakoniwa-px4sim-root
    VERSION 1.0.0
    DESCRIPTION "hakoniwa-drone-core project"
    LANGUAGES C CXX
)
if (DEFINED HAKO_DRONE_OPTION_FILEPATH)
    message(STATUS "HAKO_DRONE_OPTION_FILEPATH=${HAKO_DRONE_OPTION_FILEPATH}")
    include(${HAKO_DRONE_OPTION_FILEPATH})
else()
    message(STATUS "HAKO_DRONE_OPTION_FILEPATH is not defined. Use default options.")
    set(HAKO_DRONE_OPTION_FILEPATH "${PROJECT_SOURCE_DIR}/cmake-options/default-cmake-options.cmake")
    include(cmake-options/default-cmake-options.cmake)
endif()

set(HAKO_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(MAVLINK_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../thirdparty/mavlink_c_library_v2")
set(ROS2PDU_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../thirdparty/hakoniwa-ros2pdu")
set(GLM_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../thirdparty/glm")
#set(NOLMAN_JSON_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../thirdparty/nolman/single_include")
set(HAKO_CORE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../thirdparty/hakoniwa-core-cpp-client/src")
#link_directories(/usr/local/lib/hakoniwa)
include(FetchContent)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND EXISTS "/etc/os-release")
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
else()
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
endif()
FetchContent_MakeAvailable(json)

add_subdirectory(comm)
add_subdirectory(mavlink)
add_subdirectory(logger)
add_subdirectory(physics)
add_subdirectory(controller)
add_subdirectory(aircraft)
add_subdirectory(service)
add_subdirectory(hakoniwa)

add_subdirectory(test)

set(HAKO_CORE_DIR "${PROJECT_SOURCE_DIR}/../thirdparty/hakoniwa-core-cpp-client")

add_subdirectory(
    ${HAKO_CORE_DIR}
    ${CMAKE_BINARY_DIR}/thirdparty/hakoniwa-core-cpp-client
    )

add_subdirectory(main_for_sample)
