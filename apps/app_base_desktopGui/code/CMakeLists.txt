# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# gui模块编译子目录
if("${PRODUCT_NAME}" STREQUAL "ZCU_1" )
	set(guistyle "240-128")
	set(GUIDEP ${BOARD_TYPE}"_240")
else()
	set(guistyle "160-160")
	set(GUIDEP ${BOARD_TYPE})
endif()

add_definitions(-DARM_LINUX)

# 编译宏，需要放到project前面
if("${PRODUCT_NAME}" STREQUAL "ECU")
add_definitions(-DADV_APP_NAME="desktopGui" -DAPP_SUBTOPIC="+/M-desktopGui" -DAPP_DEFAULT_TOPIC="M-desktopGui/M-smiOS" -DADV_INI_NAME="desktopGui.ini"
				-DAPP_VERSION="0.0.5.1" -DAPP_PUBLISH_TIME="2022/06/26 17:00:00" -DADV_SHOW_NAME="桌面显示")
else()
add_definitions(-DADV_APP_NAME="desktopGui" -DAPP_SUBTOPIC="+/M-desktopGui" -DAPP_DEFAULT_TOPIC="M-desktopGui/M-smiOS" -DADV_INI_NAME="desktopGui.ini"
				-DAPP_VERSION="1.0.0.1" -DAPP_PUBLISH_TIME="2022/06/26 17:00:00" -DADV_SHOW_NAME="桌面显示")
endif()


# 项目名称
project(desktopGui)

#指定头文件目录
include_directories(${CODE_DIE}/include 
					${CMAKE_CURRENT_SOURCE_DIR}/include 
					${CMAKE_CURRENT_SOURCE_DIR}/xml 
					${CMAKE_CURRENT_SOURCE_DIR}/msg/include
					${CMAKE_CURRENT_SOURCE_DIR}/comm/include
                    ${CODE_DIE}/lib/code/msgbus/include 
                    ${CODE_DIE}/lib/code/storage/include
                    ${CODE_DIE}/lib/include 
					${CODE_DIE}/lib/include/appgui
					${CODE_DIE}/lib/include/appgui/minigui/ctrl
                    ${CODE_DIE}/drivers/hal_include)

#链接库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE})

# 指定动态库
link_libraries(m dl pthread cJson mqttclient template
               msgassist Storage framecomm iniparser guiprtl gdk -Wl,-rpath=${INSTALL_DIR}/desktopGui/lib
               hd -Wl,-rpath=/lib/hal_lib mosquitto)

#libipc.a $(INSTALL_LIB_PATH)/libdmsg.a

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./ DIR_SRCS)
aux_source_directory(./commfun DIR_SRCS)
aux_source_directory(./xml DIR_SRCS)
aux_source_directory(./hidden DIR_SRCS)
aux_source_directory(./msg/source DIR_SRCS)
#aux_source_directory(./comm/source DIR_SRCS)
aux_source_directory(./include DIR_SRCS)
#aux_source_directory(./160-160/${guistyle} DIR_SRCS)
#aux_source_directory(./msg/source DIR_SRCS)
#aux_source_directory(./${guistyle} DIR_SRCS)

# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

add_subdirectory(${guistyle})

# 生成目标可执行文件
add_executable(desktopGui ${DIR_SRCS})

#往目标中添加文件
#target_sources(gui PRIVATE ./msg/source/msg_proc.c)
#target_sources(gui PRIVATE ./msg/source/task_proc.c)
#target_sources(gui PRIVATE ./msg/source/out_msg.c)
#target_sources(gui PRIVATE ./include/mettype.c)

target_link_libraries(desktopGui guiLayer)
if("${PRODUCT_NAME}" STREQUAL "ZCU_1" )
	# 拷贝so
	add_custom_command(TARGET desktopGui
	  POST_BUILD 
	  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${CODE_DIE}/depends/miniGui/${GUIDEP}/libgdk-1.1.so.1 ${PROJECT_SOURCE_DIR}/../target/lib/  
	  COMMAND cp ${LIB_DIR}/libguiprtl.so ${PROJECT_SOURCE_DIR}/../target/lib/

	  COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/../target/bin/cfg
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/gdk ${PROJECT_SOURCE_DIR}/../target/bin/cfg/
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/icons ${PROJECT_SOURCE_DIR}/../target/bin/cfg/
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/fkgaconfig.xml ${PROJECT_SOURCE_DIR}/../target/bin/cfg/
	  COMMAND cp ${PROJECT_SOURCE_DIR}/../config/${PRODUCT_NAME}/fkgaconfig.xml ${PROJECT_SOURCE_DIR}/../target/bin/cfg
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/GDK.cfg ${PROJECT_SOURCE_DIR}/../target/bin/cfg/
	  #COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/MiniGUI.cfg ${PROJECT_SOURCE_DIR}/../target/bin/cfg/
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/gui.ini ${PROJECT_SOURCE_DIR}/../target/bin/
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/*.sh ${PROJECT_SOURCE_DIR}/../target/bin/
	  COMMENT "This command will cp so to target/lib"

	  COMMAND mkdir -p ${RELEASE_DIR}/desktopGui
	  COMMAND rm -fr ${RELEASE_DIR}/desktopGui/*
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/desktopGui
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/desktopGui
  
	  #删除目标文件，下次编译会重新触发拷贝执行
	  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
	)
else()
	# 拷贝so
	add_custom_command(TARGET desktopGui
	  POST_BUILD 
	  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${CODE_DIE}/depends/miniGui/${GUIDEP}/libgdk-1.1.so.1 ${PROJECT_SOURCE_DIR}/../target/lib/  
	  COMMAND cp ${LIB_DIR}/libguiprtl.so ${PROJECT_SOURCE_DIR}/../target/lib/

	  COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/../target/bin/cfg
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/gdk ${PROJECT_SOURCE_DIR}/../target/bin/cfg/
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/icons ${PROJECT_SOURCE_DIR}/../target/bin/cfg/
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/fkgaconfig.xml ${PROJECT_SOURCE_DIR}/../target/bin/cfg/
#	  COMMAND cp ${PROJECT_SOURCE_DIR}/../config/${PRODUCT_NAME}/fkgaconfig.xml ${PROJECT_SOURCE_DIR}/../target/bin/cfg
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/GDK.cfg ${PROJECT_SOURCE_DIR}/../target/bin/cfg/
	  #COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/MiniGUI.cfg ${PROJECT_SOURCE_DIR}/../target/bin/cfg/
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/gui.ini ${PROJECT_SOURCE_DIR}/../target/bin/
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/*.sh ${PROJECT_SOURCE_DIR}/../target/bin/
	  COMMENT "This command will cp so to target/lib"

	  COMMAND mkdir -p ${RELEASE_DIR}/desktopGui
	  COMMAND rm -fr ${RELEASE_DIR}/desktopGui/*
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/desktopGui
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/desktopGui
  
	  #删除目标文件，下次编译会重新触发拷贝执行
	  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
	)
endif()



 
