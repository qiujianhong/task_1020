# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 编译宏，需要放到project前面
if("${PRODUCT_NAME}" STREQUAL "ECU")
add_definitions(-DLC_APP_MANE="lcMonitor" -DADV_SHOW_NAME="回路巡检" -DAPP_VERSION="0.0.5.1" -DAPP_PUBLISH_TIME="2022/06/01 18:00:00")
else()
add_definitions(-DLC_APP_MANE="lcMonitor" -DADV_SHOW_NAME="回路巡检" -DAPP_VERSION="1.0.0.1" -DAPP_PUBLISH_TIME="2022/06/01 18:00:00")
endif()
	
# 项目名称
project(lcMonitor)

#指定头文件目录
include_directories(${CODE_DIE}/include 
                    ${CMAKE_CURRENT_SOURCE_DIR}/include 
					${CODE_DIE}/lib/include 
					${CODE_DIE}/lib/include/appgui
					${CODE_DIE}/drivers/hal_include)
MESSAGE(STATUS "lib = ${LIB_DIE}.")

#链接库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE} ${CODE_DIE}/depends/CTDecrypt/${BOARD_TYPE})

# 指定动态库
if("${PRODUCT_NAME}" STREQUAL "ZCU_N")
	link_libraries(m dl pthread hd -Wl,-rpath=/lib/hal_lib 
				   cJson msgassist mapmsg iniparser framecomm Storage -Wl,-rpath=${INSTALL_DIR}/lcMonitor/lib
				   mqttclient template mosquitto CTDecrypt uart)
else()
	link_libraries(m dl pthread hd -Wl,-rpath=/lib/hal_lib 
				   cJson msgassist mapmsg iniparser framecomm Storage gdk deskGui guiprtl Layer -Wl,-rpath=${INSTALL_DIR}/lcMonitor/lib
				   mqttclient template mosquitto CTDecrypt)
endif()

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./source DIR_SRCS)
aux_source_directory(./gui DIR_GUI)

# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)
 
# 生成目标可执行文件
if("${PRODUCT_NAME}" STREQUAL "ECU")
	add_executable(lcMonitor main.c ${DIR_SRCS} ${DIR_GUI})
else()
	add_executable(lcMonitor main.c ${DIR_SRCS})
endif()

# 拷贝so
if("${PRODUCT_NAME}" STREQUAL "ECU")
add_custom_command(TARGET lcMonitor
	  POST_BUILD 
	  COMMAND cp ${CODE_DIE}/depends/miniGui/${BOARD_TYPE}/libgdk-1.1.so.1 ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib/
	  COMMAND cp ${LIB_DIR}/libdeskGui.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libLayer.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libguiprtl.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp -fr ${CODE_DIE}/depends/miniGui/${BOARD_TYPE}/gdk ${CMAKE_CURRENT_SOURCE_DIR}/../config/cfg
	  
	  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
	  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libmapmsg.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib

	  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${CODE_DIE}/depends/CTDecrypt/${BOARD_TYPE}/libCTDecrypt.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/cfg/ ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/${PRODUCT_NAME}/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
	  COMMENT "This command will cp so to target/lib"
	  
	  COMMAND mkdir -p ${RELEASE_DIR}/lcMonitor
	  COMMAND rm -fr ${RELEASE_DIR}/lcMonitor/*
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/lcMonitor
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/lcMonitor
)
else()
	add_custom_command(TARGET lcMonitor
	  POST_BUILD 
	  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
	  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libmapmsg.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib

	  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${CODE_DIE}/depends/CTDecrypt/${BOARD_TYPE}/libCTDecrypt.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/${PRODUCT_NAME}/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
	  COMMENT "This command will cp so to target/lib"
	  
	  COMMAND mkdir -p ${RELEASE_DIR}/lcMonitor
	  COMMAND rm -fr ${RELEASE_DIR}/lcMonitor/*
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/lcMonitor
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/lcMonitor
	)
endif()