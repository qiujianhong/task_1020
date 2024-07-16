#!/bin/bash
/etc/boot.sh
killall dhcpcd

rm -fr /dev/ttyS*

mknod /dev/ttyS0 c 4 66
mknod /dev/ttyS1 c 4 67
mknod /dev/ttyS2 c 4 64
mknod /dev/ttyS3 c 4 69
mknod /dev/ttyS4 c 4 65
mknod /dev/ttyS5 c 245 0
mknod /dev/ttyS6 c 4 68

ln -s /dev/s3c2410_ym /dev/ym
ln -s /dev/s3c2410_kbd /dev/key

rm -rf /etc/GDK.cfg
sync;sync

ifconfig FE0:0 172.17.0.1 netmask 255.255.255.0 up
sleep 5
PID=$(ps |grep mosquitto |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	#mosquitto -c /etc/mosquitto/mosquitto.conf >/dev/null 2>&1 &
	/etc/init.d/S50mosquitto start
fi
sleep 1

# set wtdog 120s
# wtd_set 120 >/dev/null 2>&1 &

#MAC同步
hwInit FE0 >/dev/null 2>&1 &

APPTarPath=/usr/local/extapps
APPLIST=(smiOS dbCenter desktopGui acMeter rspSample btManager usbManager scsMonitor uartManager wirelessDCM taskManager stAmr)

mkdir /tmp/data
chmod 755 -R /tmp/data
mkdir /tmp/dev
chmod 755 -R /tmp/dev

# 先把狗设置成30s 防止smiOS没起来
wtd_set 30 >/dev/null 2>&1 &

cd $APPTarPath
for APPNAME in ${APPLIST[*]}
do
	./$APPNAME/bin/startup_app.sh
done
