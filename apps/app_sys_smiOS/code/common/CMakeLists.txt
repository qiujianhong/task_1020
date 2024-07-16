
#指定所依赖的cmake最低版本
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 项目名称
project(common)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/include/commonConfig.h.in"
  "${PROJECT_BINARY_DIR}/commonConfig.h"
)

include_directories(
    ./include 
    ./include/host_include
    ./server/include 
    ./utility/include
    ./client/include
	${CODE_DIE}/utility/include 
	${CODE_DIE}/include 
	${CODE_DIE}/framework/code/msgbus/include 
	${CODE_DIE}/framework/code/storage/include
	${CODE_DIE}/framework/code/msgassist/include
    ${CODE_DIE}/framework/code/mqttclient/include
	${CODE_DIE}/framework/include
	${CODE_DIE}/framework/include/appgui
	${CODE_DIE}/framework/include/hal
	${CMAKE_CURRENT_SOURCE_DIR}/depend/include/qlibc
	${CMAKE_CURRENT_SOURCE_DIR}/depend/include/
    ${PROJECT_BINARY_DIR})

add_subdirectory(server)
add_subdirectory(utility)
add_subdirectory(client)

aux_source_directory(. DIR_MAIN_SRCS)

