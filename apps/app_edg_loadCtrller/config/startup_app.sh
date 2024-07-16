#!/bin/sh
PATH=$PATH:/usr/local/extapps/loadCtrller/bin
export PATH

APP_NAME=loadCtrller

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
mkdir -p $dataShardpath/loadCtrller
mkdir -p $dataShardpath/loadCtrller/log
mkdir -p $dataShardpath/loadCtrller/data
mkdir -p $dataShardpath/loadCtrller/cfg

# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/loadCtrller/bin/loadCtrller

/usr/local/extapps/loadCtrller/bin/loadCtrller  >/dev/null  2>&1 &
echo "loadCtrller program started!"