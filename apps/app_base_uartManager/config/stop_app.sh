#!/bin/sh
BOARD=$( cat /proc/cpuinfo | grep 3358)
if [ ! -n "$BOARD" ]; then
	PID=$(ps aux|grep uartManager |grep -v grep|grep -v '.sh'|awk '{print $2}')
else
	PID=$(ps |grep uartManager |grep -v grep|grep -v '.sh'|awk '{print $1}')
fi

if [ ! $PID ]; then
	echo "no uartManager!"
else
	echo "kill uartManager!"
	kill -9 $PID
fi

