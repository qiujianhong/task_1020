# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 设置桌面gui公共库路径
set(GUI_COMMON ${CODE_DIE}/release/lib)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# gui模块编译子目录
set(guistyle "160-160")
add_subdirectory(${guistyle})
add_subdirectory(gui_prtl)

add_definitions(-DARM_LINUX)

# 项目名称
project(deskGui)

#指定头文件目录
include_directories(${CODE_DIE}/include 
					${CMAKE_CURRENT_SOURCE_DIR}/include 
					${CMAKE_CURRENT_SOURCE_DIR}/xml
					${CMAKE_CURRENT_SOURCE_DIR}/msg/include
					${CMAKE_CURRENT_SOURCE_DIR}/comm/include
                    ${CODE_DIE}/lib/code/msgbus/include 
                    ${CODE_DIE}/lib/code/storage/include
                    ${CODE_DIE}/lib/include
					${CODE_DIE}/lib/include/appgui
                    ${CODE_DIE}/drivers/hal_include
					${CMAKE_CURRENT_SOURCE_DIR}/${guistyle})

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./ DIR_SRCS)
aux_source_directory(./commfun DIR_SRCS)
aux_source_directory(./xml DIR_SRCS)
aux_source_directory(./hidden DIR_SRCS)
aux_source_directory(./msg/source DIR_SRCS)
aux_source_directory(./include DIR_SRCS)

# 设置lib生成路径
SET(LIBRARY_OUTPUT_PATH ${GUI_COMMON})

# 生成目标可执行文件
add_library(deskGui SHARED ${DIR_SRCS})





 
