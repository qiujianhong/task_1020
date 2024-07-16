# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置路径
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 设置桌面gui公共库路径
set(GUI_COMMON ${CODE_DIE}/release/lib)

#add_subdirectory(appPage)

# 项目名称
project(Layer)

add_definitions(-DARM_LINUX)

#指定头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR} 
					${CMAKE_CURRENT_SOURCE_DIR}/../include
					${CODE_DIE}/include 
					${CODE_DIE}/lib/code/storage/include 
					${CODE_DIE}/lib/include 
					${CODE_DIE}/lib/include/appgui
					${CODE_DIE}/drivers/hal_include)

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./ DIR_SRCS160)

# 设置lib生成路径
SET(LIBRARY_OUTPUT_PATH ${GUI_COMMON})

# 生成目标可执行文件
add_library(Layer SHARED ${DIR_SRCS160})

#往目标中添加文件
#target_sources(Layer PRIVATE ./menulayer.cpp)
#target_sources(Layer PRIVATE ./miniguidef.cpp)
#target_sources(Layer PRIVATE ./debuginfo.cpp)
#target_sources(Layer PRIVATE ./commdef.cpp)
#target_sources(Layer PRIVATE ./template.cpp) 

