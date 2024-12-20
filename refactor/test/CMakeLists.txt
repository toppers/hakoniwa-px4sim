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

# ソースファイルの定義
set(SOURCE_FILES
    test_main.cpp
    test_comm.cpp
)

find_package(GTest REQUIRED)

include(GoogleTest)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}
    PRIVATE GTest::GTest
    PRIVATE hako_comm
)
target_include_directories(${PROJECT_NAME}
    PRIVATE ${HAKO_SOURCE_DIR}
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
