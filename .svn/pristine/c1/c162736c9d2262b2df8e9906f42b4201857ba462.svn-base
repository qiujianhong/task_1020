#!/bin/sh
PATH=$PATH:/usr/local/extapps/puAmr/bin
export PATH

BOARD=$( cat /proc/cpuinfo | grep 3358)
if [ ! -n "$BOARD" ]; then
	PID=$(ps aux|grep puAmr |grep -v grep|grep -v '.sh'|awk '{print $2}')
else
	PID=$(ps |grep puAmr |grep -v grep|grep -v '.sh'|awk '{print $1}')
fi

if [ ! $PID ]; then
	echo "no puAmr!"
else
	echo "kill puAmr!"
	kill -9 $PID
fi

dataShardpath=/data/app
mkdir -p $dataShardpath/puAmr
mkdir -p $dataShardpath/puAmr/log
mkdir -p $dataShardpath/puAmr/data
mkdir -p $dataShardpath/puAmr/cfg

# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/puAmr/bin/puAmr

/usr/local/extapps/puAmr/bin/puAmr  >/dev/null  2>&1 &
echo "puAmr program started!"