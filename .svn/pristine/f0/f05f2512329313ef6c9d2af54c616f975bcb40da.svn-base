#!/bin/sh
PATH=$PATH:/usr/local/extapps/usbManager/bin
export PATH

BOARD=$( cat /proc/cpuinfo | grep 3358)
if [ ! -n "$BOARD" ]; then
	PID=$(ps aux|grep usbManager |grep -v grep|grep -v '.sh'|awk '{print $2}')
else
	PID=$(ps |grep usbManager |grep -v grep|grep -v '.sh'|awk '{print $1}')
fi

if [ ! $PID ]; then
	echo "no usbManager!"
else
	echo "kill usbManager!"
	kill -9 $PID
fi

dataShardpath=/data/app
devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
dataDevInfoShardpath=/data/devinfo
halpath=/lib/hal_lib

mkdir -p $dataShardpath/usbManager
mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $devTmpShardPath

# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/usbManager/bin/usbManager

/usr/local/extapps/usbManager/bin/usbManager >/dev/null  2>&1 &
echo "usbManager program started!"