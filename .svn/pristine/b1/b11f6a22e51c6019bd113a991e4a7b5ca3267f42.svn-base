#!/bin/bash
cd /lib
if [ ! -L libmosquitto.so.1 ]; then
	ln -s libmosquitto.so.1.6.9 libmosquitto.so.1
fi

if [ ! -L libmosquitto.so ]; then
	ln -s libmosquitto.so.1 libmosquitto.so
fi

ifconfig eth0:0 172.17.0.1 up

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

sync;sync

# /etc/profile not work in start
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/qt5.12/lib:/usr/lib/qt5.12/plugins/platforms:/usr/lib/qt5.12/plugins/platforminputcontexts:/usr/lib/tslib/lib
export LANG=en_US.UTF-8
export T_ROOT=/usr/lib/tslib
export TSLIB_CONSOLEDEVICE=none
export TSLIB_FBDEVICE=/dev/fb0
export TSLIB_TSDEVICE=/dev/input/event0
export TSLIB_PLUGINDIR=$T_ROOT/lib/ts
export TSLIB_CONFFILE=$T_ROOT/etc/ts.conf

export QTEDIR=/usr/lib/qt5.12
export QT_QPA_GENERIC_PLUGINS=tslib:/dev/input/event0
export QT_QPA_FONTDIR=/usr/share/fonts
export QT_QWS_FONTDIR=/usr/share/fonts
export QT_QPA_PLATFORM_PLUGIN_PATH=$QTEDIR/plugins 
export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb0:size=1024x600:mmSize=1024x600:offset=0x0:tty=/dev/tty1
export QT_QPA_FB_TSLIB=1

sleep 1
mosquitto -c /etc/mosquitto/mosquitto.conf >/dev/null 2>&1 &
sleep 1

APPTarPath=/usr/local/extapps
APPLIST=(smiOS dbCenter desktopGui wirelessDCM acMeter rspSample uartManager btManager usbManager taskManager lcMonitor stAmr)

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
