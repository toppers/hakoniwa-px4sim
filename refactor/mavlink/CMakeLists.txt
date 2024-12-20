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

# ライブラリの作成
add_library(hako_mavlink ${SOURCE_FILES})

target_include_directories(hako_mavlink PRIVATE ${MAVLINK_SOURCE_DIR}/all)
target_compile_features(hako_mavlink PRIVATE cxx_std_17)
