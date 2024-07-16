#!/bin/bash
killall dhcpcd

rm -rf /etc/GDK.cfg
sync;sync

#MAC同步
hwInit FE0 >/dev/null 2>&1 &

APPTarPath=/usr/local/extapps
APPLIST=(smiOS dbCenter desktopGui acMeter rspSample btManager radioManager scsMonitor uartManager wirelessDCM taskManager stAmr)

mkdir /tmp/data
chmod 755 -R /tmp/data
mkdir /tmp/dev
chmod 755 -R /tmp/dev

# 先把狗设置成180s 防止smiOS没起来
wtd_set 180 >/dev/null 2>&1 &

cd $APPTarPath
for APPNAME in ${APPLIST[*]}
do
	./$APPNAME/bin/startup_app.sh
done

# 映射蓝牙
sleep 2s
ln -s /dev/ttyS7 /dev/ttyBT
