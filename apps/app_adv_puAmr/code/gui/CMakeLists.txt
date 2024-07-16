#指定所依赖的cmake最低版本
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 项目名称
project(gui)

# 添加目录下所有原文件
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ ${PROJECT_NAME}_SRCS)
if("${PRODUCT_NAME}" STREQUAL "ZCU_1" ) 
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/240-128 ${PROJECT_NAME}_guistyle)
else() 
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/160-160 ${PROJECT_NAME}_guistyle)
endif()

# 设置目标生成路径
#SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../target/lib)

# add the executable 添加构建AutoTask的C文件
add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_guistyle})

#头文件搜索路径
target_include_directories(${PROJECT_NAME} PRIVATE
	"${CODE_DIE}/include"
	"${CODE_DIE}/lib/include"
	"${CODE_DIE}/lib/code/storage"
	"${CODE_DIE}/lib/include/appgui"
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../event"
	"${CMAKE_CURRENT_SOURCE_DIR}/../Common698"
	"${CMAKE_CURRENT_SOURCE_DIR}/../AutoTask"
	"${CMAKE_CURRENT_SOURCE_DIR}/../appinclude"
	"${CMAKE_CURRENT_SOURCE_DIR}/../oop/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../ctrl/include"
)