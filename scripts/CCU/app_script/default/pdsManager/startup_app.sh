#!/bin/sh

APPTarPath=/usr/local/extapps/pdsManager/bin
dataShardpath=/data/app

devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
halpath=/lib/hal_lib
dataDevInfoShardpath=/data/devinfo

mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $dataShardpath/pdsManager/log
mkdir -p $dataShardpath/pdsManager/data

# 先停止APP并卸载容器
appm -S -c edgerContainer -n pdsManager
#container uninstall edgerContainer 
docker container ls -a | grep edgerContainer > /dev/null
#如果不存在创建容器
if [ $? -ne 0 ];then    
	echo "edgerContainer is not existed, we will docker creat it and install app!!!"
	container install edgerContainer $APPTarPath/pdsManager.tar -v $halpath:$halpath -v $dataShardpath:$dataShardpath -v $devTmpShardPath:$devTmpShardPath \
	-v $appDataShardpath:$appDataShardpath -v $dataDevInfoShardpath:$dataDevInfoShardpath -v $devShardPath:$devShardPath -dev-dir
else    
	echo "edgerContainer is existed, we will uninstall app!!!"
	appm -u  -c edgerContainer -n pdsManager
	echo "install app!!!"
	appm -i  -c edgerContainer -n pdsManager -f $APPTarPath/pdsManager.tar
fi
