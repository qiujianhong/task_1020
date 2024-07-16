#!/bin/bash
AREA_NAME="$1"
APP_NAME="$2"

CURDIR=$(cd $(dirname $0) && pwd )
cd $CURDIR

basePath=$(cd ../../; pwd)
scriptsPath=$basePath/scripts/ECU
packagePath=$basePath/release/package
appPath=$basePath/release/apps
baseappPath=$basePath/depends/base_apps/ECU
halPath=$basePath/release/hal_lib
toolPATH=$basePath/tools/ECU

CAPP_ARRAY=(taskManager stAmr lcMonitor pdsManager)
MAPP_ARRAY=(gui acMeter rspSample)
 
chmod 755 -R $appPath
chmod 755 $toolPATH/appSignTool_x86

build_one_APP(){
	# 容器APP
	for i in ${!CAPP_ARRAY[@]}
	do
		if [ "$APP_NAME" == "${CAPP_ARRAY[$i]}" ]; then
			targetAppPath=$appPath/$APP_NAME
			echo "$targetAppPath"
			$toolPATH/appSignTool_x86 -f $targetAppPath/bin -b $APP_NAME -l $targetAppPath/lib -v SV01.001 -o $APP_NAME
			mkdir -p $packagePath/$APP_NAME/bin
			mv $toolPATH/$APP_NAME.tar $packagePath/$APP_NAME/bin/
			# 拷贝地区特殊启动脚本
			if [ -f $scriptsPath/app_script/$AREA_NAME/$APP_NAME/startup_app.sh ]; then
				cp $scriptsPath/app_script/$AREA_NAME/$APP_NAME/startup_app.sh $packagePath/$APP_NAME/bin/
			else
				cp $scriptsPath/app_script/default/$APP_NAME/startup_app.sh $packagePath/$APP_NAME/bin/
			fi
			cd $packagePath && tar -cvf $APP_NAME.tar $APP_NAME && rm -rf $APP_NAME
			cp $APP_NAME.tar $appPath/
			cd $CURDIR
			break
		fi
	done
	
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
	# 容器APP
	for i in ${!CAPP_ARRAY[@]}
	do
		APP_NAME=${CAPP_ARRAY[$i]}
		targetAppPath=$appPath/$APP_NAME
		echo "$targetAppPath"
		$toolPATH/appSignTool_x86 -f $targetAppPath/bin -b $APP_NAME -l $targetAppPath/lib -v SV01.001 -o $APP_NAME
		mkdir -p $packagePath/$APP_NAME/bin
		mv $toolPATH/$APP_NAME.tar $packagePath/$APP_NAME/bin/
		# 拷贝地区特殊启动脚本
		if [ -f $scriptsPath/app_script/$AREA_NAME/$APP_NAME/startup_app.sh ]; then
			cp $scriptsPath/app_script/$AREA_NAME/$APP_NAME/startup_app.sh $packagePath/$APP_NAME/bin/
		else
			cp $scriptsPath/app_script/default/$APP_NAME/startup_app.sh $packagePath/$APP_NAME/bin/
		fi
		cd $packagePath && tar -cvf $APP_NAME.tar $APP_NAME && rm -rf $APP_NAME
		cp $APP_NAME.tar $appPath/
		cd $CURDIR
	done
	
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

	# 拷贝基础APP(不需要ccoRouter)
	cp -rf $baseappPath/dbCenter.tar $packagePath/
	cp -rf $baseappPath/mapManager.tar $packagePath/
	cp -rf $baseappPath/smiOS.tar $packagePath/
	cp -rf $baseappPath/wirelessDCM.tar $packagePath/

	# 拷贝安装脚本
	cp -rf $scriptsPath/701/* $packagePath/
	rm -rf $packagePath/setup_gb.sh
	rm -rf $packagePath/setup_sec_gb.sh

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

	mv setup_zb.sh $basePath/release/tmp_s/setup.sh
	tar -zcvf package_701_ZB.$AREA_NAME.$verZ"_"$dateZ.tar.gz *
	mv package_701_ZB.$AREA_NAME.$verZ"_"$dateZ.tar.gz $basePath/release/tmp_s/

	rm -rf ./*
	mv $basePath/release/tmp_s/* ./
	rm -rf $basePath/release/tmp_s
else
	build_one_APP
fi
