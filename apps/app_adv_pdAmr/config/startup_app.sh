#!/bin/sh
PATH=$PATH:/usr/local/extapps/pdAmr/bin
export PATH

PID=$(ps | grep pdAmr |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	PID=$(ps aux| grep pdAmr |grep -v grep|grep -v '.sh'|awk '{print $2}')
	if [ ! $PID ]; then
		echo "no pdAmr!"
	else
		echo "kill pdAmr!"
		kill -9 $PID
	fi
else
	echo "kill pdAmr!"
	kill -9 $PID
fi

dataShardpath=/data/app
mkdir -p $dataShardpath/pdAmr
mkdir -p $dataShardpath/pdAmr/log
mkdir -p $dataShardpath/pdAmr/data
mkdir -p $dataShardpath/pdAmr/cfg

# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/pdAmr/bin/pdAmr

/usr/local/extapps/pdAmr/bin/pdAmr  >/dev/null  2>&1 &
echo "pdAmr program started!"