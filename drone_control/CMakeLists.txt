cmake_minimum_required(VERSION 3.10)
project(LoadableModuleProject)

# C++標準の指定
set(CMAKE_CXX_STANDARD 17)

if(EXISTS "${PROJECT_SOURCE_DIR}/../hakoniwa")
    set(HAKONIWA_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../hakoniwa")
else()
    set(HAKONIWA_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../workspace")
endif()

include_directories(
    include
    common/include
    ${HAKONIWA_SOURCE_DIR}/src/utils
)

# workspaceディレクトリ内のサブディレクトリを自動的に追加
file(GLOB STUDENT_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/workspace ${CMAKE_CURRENT_SOURCE_DIR}/workspace/*)
foreach(STUDENT_DIR IN LISTS STUDENT_DIRS)
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/workspace/${STUDENT_DIR})
        add_subdirectory(workspace/${STUDENT_DIR})
    endif()
endforeach()
