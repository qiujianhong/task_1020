#!/bin/sh

APPTarPath=/usr/local/extapps/puAmr/bin
dataShardpath=/data/app

devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
halpath=/lib/hal_lib
dataDevInfoShardpath=/data/devinfo

mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $dataShardpath/puAmr/log
mkdir -p $dataShardpath/puAmr/data
mkdir -p $dataShardpath/puAmr/cfg


# 配置文件不存在，则拷贝
if [ ! -f "/data/app/puAmr/cfg/puAmrData.cfg" ];then
	cp -fr $APPTarPath/puAmrData.cfg $dataShardpath/puAmr/cfg/
fi
# 先停止APP并卸载容器
appm -S -c amrContainer -n puAmr
appm -S -c amrContainer -n stAmr
#container uninstall amrContainer 
docker container rm -f amrContainer
docker container ls -a | grep amrContainer > /dev/null
#如果不存在创建容器
if [ $? -ne 0 ];then    
	echo "amrContainer is not existed, we will docker creat it and install app!!!"
	container install amrContainer $APPTarPath/puAmr.tar -p 5100:5100 -v $halpath:$halpath -v $dataShardpath:$dataShardpath -v $devTmpShardPath:$devTmpShardPath \
	-v $appDataShardpath:$appDataShardpath -v $dataDevInfoShardpath:$dataDevInfoShardpath -v $devShardPath:$devShardPath -dev-dir
else    
	echo "amrContainer is existed, we will uninstall app!!!"
	appm -u  -c amrContainer -n puAmr
	echo "install app!!!"
	appm -i  -c amrContainer -n puAmr -f $APPTarPath/puAmr.tar
fi
