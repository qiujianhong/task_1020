# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)
include(${CODE_DIE}/build/area.cmake)

add_definitions(-DAPP_VERSION="1.0.0.1" -DAPP_PUBLISH_TIME="2022/06/26 17:00:00" -DADV_SHOW_NAME="本地")

# 项目名称
project(ccoRouter)

#指定头文件目录
include_directories(${CODE_DIE}/include 
					${CMAKE_CURRENT_SOURCE_DIR}/area/include 
					${CMAKE_CURRENT_SOURCE_DIR}/include 
					${CMAKE_CURRENT_SOURCE_DIR}/crt_common
					${CODE_DIE}/include  
					${CODE_DIE}/lib/include 
					${CODE_DIE}/lib/include/appgui
					${CODE_DIE}/lib/code/storage/include
					${CODE_DIE}/lib/code/msgbus/include
					${CODE_DIE}/drivers/hal_include)

#链接库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE})

set(LIB_PATH_CCOROUTER /usr/local/extapp)
# 指定动态库
link_libraries(hd -Wl,-rpath=/lib/hal_lib m dl pthread 
               MsgUdp MsgMqtt mqttclient template Storage iniparser framecomm cJson msgassist gdk deskGui guiprtl Layer -Wl,-rpath=${INSTALL_DIR}/ccoRouter/lib
			   mosquitto)

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./crt_common DIR_SRCS)
aux_source_directory(./appgui	  SCRS_APPGUI)
aux_source_directory(./area/${AREA_DIR} SCRS_AREA)

# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

# 生成目标可执行文件
add_executable(ccoRouter CcoMain.c ${DIR_SRCS} ${SCRS_APPGUI} ${SCRS_AREA})

# 拷贝so
add_custom_command(TARGET ccoRouter
  POST_BUILD 
  COMMAND cp ${CODE_DIE}/depends/miniGui/${BOARD_TYPE}/libgdk-1.1.so.1 ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib/
  COMMAND cp ${LIB_DIR}/libdeskGui.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libLayer.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libguiprtl.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp -fr ${CODE_DIE}/depends/miniGui/${BOARD_TYPE}/gdk ${CMAKE_CURRENT_SOURCE_DIR}/../config/cfg
  
  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libMsgUdp.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libMsgMqtt.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMENT "This command will cp so to target/lib"
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/*.* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/cfg ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin

  COMMAND mkdir -p ${RELEASE_DIR}/ccoRouter
  COMMAND rm -fr ${RELEASE_DIR}/ccoRouter/*
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/ccoRouter
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/ccoRouter
  
  #删除目标文件，下次编译会重新触发拷贝执行
  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
)



