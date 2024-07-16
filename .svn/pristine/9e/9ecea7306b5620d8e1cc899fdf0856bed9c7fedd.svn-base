#!/bin/sh
BOARD=$( cat /proc/cpuinfo | grep 3358)
if [ ! -n "$BOARD" ]; then
	PID=$(ps aux|grep radioManager |grep -v grep|grep -v '.sh'|awk '{print $2}')
else
	PID=$(ps |grep radioManager |grep -v grep|grep -v '.sh'|awk '{print $1}')
fi

if [ ! $PID ]; then
	echo "no radioManager!"
else
	echo "kill radioManager!"
	kill -9 $PID
fi

