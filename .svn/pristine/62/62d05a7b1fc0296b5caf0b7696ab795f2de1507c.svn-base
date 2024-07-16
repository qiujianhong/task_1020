# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 编译宏，需要放到project前面
add_definitions(-DAPP_VERSION=0x00000009 -DAPP_PUBLISH_TIME="2021/07/07 18:00:00" -DADV_SHOW_NAME="USB管理")
				
# 项目名称
project(usbManager)

#指定头文件目录
include_directories(${CODE_DIE}/include 
					${CODE_DIE}/lib/include 
					${CODE_DIE}/drivers/hal_include
					${CMAKE_CURRENT_SOURCE_DIR}/include)

# 公共库路径
link_directories(${LIB_DIR})

# 指定动态库
link_libraries(m dl pthread iniparser framecomm -Wl,-rpath=${INSTALL_DIR}/${PROJECT_NAME}/lib template cJson mapmsg)

# 判断是否存在
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${AREA_DIR}/usbManager.ini)
	SET(INI_AREA "")
else()
	SET(INI_AREA ${AREA_DIR})
endif()
message("INI_AREA:" ${INI_AREA})

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./source DIR_SRCS)

# 设置目标生成路径(编译完成后, 拷贝并删除, 这样每次编译都会执行)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)
 
# 生成目标可执行文件
add_executable(${PROJECT_NAME} main.c ${DIR_SRCS})

# 拷贝so
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD  
  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/${PROJECT_NAME}
  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libmapmsg.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/*.sh ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../config/${PRODUCT_NAME}/${INI_AREA}/*.* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin 

  COMMENT "This command will cp so to target/lib"
  
  COMMAND mkdir -p ${RELEASE_DIR}/${PROJECT_NAME}
  COMMAND rm -fr ${RELEASE_DIR}/${PROJECT_NAME}/*
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/${PROJECT_NAME}
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/${PROJECT_NAME}
)

 
