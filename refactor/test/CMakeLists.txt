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
)

# 実行ファイルの定義
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}
    PRIVATE GTest::gtest
    PRIVATE GTest::gtest_main
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
