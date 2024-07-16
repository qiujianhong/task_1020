#!/bin/bash
killall dhcpcd

mkdir -p /tmp/data
chmod 755 -R /tmp/data
mkdir -p /tmp/dev
chmod 755 -R /tmp/dev

mkdir -p /mnt/ipc_socket
chmod 755 -R /mnt/ipc_socket

mkdir -p /tmp/ipc_socket
chmod 755 -R /tmp/ipc_socket

if [ ! -f "/backup/lic.lic" ]; then
	# 没有许可，设置临时许可，hal执行跳过
	touch /tmp/dev/not_verify_sign
	/usr/bin/temp_lic.sh
elif [ -z $(cat /backup/lic.lic) ]; then
	# 没有许可，设置临时许可，hal执行跳过
	touch /tmp/dev/not_verify_sign
	/usr/bin/temp_lic.sh
fi

# set wtdog 180s
wtd_set 300 >/dev/null 2>&1 &

mkdir -p /usr/sec-app/package/work/SecReinf/cfg
cp /backup/applist.xml /usr/sec-app/package/work/SecReinf/cfg/
sync
/usr/sec-app/package/work/SecReinf/bin/startup_app.sh >/dev/null 2>&1 &

liccheck >/dev/null 2>&1 &

# 映射蓝牙
sleep 2s
ln -s /dev/ttyS7 /dev/ttyBT

