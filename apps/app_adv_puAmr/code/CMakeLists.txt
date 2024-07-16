# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 编译宏，需要放到project前面
if("${PRODUCT_NAME}" STREQUAL "ECU")
add_definitions(-DADV_PU_AMR -DADV_APP_NAME="puAmr" -DAPP_SUBTOPIC="+/M-puAmr" -DAPP_DEFAULT_TOPIC="M-puAmr/M-smiOS" 
                -DADV_INI_NAME="puAmr.ini" -DADV_GUI_NAME="M-puAmr" -DADV_GUI_ICON="/usr/local/extapps/puAmr/bin/cfg/puAmr.bmp" 
				-DAPP_VERSION="0.0.5.1" -DAPP_PUBLISH_TIME="2022/08/31 23:00:00" -DADV_SHOW_NAME="集抄")
else()
add_definitions(-DADV_PU_AMR -DADV_APP_NAME="puAmr" -DAPP_SUBTOPIC="+/M-puAmr" -DAPP_DEFAULT_TOPIC="M-puAmr/M-smiOS" 
                -DADV_INI_NAME="puAmr.ini" -DADV_GUI_NAME="M-puAmr" -DADV_GUI_ICON="/usr/local/extapps/puAmr/bin/cfg/puAmr.bmp" 
				-DAPP_VERSION="1.0.0.1" -DAPP_PUBLISH_TIME="2023/08/31 20:00:00" -DADV_SHOW_NAME="集抄")
endif()
# 项目名称,放到交叉编译器设置之后，否则该项目找不到交叉编译器
project(puAmr)

#指定头文件目录,适用于后面所有（含子目录）TARGET，
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include 
                    ${CMAKE_CURRENT_SOURCE_DIR}/appinclude
                    ${CMAKE_CURRENT_SOURCE_DIR}/CommOop
                    ${CMAKE_CURRENT_SOURCE_DIR}/communication/include
					${CMAKE_CURRENT_SOURCE_DIR}/area/0.include
					${CMAKE_CURRENT_SOURCE_DIR}/oop/include
					${CMAKE_CURRENT_SOURCE_DIR}/msg/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/AutoTask	
					${CMAKE_CURRENT_SOURCE_DIR}/Stat
					${CMAKE_CURRENT_SOURCE_DIR}/event
                    ${CODE_DIE}/include 
                    ${CODE_DIE}/lib/include
					${CODE_DIE}/lib/include/appgui
					${CODE_DIE}/lib/code/msgbus/include 
					${CODE_DIE}/lib/code/storage/include
                    ${CODE_DIE}/lib/code/msgassist/include
					${CODE_DIE}/drivers/hal_include)
					
# 编译成动态库的子目录
set(SHARED_DIRS CommOop event oop AutoTask)

# 编译成静态库子目录
set(STATIC_DIRS pulse communication app698main Stat ctrl gui msg)

# 需要编译的子模块目录
set(SUB_DIRS ${SHARED_DIRS} ${STATIC_DIRS})	
				
# 添加子目录
foreach(subdir ${SUB_DIRS})
	add_subdirectory(${subdir})
endforeach(subdir)

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./ DIR_SRCS)

# 设置地区目录
aux_source_directory(./area/${AREA_DIR} SCRS_AREA)
aux_source_directory(./area/default SCRS_DEFAULT)
# 公共库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE} 
                 ${CODE_DIE}/depends/authdata/${BOARD_TYPE} ${CODE_DIE}/depends/sm4/${BOARD_TYPE})

# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

# 生成目标可执行文件
add_executable(${PROJECT_NAME} ${DIR_SRCS} ${SCRS_DEFAULT} ${SCRS_AREA})

# 需要链接的动态库so
set(DEP_SHARED CommOop Event PrtlOop AutoTask)

# 需要链接的静态库a
set(DEP_STATIC appmain comm msg pulse statis ctrl gui)

# 链接子模块
target_link_libraries(${PROJECT_NAME} ${DEP_STATIC} ${DEP_SHARED} -Wl,-rpath=${INSTALL_DIR}/puAmr/lib)

# 链接公共库
target_link_libraries(${PROJECT_NAME} 
	m dl pthread cJson mqttclient ble security radio uart template _auth_data sm4
	msgassist mapmsg algo framecomm iniparser Storage gdk deskGui guiprtl Layer	uart sm3sum -Wl,-rpath=${INSTALL_DIR}/puAmr/lib hd -Wl,-rpath=/lib/hal_lib mosquitto)

# 判断是否存在
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${AREA_DIR}/puAmr.ini)
	SET(INI_AREA default)
else()
	SET(INI_AREA ${AREA_DIR})
endif()

# 判断是否存在
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${AREA_DIR}/fkgaconfig_puAmr.xml)
	SET(XML_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../config/cfg/fkgaconfig_puAmr.xml)
else()
	SET(XML_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${AREA_DIR}/fkgaconfig_puAmr.xml)
endif()

# 拷贝so
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD 
  COMMAND cp ${CODE_DIE}/depends/miniGui/${BOARD_TYPE}/libgdk-1.1.so.1 ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib/
  COMMAND cp ${LIB_DIR}/libdeskGui.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libLayer.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libguiprtl.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp -fr ${CODE_DIE}/depends/miniGui/${BOARD_TYPE}/gdk ${CMAKE_CURRENT_SOURCE_DIR}/../config/cfg
  
  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libmsgassist.so ${PROJECT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libStorage.so ${PROJECT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libalgo.so ${PROJECT_SOURCE_DIR}/../target/lib  
  COMMAND cp ${LIB_DIR}/libframecomm.so ${PROJECT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libiniparser.so ${PROJECT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libmapmsg.so ${PROJECT_SOURCE_DIR}/../target/lib  
  COMMAND cp ${LIB_DIR}/libuart.so ${PROJECT_SOURCE_DIR}/../target/lib  
  COMMAND cp ${LIB_DIR}/libble.so ${PROJECT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libsecurity.so ${PROJECT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libradio.so ${PROJECT_SOURCE_DIR}/../target/lib  
  COMMAND cp ${LIB_DIR}/libsm3sum.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/cfg ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
  COMMAND cp ${XML_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/cfg 
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${INI_AREA}/puAmr.ini ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin 
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../config/*.sh ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin 
  COMMENT "This command will cp so to target/lib"

  COMMAND mkdir -p ${RELEASE_DIR}/puAmr
  COMMAND rm -fr ${RELEASE_DIR}/puAmr/*
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/puAmr
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/puAmr
  
  #删除目标文件，下次编译会重新触发拷贝执行
  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
)
