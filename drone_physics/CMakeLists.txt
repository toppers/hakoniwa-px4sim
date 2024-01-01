set(HAKO_CMAKE_VERSION 3.5.1)
cmake_minimum_required(VERSION ${HAKO_CMAKE_VERSION})

project(drone_physics
    VERSION 1.0.0
    DESCRIPTION "drone_physics project"
    LANGUAGES C CXX
)

if (DEFINED HAKO_CLIENT_OPTION_FILEPATH)
    include(${HAKO_CLIENT_OPTION_FILEPATH})
else()
    include(default-cmake-options.cmake)
endif()

add_executable(
    utest
    ${PROJECT_SOURCE_DIR}/body_physics.cpp
    ${PROJECT_SOURCE_DIR}/rotor_physics.cpp
    ${PROJECT_SOURCE_DIR}/utest.cpp
)
add_executable(
    ctest
    ${PROJECT_SOURCE_DIR}/ctest.c
    ${PROJECT_SOURCE_DIR}/drone_physics_c.cpp
    ${PROJECT_SOURCE_DIR}/body_physics.cpp
    ${PROJECT_SOURCE_DIR}/rotor_physics.cpp
)

enable_testing()
add_test(NAME test COMMAND ./utest ./ctest)
add_custom_target(vtest COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

