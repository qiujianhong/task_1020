#!/bin/sh
PATH=$PATH:/usr/local/extapps/desktopGui/bin
export PATH

PID=$(ps |grep desktopGui |grep -v grep|grep -v '.sh'|grep -v '/log'|awk '{print $1}')
if [ ! $PID ]; then
	echo "no desktopGui!"
else
	echo "kill desktopGui!"
	kill -9 $PID
fi

dataShardpath=/data/app
devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
dataDevInfoShardpath=/data/devinfo
halpath=/lib/hal_lib

mkdir -p $dataShardpath/desktopGui
mkdir -p $dataShardpath/desktopGui/log
mkdir -p $dataShardpath/desktopGui/data
mkdir -p $dataShardpath/desktopGui/cfg
mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $devTmpShardPath


# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/desktopGui/bin/desktopGui

/usr/local/extapps/desktopGui/bin/desktopGui >/dev/null 2>&1 &
echo "desktopGui program started!"