# 最低版本要求
cmake_minimum_required(VERSION 2.8)

# 指定编译器
set(CMAKE_C_COMPILER "/opt/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_C_FLAGS "-rdynamic -funwind-tables -ffunction-sections -Wall -Werror -fstack-protector-strong -Wl,-z,relro,-z,now -Wl,-z,noexecstack -fPIC -fdiagnostics-color=auto")

set(CMAKE_CXX_COMPILER "/opt/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++")
set(CMAKE_CXX_FLAGS "-rdynamic -funwind-tables -ffunction-sections -Wall -Werror -fstack-protector-strong -Wl,-z,relro,-z,now -Wl,-z,noexecstack -fPIC -fdiagnostics-color=auto")

# gdb调试使用
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")

# 指定lib路径
set(LIB_DIE ${CODE_DIE}/library)

# 指定终端APP的安装目录
set(INSTALL_DIR /usr/local/extapps)

# 指定发布路径,把安装路径放到发布路径中便于直接打包
set(RELEASE_DIR ${CODE_DIE}/release/subdir${INSTALL_DIR})


# 指定advPuAmr在发布目录的文件夹名称
set(FOLDER_NAME_ADVPUAMR advPuAmr)
# 指定用采APP在终端上的lib路径
set(LIB_PATH_ADVPUAMR ${INSTALL_DIR}/${FOLDER_NAME_ADVPUAMR}/lib)


# 指定advPdAmr在发布目录的文件夹名称
set(FOLDER_NAME_ADVPDAMR advPdAmr)
# 指定用采APP在终端上的lib路径
set(LIB_PATH_ADVPDAMR ${INSTALL_DIR}/${FOLDER_NAME_ADVPDAMR}/lib)

# 指定advStAmr在发布目录的文件夹名称
set(FOLDER_NAME_ADVSTAMR advStAmr)
# 指定用采APP在终端上的lib路径
set(LIB_PATH_ADVSTAMR ${INSTALL_DIR}/${FOLDER_NAME_ADVSTAMR}/lib)

# 指定taskManager在发布目录的文件夹名称
set(FOLDER_NAME_TASKMANAGER taskManager)
# 指定集任务调度管理APP在终端上的lib路径
set(LIB_PATH_TASKMANAGER ${INSTALL_DIR}/${FOLDER_NAME_TASKMANAGER}/lib)


# 指定ccoRouter在发布目录的文件夹名称
set(FOLDER_NAME_CCOROUTER ccoRouter)
# 指定集任务调度管理APP在终端上的lib路径
set(LIB_PATH_CCOROUTER ${INSTALL_DIR}/${FOLDER_NAME_CCOROUTER}/lib)


# 指定gui在发布目录的文件夹名称
set(FOLDER_NAME_GUI gui)
# 指定集任务调度管理APP在终端上的lib路径
set(LIB_PATH_GUI ${INSTALL_DIR}/${FOLDER_NAME_GUI}/lib)


# 指定mapManager在发布目录的文件夹名称
set(FOLDER_NAME_MAPMANAGER mapManager)
# 指定集任务调度管理APP在终端上的lib路径
set(LIB_PATH_MAPMANAGER ${INSTALL_DIR}/${FOLDER_NAME_MAPMANAGER}/lib)


# 指定wirelessDCM在发布目录的文件夹名称
set(FOLDER_NAME_WIRELESSDCM wirelessDCM)
# 指定集任务调度管理APP在终端上的lib路径
set(LIB_PATH_WIRELESSDCM ${INSTALL_DIR}/${FOLDER_NAME_WIRELESSDCM}/lib)


# 指定dbCenter在发布目录的文件夹名称
set(FOLDER_NAME_DBCENTER dbCenter)
# 指定集任务调度管理APP在终端上的lib路径
set(LIB_PATH_DBCENTER ${INSTALL_DIR}/${FOLDER_NAME_DBCENTER}/lib)


# 指定smiOS在发布目录的文件夹名称
set(FOLDER_NAME_SMIOS smiOS)
# 指定集任务调度管理APP在终端上的lib路径
set(LIB_PATH_SMIOS ${INSTALL_DIR}/${FOLDER_NAME_SMIOS}/lib)

# 指定maintain在发布目录的文件夹名称
set(FOLDER_NAME_SYSMAINTAIN sysMaintain)
# 指定maintain在终端上的lib路径
set(LIB_PATH_SYSMAINTAIN ${INSTALL_DIR}/${FOLDER_NAME_SYSMAINTAIN}/lib)


# 指定acMeter在发布目录的文件夹名称
set(FOLDER_NAME_ACMETER acMeter)
# 指定acMeter在终端上的lib路径
set(LIB_PATH_ACMETER ${INSTALL_DIR}/${FOLDER_NAME_ACMETER}/lib)

# 指定dbfunctest在发布目录的文件夹名称
set(FOLDER_NAME_DBFUNCTEST dbfunctest)
# 指定acMeter在终端上的lib路径
set(LIB_PATH_DBFUNCTEST ${INSTALL_DIR}/${FOLDER_NAME_DBFUNCTEST}/lib)