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
liccheckpath=$basePath/depends/liccheck
otherapppath=$basePath/scripts/CCU/area_apps
OSPath=$basePath/depends/OS/CCU/3358
IAMGEPath=$basePath/depends/image
GWIAMGEPath=$basePath/depends/gwimage
upgradePath=$basePath/release/upgrade
koPath=$basePath/release/modules

CAPP_ARRAY=(taskManager puAmr dbCenter ccoRouter pdsManager)
MAPP_ARRAY=(smiOS wirelessDCM desktopGui acMeter rspSample btManager uartManager usbManager scsMonitor iotManager cpuMonitor)

chmod 755 -R $appPath
chmod 755 $toolPATH/appSignTool_x86

build_one_APP(){
    cp -rf $scriptsPath/3358/setup_app.sh $appPath/
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
		sleep 2
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
	echo "Area: "$AREA_NAME
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
	# 拷贝容器相关工具集
	cp -rf $scriptsPath/docker_tools $packagePath/
	# 拷贝镜像 只拷国网镜像
	#cp -rf $IAMGEPath $packagePath/
	cp -rf $GWIAMGEPath $packagePath/
	
	# 打包ko 不再打包ko
	#cp -rf $basePath/drivers/CCU/3358_ko/* $koPath/
	#cd $basePath/release
	#tar -zcvf $packagePath/modules.tar.gz modules 

	cd $CURDIR

	# 地区编译结果拷贝(新终端先不考虑地区差异)
	dateZ=`grep -a -o '[0-9][0-9][0-9][0-9][0-9][0-9]' $basePath/apps/app_adv_puAmr/code/area/$AREA_NAME/$AREA_NAME""_fun.c | sed -n 1p`
	if [ "$AREA_NAME" == "default" ]; then
		# 获取版本信息
		verZ=`grep -a -o 'O1C*[0-9]*-[0-9][0-9]' $basePath/apps/app_adv_puAmr/code/area/default/default_fun.c | sed -n 1p`
	else
		# 获取版本信息
		verZ=`grep -a -o 'C[A-Z]*[0-9]*-[0-9][0-9]' $basePath/apps/app_adv_puAmr/code/area/$AREA_NAME/$AREA_NAME""_fun.c | sed -n 1p`
	fi
	echo "Area: "$AREA_NAME

	# 地区特殊要求
	if [ -d "$otherapppath/$AREA_NAME/3358" ]; then
		cp -rf $otherapppath/$AREA_NAME/3358/* $packagePath/
	    cp -rf $otherapppath/$AREA_NAME/3358/setup_app.sh $appPath/
	fi
	
	# 福建地区不使用国网的ccoRouter、wirelessDCM和dbCenter
	if [ "$AREA_NAME" != "fujian" ]; then
		cp $baseappPath/ccoRouter.tar $packagePath/
		cp $baseappPath/wirelessDCM.tar $packagePath/
		cp $baseappPath/dbCenter.tar $packagePath/
	fi
	cp $baseappPath/uartManager.tar $packagePath/
	cp $baseappPath/uartManager.ini $packagePath/
	cp $baseappPath/scsMonitor.tar $packagePath/
	cp $baseappPath/SecReinf.tar $packagePath/
	cp $baseappPath/SecReinf_lic.tar $packagePath/
	cp $OSPath/*.img $packagePath/
	
	chmod 755 -R $packagePath

	sleep 1
	# 压缩安装包
	cd $packagePath
	echo $PWD
	rm -fr $basePath/release/tmp_s
	mkdir -p $basePath/release/tmp_s

	mv setup.sh $basePath/release/tmp_s/setup.sh
	mv setup $basePath/release/tmp_s/setup
	mv force_setup $basePath/release/tmp_s/force_setup
	echo "$AREA_NAME" > area.txt
	echo "$verZ" > version.txt
	tar -zcvf package_3358_CCU.$AREA_NAME.$verZ"_"$dateZ.tar.gz *
	mv package_3358_CCU.$AREA_NAME.$verZ"_"$dateZ.tar.gz $basePath/release/tmp_s/
	
	rm -rf ./*
	mv $basePath/release/tmp_s/* ./
	rm -rf $basePath/release/tmp_s
	
	cd $lcdtoolpath && make
	cd $liccheckpath && make
	
	# MD5值校验
	echo "package_3358_CCU.$AREA_NAME.$verZ""_""$dateZ.tar.gz" > $packagePath/md5.txt
	MD5=`md5sum $packagePath/package_3358_CCU.$AREA_NAME.$verZ"_"$dateZ.tar.gz | awk '{ print $1}'`
	echo "$MD5" >> $packagePath/md5.txt
	
	chmod 755 -R $packagePath
else
	build_one_APP
fi

