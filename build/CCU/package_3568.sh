#!/bin/bash
AREA_NAME="$1"
APP_NAME="$2"

CURDIR=$(cd $(dirname $0) && pwd )
cd $CURDIR

basePath=$(cd ../../; pwd)
scriptsPath=$basePath/scripts/CCU
packagePath=$basePath/release/package
appPath=$basePath/release/apps
baseappPath=$basePath/depends/base_apps/CCU
halPath=$basePath/release/hal_lib

MAPP_ARRAY=(smiOS wirelessDCM  dbCenter ccoRouter desktopGui acMeter rspSample btManager uartManager usbManager scsMonitor iotManager taskManager puAmr)
 
chmod 755 -R $appPath

build_one_APP(){
	# 非容器
	for i in ${!MAPP_ARRAY[@]}
	do
		if [ "$APP_NAME" == "${MAPP_ARRAY[$i]}" ]; then
			cd $appPath
			tar -cvf $packagePath/$APP_NAME.tar $APP_NAME
			cp $packagePath/$APP_NAME.tar $appPath/
			cd $CURDIR
			break
		fi
	done
}

build_all_APP(){
	# 非容器
	for i in ${!MAPP_ARRAY[@]}
	do
		cd $appPath
		APP_NAME=${MAPP_ARRAY[$i]}
		tar -cvf $packagePath/$APP_NAME.tar $APP_NAME
		cp $packagePath/$APP_NAME.tar $appPath/
		cd $CURDIR
	done
}

if [ -z "$APP_NAME" -o "all" == "$APP_NAME" ]; then
	rm -rf $packagePath/*
	build_all_APP
	
	# 打包HAL库
	cd $basePath/release
	tar -zcvf $packagePath/hal_lib.tar.gz hal_lib  

	# 拷贝安装脚本
	cp -rf $scriptsPath/3358/* $packagePath/

	cd $CURDIR

	# 地区编译结果拷贝(新终端先不考虑地区差异)
	dateZ=`grep -a -o '[0-9][0-9][0-9][0-9][0-9][0-9]' $basePath/apps/app_adv_puAmr/code/area/$AREA_NAME/$AREA_NAME""_fun.c | sed -n 3p`
	if [ "$AREA_NAME" == "default" ]; then
		# 获取版本信息
		verZ=`grep -a -o '6[A-Z]*[0-9]*-[0-9][0-9]' $basePath/apps/app_adv_puAmr/code/area/default/default_fun.c | sed -n 2p`
	else
		# 获取版本信息
		verZ=`grep -a -o 'Z[A-Z]*[0-9]*-[0-9][0-9]' $basePath/apps/app_adv_puAmr/code/area/$AREA_NAME/$AREA_NAME_fun.c | sed -n 1p`
	fi
	echo "Area: "$AREA_NAME

	chmod 755 -R $packagePath

	# 压缩安装包
	cd $packagePath
	echo $PWD
	rm -fr $basePath/release/tmp_s
	mkdir -p $basePath/release/tmp_s

	mv setup.sh $basePath/release/tmp_s/setup.sh
	tar -zcvf package_3568_CCU.$AREA_NAME.$verZ"_"$dateZ.tar.gz *
	mv package_3568_CCU.$AREA_NAME.$verZ"_"$dateZ.tar.gz $basePath/release/tmp_s/

	rm -rf ./*
	mv $basePath/release/tmp_s/* ./
	rm -rf $basePath/release/tmp_s
else
	build_one_APP
fi

