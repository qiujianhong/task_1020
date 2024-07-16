#!/bin/bash
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
sleep 5
PID=$(ps |grep mosquitto |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	#mosquitto -c /etc/mosquitto/mosquitto.conf >/dev/null 2>&1 &
	/etc/init.d/S50mosquitto start
fi
sleep 1

mkdir -p /tmp/data
chmod 755 -R /tmp/data
mkdir -p /tmp/dev
chmod 755 -R /tmp/dev

mkdir -p /mnt/ipc_socket
chmod 755 -R /mnt/ipc_socket

mkdir -p /tmp/ipc_socket
chmod 755 -R /tmp/ipc_socket

# 检查lic
lics=`ls /data/devinfo/*.lic`
if [ -n "$lics" ]; then
	if [ -f "/usr/sec-app/package/work/SecReinf_lic.tar" ]; then
		cp /usr/sec-app/package/work/SecReinf_lic.tar /usr/sec-app/package/work/SecReinf.tar
		rm /usr/sec-app/package/work/SecReinf_lic.tar
	fi
fi

# set wtdog 120s
wtd_set 300 >/dev/null 2>&1 &

rm -rf /usr/sec-app/package/work/SecReinf
cd /usr/sec-app/package/work
tar -xvf SecReinf.tar
chmod 755 -R /usr/sec-app/package/work/SecReinf
cp /backup/applist.xml /usr/sec-app/package/work/SecReinf/cfg/
sync

/usr/sec-app/package/work/SecReinf/bin/startup_app.sh

liccheck >/dev/null 2>&1 &
