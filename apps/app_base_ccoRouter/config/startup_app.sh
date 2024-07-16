#!/bin/sh
PATH=$PATH:/usr/local/extapps/ccoRouter/bin
export PATH

BOARD=$( cat /proc/cpuinfo | grep 3358)
if [ ! -n "$BOARD" ]; then
	PID=$(ps aux|grep ccoRouter |grep -v grep|grep -v '.sh'|awk '{print $2}')
else
	PID=$(ps |grep ccoRouter |grep -v grep|grep -v '.sh'|awk '{print $1}')
fi

if [ ! $PID ]; then
	echo "no ccoRouter!"
else
	echo "kill ccoRouter!"
	kill -9 $PID
fi

dataShardpath=/data/app
mkdir -p $dataShardpath/ccoRouter
mkdir -p $dataShardpath/ccoRouter/log
mkdir -p $dataShardpath/ccoRouter/data
mkdir -p $dataShardpath/ccoRouter/cfg

# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/ccoRouter/bin/ccoRouter

/usr/local/extapps/ccoRouter/bin/ccoRouter  >/dev/null  2>&1 &
echo "ccoRouter program started!"