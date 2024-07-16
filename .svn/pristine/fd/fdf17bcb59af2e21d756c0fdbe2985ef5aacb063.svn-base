#!/bin/bash
cd /lib
if [ ! -L libmosquitto.so.1 ]; then
	ln -s libmosquitto.so.1.6.9 libmosquitto.so.1
fi

if [ ! -L libmosquitto.so ]; then
	ln -s libmosquitto.so.1 libmosquitto.so
fi

ifconfig eth1 10.10.0.66 up

### mount nand/emmc partions ###
FIND_FILE="/proc/mounts"
FIND_STR="mmcblk1p1"

if test -e "/dev/mmcblk1p1"; then
/bin/mount  -t ext4 -o noatime -o nodiratime /dev/mmcblk1p1 /data
if [ `grep -c "$FIND_STR" $FIND_FILE` -ne '0' ];then
    echo "The File Has part1!"
else
    echo "no part1"
    fsck.ext4 -y /dev/mmcblk1p1
    /bin/mount  -t ext4 -o noatime -o nodiratime /dev/mmcblk1p1 /data
fi

echo 3 > /proc/sys/vm/drop_caches

fi

#rm -fr /dev/ttyS*

#mknod /dev/ttyS0 c 4 66
#mknod /dev/ttyS1 c 4 67
#mknod /dev/ttyS2 c 4 64
#mknod /dev/ttyS3 c 4 69
#mknod /dev/ttyS4 c 4 65
#mknod /dev/ttyS5 c 245 0
#mknod /dev/ttyS6 c 4 68

ln -s /dev/s3c2410_ym /dev/ym
ln -s /dev/s3c2410_kbd /dev/key

sync;sync
sleep 5
mosquitto -c /etc/mosquitto/mosquitto.conf >/dev/null 2>&1 &
sleep 1

APPTarPath=/usr/local/extapps
APPLIST=(smiOS wirelessDCM dbCenter ccoRouter desktopGui acMeter rspSample btManager uartManager usbManager scsMonitor iotManager taskManager puAmr)

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
