#!/bin/sh
PATH=${PATH:+$PATH:}/usr/local/extapps/mapManager/bin
export PATH

PID=$(ps |grep mapManager |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	PID=$(ps aux|grep mapManager |grep -v grep|grep -v '.sh'|awk '{print $2}')
	if [ ! $PID ]; then
		echo "no mapManager!"
	else
		echo "kill mapManager!"
		kill -9 $PID
	fi
else
	echo "kill mapManager!"
	kill -9 $PID
fi

dataShardpath=/data/app
devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
dataDevInfoShardpath=/data/devinfo
halpath=/lib/hal_lib

mkdir -p $dataShardpath/mapManager
mkdir -p $dataShardpath/mapManager/log
mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $devTmpShardPath


# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/mapManager/bin/mapManager

/usr/local/extapps/mapManager/bin/mapManager >/dev/null  2>&1 &
echo "mapManager program started!"