#!/bin/sh

APPTarPath=/usr/local/extapps/taskManager/bin
dataShardpath=/data/app

devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
halpath=/lib/hal_lib
dataDevInfoShardpath=/data/devinfo
esamdev=/sys

mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $dataShardpath/taskManager/log
mkdir -p $dataShardpath/taskManager/data

# ��ֹͣAPP��ж������
appm -S -c edgerContainer -n taskManager
#container uninstall edgerContainer 
docker container ls -a | grep edgerContainer > /dev/null
#��������ڴ�������
if [ $? -ne 0 ];then    
	echo "edgerContainer is not existed, we will docker creat it and install app!!!"
	container install edgerContainer $APPTarPath/taskManager.tar -v $halpath:$halpath -v $dataShardpath:$dataShardpath -v $devTmpShardPath:$devTmpShardPath \
	-v $appDataShardpath:$appDataShardpath -v $dataDevInfoShardpath:$dataDevInfoShardpath -v $devShardPath:$devShardPath -v $esamdev:$esamdev -dev-dir
else    
	echo "edgerContainer is existed, we will uninstall app!!!"
	appm -u  -c edgerContainer -n taskManager
	echo "install app!!!"
	appm -i  -c edgerContainer -n taskManager -f $APPTarPath/taskManager.tar
fi
