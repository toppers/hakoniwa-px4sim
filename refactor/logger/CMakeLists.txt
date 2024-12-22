cmake_minimum_required(VERSION 3.10...3.25)

# プロジェクトの定義
project(hakoniwa-drone-core-logger VERSION 1.0 LANGUAGES C CXX)

# OSの判定
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_TYPE "win")
else()
    set(OS_TYPE "posix")
endif()
message(STATUS "OS_TYPE=${OS_TYPE}")

# ソースファイルの定義
set(SOURCE_FILES
    impl/logger.cpp
)
add_library(hako_logger ${SOURCE_FILES})

target_include_directories(hako_logger 
    PRIVATE ${HAKO_SOURCE_DIR})
target_compile_features(hako_logger PRIVATE cxx_std_17)

target_link_libraries(hako_logger PRIVATE hako_comm)
