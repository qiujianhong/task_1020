#!/bin/bash
killall dhcpcd

rm -rf /etc/GDK.cfg
sync;sync

#MACͬ��
hwInit FE0 >/dev/null 2>&1 &

APPTarPath=/usr/local/extapps
APPLIST=(smiOS dbCenter desktopGui acMeter rspSample btManager scsMonitor uartManager wirelessDCM taskManager stAmr)

mkdir /tmp/data
chmod 755 -R /tmp/data
mkdir /tmp/dev
chmod 755 -R /tmp/dev

if [ ! -f "/backup/lic.lic" ]; then
	# û����ɣ�������ʱ��ɣ�halִ������
	touch /tmp/dev/not_verify_sign
	/usr/bin/temp_lic.sh
elif [ -z $(cat /backup/lic.lic) ]; then
	# û����ɣ�������ʱ��ɣ�halִ������
	touch /tmp/dev/not_verify_sign
	/usr/bin/temp_lic.sh
fi

# �Ȱѹ����ó�180s ��ֹsmiOSû����
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

# ӳ������
sleep 2s
ln -s /dev/ttyS7 /dev/ttyBT
