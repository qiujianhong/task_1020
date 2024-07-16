# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# 加载公共设置
include(${CMAKE_CURRENT_SOURCE_DIR}/env.cmake)

# 项目名称
project(dbfunctest)

#指定头文件目录
include_directories(
		${CODE_DIE}/../../include 
		${CODE_DIE}/../../lib/include 
		${CMAKE_CURRENT_SOURCE_DIR}/include 
		${CMAKE_CURRENT_SOURCE_DIR}/include/mqtt 
		${CMAKE_CURRENT_SOURCE_DIR}/include/mqtt/utility/include 
		${CMAKE_CURRENT_SOURCE_DIR}/include/mqtt/client/include)

MESSAGE(STATUS "lib = ${CODE_DIE}/lib.")

#链接库路径
link_directories(${CODE_DIE}/lib ${CODE_DIE}/lib/mqtt)

# 指定动态库
link_libraries(pthread msgassist framecomm iniparser cJson xlclient xlutility mosquitto ssl crypto qlibc-static -Wl,-rpath=/usr/local/extapps/test/lib)

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_SRCS 变量
aux_source_directory(./source DIR_SRCS)

# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CODE_DIE}/target/bin)

# 生成目标可执行文件
add_executable(dbfunctest main.c ${DIR_SRCS})

#add_executable(mqtttest test.c ${DIR_SRCS})

# 拷贝so
add_custom_command(TARGET dbfunctest
  POST_BUILD 
  COMMAND cp ${CODE_DIE}/lib/*.so* ${CODE_DIE}/target/lib
  COMMAND cp ${CODE_DIE}/lib/mqtt/libzlog.so.1.2 ${CODE_DIE}/target/lib
  COMMAND cp ${CODE_DIE}/lib/mqtt/libssl.so.1.1 ${CODE_DIE}/target/lib
  COMMAND cp ${CODE_DIE}/lib/mqtt/libcrypto.so.1.1 ${CODE_DIE}/target/lib
  COMMAND cp ${CODE_DIE}/lib/mqtt/libmosquitto.so.1 ${CODE_DIE}/target/lib
  COMMAND cp ${CODE_DIE}/config/*  ${CODE_DIE}/target/bin
  COMMENT "This command will cp so to target/lib"
)


 
