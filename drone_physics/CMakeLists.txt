set(HAKO_CMAKE_VERSION 3.20)
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

