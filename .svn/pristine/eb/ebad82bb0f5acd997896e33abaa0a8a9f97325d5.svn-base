#!/bin/sh
BOARD=$( cat /proc/cpuinfo | grep 3358)
if [ ! -n "$BOARD" ]; then
	PID=$(ps aux|grep usbManager |grep -v grep|grep -v '.sh'|awk '{print $2}')
else
	PID=$(ps |grep usbManager |grep -v grep|grep -v '.sh'|awk '{print $1}')
fi

if [ ! $PID ]; then
	echo "no usbManager!"
else
	echo "kill usbManager!"
	kill -9 $PID
fi

