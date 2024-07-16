#!/bin/sh

APPTarPath=/usr/local/extapps/puAmr/bin
dataShardpath=/data
updateShardpath=/update
logShardpath=/log

devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
halpath=/lib/hal_lib
dataDevInfoShardpath=/data/devinfo
esamdev=/sys

mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $dataShardpath/app/puAmr/log
mkdir -p $dataShardpath/app/puAmr/data
mkdir -p $dataShardpath/app/puAmr/cfg


# 先停止APP并卸载容器
appm -S -c amrContainer -n puAmr
#container uninstall amrContainer 
#docker container rm -f amrContainer
docker container ls -a | grep amrContainer > /dev/null
#如果不存在创建容器
if [ $? -ne 0 ];then    
	echo "amrContainer is not existed, we will docker creat it and install app!!!"
	container install amrContainer $APPTarPath/puAmr.tar -v $halpath:$halpath -v $dataShardpath:$dataShardpath -v $updateShardpath:$updateShardpath \
	-v $logShardpath:$logShardpath -v $devTmpShardPath:$devTmpShardPath -v $appDataShardpath:$appDataShardpath -v $devShardPath:$devShardPath \
    -v $esamdev:$esamdev -dev-dir
else    
	echo "amrContainer is existed, we will uninstall app!!!"
	appm -u  -c amrContainer -n puAmr
	echo "install app!!!"
	appm -i  -c amrContainer -n puAmr -f $APPTarPath/puAmr.tar
fi
