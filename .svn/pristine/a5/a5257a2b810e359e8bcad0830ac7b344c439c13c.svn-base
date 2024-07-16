#!/bin/bash
AREA_NAME="$1"
APP_NAME="$2"

CURR_DIR=$(cd $(dirname $0) && pwd )
cd $CURR_DIR
basePath=$(cd ../../../; pwd)
scriptsPath=$basePath/scripts/ECU
packagePath=$basePath/release/package
appPath=$basePath/release/apps
halPath=$basePath/release/hal_lib
toolPATH=$basePath/tools/ECU

chmod 755 -R $appPath

builddbCenter(){
	# 通过智芯的打包工具打包安装在容器内的dbCenter
	cd $appPath
	dbCenterPath=$appPath/dbCenter
	chmod 755 $toolPATH/appSignTool_x86
	echo "$dbCenterPath"
	cd $CURR_DIR
	$toolPATH/appSignTool_x86 -f $dbCenterPath/bin -b dbCenter -l $dbCenterPath/lib -v SV01.001 -o dbCenter
	mkdir -p $packagePath/dbCenter/bin
	cp $toolPATH/dbCenter.tar $packagePath/dbCenter/bin/
	cp $scriptsPath/app_script/default/dbCenter/dbCenterData.cfg $packagePath/dbCenter/bin/
	cp $scriptsPath/app_script/default/dbCenter/startup_app.sh $packagePath/dbCenter/bin/
	cd $packagePath && tar -cvf dbCenter.tar dbCenter && rm -rf dbCenter
	
	cd $appPath
}

echo "area: $AREA_NAME, name: $APP_NAME"	

buildwirelessDCM(){
	# 打包安装在主机内的wirelessDCM
	cd $appPath
	chmod 755 -R wirelessDCM
	tar -cvf $packagePath/wirelessDCM.tar wirelessDCM
}

if [ -z "$APP_NAME" -o "all" == "$APP_NAME" ]; then
	rm -rf $packagePath/*
	builddbCenter
	buildwirelessDCM
elif [ "dbCenter" == "$APP_NAME" ]; then
	builddbCenter
elif [ "wirelessDCM" == "$APP_NAME" ]; then
	buildwirelessDCM
fi
