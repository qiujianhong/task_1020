# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 编译宏，需要放到project前面
add_definitions(-DAPP_VERSION="1.0.0.1" -DAPP_PUBLISH_TIME="2024/6/27 20:00:00" -DADV_SHOW_NAME="调控")
				
# 项目名称
project(loadCtrller)

#指定头文件目录
include_directories(${CODE_DIE}/include 
                    ${CMAKE_CURRENT_SOURCE_DIR}/include 
					${CMAKE_CURRENT_SOURCE_DIR}/ctrl/include 
					${CODE_DIE}/lib/include 
					${CODE_DIE}/lib/include/appgui
					${CODE_DIE}/drivers/hal_include)

#链接库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE})

# 指定动态库
link_libraries(m dl pthread hd -Wl,-rpath=/lib/hal_lib 
				   cJson msgassist mapmsg iniparser framecomm Storage gdk deskGui guiprtl Layer -Wl,-rpath=${INSTALL_DIR}/loadCtrller/lib
				   mqttclient template mosquitto)

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./source DIR_SRCS)
aux_source_directory(./gui DIR_GUI)
aux_source_directory(./ctrl DIR_CTRL)

# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)
 
# 生成目标可执行文件
add_executable(loadCtrller main.c ${DIR_SRCS} ${DIR_GUI} ${DIR_CTRL})

# 拷贝so
add_custom_command(TARGET loadCtrller
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
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/cfg/ ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/${PRODUCT_NAME}/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
	  COMMENT "This command will cp so to target/lib"
	  
	  COMMAND mkdir -p ${RELEASE_DIR}/loadCtrller
	  COMMAND rm -fr ${RELEASE_DIR}/loadCtrller/*
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/loadCtrller
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/loadCtrller
)

