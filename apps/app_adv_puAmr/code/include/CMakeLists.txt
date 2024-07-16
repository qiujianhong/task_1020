#指定所依赖的cmake最低版本
cmake_minimum_required(VERSION 3.15)

#**********以下是原advanced_yc\common\include\build.make的等价(全面切CMAKE需剥离出去)*******************#
set(PRTL "gb")
#set(PRTL "zj")

if(${PRTL} STREQUAL "gb")
	add_definitions(-DZJ_PRTL=0 -DGB_PRTL=1)
else()
	add_definitions(-DZJ_PRTL=1 -DGB_PRTL=0)
endif()

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
	set(CMAKE_CROSSCOMPILING TRUE)
	set(CMAKE_C_COMPILER "/opt/gcc-linaro-4.9-2016.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc")
	set(CMAKE_AR "/opt/gcc-linaro-4.9-2016.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-ar")
else()
	set(CMAKE_C_COMPILER "gcc")
	set(CMAKE_AR "ar")
endif()

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
	if(${PRTL} STREQUAL "gb")
		set(INSTALL_LIB_PATH "${PREVDIR}/common/exe/prtl_gb/arm/mnt/lib")
		set(INSTALL_BIN_PATH "${PREVDIR}/common/exe/prtl_gb/arm/mnt/bin")
	else()
		set(INSTALL_LIB_PATH "${PREVDIR}/common/exe/prtl_zj/arm/mnt/lib")
		set(INSTALL_BIN_PATH "${PREVDIR}/common/exe/prtl_zj/arm/mnt/bin")
	endif()
	set(MINIGUI_SO_PATH "${PREVDIR}/common/exe/gui/arm")
else()
	if(${PRTL} STREQUAL "gb")
		set(INSTALL_LIB_PATH "${PREVDIR}/common/exe/prtl_gb/pc/mnt/lib")
		set(INSTALL_BIN_PATH "${PREVDIR}/common/exe/prtl_gb/pc/mnt/bin")
	else()
		set(INSTALL_LIB_PATH "${PREVDIR}/common/exe/prtl_zj/pc/mnt/lib")
		set(INSTALL_BIN_PATH "${PREVDIR}/common/exe/prtl_zj/pc/mnt/bin")
	endif()
	set(MINIGUI_SO_PATH "${PREVDIR}/common/exe/gui/pc")
endif()

#头文件目录
set(INCLUDE_PATH "${PREVDIR}/common/include")
set(XML_CFG_PATH "${PREVDIR}/common/xml")
set(COMMON_PATH	 "${PREVDIR}/platform/Src/Common")
set(INSTALL_PATH "${PREVDIR}/common/exe/install")
set(OS_PATH "${PREVDIR}/common/exe/OS")

#**********以上是原advanced_yc\common\include\build.make的等价(全面切CMAKE需剥离出去)*******************#

#**********以下是原advanced_yc\common\include\auto.make的等价(全面切CMAKE需剥离出去)*******************#
# gui模块编译子目录
set(guistyle "style231")

# gui进程使用xml配置文件
set(gui_xml "fkgaconfig_stylejzq.xml")

# 操作系统对应目录
set(OS_PATH_NAME "OS_III")

# 安装脚本对应目录
set(INSTALLSH_FILE_NAME "install_sh/install_698_style_jzq.sh")

# 交采7022芯片和7878芯片的标志
set(MEAIC "acnew")

# 载波程序编译标志
set(PLC_MAKE_EN 1)
#**********以上是原advanced_yc\common\include\auto.make的等价(全面切CMAKE需剥离出去)*******************#