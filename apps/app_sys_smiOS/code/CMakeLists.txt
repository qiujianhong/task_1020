#指定所依赖的cmake最低版本
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(REAV_CODE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../app_sys_smiOS/code)
# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

if("${PRODUCT_NAME}" STREQUAL "ECU")
add_definitions(-DAPP_VERSION="0.0.5.1" -DAPP_PUBLISH_TIME="2022/07/04 11:00:00" -DADV_SHOW_NAME="系统")
else()
add_definitions(-DAPP_VERSION="1.0.0.1" -DAPP_PUBLISH_TIME="2022/07/04 11:00:00" -DADV_SHOW_NAME="系统")
endif()

# 项目名称
project(smiOS)

# gui模块编译子目录
if("${PRODUCT_NAME}" STREQUAL "ZCU_1" )
	set(guistyle "${REAV_CODE_DIR}/appgui/240-128")
	set(GUIDEP ${BOARD_TYPE}"_240")
else()
	set(guistyle "${REAV_CODE_DIR}/appgui/160-160")
	set(GUIDEP ${BOARD_TYPE})
endif()

set(smiOS_VERSION_MAJOR 0)
set(smiOS_VERSION_MINOR 0)
set(smiOS_VERSION_PATCH 1)
set(smiOS_DEPEND ${CMAKE_CURRENT_SOURCE_DIR}/depend/)

set(DISABLE_HAL 0)

MESSAGE(STATUS"PRODUCT_NAME: ${PRODUCT_NAME}")

if("${PRODUCT_NAME}" STREQUAL "ZCU_N")
	link_libraries(dl m pthread event threadpool sqlite3 qlibc qlibcext crypto ssl cJson framecomm iniparser msgassist template Storage -Wl,-rpath=${INSTALL_DIR}/smiOS/lib
			       hd -Wl,-rpath=/lib/hal_lib/ mosquitto)
else()
	link_libraries(dl m pthread event threadpool sqlite3 qlibc qlibcext crypto ssl cJson gdk deskGui guiprtl Layer framecomm iniparser msgassist template Storage -Wl,-rpath=${INSTALL_DIR}/smiOS/lib
				   hd -Wl,-rpath=/lib/hal_lib/ mosquitto)
endif()

set(smiOS_VERSION_SVN "0")
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/include/smiOSConfig.h.in"
  "${PROJECT_BINARY_DIR}/smiOSConfig.h"
)

# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

#指定头文件目录,适用于后面所有（含子目录）TARGET，
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include 
                    ${CODE_DIE}/include 
					${CODE_DIE}/lib/include 
                    ${CODE_DIE}/lib/code/msgbus/include 
					${CODE_DIE}/lib/code/storage/include
                    ${CODE_DIE}/lib/code/msgassist/include
					${CODE_DIE}/lib/code/mqttclient/include
					${CODE_DIE}/lib/include/appgui
					${CODE_DIE}/drivers/hal_include
				    ${smiOS_DEPEND}/include/qlibc
				    ${smiOS_DEPEND}/include/)
					
# 公共库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE} ${smiOS_DEPEND}/lib)


include_directories(
    ./include 
    ./server/include 
    ./utility/include
    ./docker/include
    ./app/include
    ./system/include
    ./platform/include
    ./fcgi/include
    ./common/include
    ./common/server/include
    ./common/utility/include
    ./common/client/include
    ${PROJECT_BINARY_DIR})

add_subdirectory(common/server common.server.dir)
add_subdirectory(common/utility common.utility.dir)       
add_subdirectory(common/client common.client.dir)   
add_subdirectory(server)
add_subdirectory(utility)
add_subdirectory(docker)
add_subdirectory(app)
add_subdirectory(system)
add_subdirectory(platform)
#add_subdirectory(fcgi)

add_subdirectory(${guistyle})


aux_source_directory(. DIR_MAIN_SRCS)

if("${PRODUCT_NAME}" STREQUAL "ZCU_N")
	add_executable(
		${PROJECT_NAME}  
		${DIR_MAIN_SRCS}
		$<TARGET_OBJECTS:common.server>
		$<TARGET_OBJECTS:common.utility>
		$<TARGET_OBJECTS:common.client>
		$<TARGET_OBJECTS:server>
		$<TARGET_OBJECTS:utility>
		$<TARGET_OBJECTS:docker>
		$<TARGET_OBJECTS:app>
		$<TARGET_OBJECTS:system>
		$<TARGET_OBJECTS:platform>
	#    $<TARGET_OBJECTS:xlfcgi>
	#	$<TARGET_OBJECTS:appgui>
    )
else()
	add_executable(
		${PROJECT_NAME}  
		${DIR_MAIN_SRCS}
		$<TARGET_OBJECTS:common.server>
		$<TARGET_OBJECTS:common.utility>
		$<TARGET_OBJECTS:common.client>
		$<TARGET_OBJECTS:server>
		$<TARGET_OBJECTS:utility>
		$<TARGET_OBJECTS:docker>
		$<TARGET_OBJECTS:app>
		$<TARGET_OBJECTS:system>
		$<TARGET_OBJECTS:platform>
	#    $<TARGET_OBJECTS:xlfcgi>
		$<TARGET_OBJECTS:appgui>
    )
endif()


# 拷贝so
if("${PRODUCT_NAME}" STREQUAL "ZCU_N")
	add_custom_command(TARGET ${PROJECT_NAME}
	  POST_BUILD 
	  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib/libdeskGui.so
	  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib/libLayer.so
	  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib/libguiprtl.so
	  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib/libgdk-1.1.so.1
	  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/cfg
	  
	  COMMAND cp ${smiOS_DEPEND}/lib/*.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 	 
	  COMMAND cp ${LIB_DIR}/libmsgassist.so ${PROJECT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libStorage.so ${PROJECT_SOURCE_DIR}/../target/lib 	 
	  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/*  ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin	 
	  COMMENT "This command will cp so to target/lib"

	  COMMAND mkdir -p ${RELEASE_DIR}/smiOS
	  COMMAND rm -fr ${RELEASE_DIR}/smiOS/*
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/smiOS
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/smiOS
	  
	  #删除目标文件，下次编译会重新触发拷贝执行
	  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
	)
else()
	add_custom_command(TARGET ${PROJECT_NAME}
	  POST_BUILD 
	  COMMAND cp ${CODE_DIE}/depends/miniGui/${GUIDEP}/libgdk-1.1.so.1 ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib/
	  COMMAND cp ${LIB_DIR}/libdeskGui.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libLayer.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libguiprtl.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp -fr ${CODE_DIE}/depends/miniGui/${BOARD_TYPE}/gdk ${CMAKE_CURRENT_SOURCE_DIR}/../config/cfg/

	  COMMAND cp ${smiOS_DEPEND}/lib/*.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib 	 
	  COMMAND cp ${LIB_DIR}/libmsgassist.so ${PROJECT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libStorage.so ${PROJECT_SOURCE_DIR}/../target/lib 	 
	  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/*  ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin	 
	  
	  COMMENT "This command will cp so to target/lib"

	  COMMAND mkdir -p ${RELEASE_DIR}/smiOS
	  COMMAND rm -fr ${RELEASE_DIR}/smiOS/*
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/smiOS
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/smiOS
	  
	  #删除目标文件，下次编译会重新触发拷贝执行
	  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
	)
endif()