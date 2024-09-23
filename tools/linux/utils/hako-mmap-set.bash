#!/bin/bash

show_usage() {
    echo "Usage:"
    echo "  $0 [-r] -p /path/to  # Set custom path or reset to default"
    exit 1
}

# デフォルト値
default_shm_type="shm"
if [ -z "$default_core_mmap_path" ]
then
  default_core_mmap_path="/var/lib/hakoniwa/mmap"
fi
if [ -z "$config_file" ]
then
  config_file="/etc/hakoniwa/cpp_core_config.json"
fi

# 引数チェック
while getopts ":rp:" opt; do
  case $opt in
    r)
      reset=1
      ;;
    p)
      path_to=$OPTARG
      ;;
    \?)
      show_usage
      ;;
  esac
done

# パスが指定されていない場合の処理
if [ -z "$path_to" ] && [ -z "$reset" ]; then
    echo "Error: Path must be specified with -p option."
    show_usage
fi

# コンフィグファイルの存在チェック
if [ ! -f "$config_file" ]; then
    echo "Error: Configuration file does not exist at $config_file"
    exit 1
fi

# jqがインストールされているかチェック
if ! command -v jq &> /dev/null; then
    echo "Error: jq is not installed. Please install jq to use this script."
    exit 1
fi

# -r オプションが指定されている場合、デフォルト値にリセット
if [ ! -z "$reset" ]; then
    jq --arg shm_type "$default_shm_type" --arg core_mmap_path "$default_core_mmap_path" \
       '.shm_type = $shm_type | .core_mmap_path = $core_mmap_path' \
       "$config_file" > tmp.$$.json && mv tmp.$$.json "$config_file"
    echo "Configuration reset to default."
else
    # カスタムパスで更新
    jq --arg shm_type "mmap" --arg core_mmap_path "$path_to" \
       '.shm_type = $shm_type | .core_mmap_path = $core_mmap_path' \
       "$config_file" > tmp.$$.json && mv tmp.$$.json "$config_file"
    echo "Configuration updated with custom path."
fi

echo "INFO: updated ${config_file}"