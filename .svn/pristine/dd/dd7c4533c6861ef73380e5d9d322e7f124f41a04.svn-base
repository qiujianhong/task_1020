#!/bin/bash

/etc/boot.sh

killall dhcpcd

rm -fr /dev/ttyS*

mknod /dev/ttyS0 c 4 66
mknod /dev/ttyS1 c 4 67
mknod /dev/ttyS2 c 4 64
mknod /dev/ttyS3 c 4 69
mknod /dev/ttyS4 c 4 65
mknod /dev/ttyS5 c 241 0
mknod /dev/ttyS6 c 4 68

ln -s /dev/s3c2410_ym /dev/ym
ln -s /dev/s3c2410_kbd /dev/key

rm -rf /etc/GDK.cfg

cp -rf /backup/uboot.img /tmp/
chmod 755 /tmp/uboot.img
cp -rf /backup/boot.img /tmp/
chmod 755 /tmp/boot.img

sync;sync
sleep 5
PID=$(ps |grep mosquitto |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	#mosquitto -c /etc/mosquitto/mosquitto.conf >/dev/null 2>&1 &
	/etc/init.d/S50mosquitto start
fi
sleep 1

CURR_TAR=`ls /lib/modules | grep .ko` 

for i in ${CURR_TAR[@]}
do
    insmod /lib/modules/$i
done

mkdir -p /tmp/data
chmod 755 -R /tmp/data
mkdir -p /tmp/dev
chmod 755 -R /tmp/dev

mkdir -p /mnt/ipc_socket
chmod 755 -R /mnt/ipc_socket

mkdir -p /tmp/ipc_socket
chmod 755 -R /tmp/ipc_socket

echo 1080000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
echo 1080000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_max_freq
echo 1080000 > /sys/devices/system/cpu/cpu2/cpufreq/scaling_max_freq
echo 1080000 > /sys/devices/system/cpu/cpu3/cpufreq/scaling_max_freq
# set wtdog 300s
wtd_set 300 >/dev/null 2>&1 &

/etc/version.sh

cp -rf /backup/applist.xml /usr/sec-app/package/work/SecReinf/cfg/
/usr/sec-app/package/work/SecReinf/bin/startup_app.sh

sleep 5
# 启动容器服务镜像
ResMonitor >/dev/null 2>&1 &
