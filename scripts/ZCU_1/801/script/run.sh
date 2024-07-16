#!/bin/bash
killall dhcpcd

rm -rf /etc/GDK.cfg
sync;sync

#MACͬ��
hwInit FE0 >/dev/null 2>&1 &

APPTarPath=/usr/local/extapps
APPLIST=(smiOS dbCenter desktopGui acMeter rspSample btManager radioManager scsMonitor uartManager wirelessDCM taskManager stAmr)

mkdir /tmp/data
chmod 755 -R /tmp/data
mkdir /tmp/dev
chmod 755 -R /tmp/dev

# �Ȱѹ����ó�180s ��ֹsmiOSû����
wtd_set 180 >/dev/null 2>&1 &

cd $APPTarPath
for APPNAME in ${APPLIST[*]}
do
	./$APPNAME/bin/startup_app.sh
done

# ӳ������
sleep 2s
ln -s /dev/ttyS7 /dev/ttyBT
