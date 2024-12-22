cmake_minimum_required(VERSION 3.10...3.25)

# プロジェクトの定義
project(hakoniwa-drone-core-aircraft VERSION 1.0 LANGUAGES C CXX)

# OSの判定
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_TYPE "win")
else()
    set(OS_TYPE "posix")
endif()
message(STATUS "OS_TYPE=${OS_TYPE}")

# ソースファイルの定義
set(SOURCE_FILES

)
add_library(hako_aircraft ${SOURCE_FILES})

if(OS_TYPE STREQUAL "posix")
    target_compile_options(hako_aircraft PRIVATE
    )
else()
    target_compile_options(hako_aircraft PRIVATE
    )
endif()


target_include_directories(hako_aircraft 
    PRIVATE ${HAKO_SOURCE_DIR}
    PRIVATE ${GLM_SOURCE_DIR})

target_compile_features(hako_aircraft PRIVATE cxx_std_17)

target_link_libraries(hako_aircraft PRIVATE 
    hako_logger
    drone_physics)
