# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 编译宏，需要放到project前面
add_definitions(-DADV_GUI_NAME="M-pdAmr" -DADV_APP_NAME="pdAmr" -DADV_GUI_ICON="/usr/local/extapps/pdAmr/bin/cfg/pdAmr.bmp"
				-DAPP_VERSION=0x0000000C -DAPP_PUBLISH_TIME="2022/01/24 15:30:00"
				-DADV_SHOW_NAME="配网")
				
# 项目名称,放到交叉编译器设置之后，否则该项目找不到交叉编译器
project(pdAmr)

#指定头文件目录,适用于后面所有（含子目录）TARGET，
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include 
					${CMAKE_CURRENT_SOURCE_DIR}/area/0.include 
					${CMAKE_CURRENT_SOURCE_DIR}/app104/include 
					${CODE_DIE}/include 
					${CODE_DIE}/lib/include 
					${CODE_DIE}/lib/include/appgui
					${CODE_DIE}/drivers/hal_include)
					

# 编译成动态库的子目录
set(SHARED_DIRS event)

# 编译成静态库子目录
set(STATIC_DIRS app104 commucation gui msg)

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

# 公共库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE} ${CODE_DIE}/depends/authdata/${BOARD_TYPE})

# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

# 需要链接的动态库so
set(DEP_SHARED Event)

# 需要链接的静态库a
set(DEP_STATIC app104 commucation gui msg)

# 生成目标可执行文件
add_executable(${PROJECT_NAME} ${DIR_SRCS} ${SCRS_DEFAULT} ${SCRS_AREA})

# 链接子模块
target_link_libraries(${PROJECT_NAME} ${DEP_SHARED} ${DEP_STATIC} -Wl,-rpath=${INSTALL_DIR}/pdAmr/lib)

# 链接公共库
target_link_libraries(${PROJECT_NAME} 
	m dl pthread mqttclient template _auth_data
	msgassist Storage mapmsg framecomm iniparser mxml
	gdk deskGui guiprtl Layer -Wl,-rpath=${INSTALL_DIR}/pdAmr/lib
	hd -Wl,-rpath=/lib/hal_lib mosquitto)

# 拷贝so
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD 
  COMMAND cp ${CODE_DIE}/depends/miniGui/${BOARD_TYPE}/libgdk-1.1.so.1 ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib/
  COMMAND cp ${LIB_DIR}/libdeskGui.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libLayer.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libguiprtl.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp -fr ${CODE_DIE}/depends/miniGui/${BOARD_TYPE}/gdk ${CMAKE_CURRENT_SOURCE_DIR}/../config/cfg
  
  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libmsgassist.so ${PROJECT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libmapmsg.so ${PROJECT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libmxml.so ${PROJECT_SOURCE_DIR}/../target/lib  
  COMMAND cp ${LIB_DIR}/libframecomm.so ${PROJECT_SOURCE_DIR}/../target/lib 
  COMMAND cp ${LIB_DIR}/libiniparser.so ${PROJECT_SOURCE_DIR}/../target/lib 
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin 
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config_area/${AREA_DIR}/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
  COMMENT "This command will cp so to target"

  COMMAND mkdir -p ${RELEASE_DIR}/${PROJECT_NAME}
  COMMAND rm -fr ${RELEASE_DIR}/${PROJECT_NAME}/*
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/${PROJECT_NAME}
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/${PROJECT_NAME}
)