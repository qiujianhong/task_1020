#指定所依赖的cmake最低版本
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 编译宏，需要放到project前面
if("${PRODUCT_NAME}" STREQUAL "ECU")
	add_definitions( -DAPP_VERSION="0.0.5.3" -DAPP_PUBLISH_TIME="2022/08/17 10:00:00" -DADV_SHOW_NAME="数据中心" )
else()
	add_definitions( -DAPP_VERSION="1.0.0.6" -DAPP_PUBLISH_TIME="2022/08/17 11:00:00" -DADV_SHOW_NAME="数据中心" )
endif()

# 项目名称
project(dbCenter)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/include/dbCenterConfig.h.in"
  "${PROJECT_BINARY_DIR}/dbCenterConfig.h"
)

#指定头文件目录,适用于后面所有（含子目录）TARGET，
include_directories(
	./include 
	./db/include 
	./server/include 
	./utility/include
	./common/include
	./common/include/host_include
	./common/server/include
	./common/utility/include
	./common/client/include
	./common/include/host_include
	./depend/include
	./depend/include/qlibc
	${PROJECT_BINARY_DIR}
	${CODE_DIE}/drivers/hal_include
)

# 需要编译的子模块目录
add_subdirectory(common/server common.server.dir)
add_subdirectory(common/utility common.utility.dir)      
add_subdirectory(common/client common.client.dir)      
add_subdirectory(client)
add_subdirectory(db)
add_subdirectory(server)
add_subdirectory(utility)

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_MAIN_SRCS 变量
aux_source_directory(. DIR_MAIN_SRCS)

# 公共库路径
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/depend/lib ${HAL_DIR})

# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

# 生成目标可执行文件
add_executable(
    dbCenter 
    ${DIR_MAIN_SRCS} 
    $<TARGET_OBJECTS:common.server> 
    $<TARGET_OBJECTS:common.utility> 
    $<TARGET_OBJECTS:common.client> 
    $<TARGET_OBJECTS:db> 
    $<TARGET_OBJECTS:server> 
    $<TARGET_OBJECTS:utility>)

# 需要链接的动态库so
set(DEP_SHARED pthread crypto mosquitto ssl dl m)

# 需要链接的静态库a
set(DEP_STATIC zlog event threadpool sqlite3 qlibcext-static qlibc-static)

# 链接子模块
target_link_libraries(${PROJECT_NAME} ${DEP_STATIC} ${DEP_SHARED} -Wl,-rpath=${INSTALL_DIR}/dbCenter/lib hd -Wl,-rpath=/lib/hal_lib)

# 拷贝so
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD 
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/depend/lib/libcrypto.so.1.1 ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/depend/lib/libmosquitto.so.1 ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/depend/lib/libssl.so.1.1 ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
  COMMENT "This command will cp so to target/lib"

  COMMAND mkdir -p ${RELEASE_DIR}/dbCenter
  COMMAND rm -fr ${RELEASE_DIR}/dbCenter/*
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/dbCenter
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/dbCenter
  
  #删除目标文件，下次编译会重新触发拷贝执行
  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
)

