#!/bin/sh
PID=$(ps | grep pdsManager |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	PID=$(ps aux| grep pdsManager |grep -v grep|grep -v '.sh'|awk '{print $2}')
	if [ ! $PID ]; then
		echo "no pdsManager!"
	else
		echo "kill pdsManager!"
		kill -9 $PID
	fi
else
	echo "kill pdsManager!"
	kill -9 $PID
fi
