# ローダブルモジュールのビルド設定
project(hakogyro)

# ソースファイル
set(SOURCE_FILES hako_module_drone_sensor_gyro_impl.cpp)

# ローダブルモジュールとして動的ライブラリを生成
add_library(
    ${PROJECT_NAME} MODULE 
    ${SOURCE_FILES}
    ../hako_module_drone_sensor_gyro.c
)

target_include_directories(
    ${PROJECT_NAME} 
    PRIVATE ../../include
    PRIVATE ..
)
