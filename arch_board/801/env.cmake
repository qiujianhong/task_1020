cmake_minimum_required(VERSION 2.8)

include(${CODE_DIE}/build/product.cmake)
include(${CODE_DIE}/build/board.cmake)
include(${CODE_DIE}/build/area.cmake)

# 指定编译器
set(CMAKE_C_COMPILER "/opt/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_C_FLAGS "-rdynamic -funwind-tables -ffunction-sections -Wall -Werror -fstack-protector-strong -Wl,-z,relro,-z,now -Wl,-z,noexecstack -fPIC -fdiagnostics-color=auto")

set(CMAKE_CXX_COMPILER "/opt/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++")
set(CMAKE_CXX_FLAGS "-rdynamic -funwind-tables -ffunction-sections -Wall -Werror -fstack-protector-strong -Wl,-z,relro,-z,now -Wl,-z,noexecstack -fPIC -fdiagnostics-color=auto")

# 判断是否存在
if(NOT EXISTS ${CODE_DIE}/build/FB.txt)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wl,-Map=object.map,--cref,--gc-section -s")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wl,-Map=object.map,--cref,--gc-section -s")
endif()

# 指定lib路径
set(LIB_DIR ${CODE_DIE}/release/lib)

# 指定hal_lib路径
set(HAL_DIR ${CODE_DIE}/release/hal_lib)

# 指定APP路径
set(APP_DIR ${CODE_DIE}/release/apps)

# 指定APP安装路径
set(RELEASE_DIR ${CODE_DIE}/release/apps)
set(INSTALL_DIR /usr/local/extapps)