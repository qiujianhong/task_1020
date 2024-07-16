#!/bin/sh

APPTarPath=/usr/local/extapps/ccoRouter/bin
dataShardpath=/data/app

devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
halpath=/lib/hal_lib
dataDevInfoShardpath=/data/devinfo

mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $dataShardpath/ccoRouter/log
mkdir -p $dataShardpath/ccoRouter/data
mkdir -p $dataShardpath/ccoRouter/cfg

# 先停止APP并卸载容器
appm -S -c baseContainer -n ccoRouter
#container uninstall baseContainer 
docker container ls -a | grep baseContainer > /dev/null
#如果不存在创建容器
if [ $? -ne 0 ];then    
	echo "baseContainer is not existed, we will docker creat it and install app!!!"
	container install baseContainer $APPTarPath/ccoRouter.tar -v $halpath:$halpath -v $devShardPath:$devShardPath -v $dataShardpath:$dataShardpath \
	-v $devTmpShardPath:$devTmpShardPath -v $appDataShardpath:$appDataShardpath -v $dataDevInfoShardpath:$dataDevInfoShardpath -dev-dir
else    
	echo "baseContainer is existed, we will uninstall app!!!"
	appm -u -c baseContainer -n ccoRouter
	echo "install app!!!"
	appm -i -c baseContainer -n ccoRouter -f $APPTarPath/ccoRouter.tar
fi
