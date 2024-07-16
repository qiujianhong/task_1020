# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

add_definitions(-DARM_LINUX)

# 项目名称
project(sysMaintain)

#指定头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include 
                    ${CODE_DIE}/include 
                    ${CODE_DIE}/lib/code/msgbus/include 
					${CODE_DIE}/lib/code/storage/include
                    ${CODE_DIE}/lib/include 
					${CODE_DIE}/drivers/hal_include)
					
#链接库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE})


# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./ DIR_SRCS)

# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

set(HAL_LIB hd -Wl,-rpath=/lib/hal_lib)

set(LINK_LIB framecomm iniparser Storage -Wl,-rpath=${INSTALL_DIR}/sysMaintain/lib)
link_libraries(m dl pthread)
# 生成目标可执行文件
add_executable(sysMaintain ${DIR_SRCS})

target_link_libraries(sysMaintain ${LINK_LIB} ${HAL_LIB})
# 拷贝so
add_custom_command(TARGET sysMaintain
  POST_BUILD 
  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../config/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
  COMMAND mkdir -p ${RELEASE_DIR}/sysMaintain
  COMMAND rm -fr ${RELEASE_DIR}/sysMaintain/*
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/sysMaintain
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/sysMaintain
)
