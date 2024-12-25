cmake_minimum_required(VERSION 3.10...3.25)

# プロジェクトの定義
project(hakoniwa-drone-core-controller VERSION 1.0 LANGUAGES C CXX)

# OSの判定
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_TYPE "win")
else()
    set(OS_TYPE "posix")
endif()

# ソースファイルの定義
set(SOURCE_FILES
    impl/flight_controller/flight_controller.cpp
    impl/radio_controller/radio_controller.cpp
    impl/aircraft_controller_factory.cpp
)
add_library(hako_controller ${SOURCE_FILES})


if(OS_TYPE STREQUAL "posix")
else()
endif()

target_include_directories(hako_controller 
    PRIVATE ${HAKO_SOURCE_DIR}
    PRIVATE ${HAKO_SOURCE_DIR}/physics
    PRIVATE ${GLM_SOURCE_DIR}
    PRIVATE ${NOLMAN_JSON_SOURCE_DIR}
)

target_compile_features(hako_controller PRIVATE cxx_std_17)

target_link_libraries(hako_controller PRIVATE 
    drone_physics
)
