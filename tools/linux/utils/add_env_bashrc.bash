#!/bin/bash

# TOP_DIR の設定 (必要に応じて適切なディレクトリに変更)
TOP_DIR="${ROOT_DIR}/usr/local"
#echo ${ROOT_DIR}

# 環境変数の定義
LD_LIBRARY_PATH_VAR="${TOP_DIR}/lib/hakoniwa:${TOP_DIR}/lib/hakoniwa/py"
PATH_VAR="${TOP_DIR}/bin/hakoniwa"
HAKO_CUSTOM_JSON_PATH_VAR="${ROOT_DIR}/var/lib/hakoniwa/config/custom.json"
PYTHONPATH_VAR="${TOP_DIR}/lib/hakoniwa:${TOP_DIR}/lib/hakoniwa/py"
DRONE_CONFIG_PATH_VAR="config/mixer-api"
HAKO_CONTROLLER_PARAM_FILE_VAR="../drone_control/config/param-api-mixer.txt"

# .bashrc に環境変数を追加する関数
add_to_bashrc() {
    local var_name="$1"
    local var_value="$2"
    
    echo "export ${var_name}=${var_value}" >> ~/.bashrc
    echo "${var_name} added to .bashrc"
}

# 環境変数を.bashrcに追加 (すでに存在しない場合のみ)
add_to_bashrc "LD_LIBRARY_PATH" "${LD_LIBRARY_PATH_VAR}:${LD_LIBRARY_PATH}"
add_to_bashrc "PATH" "${PATH_VAR}:${PATH}"
add_to_bashrc "PYTHONPATH" "${PYTHONPATH_VAR}:${PYTHONPATH}"
add_to_bashrc "DRONE_CONFIG_PATH" "${DRONE_CONFIG_PATH_VAR}"
add_to_bashrc "HAKO_CONTROLLER_PARAM_FILE" "${HAKO_CONTROLLER_PARAM_FILE_VAR}"
add_to_bashrc "HAKO_CUSTOM_JSON_PATH" "${HAKO_CUSTOM_JSON_PATH_VAR}"

# .bashrc を再読み込み
source ~/.bashrc

echo "Installation complete. Environment variables have been set."
