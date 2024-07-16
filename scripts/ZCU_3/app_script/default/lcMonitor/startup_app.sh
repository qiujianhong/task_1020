#!/bin/sh
APPTarPath=/usr/local/extapps/lcMonitor/bin
dataShardpath=/data/app

devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
halpath=/lib/hal_lib
dataDevInfoShardpath=/data/devinfo
esamdev=/sys

mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $dataShardpath/lcMonitor/log
mkdir -p $dataShardpath/lcMonitor/data

# 先停止APP并卸载容器
appm -S -c edgerContainer -n lcMonitor
#container uninstall edgerContainer 
docker container ls -a | grep edgerContainer > /dev/null
#如果不存在创建容器
if [ $? -ne 0 ];then    
	echo "edgerContainer is not existed, we will docker creat it and install app!!!"
	container install edgerContainer $APPTarPath/lcMonitor.tar -v $halpath:$halpath -v $devShardPath:$devShardPath -v $dataShardpath:$dataShardpath \
	-v $devTmpShardPath:$devTmpShardPath -v $appDataShardpath:$appDataShardpath -v $dataDevInfoShardpath:$dataDevInfoShardpath -v $esamdev:$esamdev -dev-dir
else    
	echo "edgerContainer is existed, we will uninstall app!!!"
	appm -u  -c edgerContainer -n lcMonitor
	echo "install app!!!"
	appm -i  -c edgerContainer -n lcMonitor -f $APPTarPath/lcMonitor.tar
fi
