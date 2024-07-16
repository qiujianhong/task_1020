#!/bin/sh
PATH=$PATH:/usr/local/extapps/radioManager/bin
export PATH

BOARD=$( cat /proc/cpuinfo | grep 3358)
if [ ! -n "$BOARD" ]; then
	PID=$(ps aux|grep radioManager |grep -v grep|grep -v '.sh'|awk '{print $2}')
else
	PID=$(ps |grep radioManager |grep -v grep|grep -v '.sh'|awk '{print $1}')
fi

if [ ! $PID ]; then
	echo "no radioManager!"
else
	echo "kill radioManager!"
	kill -9 $PID
fi

dataShardpath=/data/app
mkdir -p $dataShardpath/radioManager
mkdir -p $dataShardpath/radioManager/log
mkdir -p $dataShardpath/radioManager/data
mkdir -p $dataShardpath/radioManager/cfg

# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/radioManager/bin/radioManager

/usr/local/extapps/radioManager/bin/radioManager  >/dev/null  2>&1 &
echo "radioManager program started!"
