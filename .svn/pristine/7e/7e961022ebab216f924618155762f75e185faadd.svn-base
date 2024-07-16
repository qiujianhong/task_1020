# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

if("${PRODUCT_NAME}" STREQUAL "ECU")
add_definitions(-DAPP_VERSION="0.0.5.1" -DAPP_PUBLISH_TIME="2022/06/26 17:00:00")
else()
add_definitions(-DAPP_VERSION="1.0.0.1" -DAPP_PUBLISH_TIME="2022/06/26 17:00:00")
endif()

# 项目名称
project(rspSample)

#指定头文件目录
include_directories(${CODE_DIE}/include 
					${CMAKE_CURRENT_SOURCE_DIR}/include 
					${CODE_DIE}/lib/include 
                    ${CODE_DIE}/lib/code/storage/include
                    ${CODE_DIE}/lib/include 
                    ${CODE_DIE}/drivers/hal_include)

#链接库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE})

# 指定动态库
link_libraries(m dl pthread hd -Wl,-rpath=/lib/hal_lib Storage cJson msgassist iniparser framecomm mapmsg -Wl,-rpath=${INSTALL_DIR}/rspSample/lib 
               mqttclient template mosquitto)

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./source DIR_SRCS)

# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

# 生成目标可执行文件
add_executable(rspSample main.c ${DIR_SRCS})

if(NOT EXISTS ${PROJECT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${AREA_DIR}/rspSample.ini)
	SET(INI_AREA default)
else()
	SET(INI_AREA ${AREA_DIR})
endif()

if(EXISTS ${PROJECT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${BOARD_TYPE}/${AREA_DIR}/rspSample.ini)
	SET(INI_AREA ${BOARD_TYPE}/${AREA_DIR})
endif()

# 拷贝so
add_custom_command(TARGET rspSample
  POST_BUILD 
  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libmapmsg.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib   
  COMMENT "This command will cp so to target/lib"
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/*.sh ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${INI_AREA}/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin 
  
  COMMAND mkdir -p ${RELEASE_DIR}/rspSample
  COMMAND rm -fr ${RELEASE_DIR}/rspSample/*
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/rspSample
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/rspSample
  
  #删除目标文件，下次编译会重新触发拷贝执行
  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
)



