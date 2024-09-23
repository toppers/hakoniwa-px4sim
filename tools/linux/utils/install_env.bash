#!/bin/bash

# root ディレクトリ作成 (存在する場合は作成しない)
mkdir -p ${ROOT_DIR}/etc/hakoniwa 
mkdir -p ${ROOT_DIR}/var/lib/hakoniwa/mmap
mkdir -p ${ROOT_DIR}/var/lib/hakoniwa/config
mkdir -p ${ROOT_DIR}/usr/local/lib/hakoniwa
mkdir -p ${ROOT_DIR}/usr/local/bin/hakoniwa
mkdir -p ${ROOT_DIR}/usr/local/include/hakoniwa
mkdir -p ${ROOT_DIR}/usr/local/lib/hakoniwa/hako_binary
mkdir -p ${ROOT_DIR}/usr/local/lib/hakoniwa/py
mkdir -p ${ROOT_DIR}/usr/local/lib/hakoniwa/py/hako_binary

# px4sim をクローン (既にクローンされているか確認)
if [ ! -d hakoniwa-px4sim ]; then
  git clone https://github.com/toppers/hakoniwa-px4sim.git
else
  echo "hakoniwa-px4sim is already cloned."
fi

CURR_DIR=`pwd`
cd hakoniwa-px4sim/hakoniwa

# hakoniwa-core-cpp-client をサブモジュールとして初期化と更新 (既にクローンされているか確認)
if [ ! -d third-party/hakoniwa-core-cpp-client/core ]; then
  git submodule init third-party/hakoniwa-core-cpp-client
  git submodule update --recursive third-party/hakoniwa-core-cpp-client
else
  echo "hakoniwa-core-cpp-client is already initialized."
fi

# hakoniwa-core-cpp をサブモジュールとして初期化と更新
(
  cd third-party/hakoniwa-core-cpp-client && \
  git submodule init core && \
  git submodule update --recursive core
)

# glmのクローン
git submodule init third-party/glm
git submodule update --recursive third-party/glm
# mavlinkのクローン
git submodule init third-party/c_library_v2/
git submodule update --recursive third-party/c_library_v2/
# hakoniwa-ros2pduのクローン
git submodule init third-party/hakoniwa-ros2pdu
git submodule update --recursive third-party/hakoniwa-ros2pdu


cd ${CURR_DIR}

# config ファイルのコピー (存在確認してからコピー)
config_src="hakoniwa-px4sim/hakoniwa/third-party/hakoniwa-core-cpp-client/core/cpp_core_config.json"
config_dst="${ROOT_DIR}/etc/hakoniwa/cpp_core_config.json"
if [ -f "$config_src" ]; then
  cp "$config_src" "$config_dst"
else
  echo "Config file not found: $config_src"
  exit 1
fi

# Google Test のインストール (失敗時はエラーメッセージ表示)
if ! ${SUDO} apt install -y libgtest-dev; then
  echo "Failed to install Google Test."
  exit 1
fi

# jq のインストール (失敗時はエラーメッセージ表示)
if ! ${SUDO} apt install -y jq; then
  echo "Failed to install jq."
  exit 1
fi
