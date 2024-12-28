# ローダブルモジュールのビルド設定
project(RadioController)

# ソースファイル
set(SOURCE_FILES
    hako_module_drone_controller_impl.cpp
)

# ローダブルモジュールとして動的ライブラリを生成
add_library(
    ${PROJECT_NAME} MODULE 
    ${SOURCE_FILES}
    ../../common/src/frame_convertor.cpp
    ../hako_module_drone_controller.c
)
# ヘッダーファイルのインクルードディレクトリ指定
target_include_directories(
    ${PROJECT_NAME} 
    PRIVATE utils
    PRIVATE include
    PRIVATE ../../include
    PRIVATE ..
    PRIVATE .
)
