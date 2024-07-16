#!/bin/sh
PATH=$PATH:/usr/local/extapps/lcMonitor/bin
export PATH

PID=$(ps | grep lcMonitor |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	PID=$(ps aux| grep lcMonitor |grep -v grep|grep -v '.sh'|awk '{print $2}')
	if [ ! $PID ]; then
		echo "no lcMonitor!"
	else
		echo "kill lcMonitor!"
		kill -9 $PID
	fi
else
	echo "kill lcMonitor!"
	kill -9 $PID
fi

dataShardpath=/data/app
mkdir -p $dataShardpath/lcMonitor
mkdir -p $dataShardpath/lcMonitor/log
mkdir -p $dataShardpath/lcMonitor/data
mkdir -p $dataShardpath/lcMonitor/cfg

# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/lcMonitor/bin/lcMonitor

/usr/local/extapps/lcMonitor/bin/lcMonitor  >/dev/null  2>&1 &
echo "lcMonitor program started!"