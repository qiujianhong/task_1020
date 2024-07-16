#!/bin/sh
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
