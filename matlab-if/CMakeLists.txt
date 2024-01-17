set(HAKO_CMAKE_VERSION 3.5.1)
cmake_minimum_required(VERSION ${HAKO_CMAKE_VERSION})

project(drone_physics
    VERSION 1.0.0
    DESCRIPTION "matlab interface project"
    LANGUAGES C CXX
)

if (DEFINED HAKO_CLIENT_OPTION_FILEPATH)
    include(${HAKO_CLIENT_OPTION_FILEPATH})
else()
    include(../cmake-options/default-cmake-options.cmake)
endif()



add_executable(acctest acctest.c)
target_link_libraries(acctest ../drone_physics/drone_physics)


enable_testing()
add_test(NAME test COMMAND ./acctest)
add_custom_target(vtest COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

