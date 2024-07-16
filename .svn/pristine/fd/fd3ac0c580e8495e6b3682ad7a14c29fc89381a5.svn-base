#!/bin/bash
cd /usr/lib


ln -s /dev/s3c2410_kbd /dev/key

killall dhcpcd
rm -rf /etc/GDK.cfg
sync;sync
sleep 5
/etc/setnetconf.sh > /tmp/netset.log

#mosquitto -c /etc/mosquitto/mosquitto.conf >/dev/null 2>&1 &
sleep 1


mkdir -p /tmp/data
chmod 755 -R /tmp/data
mkdir -p /tmp/dev
chmod 755 -R /tmp/dev

mkdir -p /mnt/ipc_socket
chmod 755 -R /mnt/ipc_socket

mkdir -p /tmp/ipc_socket
chmod 755 -R /tmp/ipc_socket

# 创建utmp文件用于用户登陆和退出检测
if [ ! -f /var/run/utmp ]; then
	touch /var/run/utmp
	chmod 600 /var/run/utmp
fi

# 检查lic
lics=`ls /data/devinfo/*.lic`
if [ -n "$lics" ]; then
	if [ -f "/usr/sec-app/package/work/SecReinf_lic.tar" ]; then
		cp /usr/sec-app/package/work/SecReinf_lic.tar /usr/sec-app/package/work/SecReinf.tar
		rm /usr/sec-app/package/work/SecReinf_lic.tar
	fi
fi

# 先把狗设置成30s 防止smiOS没起来
wtd_set 300 >/dev/null 2>&1 &

rm -rf /usr/sec-app/package/work/SecReinf
cd /usr/sec-app/package/work
tar -xvf SecReinf.tar
chmod 755 -R /usr/sec-app/package/work/SecReinf
cp /backup/applist.xml /usr/sec-app/package/work/SecReinf/cfg/
sync

/usr/sec-app/package/work/SecReinf/bin/startup_app.sh

liccheck >/dev/null 2>&1 &
