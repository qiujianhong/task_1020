#!/bin/sh
PID=$(ps | grep lcMonitor |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	PID=$(ps aux| grep lcMonitor |grep -v grep|grep -v '.sh'|awk '{print $2}')
	if [ ! $PID ]; then
		echo "no lcMonitor!"
	else
		echo "kill lcMonitor!"
		kill -9 $PID
	fi
else
	echo "kill lcMonitor!"
	kill -9 $PID
fi
