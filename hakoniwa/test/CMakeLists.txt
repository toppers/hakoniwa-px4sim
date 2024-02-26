set(HAKO_CMAKE_VERSION ${HAKO_CMAKE_VERSION})

project(hako-px4sim-test
    LANGUAGES C CXX
)

find_package(GTest REQUIRED)

include(GoogleTest)

add_executable(
    hako-px4sim-test
    src/assets/physics/rotor_dynamics_test.cpp
    src/assets/physics/thrust_dynamics_test.cpp
    src/assets/utils/utils_test.cpp
    src/assets/sensor/acc_test.cpp
    src/assets/sensor/gyro_test.cpp
    src/assets/sensor/baro_test.cpp
    src/assets/sensor/gps_test.cpp
    src/assets/sensor/mag_test.cpp

    ${PHYSICS_SOURCE_DIR}/rotor_physics.cpp
    ${PHYSICS_SOURCE_DIR}/body_physics.cpp
    main.cpp
)

target_include_directories(
    hako-px4sim-test
    PRIVATE /usr/local/include
    PRIVATE /mingw64/include
    PRIVATE ${MAVLINK_SOURCE_DIR}/all
    PRIVATE ${PROJECT_SOURCE_DIR}/../src
    PRIVATE ${HAKONIWA_PDU_SOURCE_DIR}
    PRIVATE ${HAKONIWA_SOURCE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/../src/assets/drone
    PRIVATE ${PROJECT_SOURCE_DIR}/../src/assets/drone/physics
    PRIVATE ${PROJECT_SOURCE_DIR}/../src/assets/drone/include
    PRIVATE ${GLM_SOURCE_DIR}
    PRIVATE ${HAKONIWA_CORE_SOURCE_DIR}/include
    PRIVATE ${SENSOR_SOURCE_DIR}/include
    PRIVATE ${SENSOR_SOURCE_DIR}/sensors/gyro/include
    PRIVATE ${GTEST_INCLUDE_DIRS}
    PRIVATE ${PHYSICS_SOURCE_DIR}
)

target_link_libraries(hako-px4sim-test
    -pthread
    GTest::GTest
)

gtest_add_tests(TARGET hako-px4sim-test)
