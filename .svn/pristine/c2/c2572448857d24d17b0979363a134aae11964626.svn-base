#!/bin/bash
killall dhcpcd

rm -rf /etc/GDK.cfg
sync;sync

#MAC同步
hwInit FE0 >/dev/null 2>&1 &

APPTarPath=/usr/local/extapps
APPLIST=(smiOS dbCenter desktopGui acMeter rspSample btManager scsMonitor uartManager wirelessDCM taskManager stAmr)

mkdir /tmp/data
chmod 755 -R /tmp/data
mkdir /tmp/dev
chmod 755 -R /tmp/dev

if [ ! -f "/backup/lic.lic" ]; then
	# 没有许可，设置临时许可，hal执行跳过
	touch /tmp/dev/not_verify_sign
	/usr/bin/temp_lic.sh
elif [ -z $(cat /backup/lic.lic) ]; then
	# 没有许可，设置临时许可，hal执行跳过
	touch /tmp/dev/not_verify_sign
	/usr/bin/temp_lic.sh
fi

# 先把狗设置成180s 防止smiOS没起来
wtd_set 180 >/dev/null 2>&1 &

mkdir -p /usr/sec-app/package/work/SecReinf/cfg
cp /backup/applist.xml /usr/sec-app/package/work/SecReinf/cfg/
sync
/usr/sec-app/package/work/SecReinf/bin/startup_app.sh >/dev/null 2>&1 &

#cd $APPTarPath
#for APPNAME in ${APPLIST[*]}
#do
#	./$APPNAME/bin/startup_app.sh
#done

# 映射蓝牙
sleep 2s
ln -s /dev/ttyS7 /dev/ttyBT
