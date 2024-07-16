# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(REAV_CODE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../app_base_wirelessDCM/code)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 编译宏，需要放到project前面
add_definitions(-DAPP_VERSION="1.0.0.9" -DAPP_PUBLISH_TIME="2023/6/24 18:30:00" -DADV_SHOW_NAME="拨号")
	
# 项目名称
project(wirelessDCM)

# gui模块编译子目录
if("${PRODUCT_NAME}" STREQUAL "ZCU_1" )
	set(guistyle "${REAV_CODE_DIR}/appgui/240-128")
	set(GUIDEP ${BOARD_TYPE}"_240")
else()
	set(guistyle "${REAV_CODE_DIR}/appgui/160-160")
	set(GUIDEP ${BOARD_TYPE})
endif()


#指定头文件目录
include_directories(${CODE_DIE}/include 
                    ${CMAKE_CURRENT_SOURCE_DIR}/include 
					${CODE_DIE}/lib/include
					${CODE_DIE}/lib/include/appgui
					${CODE_DIE}/drivers/hal_include)
					
#链接库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE})

# 指定动态库
link_libraries(m dl pthread hd -Wl,-rpath=/lib/hal_lib 
               cJson msgassist Storage iniparser gdk deskGui guiprtl Layer framecomm -Wl,-rpath=${INSTALL_DIR}/wirelessDCM/lib mqttclient template mosquitto)

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./source DIR_SRCS)
aux_source_directory(${guistyle} SCRS_APPGUI)
# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

# 生成目标可执行文件
if("${PRODUCT_NAME}" STREQUAL "ZCU_N")
	add_executable(wirelessDCM main.c ${DIR_SRCS})
else()
	add_executable(wirelessDCM main.c ${DIR_SRCS} ${SCRS_APPGUI})
endif()

add_custom_command(TARGET wirelessDCM
  POST_BUILD 
  COMMAND cp ${CODE_DIE}/depends/miniGui/${GUIDEP}/libgdk-1.1.so.1 ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libdeskGui.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libLayer.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libguiprtl.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
  COMMENT "This command will cp ${INIPATH} to target/lib"
  COMMAND cp -fr ${CODE_DIE}/depends/miniGui/${BOARD_TYPE}/gdk ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/cfg
  
  COMMAND mkdir -p ${RELEASE_DIR}/wirelessDCM
  COMMAND rm -fr ${RELEASE_DIR}/wirelessDCM/*
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/wirelessDCM
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/wirelessDCM
  
  #删除目标文件，下次编译会重新触发拷贝执行
  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
)

