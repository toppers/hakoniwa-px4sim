cmake_minimum_required(VERSION 3.10...3.25)

# プロジェクトの定義
project(hakoniwa-drone-core VERSION 1.0 LANGUAGES C CXX)

# OSの判定
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_TYPE "win")
else()
    set(OS_TYPE "posix")
endif()

# ソースファイルの定義
set(SOURCE_FILES
    impl/hakoniwa_pdu_accessor.cpp
)
add_library(hako_core ${SOURCE_FILES})


if(OS_TYPE STREQUAL "posix")
    target_compile_options(hako_core PRIVATE
        -Wno-pedantic
        -Wno-address-of-packed-member
        -Wno-unused-function
        -Wno-missing-field-initializers
    )
else()
    target_compile_options(hako_core PRIVATE
        /wd4200  # Zero-sized array
        /wd4706  # Assignment within conditional expression
        /wd4996  # Deprecated API
        /wd4244  # 'double'から'float'への変換の警告を抑制
        /wd4309  # 定数値の切り捨ての警告を抑制
        /wd4267  # Suppress warning C4267: size_t to int conversion
    )
endif()



target_include_directories(hako_core 
    PRIVATE ${HAKO_SOURCE_DIR}
    PRIVATE ${HAKO_CORE_SOURCE_DIR}
    PRIVATE ${ROS2PDU_SOURCE_DIR}/pdu/types
    )

target_compile_features(hako_core PRIVATE cxx_std_17)

target_link_libraries(hako_core PRIVATE 
    hako_service
    )
