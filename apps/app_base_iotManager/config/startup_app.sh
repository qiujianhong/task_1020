#!/bin/sh
PATH=$PATH:/usr/local/extapps/iotManager/bin
export PATH

PID=$(ps | grep iotManager |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	PID=$(ps aux| grep iotManager |grep -v grep|grep -v '.sh'|awk '{print $2}')
	if [ ! $PID ]; then
		echo "no iotManager!"
	else
		echo "kill iotManager!"
		kill -9 $PID
	fi
else
	echo "kill iotManager!"
	kill -9 $PID
fi

dataShardpath=/data/app
mkdir -p $dataShardpath/iotManager
mkdir -p $dataShardpath/iotManager/log
mkdir -p $dataShardpath/iotManager/data
mkdir -p $dataShardpath/iotManager/cfg

# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/iotManager/bin/iotManager

/usr/local/extapps/iotManager/bin/iotManager  >/dev/null  2>&1 &
echo "iotManager program started!"