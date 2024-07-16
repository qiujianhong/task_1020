#!/bin/sh
PATH=$PATH:/usr/local/extapps/desktopGui/bin
export PATH

APP_NAME=desktopGui

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
mkdir -p $dataShardpath/desktopGui
mkdir -p $dataShardpath/desktopGui/log
mkdir -p $dataShardpath/desktopGui/data
mkdir -p $dataShardpath/desktopGui/cfg

# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/desktopGui/bin/desktopGui
cd /usr/local/extapps/desktopGui/bin
./desktopGui  >/dev/null  2>&1 &
echo "desktopGui program started!"