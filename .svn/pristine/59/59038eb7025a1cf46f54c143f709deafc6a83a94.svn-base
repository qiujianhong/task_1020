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
toolPATH=$basePath/tools/CCU
lcdtoolpath=$basePath/depends/lcdctrl
otherapppath=$basePath/scripts/CCU/area_apps

CAPP_ARRAY=(dbCenter)
MAPP_ARRAY=(wirelessDCM)
 
chmod 755 -R $appPath
chmod 755 $toolPATH/appSignTool_x86

build_one_APP(){
	# 容器APP(当前没有打包脚本)
	for i in ${!CAPP_ARRAY[@]}
	do
		if [ "$APP_NAME" == "${CAPP_ARRAY[$i]}" ]; then
			mkdir -p $packagePath/$APP_NAME/bin
			# 国网地区使用智芯的打包工具
			targetAppPath=$appPath/$APP_NAME
			echo "$targetAppPath"
			$toolPATH/appSignTool_x86 -f $targetAppPath/bin -b $APP_NAME -l $targetAppPath/lib -v SV01.001 -o $APP_NAME
			sleep 1
			mkdir -p $packagePath/$APP_NAME/bin
			mv $toolPATH/$APP_NAME.tar $packagePath/$APP_NAME/bin/
			sleep 1
			# 拷贝地区特殊启动脚本
			if [ -f $scriptsPath/app_script/$AREA_NAME/$APP_NAME/startup_app.sh ]; then
				cp $scriptsPath/app_script/$AREA_NAME/$APP_NAME/startup_app.sh $packagePath/$APP_NAME/bin/
			else
				cp $scriptsPath/app_script/default/$APP_NAME/startup_app.sh $packagePath/$APP_NAME/bin/
			fi
			cd $packagePath && tar -cvf $APP_NAME.tar $APP_NAME
			sleep 2
			rm -rf $APP_NAME
			
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
		mkdir -p $packagePath/$APP_NAME/bin
		# 国网地区使用智芯的打包工具
		targetAppPath=$appPath/$APP_NAME
		echo "$targetAppPath"
		$toolPATH/appSignTool_x86 -f $targetAppPath/bin -b $APP_NAME -l $targetAppPath/lib -v SV01.001 -o $APP_NAME
		sleep 1
		mkdir -p $packagePath/$APP_NAME/bin
		mv $toolPATH/$APP_NAME.tar $packagePath/$APP_NAME/bin/
		sleep 2
		# 拷贝地区特殊启动脚本
		if [ -f $scriptsPath/app_script/$AREA_NAME/$APP_NAME/startup_app.sh ]; then
			cp $scriptsPath/app_script/$AREA_NAME/$APP_NAME/startup_app.sh $packagePath/$APP_NAME/bin/
		else
			cp $scriptsPath/app_script/default/$APP_NAME/startup_app.sh $packagePath/$APP_NAME/bin/
		fi
		cd $packagePath && tar -cvf $APP_NAME.tar $APP_NAME
		sleep 3
		cp $APP_NAME.tar $appPath/
		rm -rf $APP_NAME
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
	
	
else
	build_one_APP
fi

