#!/bin/sh
PATH=$PATH:/usr/local/extapps/uartManager/bin
export PATH

BOARD=$( cat /proc/cpuinfo | grep 3358)
if [ ! -n "$BOARD" ]; then
	PID=$(ps aux|grep uartManager |grep -v grep|grep -v '.sh'|awk '{print $2}')
else
	PID=$(ps |grep uartManager |grep -v grep|grep -v '.sh'|awk '{print $1}')
fi

if [ ! $PID ]; then
	echo "no uartManager!"
else
	echo "kill uartManager!"
	kill -9 $PID
fi

dataShardpath=/data/app
devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
dataDevInfoShardpath=/data/devinfo
halpath=/lib/hal_lib

mkdir -p $dataShardpath/uartManager
mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $devTmpShardPath

# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/uartManager/bin/uartManager

/usr/local/extapps/uartManager/bin/uartManager >/dev/null  2>&1 &
echo "uartManager program started!"