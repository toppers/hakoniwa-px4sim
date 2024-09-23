#!/bin/bash

script_path=$(realpath "$0")
echo "Script path: $script_path"

which sudo
if [ $? -eq 0 ]
then
    export SUDO=`which sudo`
else
    export SUDO=
fi

DIR_PATH=`(cd "$(dirname $script_path)" && pwd)`
UTILS_PATH=${DIR_PATH}/utils

# ROOT_DIR ディレクトリは好みで変えてください
CURR_DIR=`pwd`
export ROOT_DIR=${CURR_DIR}/root

bash ${UTILS_PATH}/install_env.bash
bash ${UTILS_PATH}/build_hako.bash
bash ${UTILS_PATH}/install_hako.bash

cp ${DIR_PATH}/config/* ${ROOT_DIR}/var/lib/hakoniwa/config/

export default_core_mmap_path=${ROOT_DIR}/var/lib/hakoniwa/mmap
export config_file=${ROOT_DIR}/etc/hakoniwa/cpp_core_config.json
bash ${UTILS_PATH}/hako-mmap-set.bash -p ${ROOT_DIR}/var/lib/hakoniwa/mmap

bash ${UTILS_PATH}/add_env_bashrc.bash
