set(HAKO_CMAKE_VERSION 3.5.1)
cmake_minimum_required(VERSION ${HAKO_CMAKE_VERSION})
include_directories(PUBLIC ../drone_physics)

project(drone_physics
    VERSION 1.0.0
    DESCRIPTION "matlab interface project"
    LANGUAGES C CXX
)

MESSAGE(STATUS "HAKONIWA_MATLAB_BUILD=" ${HAKONIWA_MATLAB_BUILD})
if (DEFINED HAKONIWA_MATLAB_BUILD)
    add_library(
        drone_physics_matlab STATIC
        drone_system/23a/drone_system_ert_rtw/drone_system.c
        drone_system/23a/slprj/ert/drone_impl/drone_impl.c
    )
    target_include_directories(
        drone_physics_matlab
        PRIVATE .
        PRIVATE drone_system/23a/slprj/ert/_sharedutils
        PRIVATE drone_system/23a/slprj/ert/drone_impl
    )
else()
    add_library(
        drone_physics_matlab STATIC
        drone_physics_matlab_sample.c
        drone_acceleration_by_linear_at_hover.c
    )
    target_include_directories(
        drone_physics_matlab
        PRIVATE .
    )
endif()

if (NOT DEFINED HAKONIWA_BUILD)
    if (DEFINED HAKO_CLIENT_OPTION_FILEPATH)
        include(${HAKO_CLIENT_OPTION_FILEPATH})
    else()
        include(../cmake-options/default-cmake-options.cmake)
    endif()

    add_executable(acctest acctest.cpp)
    target_link_directories(acctest PUBLIC ../drone_physics)
    target_link_libraries(acctest PRIVATE drone_physics_c drone_physics_matlab)
    enable_testing()
    add_test(NAME test COMMAND ./acctest)

    add_executable(ltest linear_test.cpp drone_acceleration_by_linear_at_hover.c)
    target_link_directories(ltest PRIVATE ../drone_physics)
    target_link_libraries(ltest PRIVATE drone_physics_c)
    enable_testing()
    add_test(NAME ltest COMMAND ./ltest)


    add_custom_target(vtest COMMAND ${CMAKE_CTEST_COMMAND} --verbose)
endif()
