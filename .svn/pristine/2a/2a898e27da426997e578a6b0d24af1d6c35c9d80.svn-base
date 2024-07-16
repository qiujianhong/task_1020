#!/bin/sh
PID=$(ps | grep sysMaintain |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	PID=$(ps aux| grep sysMaintain |grep -v grep|grep -v '.sh'|awk '{print $2}')
	if [ ! $PID ]; then
		echo "no sysMaintain!"
	else
		echo "kill sysMaintain!"
		kill -9 $PID
	fi
else
	echo "kill sysMaintain!"
	kill -9 $PID
fi
