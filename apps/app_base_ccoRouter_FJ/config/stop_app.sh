#!/bin/sh
BOARD=$( cat /proc/cpuinfo | grep 3358)
if [ ! -n "$BOARD" ]; then
	PID=$(ps aux|grep ccoRouter |grep -v grep|grep -v '.sh'|awk '{print $2}')
else
	PID=$(ps |grep ccoRouter |grep -v grep|grep -v '.sh'|awk '{print $1}')
fi

if [ ! $PID ]; then
	echo "no ccoRouter!"
else
	echo "kill ccoRouter!"
	kill -9 $PID
fi
