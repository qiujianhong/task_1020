#!/bin/sh
echo "pkill wirelessDCM!"
pkill wirelessDCM

PATH=${PATH:+$PATH:}/usr/local/extapps/wirelessDCM/bin
export PATH


dataShardpath=/data/app
devShardPath=/dev
devTmpShardPath=/tmp/dev
appDataShardpath=/tmp/data
dataDevInfoShardpath=/data/devinfo
halpath=/lib/hal_lib

mkdir -p $dataShardpath/wirelessDCM
mkdir -p $dataDevInfoShardpath
mkdir -p $appDataShardpath
mkdir -p $devTmpShardPath


# 文件系统同步
sync;sync

chmod 755 /usr/local/extapps/wirelessDCM/bin/wirelessDCM

/usr/local/extapps/wirelessDCM/bin/wirelessDCM  >/dev/null  2>&1 &
echo "wirelessDCM program started!"