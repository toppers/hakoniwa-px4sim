cmake_minimum_required(VERSION 3.10...3.25)

# プロジェクトの定義
project(hakoniwa-drone-core-sample VERSION 1.0 LANGUAGES C CXX)

# OSの判定
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_TYPE "win")
else()
    set(OS_TYPE "posix")
endif()

set(TARGETS
    hako_dronesim
    hako_px4sim
)

add_executable(hako_dronesim main_drone.cpp)
add_executable(hako_px4sim main_px4.cpp)

foreach(TARGET ${TARGETS})
    MESSAGE(STATUS "TARGET: ${TARGET}")
    if(OS_TYPE STREQUAL "posix")
        target_compile_options(${TARGET} PRIVATE
            -Wno-pedantic
            -Wno-address-of-packed-member
            -Wno-unused-function
            -Wno-missing-field-initializers
        )
    else()
        target_compile_options(${TARGET} PRIVATE
            /wd4200  # Zero-sized array
            /wd4706  # Assignment within conditional expression
            /wd4996  # Deprecated API
            /wd4244  # 'double'から'float'への変換の警告を抑制
            /wd4309  # 定数値の切り捨ての警告を抑制
            /wd4267  # Suppress warning C4267: size_t to int conversion
        )
    endif()

    MESSAGE(STATUS "HAKO_SOURCE_DIR: ${HAKO_SOURCE_DIR}")
    MESSAGE(STATUS "nlohmann_json_SOURCE_DIR: ${nlohmann_json_SOURCE_DIR}")
    target_include_directories(${TARGET} 
        PRIVATE ${HAKO_SOURCE_DIR}
        PRIVATE ${GLM_SOURCE_DIR}
        #PRIVATE ${NOLMAN_JSON_SOURCE_DIR}
        PRIVATE ${nlohmann_json_SOURCE_DIR}/single_include
        PRIVATE ${MAVLINK_SOURCE_DIR}/all
        PRIVATE ${ROS2PDU_SOURCE_DIR}/pdu/types
        PRIVATE ${HAKO_CORE_SOURCE_DIR}
    )
    target_compile_features(${TARGET} PRIVATE cxx_std_17)
    target_link_libraries(${TARGET} 
        PRIVATE hako_drone_core
        PRIVATE assets
        PRIVATE conductor
        )
endforeach()

