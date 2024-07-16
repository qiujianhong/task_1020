#!/bin/sh
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

