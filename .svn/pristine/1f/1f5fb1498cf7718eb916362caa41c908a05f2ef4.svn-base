# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置路径
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

if("${PRODUCT_NAME}" STREQUAL "ECU")
add_definitions(-DAPP_VERSION="0.0.5.1" -DAPP_PUBLISH_TIME="2022/11/18 14:00:00")
else()
add_definitions(-DAPP_VERSION="1.0.0.1" -DAPP_PUBLISH_TIME="2024/1/19 14:00:00")
endif()

# 项目名称
project(taskManager)

#指定头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include 
                    ${CMAKE_CURRENT_SOURCE_DIR}/area/include 
		    ${CMAKE_CURRENT_SOURCE_DIR}/area/${AREA_DIR}/include 
                    ${CODE_DIE}/include 
                    ${CODE_DIE}/lib/code/msgbus/include 
                    ${CODE_DIE}/lib/code/storage/include
                    ${CODE_DIE}/lib/include 
                    ${CODE_DIE}/drivers/hal_include)

#链接库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE} 
                 ${CODE_DIE}/depends/authdata/${BOARD_TYPE})

#链接库
link_libraries(hd -Wl,-rpath=/lib/hal_lib m dl pthread mqttclient _auth_data template 
               Storage MsgMqtt MsgUdp cJson msgassist mosquitto iniparser ble framecomm algo mingather -Wl,-rpath=${INSTALL_DIR}/taskManager/lib)

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./cts_main DIR_SRCS)
aux_source_directory(./cts_common DIR_SRCS)
aux_source_directory(./cts_prtl/ptrl_698 DIR_SRCS)
aux_source_directory(./cts_prtl/prtl_1997 DIR_SRCS)
aux_source_directory(./cts_prtl/prtl_2007 DIR_SRCS)
aux_source_directory(./cts_prtl/prtl_cap DIR_SRCS)
aux_source_directory(./cts_prtl/prtl_protector DIR_SRCS)
aux_source_directory(./cts_prtl/prtl_dlms DIR_SRCS)
aux_source_directory(./cts_prtl/prtl_188 DIR_SRCS)
aux_source_directory(./area/${AREA_DIR} SCRS_AREA)
# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

# 生成bin
add_executable(taskManager ${DIR_SRCS} ${SCRS_AREA})


# 拷贝so
add_custom_command(TARGET taskManager
  POST_BUILD  
  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libMsgMqtt.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libMsgUdp.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib  
  COMMAND cp ${LIB_DIR}/libalgo.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libmingather.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libble.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMENT "This command will cp so to target/lib"
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../config/${AREA_DIR}/taskManager.ini ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin 
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../config/*.sh ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
  
  COMMAND mkdir -p ${RELEASE_DIR}/taskManager
  COMMAND rm -fr ${RELEASE_DIR}/taskManager/*
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/taskManager
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/taskManager
  
  #删除目标文件，下次编译会重新触发拷贝执行
  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
 )
