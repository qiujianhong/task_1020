#!/bin/sh
PID=$(ps |grep mapManager |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	PID=$(ps aux|grep mapManager |grep -v grep|grep -v '.sh'|awk '{print $2}')
	if [ ! $PID ]; then
		echo "no mapManager!"
	else
		echo "kill mapManager!"
		kill -9 $PID
	fi
else
	echo "kill mapManager!"
	kill -9 $PID
fi
