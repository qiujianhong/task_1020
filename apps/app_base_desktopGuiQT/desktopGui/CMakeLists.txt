cmake_minimum_required(VERSION 3.5)

# 设置代码根目录
set(CODE_DIE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
if(DEFINED QT_BORAD)
	# 加载公共设置
	include(${CODE_DIE}/build/env.cmake)
else()
	include(${CODE_DIE}/arch_board/x86/env.cmake)
endif()

# 编译宏，需要放到project前面
add_definitions(-DADV_APP_NAME="desktopGui" -DAPP_SUBTOPIC="+/M-desktopGui" -DAPP_DEFAULT_TOPIC="M-desktopGui/M-smiOS" -DADV_INI_NAME="desktopGui.ini"
					-DAPP_VERSION=0x00000001 -DAPP_PUBLISH_TIME="2022/05/13 15:00:00" -DADV_SHOW_NAME="桌面显示")
					
project(desktopGui VERSION 0.1 LANGUAGES CXX C)

#指定头文件目录
include_directories(${CODE_DIE}/include
		    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/msg
		    ${CMAKE_CURRENT_SOURCE_DIR}/msg/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/common
                    ${CMAKE_CURRENT_SOURCE_DIR}/desktop
                    ${CMAKE_CURRENT_SOURCE_DIR}/desktop/menu
                    ${CMAKE_CURRENT_SOURCE_DIR}/desktop/controlstatus
                    ${CMAKE_CURRENT_SOURCE_DIR}/desktop/paramshow
                    ${CMAKE_CURRENT_SOURCE_DIR}/desktop/powerbuy
                    ${CODE_DIE}/include
                    ${CODE_DIE}/lib/include)


#链接库路径
link_directories(${LIB_DIR}  ${CODE_DIE}/depends/mosquitto/${BOARD_TYPE})

if(DEFINED QT_BORAD)
	set(CMAKE_PREFIX_PATH "/opt/qt5.12")
endif()


file(GLOB_RECURSE QRC_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QRC_SOURCE_FILES imgs.qrc)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets Charts Gui Xml Multimedia LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Charts Gui Xml Multimedia LinguistTools REQUIRED)

set(TS_FILES desktopGui_zh_CN.ts)


qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})

SOURCE_GROUP("Resource Files" FILES ${QRC_SOURCE_FILES})


# 设置目标生成路径
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../target/bin)

set(PROJECT_SOURCES
        main.cpp
        common/common.c
        common/common.h
#        KChart/kchart.cpp
#        KChart/kchart.h
        msg/msgthread.cpp
        msg/msgthread.h
        msg/source/msg_proc.c
        msg/source/task_proc.c
        msg/source/out_msg.c
        desktop/login.cpp
        desktop/login.h
        desktop/login.ui
        desktop/desktop.cpp
        desktop/desktop.h
        desktop/desktop.ui
        desktop/menu.cpp
        desktop/menu.h
        desktop/menu.ui
        desktop/menu/currentdata.cpp
        desktop/menu/currentdata.h
        desktop/menu/currentdata.ui
        desktop/menu/historydata.cpp
        desktop/menu/historydata.h
        desktop/menu/historydata.ui
        desktop/menu/paramset.cpp
        desktop/menu/paramset.h
        desktop/menu/paramset.ui
        desktop/menu/mntcomset.cpp
        desktop/menu/mntcomset.h
        desktop/menu/mntcomset.ui
		desktop/menu/passage.cpp
        desktop/menu/passage.h
        desktop/menu/passage.ui
		desktop/menu/meter.cpp
        desktop/menu/meter.h
        desktop/menu/meter.ui
		desktop/menu/pulse.cpp
        desktop/menu/pulse.h
        desktop/menu/pulse.ui
		desktop/menu/group.cpp
        desktop/menu/group.h
        desktop/menu/group.ui
		desktop/menu/mnttime.cpp
        desktop/menu/mnttime.h
        desktop/menu/mnttime.ui
		desktop/menu/passwordset.cpp
        desktop/menu/passwordset.h
        desktop/menu/passwordset.ui
        desktop/menu/gprscfg.cpp
        desktop/menu/gprscfg.h
        desktop/menu/gprscfg.ui
        desktop/menu/ethernetcfg.cpp
        desktop/menu/ethernetcfg.h
        desktop/menu/ethernetcfg.ui
        desktop/menu/mntinfo.cpp
        desktop/menu/mntinfo.h
        desktop/menu/mntinfo.ui
        desktop/menu/sysmanage.cpp
        desktop/menu/sysmanage.h
        desktop/menu/sysmanage.ui
        desktop/paramshow.cpp
        desktop/paramshow.h
        desktop/paramshow.ui
        desktop/controlstatus.cpp
        desktop/controlstatus.h
        desktop/controlstatus.ui
        desktop/powerbuy.cpp
        desktop/powerbuy.h
        desktop/powerbuy.ui
        ${TS_FILES}
        ${QRC_FILES}
)

link_libraries(pthread mqttclient template cJson msgassist Storage framecomm iniparser -Wl,-rpath=${LIB_PATH_GUI} mosquitto)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(desktopGui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET desktopGui APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(desktopGui SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(desktopGui
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(desktopGui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Charts)

if(DEFINED QT_BORAD)
	# 拷贝
	add_custom_command(TARGET desktopGui
	  POST_BUILD 
	  COMMAND cp ${LIB_DIR}/libStorage.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libmsgassist.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libiniparser.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libframecomm.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  COMMAND cp ${LIB_DIR}/libcJson.so ${CMAKE_CURRENT_SOURCE_DIR}/../target/lib
	  
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/desktopGui.ini ${PROJECT_SOURCE_DIR}/../target/bin/
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/dict ${PROJECT_SOURCE_DIR}/../target/bin/
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../config/skin.rcc ${PROJECT_SOURCE_DIR}/../target/bin/
	  
	  COMMENT "This command will cp app to release path"
	  COMMAND mkdir -p ${RELEASE_DIR}/desktopGui
	  COMMAND rm -fr ${RELEASE_DIR}/desktopGui/*
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/lib ${RELEASE_DIR}/desktopGui
	  COMMAND cp -fr ${PROJECT_SOURCE_DIR}/../target/bin ${RELEASE_DIR}/desktopGui
	)
endif()

set_target_properties(desktopGui PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(desktopGui)
endif()
