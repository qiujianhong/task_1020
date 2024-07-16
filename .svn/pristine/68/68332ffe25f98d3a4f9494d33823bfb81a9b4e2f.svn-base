#!/bin/sh
PATH=$PATH:/usr/local/extapps/pdsManager/bin
export PATH

PID=$(ps | grep pdsManager |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	PID=$(ps aux| grep pdsManager |grep -v grep|grep -v '.sh'|awk '{print $2}')
	if [ ! $PID ]; then
		echo "no pdsManager!"
	else
		echo "kill pdsManager!"
		kill -9 $PID
	fi
else
	echo "kill pdsManager!"
	kill -9 $PID
fi

dataShardpath=/data/app
mkdir -p $dataShardpath/pdsManager
mkdir -p $dataShardpath/pdsManager/log
mkdir -p $dataShardpath/pdsManager/data
mkdir -p $dataShardpath/pdsManager/cfg

# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/pdsManager/bin/pdsManager

/usr/local/extapps/pdsManager/bin/pdsManager  >/dev/null  2>&1 &
echo "pdsManager program started!"