cmake_minimum_required(VERSION 3.10...3.25)

# プロジェクトの定義
project(hakoniwa-drone-core-test VERSION 1.0 LANGUAGES C CXX)

# OSの判定
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_TYPE "win")
else()
    set(OS_TYPE "posix")
endif()
message(STATUS "OS_TYPE=${OS_TYPE}")

# Google Testの取得とビルド（Windows向け）
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
)
FetchContent_MakeAvailable(googletest)

# ソースファイルの定義
set(SOURCE_FILES
    test_main.cpp
    test_comm.cpp
    test_mavlink_comm.cpp
    test_mavlink.cpp
    test_logger.cpp
    aircraft/test_sensor_assembler.cpp
    aircraft/test_acc.cpp
    aircraft/test_baro.cpp
    aircraft/test_gps.cpp
    aircraft/test_gyro.cpp
    aircraft/test_mag.cpp
    aircraft/test_thrust_dynamics.cpp
    aircraft/test_rotor_dynamics.cpp
    aircraft/test_body_dynamics.cpp
    aircraft/test_aircraft.cpp
)

# 実行ファイルの定義
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
if(OS_TYPE STREQUAL "posix")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wno-pedantic
        -Wno-address-of-packed-member
        -Wno-unused-function
        -Wno-missing-field-initializers
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        /wd4200  # Zero-sized array
        /wd4706  # Assignment within conditional expression
        /wd4996  # Deprecated API
        /wd4244  # 'double'から'float'への変換の警告を抑制
        /wd4309  # 定数値の切り捨ての警告を抑制
        /wd4267  # Suppress warning C4267: size_t to int conversion
    )
endif()
target_link_libraries(${PROJECT_NAME}
    PRIVATE GTest::gtest
    PRIVATE GTest::gtest_main
    PRIVATE hako_mavlink
    PRIVATE hako_logger
    PRIVATE drone_physics
    PRIVATE hako_aircraft
)
target_include_directories(${PROJECT_NAME}
    PRIVATE ${HAKO_SOURCE_DIR}
    PRIVATE ${MAVLINK_SOURCE_DIR}/all
    PRIVATE ${ROS2PDU_SOURCE_DIR}/pdu/types
    PRIVATE ${GLM_SOURCE_DIR}
    PRIVATE ${NOLMAN_JSON_SOURCE_DIR}
)

# C++標準設定
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# CTestの登録
include(CTest)
enable_testing()
add_test(NAME AllTests COMMAND ${PROJECT_NAME})
