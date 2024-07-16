# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置路径
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 项目名称
project(msgbus)
 
#指定头文件目录
include_directories(${CODE_DIE}/include ${CODE_DIE}/lib/include ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 设置lib生成路径
SET(LIBRARY_OUTPUT_PATH ${LIB_DIR})

add_library(MsgUdp SHARED msgUdp.c)
add_library(MsgMqtt SHARED msgMqtt.c)
