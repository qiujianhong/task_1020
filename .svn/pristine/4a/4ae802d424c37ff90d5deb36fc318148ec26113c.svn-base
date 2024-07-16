#!/bin/bash

#延时10秒，等待规约回复完成
sleep 10

#解压升级包,并重命名第一级文件夹的名字为upgrade
mkdir -p /data/app/$1/upgrade/ && tar -zxvf /data/app/$1/upgrade.tar.gz --strip-components=1 --directory /data/app/$1/upgrade/

#升级包目录权限
chmod -R 755 /data/app/$1/upgrade/

#执行升级,先CD到目录再执行，防止upgrade.sh里相对路径解析出错
cd /data/app/$1/upgrade/ && ./upgrade.sh 