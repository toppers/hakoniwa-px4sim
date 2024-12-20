cmake_minimum_required(VERSION 3.10...3.25)

# プロジェクトの定義
project(hakoniwa-drone-core-mavlink VERSION 1.0 LANGUAGES C CXX)

# OSの判定
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_TYPE "win")
else()
    set(OS_TYPE "posix")
endif()
message(STATUS "OS_TYPE=${OS_TYPE}")

# ソースファイルの定義
set(SOURCE_FILES
    mavlink_dump.cpp
)
add_library(hako_mavlink ${SOURCE_FILES})

if(OS_TYPE STREQUAL "posix")
    target_compile_options(hako_mavlink PRIVATE
        -Wno-pedantic
        -Wno-address-of-packed-member
        -Wno-unused-function
        -Wno-missing-field-initializers
    )
else()
    target_compile_options(hako_mavlink PRIVATE
        /wd4200  # Zero-sized array
        /wd4706  # Assignment within conditional expression
        /wd4996  # Deprecated API
    )
endif()


target_include_directories(hako_mavlink PRIVATE ${MAVLINK_SOURCE_DIR}/all)
target_compile_features(hako_mavlink PRIVATE cxx_std_17)
