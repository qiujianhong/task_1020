#!/bin/sh
PID=$(ps |grep desktopGui |grep -v grep|grep -v '.sh'|grep -v '/log'|awk '{print $1}')
if [ ! $PID ]; then
	echo "no desktopGui!"
else
	echo "kill desktopGui!"
	kill -9 $PID
fi
