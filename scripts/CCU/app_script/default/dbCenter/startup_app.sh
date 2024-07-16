#!/bin/sh

APPTarPath=/usr/local/extapps/dbCenter/bin
dataShardpath=/data/app

devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
halpath=/lib/hal_lib
dataDevInfoShardpath=/data/devinfo

mkdir -p $dataDevInfoShardpath
mkdir -p $dataDevInfoShardbackuppath
mkdir -p $appDataShardpath
mkdir -p $dataShardpath/dbCenter/log
mkdir -p $dataShardpath/dbCenter/data
mkdir -p $dataShardpath/dbCenter/cfg


# 配置文件不存在，则拷贝
if [ ! -f "/data/app/dbCenter/cfg/dbCenterData.cfg" ];then
	cp -fr $APPTarPath/dbCenterData.cfg $dataShardpath/dbCenter/cfg/
fi
# 先停止APP并卸载容器
appm -S -c baseContainer -n dbCenter
#container uninstall baseContainer 
docker container ls -a | grep baseContainer > /dev/null
#如果不存在创建容器
if [ $? -ne 0 ];then    
	echo "baseContainer is not existed, we will docker creat it and install app!!!"
	container install baseContainer $APPTarPath/dbCenter.tar -v $halpath:$halpath -v $devShardPath:$devShardPath -v $dataShardpath:$dataShardpath \
	-v $devTmpShardPath:$devTmpShardPath -v $appDataShardpath:$appDataShardpath -v $dataDevInfoShardpath:$dataDevInfoShardpath -dev-dir
else    
	echo "baseContainer is existed, we will uninstall app!!!"
	appm -u  -c baseContainer -n dbCenter
	echo "install app!!!"
	appm -i  -c baseContainer -n dbCenter -f $APPTarPath/dbCenter.tar
fi

#appm -s -c baseContainer -n dbCenter

#ps aux |grep dbCenter | grep -v grep > /dev/null
#如果不存在创建容器
#if [ $? -ne 0 ];then    
#	echo "start dbCenter fail, re-install app!!!"
#	appm -i  -c baseContainer -n dbCenter -f $APPTarPath/dbCenter.tar
#fi
