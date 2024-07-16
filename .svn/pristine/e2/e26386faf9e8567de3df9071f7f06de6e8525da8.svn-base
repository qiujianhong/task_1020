# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

# 加载公共设置
include(${CODE_DIE}/build/env.cmake)

# 编译宏，需要放到project前面
if("${PRODUCT_NAME}" STREQUAL "ECU")
add_definitions(-DADV_PU_AMR -DADV_APP_NAME="btManager" -DAPP_VERSION="0.0.5.1" -DAPP_PUBLISH_TIME="2022/6/26 17:00:00")
else()
add_definitions(-DADV_PU_AMR -DADV_APP_NAME="btManager" -DAPP_VERSION="1.0.0.1" -DAPP_PUBLISH_TIME="2022/6/26 17:00:00")
endif()
				
# 项目名称
project(btManager)

#指定头文件目录
include_directories(${CODE_DIE}/include 
                    ${CODE_DIE}/lib/include 
                    ${CMAKE_CURRENT_SOURCE_DIR}/include 
                    ${CODE_DIE}/lib/code/msgbus/include 
					          ${CODE_DIE}/lib/code/storage/include
					          ${CODE_DIE}/drivers/hal_include)
MESSAGE(STATUS "lib = ${LIB_DIE}.")
#链接库路径
link_directories(${LIB_DIR} ${HAL_DIR} ${CODE_DIE}/depends/miniGui/${BOARD_TYPE} ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE} 
${CODE_DIE}/depends/CTDecrypt/${BOARD_TYPE} ${CODE_DIE}/depends/authdata/${BOARD_TYPE})

# 指定动态库
link_libraries(m dl pthread cJson msgassist ble _auth_data Storage iniparser framecomm -Wl,-rpath=${INSTALL_DIR}/btManager/lib hd -Wl,-rpath=/lib/hal_lib mqttclient template mosquitto)

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./source DIR_SRCS)

# 设置目标生成路径(编译完成后, 拷贝并删除, 这样每次编译都会执行)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target)

# 生成目标可执行文件
add_executable(btManager main.c ${DIR_SRCS})

# 拷贝so
add_custom_command(TARGET btManager
  POST_BUILD 
  
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../target/btManager ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/btManager
  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libble.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
  COMMAND cp -fr ${CMAKE_CURRENT_SOURCE_DIR}/../config/* ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin
  COMMENT "This command will cp so to target/lib"
  
  COMMAND rm -rf ${RELEASE_DIR}/btManager
  COMMAND mkdir -p ${RELEASE_DIR}/btManager
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/btManager
  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/btManager
  
  #删除目标文件，下次编译会重新触发拷贝执行
  COMMAND rm -fr ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin/${PROJECT_NAME}
)



 
