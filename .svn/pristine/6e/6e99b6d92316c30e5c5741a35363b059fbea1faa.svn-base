#!/bin/sh
PID=$(ps | grep iotManager |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	PID=$(ps aux| grep iotManager |grep -v grep|grep -v '.sh'|awk '{print $2}')
	if [ ! $PID ]; then
		echo "no iotManager!"
	else
		echo "kill iotManager!"
		kill -9 $PID
	fi
else
	echo "kill iotManager!"
	kill -9 $PID
fi
