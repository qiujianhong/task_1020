#!/bin/sh
PATH=$PATH:/usr/local/extapps/sysMaintain/bin
export PATH

PID=$(ps | grep sysMaintain |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	PID=$(ps aux| grep sysMaintain |grep -v grep|grep -v '.sh'|awk '{print $2}')
	if [ ! $PID ]; then
		echo "no sysMaintain!"
	else
		echo "kill sysMaintain!"
		kill -9 $PID
	fi
else
	echo "kill sysMaintain!"
	kill -9 $PID
fi

dataShardpath=/data/app
mkdir -p $dataShardpath/sysMaintain
mkdir -p $dataShardpath/sysMaintain/log
mkdir -p $dataShardpath/sysMaintain/data
mkdir -p $dataShardpath/sysMaintain/cfg

# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/sysMaintain/bin/sysMaintain

/usr/local/extapps/sysMaintain/bin/sysMaintain  >/dev/null  2>&1 &
echo "sysMaintain program started!"