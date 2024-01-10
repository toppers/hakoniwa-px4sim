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

add_library(
    drone_physics
    body_physics.cpp
    rotor_physics.cpp
)

add_library(
    drone_physics_c
    drone_physics_c.cpp
    body_physics.cpp
    rotor_physics.cpp
)

add_executable(utest utest.cpp)
target_link_libraries(utest drone_physics)

add_executable(ctest ctest.c)
target_link_libraries(ctest drone_physics_c)

add_executable(rotortest rotortest.cpp)
target_link_libraries(rotortest drone_physics)

add_executable(examples examples.cpp)
target_link_libraries(examples drone_physics)

add_executable(cexamples cexamples.c)
target_link_libraries(cexamples drone_physics_c)

enable_testing()
add_test(NAME test COMMAND ./utest)
add_custom_target(vtest COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

