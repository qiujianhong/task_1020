# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(REAV_CODE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../app_adv_puAmr/code)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 编译宏，需要放到project前面
if("${PRODUCT_NAME}" STREQUAL "ECU")
add_definitions(-DADV_ST_AMR -DADV_APP_NAME="stAmr" -DAPP_SUBTOPIC="+/M-stAmr" -DAPP_DEFAULT_TOPIC="M-stAmr/M-smiOS" 
                -DADV_INI_NAME="stAmr.ini" -DADV_GUI_NAME="M-stAmr" -DADV_GUI_ICON="/usr/local/extapps/stAmr/bin/cfg/stAmr.bmp"
                -DAPP_VERSION="0.0.5.1" -DAPP_PUBLISH_TIME="2022/07/02 15:00:00" -DADV_SHOW_NAME="专变")
else()
add_definitions(-DADV_ST_AMR -DADV_APP_NAME="stAmr" -DAPP_SUBTOPIC="+/M-stAmr" -DAPP_DEFAULT_TOPIC="M-stAmr/M-smiOS" 
                -DADV_INI_NAME="stAmr.ini" -DADV_GUI_NAME="M-stAmr" -DADV_GUI_ICON="/usr/local/extapps/stAmr/bin/cfg/stAmr.bmp"
                -DAPP_VERSION="1.0.0.1" -DAPP_PUBLISH_TIME="2022/08/23 17:00:00" -DADV_SHOW_NAME="专变")
endif()

# 项目名称,放到交叉编译器设置之后，否则该项目找不到交叉编译器
project(stAmr)

# gui模块编译子目录
if("${PRODUCT_NAME}" STREQUAL "ZCU_1" )
	set(guistyle "${REAV_CODE_DIR}/gui/240-128")
	set(GUIDEP ${BOARD_TYPE}"_240")
else()
	set(guistyle "${REAV_CODE_DIR}/gui/160-160")
	set(GUIDEP ${BOARD_TYPE})
endif()

#指定头文件目录,适用于后面所有（含子目录）TARGET，
include_directories(${REAV_CODE_DIR}/include 
                    ${REAV_CODE_DIR}/appinclude
                    ${REAV_CODE_DIR}/CommOop
                    ${REAV_CODE_DIR}/communication/include
					${REAV_CODE_DIR}/area/0.include 
					${REAV_CODE_DIR}/oop/include
					${REAV_CODE_DIR}/msg/include
					${REAV_CODE_DIR}/AutoTask
					${REAV_CODE_DIR}/Stat
					${REAV_CODE_DIR}/event
                    ${CODE_DIE}/include 
					${CODE_DIE}/lib/include 
                    ${CODE_DIE}/lib/code/msgbus/include 
					${CODE_DIE}/lib/code/storage/include
                    ${CODE_DIE}/lib/code/msgassist/include
					${CODE_DIE}/lib/include/appgui
					${CODE_DIE}/drivers/hal_include)
					
# 添加子目录
add_subdirectory(${REAV_CODE_DIR}/CommOop CommOop.out)
add_subdirectory(${REAV_CODE_DIR}/event event.out)
add_subdirectory(${REAV_CODE_DIR}/oop oop.out)
add_subdirectory(${REAV_CODE_DIR}/AutoTask AutoTask.out)
add_subdirectory(${REAV_CODE_DIR}/pulse pulse.out)
add_subdirectory(${REAV_CODE_DIR}/communication communication.out)
add_subdirectory(${REAV_CODE_DIR}/app698main app698main.out)
add_subdirectory(${REAV_CODE_DIR}/Stat Stat.out)
add_subdirectory(${REAV_CODE_DIR}/ctrl ctrl.out)
#if("${PRODUCT_NAME}" STREQUAL "CCU" OR "${PRODUCT_NAME}" STREQUAL "ECU")
	add_subdirectory(${REAV_CODE_DIR}/gui gui.out)
#	add_subdirectory(${guistyle} guistyle.out)
#endif()
add_subdirectory(${REAV_CODE_DIR}/msg msg.out)
# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(${REAV_CODE_DIR}/ DIR_SRCS)

# 设置地区目录
aux_source_directory(${REAV_CODE_DIR}/area/${AREA_DIR} SCRS_AREA)

# 公共库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE} 
                 ${CODE_DIE}/depends/authdata/${BOARD_TYPE} ${CODE_DIE}/depends/sm4/${BOARD_TYPE})

# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

# 生成目标可执行文件
add_executable(${PROJECT_NAME} ${DIR_SRCS} ${SCRS_AREA})

# 需要链接的动态库so
set(DEP_SHARED CommOop Event PrtlOop AutoTask)

# 需要链接的静态库a
#if("${PRODUCT_NAME}" STREQUAL "CCU" OR "${PRODUCT_NAME}" STREQUAL "ECU")
	set(DEP_STATIC appmain comm msg pulse statis ctrl gui)
#else()
#	set(DEP_STATIC appmain comm msg pulse statis ctrl)
#endif()

# 链接子模块
target_link_libraries(${PROJECT_NAME} ${DEP_STATIC} ${DEP_SHARED} -Wl,-rpath=${LIB_PATH_ADVSTAMR})

# 链接公共库
#if("${PRODUCT_NAME}" STREQUAL "CCU" OR "${PRODUCT_NAME}" STREQUAL "ECU")
	target_link_libraries(${PROJECT_NAME} 
		m dl pthread cJson mqttclient ble security radio uart template _auth_data sm4
		msgassist mapmsg algo framecomm iniparser Storage gdk deskGui guiprtl Layer	sm3sum -Wl,-rpath=${INSTALL_DIR}/stAmr/lib
		hd -Wl,-rpath=/lib/hal_lib mosquitto)
#else()
#	target_link_libraries(${PROJECT_NAME} 
#		m dl pthread cJson mqttclient ble security radio uart template _auth_data sm4
#		msgassist mapmsg algo framecomm iniparser Storage sm3sum -Wl,-rpath=${INSTALL_DIR}/stAmr/lib
#		hd -Wl,-rpath=/lib/hal_lib mosquitto)

#endif()
# 判断是否存在
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/../config/${AREA_DIR}/cfg/fkgaconfig_stAmr.xml)
	SET(XML_AREA default)
else()
	SET(XML_AREA ${AREA_DIR})
endif()

# 判断是否存在
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${AREA_DIR}/stAmr.ini)
	SET(INI_AREA default)
else()
	SET(INI_AREA ${AREA_DIR})
endif()

# 拷贝so
if("${PRODUCT_NAME}" STREQUAL "CCU" OR "${PRODUCT_NAME}" STREQUAL "ECU")
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD 
		COMMAND cp ${REAV_CODE_DIR}/../target/lib/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
		COMMAND rm -fr ${REAV_CODE_DIR}/../target/lib/*
		COMMAND cp ${CODE_DIE}/depends/miniGui/${GUIDEP}/libgdk-1.1.so.1 ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib/
		COMMAND cp ${LIB_DIR}/libdeskGui.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
		COMMAND cp ${LIB_DIR}/libLayer.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
		COMMAND cp ${LIB_DIR}/libguiprtl.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
		COMMAND cp -fr ${CODE_DIE}/depends/miniGui/${GUIDEP}/gdk ${CMAKE_CURRENT_SOURCE_DIR}/../config/cfg

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
		COMMAND cp ${LIB_DIR}/libsm3sum.so ${PROJECT_SOURCE_DIR}/../target/lib  
		COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/cfg/ ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
		COMMAND cp ${PROJECT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${INI_AREA}/stAmr.ini ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
#		if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../config/${PRODUCT_NAME}/fkgaconfig_stAmr.xml)
			COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../config/${PRODUCT_NAME}/fkgaconfig_stAmr.xml ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/cfg
#		endif()
		COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${INI_AREA}/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
		COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/*.sh ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
		COMMENT "This command will cp(${XML_AREA}) so to target/bin"

		COMMAND mkdir -p ${RELEASE_DIR}/stAmr
		COMMAND rm -fr ${RELEASE_DIR}/stAmr/*
		COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/stAmr
		COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/stAmr
		
		#删除目标文件，下次编译会重新触发拷贝执行
		COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
	)
else()
	add_custom_command(TARGET ${PROJECT_NAME}
		POST_BUILD 
		COMMAND cp ${REAV_CODE_DIR}/../target/lib/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
		COMMAND rm -fr ${REAV_CODE_DIR}/../target/lib/*
		COMMAND cp ${CODE_DIE}/depends/miniGui/${GUIDEP}/libgdk-1.1.so.1 ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib/
		COMMAND cp ${LIB_DIR}/libdeskGui.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
		COMMAND cp ${LIB_DIR}/libLayer.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
		COMMAND cp ${LIB_DIR}/libguiprtl.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
#		COMMAND cp -fr ${CODE_DIE}/depends/miniGui/${GUIDEP}/gdk ${CMAKE_CURRENT_SOURCE_DIR}/../config/cfg

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
		COMMAND cp ${LIB_DIR}/libsm3sum.so ${PROJECT_SOURCE_DIR}/../target/lib  
		COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/cfg/ ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
		COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../config/${PRODUCT_NAME}/fkgaconfig_stAmr.xml ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/cfg
		COMMAND cp ${PROJECT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${INI_AREA}/stAmr.ini ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
		COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/*.sh ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
		COMMENT "This command will cp(${XML_AREA}) so to target/bin"

		COMMAND mkdir -p ${RELEASE_DIR}/stAmr
		COMMAND rm -fr ${RELEASE_DIR}/stAmr/*
		COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/stAmr
		COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/stAmr
		
		#删除目标文件，下次编译会重新触发拷贝执行
		COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
	)
endif()

