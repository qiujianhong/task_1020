#!/bin/bash

echo "------------- Remove All Containers -------------"
dks=`docker ps -a --format "{{.Names}}"`
for dk in $dks
do 
	container stop $dk
	container uninstall $dk
done

# 删除安全加固容器结束
rm -rf /etc/init_finsh

# 文件系统同步
sync;sync

reboot
sleep 5
#防止reboot失败，加参数重启
echo "reboot失败 执行reboot -nf"
reboot -nf