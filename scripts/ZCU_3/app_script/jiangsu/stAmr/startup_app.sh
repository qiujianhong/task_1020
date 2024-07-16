#!/bin/sh
APPTarPath=/usr/local/extapps/stAmr/bin
dataShardpath=/data/app

devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
halpath=/lib/hal_lib
dataDevInfoShardpath=/data/devinfo

mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $dataShardpath/stAmr/log
mkdir -p $dataShardpath/stAmr/data
mkdir -p $dataShardpath/stAmr/cfg

# 先停止APP并卸载容器
appm -S -c amrContainer -n stAmr
appm -S -c amrContainer -n puAmr
#container uninstall amrContainer 
#docker container rm -f amrContainer
docker container ls -a | grep amrContainer > /dev/null
#如果不存在创建容器
if [ $? -ne 0 ];then    
	echo "amrContainer is not existed, we will docker creat it and install app!!!"
	container install amrContainer $APPTarPath/stAmr.tar -p 5100:5100 -v $halpath:$halpath -v $devShardPath:$devShardPath -v $dataShardpath:$dataShardpath \
	-v $devTmpShardPath:$devTmpShardPath -v $appDataShardpath:$appDataShardpath -v $dataDevInfoShardpath:$dataDevInfoShardpath -dev-dir
else    
	echo "amrContainer is existed, we will uninstall app!!!"
	appm -u  -c amrContainer -n stAmr
	echo "install app!!!"
	appm -i  -c amrContainer -n stAmr -f $APPTarPath/stAmr.tar
fi
