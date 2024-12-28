cmake_minimum_required(VERSION 3.10...3.25)

# プロジェクトの定義
project(hakoniwa-drone-core-comm VERSION 1.0 LANGUAGES C CXX)

# OSの判定
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_TYPE "win")
else()
    set(OS_TYPE "posix")
endif()

# ソースファイルの定義
set(SOURCE_FILES
    tcp_connector.cpp
    udp_connector.cpp
)

# ライブラリの作成
add_library(hako_comm ${SOURCE_FILES})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(hako_comm ws2_32)
else()
    target_include_directories(hako_comm PRIVATE /usr/local/include)
endif()

target_include_directories(hako_comm PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_features(hako_comm PRIVATE cxx_std_17)
