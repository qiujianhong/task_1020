#!/bin/sh
PATH=$PATH:/usr/local/extapps/wirelessDCM/bin
export PATH

APP_NAME=wirelessDCM

kill_app(){

	PID=$(ps |grep $APP_NAME |grep -v grep|grep -v '.sh'|grep -v 'pppdlog'|grep -v 'log'|awk '{print $1}')
	aPID=$(($PID+0))
	if [ "0" != "$aPID" ]; then
		echo "kill $APP_NAME pid $aPID!"
		kill -9 $PID
		return
	fi
	
	PID=$(ps |grep $APP_NAME |grep -v grep|grep -v '.sh'|grep -v 'pppdlog'|grep -v 'log'|awk '{print $2}')
	aPID=$(($PID+0))
	if [ "0" != "$aPID" ]; then
		echo "kill $APP_NAME pid $aPID!"
		kill -9 $PID
		return
	fi
	
	PID=$(ps -ef |grep $APP_NAME |grep -v grep|grep -v '.sh'|grep -v 'pppdlog'|grep -v 'log'|awk '{print $1}')
	aPID=$(($PID+0))
	if [ "0" != "$aPID" ]; then
		echo "kill $APP_NAME pid $aPID!"
		kill -9 $PID
		return
	fi
	
	PID=$(ps -ef |grep $APP_NAME |grep -v grep|grep -v '.sh'|grep -v 'pppdlog'|grep -v 'log'|awk '{print $2}')
	aPID=$(($PID+0))
	if [ "0" != "$aPID" ]; then
		echo "kill $APP_NAME pid $aPID!"
		kill -9 $PID
		return
	fi
	
	echo "no $APP_NAME"
}

kill_app

dataShardpath=/data/app
devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
dataDevInfoShardpath=/data/devinfo
halpath=/lib/hal_lib

mkdir -p $dataShardpath/wirelessDCM
mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $devTmpShardPath


# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/wirelessDCM/bin/wirelessDCM

/usr/local/extapps/wirelessDCM/bin/wirelessDCM  >/dev/null  2>&1 &
echo "wirelessDCM program started!"