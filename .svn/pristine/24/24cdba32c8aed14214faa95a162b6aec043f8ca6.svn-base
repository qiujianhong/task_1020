# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 编译宏，需要放到project前面
if("${PRODUCT_NAME}" STREQUAL "ECU")
add_definitions(-DAPP_VERSION="0.0.5.1" -DAPP_PUBLISH_TIME="2022/06/16 18:00:00" -DADV_SHOW_NAME="iotManager")
else()
add_definitions(-DAPP_VERSION="1.0.0.1" -DAPP_PUBLISH_TIME="2022/06/16 18:00:00" -DADV_SHOW_NAME="iotManager")
endif()
				
# 项目名称
project(iotManager)

#指定头文件目录
include_directories(${CODE_DIE}/include 
					${CODE_DIE}/lib/include 
					${CODE_DIE}/drivers/hal_include
					${CMAKE_CURRENT_SOURCE_DIR}/include)

# 公共库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE})

# 指定动态库
link_libraries(m dl pthread msgassist mapmsg iniparser framecomm Storage -Wl,-rpath=${INSTALL_DIR}/${PROJECT_NAME}/lib hd -Wl,-rpath=/lib/hal_lib mqttclient template mosquitto cJson sm3sum)

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./source DIR_SRCS)

# 设置目标生成路径(编译完成后, 拷贝并删除, 这样每次编译都会执行)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)
 
# 生成目标可执行文件
add_executable(${PROJECT_NAME} main.c ${DIR_SRCS})

# 拷贝so
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD  
  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/${PROJECT_NAME}
 COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  
  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libmapmsg.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libsm3sum.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
  COMMENT "This command will cp so to target/lib"
  
  COMMAND mkdir -p ${RELEASE_DIR}/${PROJECT_NAME}
  COMMAND rm -fr ${RELEASE_DIR}/${PROJECT_NAME}/*
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/${PROJECT_NAME}
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/${PROJECT_NAME}
)

 
