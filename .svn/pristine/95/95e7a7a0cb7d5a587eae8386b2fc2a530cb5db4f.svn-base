#!/bin/sh
APP_NAME=rspSample

kill_app(){

	PID=$(ps |grep $APP_NAME |grep -v grep|grep -v '.sh'|grep -v 'pppdlog'|grep -v 'log'|awk '{print $1}')
	aPID=$(($PID+0))
	if [ "0" != "$aPID" ]; then
		echo "kill $APP_NAME pid $aPID!"
		kill -9 $PID
		return
	fi
	
	PID=$(ps |grep $APP_NAME |grep -v grep|grep -v '.sh'|grep -v 'pppdlog'|grep -v 'log'|awk '{print $2}')
	aPID=$(($PID+0))
	if [ "0" != "$aPID" ]; then
		echo "kill $APP_NAME pid $aPID!"
		kill -9 $PID
		return
	fi
	
	PID=$(ps -ef |grep $APP_NAME |grep -v grep|grep -v '.sh'|grep -v 'pppdlog'|grep -v 'log'|awk '{print $1}')
	aPID=$(($PID+0))
	if [ "0" != "$aPID" ]; then
		echo "kill $APP_NAME pid $aPID!"
		kill -9 $PID
		return
	fi
	
	PID=$(ps -ef |grep $APP_NAME |grep -v grep|grep -v '.sh'|grep -v 'pppdlog'|grep -v 'log'|awk '{print $2}')
	aPID=$(($PID+0))
	if [ "0" != "$aPID" ]; then
		echo "kill $APP_NAME pid $aPID!"
		kill -9 $PID
		return
	fi
	
	echo "no $APP_NAME"
}

kill_app

