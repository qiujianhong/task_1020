#!/bin/sh
PID=$(ps | grep pdAmr |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	PID=$(ps aux| grep pdAmr |grep -v grep|grep -v '.sh'|awk '{print $2}')
	if [ ! $PID ]; then
		echo "no pdAmr!"
	else
		echo "kill pdAmr!"
		kill -9 $PID
	fi
else
	echo "kill pdAmr!"
	kill -9 $PID
fi
