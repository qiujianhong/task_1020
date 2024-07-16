#!/bin/sh
BOARD=$( cat /proc/cpuinfo | grep 3358)
if [ ! -n "$BOARD" ]; then
	PID=$(ps aux|grep scsMonitor |grep -v grep|grep -v '.sh'|awk '{print $2}')
else
	PID=$(ps |grep scsMonitor |grep -v grep|grep -v '.sh'|awk '{print $1}')
fi

if [ ! $PID ]; then
	echo "no scsMonitor!"
else
	echo "kill scsMonitor!"
	kill -9 $PID
fi

