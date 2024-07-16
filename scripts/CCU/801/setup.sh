#!/bin/bash
CURDIR=$(cd $(dirname $0) && pwd )
echo $CURDIR
cd $CURDIR


updatetools(){
	step="$1"
	Info="$2"
	Other="$3"
	
	if [ -f /tmp/lcdctrl ]; then
		AREA_T=$(cat $CURDIR/area.txt)
		VERSION_T=$(cat $CURDIR/version.txt)
		if [ ! -z "$AREA_T" ]; then
			AREA=$AREA_T
		fi
		if [ ! -z "$VERSION_T" ]; then
			VERSION=$VERSION_T
		fi
	
		echo "" > /tmp/showlcdtxt
		echo "      U������" >> /tmp/showlcdtxt
		echo "" >> /tmp/showlcdtxt
		echo " "$AREA"  "$VERSION >> /tmp/showlcdtxt
		echo "" >> /tmp/showlcdtxt
		echo "����: "$step"%" >> /tmp/showlcdtxt
		
		num=$(($step / 5))
		stepshow=
		for i in `seq 1 20`; do
			if [ $i -le $num ]; then
				stepshow=$stepshow"-"
			fi
		done
		echo $stepshow >> /tmp/showlcdtxt
		echo "" >> /tmp/showlcdtxt
		infilen=${#Info}
		spacepass="                    "
		if [ $infilen -ge 20 ]; then
			echo $infilen >> /tmp/showlcdtxt
		else
			spacelen=$((20 - $infilen))
			spacelen=$(($spacelen / 2))
			echo "${spacepass:0:$spacelen}"$Info >> /tmp/showlcdtxt
		fi
		echo " "$Other >> /tmp/showlcdtxt
		/tmp/lcdctrl 160 160 0 -f /tmp/showlcdtxt
		/tmp/lcdctrl 160 160 0 -f /tmp/showlcdtxt
		sleep 1
	fi
}

TARNAME=$(cat md5.txt |grep "package_"|grep ".tar.gz")
MD5=$(cat md5.txt |grep -v "package_")
MD5=$(echo $MD5 | tr "[:lower:]" "[:upper:]")

TARMD5=`md5sum $TARNAME | awk '{ print $1}'`
TARMD5=$(echo $TARMD5 | tr "[:lower:]" "[:upper:]")

if [ "$TARMD5" != "$MD5" ]; then
	updatetools 0 "�����쳣������U��"
	echo "error APPs, please check udisk"
	sleep 2s
	exit 0
fi

OSBackPath=/backup
secReinfPath=/usr/sec-app/package/work
APPTarPath=/usr/local/extapps
libpath=/lib
datapath=/data/app
APPLIST=(smiOS wirelessDCM dbCenter ccoRouter desktopGui acMeter rspSample btManager uartManager usbManager scsMonitor iotManager taskManager puAmr pdsManager sysUpdate cpuMonitor)

updatetools 10 "��ѹ����"

tar -zxvf package*.tar.gz

# ��ֹͣAPP
updatetools 20 "ֹͣ����"
echo "pkill All APPs!"
$APPTarPath/smiOS/bin/stop_app.sh

# ��ι��ʱ�������5���ӣ���ֹ�ر�smiOS����������
wtd_set 300

rm -rf /home/sysadm/app_package/*
rm -rf /backup/app_back/*

# ֹͣ����
echo "------------- Remove All Containers -------------"
dks=`docker ps -a --format "{{.Names}}"`
for dk in $dks
do 
	container stop $dk
	container uninstall $dk
done

updatetools 30 "ֹͣ����"

echo "------------- Kill All APPs -------------"
for APPNAME in ${APPLIST[*]}
do
	PID=$(ps aux|grep $APPNAME |grep -v grep|grep -v '.sh'|awk '{print $1}')
	if [ ! $PID ]; then
		echo "no $APPNAME!"
	else
		echo "kill $APPNAME!"
		kill -9 $PID
	fi
done

PID=$(ps aux|grep secReinf |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	echo "no secReinf!"
else
	echo "kill secReinf!"
	kill -9 $PID
fi

updatetools 50 "�������"
# ����APP��emmc��
cd $CURDIR
chmod 755 *.tar
rm -rf $APPTarPath/*
mkdir -p $APPTarPath
for APPNAME in ${APPLIST[*]}
do
	cp -fr $APPNAME"".tar $APPTarPath/
done
chmod 777 -R $APPTarPath

cp -fr script/applist.xml $OSBackPath/

# ��ѹԶ�������ű�
tar -xvf $APPTarPath/sysUpdate.tar -C $APPTarPath/
chmod 755 -R $APPTarPath/sysUpdate/

updatetools 80 "�������"
# ����hal
cd $CURDIR
tar -zxvf hal_lib.tar.gz
rm -rf $libpath/hal_lib
cp -rf hal_lib $libpath

# �����ű�
cp wtd_set /usr/bin/
chmod 755 /usr/bin/wtd_set

cp esam_no /usr/bin/
chmod 755 /usr/bin/esam_no

cp script/run.sh /usr/bin/
chmod 755 /usr/bin/run.sh
cp script/temp_lic.sh /usr/bin/
chmod 755 /usr/bin/temp_lic.sh

mkdir -p $secReinfPath/SecReinf/cfg
cp -fr script/applist.xml $secReinfPath/SecReinf/cfg/
cp -fr script/applist.xml /backup/
chmod 755 -R $secReinfPath/SecReinf/cfg/

# �����ļ�
mkdir -p /data/app/wirelessDCM/data
cp -rf $CURDIR/script/default_conf.ini /data/app/wirelessDCM/data/
chmod 755 /data/app/wirelessDCM/data/default_conf.ini

rm -rf /data/app/ccoRouter/cfg/ccoRouter.ini
rm -rf /data/app/uartManager/cfg/uartManager.ini
mkdir -p /data/app/uartManager/cfg/
cp -rf $CURDIR/uartManager.ini /data/app/uartManager/cfg/
#��һ���������������ļ�
mkdir -p /data/app/dbCenter/cfg
cp -fr $CURDIR/script/dbCenterData.cfg /data/app/dbCenter/cfg
chmod 755 /data/app/dbCenter/cfg/dbCenterData.cfg

# ɾ���ɲ���
rm -rf /data/app/usbManager/*
rm -rf /data/app/smiOS/data/appRegisterList
rm -rf /data/app/scsMonitor/cfg/*

cp $CURDIR/lcdctrl /usr/bin/
chmod 755 /usr/bin/lcdctrl
cp $CURDIR/liccheck /usr/bin/
chmod 755 /usr/bin/liccheck

# ɾ����ȫ�ӹ���������
rm -rf /etc/init_finsh

mkdir -p /data/devinfo
# �ļ�ϵͳͬ��
sync;sync

updatetools 90 "��װ����"
LICSTATUS=0
ESAM_NO=""
if [ ${LICSTATUS} -eq 0 ]; then
    # û������ļ��·����
	ESAM_NO=`esam_no`
	echo "ESAMNO: $ESAM_NO"
	ESAM_CHECK=$ESAM_NO","
	APPNAMES=`cat $CURDIR/applic.csv | grep 'APPANME'`
	APPNAMES=`echo $APPNAMES | tr ',' ' '`
	# echo $APPNAMES
	# echo ""
	
	APPNOS=`cat $CURDIR/applic.csv | grep 'APPNO'`
	APPNOS=`echo $APPNOS | tr ',' ' '`
	# echo $APPNOS
	
	APPLICS=`cat $CURDIR/applic.csv | grep ${ESAM_CHECK}`
	APPLICS=`echo $APPLICS | tr ',' ' '`
	# echo $APPLICS
	
	if [ "$APPLICS" != "" -a -n "$ESAM_NO" ]; then
		LICSTATUS=1
		updatetools 95 "��װ����ļ�"
		
		rm -rf /backup/*.lic
		rm -rf /data/devinfo/*.lic
		
		APPNAMELIST=($APPNAMES)
		APPNOLIST=($APPNOS)
		APPLICLIST=($APPLICS)
		
		for i in ${!APPNAMELIST[@]}
		do
			if [ "$i" != "0" ]; then
				echo "set ${APPNAMELIST[$i]} lic $LIC"
				esam_no ${APPNOLIST[$i]} ${APPNAMELIST[$i]} ${APPLICLIST[$i]}
			fi
		done
		
		# �ļ�ϵͳͬ��
		sync;sync
	fi
fi

# ����Ƿ������
Files=`ls /data/devinfo/`
if [ -n "$Files" ]; then
	FileList=($Files)
	for i in ${FileList[@]}
	do
		FileName=$i
		if [ "${FileName:0-4:4}" == ".lic" ]; then
			LICSTATUS=1
			echo "Has applics"
			break
		fi
	done
fi

if [ ${LICSTATUS} -eq 0 ]; then
	echo "���ȱʧ"
    updatetools 95 "���ȱʧ" "$ESAM_NO"
	wtd_set 300
	sleep 2s
fi

# ����
wtd_set 10

updatetools 100 "�����ɹ�  ��������"
sleep 1

echo "��װ��ϣ�׼��reboot!!!"

if [ "$1" = "noReboot" ];then
    echo "noReboot"
else
    echo "reboot"
    reboot
    sleep 5
    #��ֹrebootʧ�ܣ��Ӳ�������
    echo "rebootʧ�� ִ��reboot -nf"
    reboot -nf
fi
