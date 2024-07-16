#!/bin/bash
AREA_NAME="$1"
APP_NAME="$2"

CURDIR=$(cd $(dirname $0) && pwd )
cd $CURDIR

basePath=$(cd ../../; pwd)
scriptsPath=$basePath/scripts/ZCU_1
packagePath=$basePath/release/package
appPath=$basePath/release/apps
baseappPath=$basePath/depends/base_apps/ZCU_1
halPath=$basePath/release/hal_lib
toolPATH=$basePath/tools/ZCU_1
lcdtoolpath=$basePath/depends/lcdctrl
otherapppath=$basePath/scripts/ZCU_1/area_apps
upgradePath=$basePath/release/upgrade

CAPP_ARRAY=()
MAPP_ARRAY=(smiOS dbCenter desktopGui acMeter rspSample usbManager scsMonitor uartManager wirelessDCM taskManager stAmr radioManager)
 
chmod 755 -R $appPath
chmod 755 $toolPATH/appSignTool_x86

build_one_APP(){
    cp -rf $scriptsPath/3358/setup_app.sh $appPath/
	# 容器APP(当前没有打包脚本)
	for i in ${!CAPP_ARRAY[@]}
	do
		if [ "$APP_NAME" == "${CAPP_ARRAY[$i]}" ]; then
			APP_NAME=${CAPP_ARRAY[$i]}
			mkdir -p $packagePath/$APP_NAME/bin
			# 国网地区使用智芯的打包工具
			targetAppPath=$appPath/$APP_NAME
			echo "$targetAppPath"
			$toolPATH/appSignTool_x86 -f $targetAppPath/bin -b $APP_NAME -l $targetAppPath/lib -v SV01.001 -o $APP_NAME
			mkdir -p $packagePath/$APP_NAME/bin
			mv $toolPATH/$APP_NAME.tar $packagePath/$APP_NAME/bin/
			sleep 1
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
	
	# 地区特殊要求
	if [ -d "$otherapppath/$AREA_NAME/3358" ]; then
		cp -rf $otherapppath/$AREA_NAME/3358/* $packagePath/
	    cp -rf $otherapppath/$AREA_NAME/3358/setup_app.sh $appPath/
	fi
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
	
	sleep 1
	
	# 打包远程升级脚本
	cd $upgradePath
	tar -cvf $packagePath/sysUpdate.tar sysUpdate
	
	# 打包HAL库
	cd $basePath/release
	tar -zcvf $packagePath/hal_lib.tar.gz hal_lib  
	
	# 拷贝安装脚本
	cp -rf $scriptsPath/3358/* $packagePath/
	cp -rf $scriptsPath/3358/setup_app.sh $appPath/

	cd $CURDIR

	# 地区编译结果拷贝(新终端先不考虑地区差异)
	dateZ=`grep -a -o '[0-9][0-9][0-9][0-9][0-9][0-9]' $basePath/apps/app_adv_puAmr/code/area/$AREA_NAME/$AREA_NAME""_fun.c | sed -n 5p`
	if [ "$AREA_NAME" == "default" ]; then
		# 获取版本信息
		verZ=`grep -a -o 'O1Z[0-9]-[0-9][0-9]' $basePath/apps/app_adv_puAmr/code/area/default/default_fun.c | sed -n 1p`
	else
		# 获取版本信息
		verZ=`grep -a -o '1[A-Z][0-9][0-9]-[0-9][0-9]' $basePath/apps/app_adv_puAmr/code/area/$AREA_NAME/$AREA_NAME""_fun.c | sed -n 1p`
	fi
	
	echo "Area: "$AREA_NAME

	# 地区特殊要求
	if [ -d "$otherapppath/$AREA_NAME/3358" ]; then
		cp -rf $otherapppath/$AREA_NAME/3358/* $packagePath/
		cp -rf $otherapppath/$AREA_NAME/3358/setup_app.sh $appPath/
	fi
    cp $baseappPath/scsMonitor.tar $packagePath/
	chmod 755 -R $packagePath

	sleep 1
	# 压缩安装包
	cd $packagePath
	echo $PWD
	rm -fr $basePath/release/tmp_s
	mkdir -p $basePath/release/tmp_s

	mv setup.sh $basePath/release/tmp_s/setup.sh
	mv setup $basePath/release/tmp_s/setup
	echo "$AREA_NAME" > area.txt
	echo "$verZ" > version.txt
	mv force_setup $basePath/release/tmp_s/force_setup
	tar -zcvf package_3358_ZCU1.$AREA_NAME.$verZ"_"$dateZ.tar.gz *
	mv package_3358_ZCU1.$AREA_NAME.$verZ"_"$dateZ.tar.gz $basePath/release/tmp_s/

	rm -rf ./*
	mv $basePath/release/tmp_s/* ./
	rm -rf $basePath/release/tmp_s
	
	cd $lcdtoolpath && make
	
	# MD5值校验
	echo "package_3358_ZCU1.$AREA_NAME.$verZ""_""$dateZ.tar.gz" > $packagePath/md5.txt
	MD5=`md5sum $packagePath/package_3358_ZCU1.$AREA_NAME.$verZ"_"$dateZ.tar.gz | awk '{ print $1}'`
	echo "$MD5" >> $packagePath/md5.txt
	
	chmod 755 -R $packagePath
else
	build_one_APP
fi

