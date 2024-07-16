# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

add_definitions(-DARM_LINUX -DAPP_VERSION="0.0.5.1" -DAPP_PUBLISH_TIME="2022/07/02 15:00:00"
                -DAPP_SUBTOPIC="+/M-mapManager" -DAPP_DEFAULT_TOPIC="M-mapManager/M-smiOS"
                -DADV_SHOW_NAME="模组")

# 项目名称
project(mapManager)

#指定头文件目录
include_directories(${CODE_DIE}/include 
				    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CODE_DIE}/lib/code/msgbus/include
					${CODE_DIE}/lib/code/storage/include
                    ${CODE_DIE}/lib/include 
					${CODE_DIE}/lib/include/appgui 
					${CODE_DIE}/drivers/hal_include) 
					
#链接库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE})

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./ DIR_SRCS)
aux_source_directory(./include DIR_SRCS)
aux_source_directory(./source DIR_SRCS)
aux_source_directory(./appgui DIR_SRCS)
# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

set(LINK_LIB m dl pthread mqttclient template
             msgassist framecomm cJson iniparser MsgMqtt MsgUdp Storage gdk deskGui guiprtl Layer -Wl,-rpath=${INSTALL_DIR}/mapManager/lib
             hd -Wl,-rpath=/lib/hal_lib mosquitto)

#add_subdirectory(${guistyle})
#ADD_COMPILE_OPTIONS()
# 生成目标可执行文件
add_executable(mapManager ${DIR_SRCS})

# 链接公共库
target_link_libraries(mapManager ${LINK_LIB})

# 拷贝so
add_custom_command(TARGET mapManager
  POST_BUILD 
  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libMsgUdp.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libMsgMqtt.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMENT "This command will cp so to target/lib"
  
  #COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/../target/bin/cfg
  COMMAND cp ${CODE_DIE}/depends/miniGui/${BOARD_TYPE}/libgdk-1.1.so.1 ${PROJECT_SOURCE_DIR}/../target/lib/  
  COMMAND cp ${LIB_DIR}/libguiprtl.so ${PROJECT_SOURCE_DIR}/../target/lib/
  COMMAND cp ${LIB_DIR}/libdeskGui.so ${PROJECT_SOURCE_DIR}/../target/lib/
  COMMAND cp ${LIB_DIR}/libLayer.so ${PROJECT_SOURCE_DIR}/../target/lib/
  
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/cfg ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/*.ini ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/*.sh ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/
  COMMAND cp -fr ${CODE_DIE}/depends/miniGui/${BOARD_TYPE}/gdk ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/cfg
  
  COMMAND mkdir -p ${RELEASE_DIR}/mapManager
  COMMAND rm -fr ${RELEASE_DIR}/mapManager/*
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/mapManager
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/mapManager
  
  #删除目标文件，下次编译会重新触发拷贝执行
  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
)
