cmake_minimum_required(VERSION 3.10...3.25)

project(drone_physics
    VERSION 1.0.0
    DESCRIPTION "drone_physics project"
    LANGUAGES C CXX
)

if (NOT DEFINED HAKO_CLIENT_OPTION_FILEPATH)
    MESSAGE(STATUS "HAKO_CLIENT_OPTION_FILEPATH is not defined. Use default options.")
    include(default-cmake-options.cmake)
endif()

add_library(
    drone_physics
    drone_frames.cpp
    rotor_physics.cpp
    body_physics.cpp
)

add_library(
    drone_physics_c
    drone_physics_c.cpp
    drone_frames.cpp
    body_physics.cpp
    rotor_physics.cpp
)

add_executable(utest utest.cpp)
target_link_libraries(utest drone_physics)

#if (NOT CMAKE_SYSTEM_NAME MATCHES "Linux")
add_executable(ctest ctest.c)
target_link_libraries(ctest drone_physics_c)
#endif()

add_executable(examples examples.cpp)
target_link_libraries(examples drone_physics)

add_executable(cexamples cexamples.c)
target_link_libraries(cexamples drone_physics_c)


enable_testing()
add_test(NAME utest COMMAND ./utest)
add_test(NAME ctest COMMAND ./ctest)

#add_executable(glmtest glmtest.cpp)
#target_include_directories(glmtest PRIVATE ../hakoniwa/third-party/glm)
#add_test(NAME glmtest COMMAND ./glmtest)

add_custom_target(vtest COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

