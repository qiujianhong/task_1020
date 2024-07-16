#!/bin/sh
PATH=$PATH:/usr/local/extapps/cpuMonitor/bin
export PATH

BOARD=$( cat /proc/cpuinfo | grep 3358)
if [ ! -n "$BOARD" ]; then
	PID=$(ps aux|grep cpuMonitor |grep -v grep|grep -v '.sh'|awk '{print $2}')
else
	PID=$(ps |grep cpuMonitor |grep -v grep|grep -v '.sh'|awk '{print $1}')
fi

if [ ! $PID ]; then
	echo "no cpuMonitor!"
else
	echo "kill cpuMonitor! $PID"
	kill -9 $PID
fi

dataShardpath=/data/app
mkdir -p $dataShardpath/cpuMonitor
mkdir -p $dataShardpath/cpuMonitor/log


# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/cpuMonitor/bin/cpuMonitor

/usr/local/extapps/cpuMonitor/bin/cpuMonitor  >/dev/null  2>&1 &
echo "cpuMonitor program started!"
