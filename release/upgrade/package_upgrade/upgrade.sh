#!/bin/bash
CURDIR=$(cd $(dirname $0) && pwd )

#扫描目录找到升级文件
upgrade=`ls *.tar`

rm -rf /home/sysadm/app_package/*
rm -rf /backup/app_back/*

#将升级程序拷到/data/app/backuptmp/目录下
mkdir -p /data/app/backuptmp
rm -rf /data/app/backuptmp/*
cp -rf $CURDIR/* /data/app/backuptmp/

bSEC=0
echo "upgrade md5 check " > $CURDIR/md5check
#执行安装
for file in ${upgrade[*]}
do
    filename=${file%.*}
    echo "upgrade install for $filename"
	TARMD5=`md5sum $CURDIR/$filename.tar | awk '{ print $1}'`
	TARMD5=$(echo $TARMD5 | tr "[:lower:]" "[:upper:]")
	echo "$filename md5sum $TARMD5" >> $CURDIR/md5check
	MD5=$(cat $CURDIR/$filename.md5)
	MD5=$(echo $MD5 | tr "[:lower:]" "[:upper:]")
	echo "$filename filemd5 $MD5" >> $CURDIR/md5check
	
	if [ "$TARMD5" != "$MD5" ]; then
		echo "md5check error" >> $CURDIR/md5check
		continue
	fi
	echo "md5check ok" >> $CURDIR/md5check

	if [ "$file" == "SecReinf.tar" ]; then
		cp -rf /usr/sec-app/package/work/SecReinf/cfg/applist.xml /usr/sec-app/package/work/
		cp -rf $file /usr/sec-app/package/work/
		
		PID=$(ps |grep secReinf |grep -v grep|grep -v '.sh'|awk '{print $1}')
		if [ ! $PID ]; then
			echo "no secReinf!"
		else
			echo "kill secReinf!"
			kill -9 $PID
		fi
		cd /usr/sec-app/package/work/
		tar -xvf SecReinf.tar
		cp -rf /usr/sec-app/package/work/applist.xml /usr/sec-app/package/work/SecReinf/cfg/
		chmod 755 -R SecReinf
		rm /usr/sec-app/package/work/applist.xml
		bSEC=1
		cd $CURDIR
	else
		if [ -f /usr/local/extapps/$filename/bin/stop_app.sh ]; then
			/usr/local/extapps/$filename/bin/stop_app.sh
		fi
		tar -xvf $file --directory /usr/local/extapps/ && mv $file /usr/local/extapps/
		
		chmod 755 -R /usr/local/extapps/$filename
		sh /usr/local/extapps/$filename/bin/startup_app.sh
		if [ "$file" == "smiOS.tar" ]; then
			bSEC=1
		fi
	fi
done
#检查是否有hal更新
halfile=hal_lib.tar.gz
if [ -f "$halfile" ]; then
	TARMD5=`md5sum $CURDIR/hal_lib.tar.gz | awk '{ print $1}'`
	TARMD5=$(echo $TARMD5 | tr "[:lower:]" "[:upper:]")
	echo "$halfile md5sum $TARMD5" >> $CURDIR/md5check
	MD5=$(cat $CURDIR/hal.md5)
	MD5=$(echo $MD5 | tr "[:lower:]" "[:upper:]")
	echo "$halfile filemd5 $MD5" >> $CURDIR/md5check
	if [ "$TARMD5" != "$MD5" ]; then
		echo "hal md5check error" >> $CURDIR/md5check
	else
		echo "hal md5check ok" >> $CURDIR/md5check
		tar -zxvf hal_lib.tar.gz
		rm -rf /lib/hal_lib
		cp -rf hal_lib /lib
		bSEC=1
	fi	
fi
VERSIONSYS=$(cat /proc/version)
echo $VERSIONSYS

VER_SYS=2
VER_OS="+++OS_RK3358"
result=$(echo $VERSIONSYS | grep "${VER_OS}")
if [[ "$result" != "" ]]
then
	echo "普通系统"
	VER_SYS=1
else
    echo "正式系统"
    VER_SYS=0
fi
#检查是否有工具集更新
if [ ${VER_SYS} -eq 0 ]; then
	dockertools=docker_tools.tar.gz
	if [ -f "$dockertools" ]; then
		TARMD5=`md5sum $CURDIR/docker_tools.tar.gz | awk '{ print $1}'`
		TARMD5=$(echo $TARMD5 | tr "[:lower:]" "[:upper:]")
		echo "$dockertools md5sum $TARMD5" >> $CURDIR/md5check
		MD5=$(cat $CURDIR/docker_tools.md5)
		MD5=$(echo $MD5 | tr "[:lower:]" "[:upper:]")
		echo "$dockertools filemd5 $MD5" >> $CURDIR/md5check
		if [ "$TARMD5" != "$MD5" ]; then
			echo "docker_tools md5check error" >> $CURDIR/md5check
		else
			echo "docker_tools md5check ok" >> $CURDIR/md5check
			tar -zxvf docker_tools.tar.gz
			cp -rf docker_tools/* /usr/bin
			chmod 755 /usr/bin/appm
			chmod 755 /usr/bin/appm_docker
			chmod 755 /usr/bin/ResMonitor
			chmod 755 /usr/bin/container 
			bSEC=1
		fi	
	fi
fi

#增加升级数据中心配置文件的功能
dbCentercfg=dbCenterData.cfg
if [ -f "$dbCentercfg" ]; then
	TARMD5=`md5sum $CURDIR/dbCenterData.cfg | awk '{ print $1}'`
	TARMD5=$(echo $TARMD5 | tr "[:lower:]" "[:upper:]")
	echo "$dbCentercfg md5sum $TARMD5" >> $CURDIR/md5check
	MD5=$(cat $CURDIR/dbcfg.md5)
	MD5=$(echo $MD5 | tr "[:lower:]" "[:upper:]")
	echo "$dbCentercfg filemd5 $MD5" >> $CURDIR/md5check
	if [ "$TARMD5" != "$MD5" ]; then
		echo "dbCentercfg md5check error" >> $CURDIR/md5check
	else
		echo "dbCentercfg md5check ok" >> $CURDIR/md5check
		cp -rf dbCenterData.cfg /data/app/dbCenter/cfg/
		bSEC=1
	fi	
fi

#检查是否有U盘无法二次识别的bug修复文件
udiskupdate=udisk_update.tar.gz
if [ -f "$udiskupdate" ]; then
	TARMD5=`md5sum $CURDIR/udisk_update.tar.gz | awk '{ print $1}'`
	TARMD5=$(echo $TARMD5 | tr "[:lower:]" "[:upper:]")
	echo "$udiskupdate md5sum $TARMD5" >> $CURDIR/md5check
	MD5=$(cat $CURDIR/udiskupdate.md5)
	MD5=$(echo $MD5 | tr "[:lower:]" "[:upper:]")
	echo "$udiskupdate filemd5 $MD5" >> $CURDIR/md5check
	if [ "$TARMD5" != "$MD5" ]; then
		echo "udiskupdate md5check error" >> $CURDIR/md5check
	else
		echo "udiskupdate md5check ok" >> $CURDIR/md5check
		tar -zxvf udisk_update.tar.gz
		cp -fr udisk_update/010-usb.rules /etc/udev/rules.d/
		cp -fr udisk_update/usb_udev.sh /etc/udev/rules.d/
		chattr -i /usr/lib/systemd/system/systemd-udevd.service
		chmod 777 /usr/lib/systemd/system/systemd-udevd.service
		cp -fr udisk_update/systemd-udevd.service /usr/lib/systemd/system/
		chmod 755 /etc/udev/rules.d/*
		chmod 644 /usr/lib/systemd/system/systemd-udevd.service
		chattr +i /usr/lib/systemd/system/systemd-udevd.service
		bSEC=1
	fi	
fi

#增加升级daemon.json功能
daemonjson=daemon.json
if [ -f "$daemonjson" ]; then
	TARMD5=`md5sum $CURDIR/daemon.json | awk '{ print $1}'`
	TARMD5=$(echo $TARMD5 | tr "[:lower:]" "[:upper:]")
	echo "$daemonjson md5sum $TARMD5" >> $CURDIR/md5check
	MD5=$(cat $CURDIR/daemonjson.md5)
	MD5=$(echo $MD5 | tr "[:lower:]" "[:upper:]")
	echo "$daemonjson filemd5 $MD5" >> $CURDIR/md5check
	if [ "$TARMD5" != "$MD5" ]; then
		echo "daemonjson md5check error" >> $CURDIR/md5check
	else
		echo "daemonjson md5check ok" >> $CURDIR/md5check
		cp -rf daemon.json /etc/docker/
		bSEC=1
	fi	
fi

#增加拷贝ccoRouter配置文件功能
paramamr=paramamr.xin
if [ -f "$paramamr" ]; then
	TARMD5=`md5sum $CURDIR/paramamr.xin | awk '{ print $1}'`
	TARMD5=$(echo $TARMD5 | tr "[:lower:]" "[:upper:]")
	echo "$paramamr md5sum $TARMD5" >> $CURDIR/md5check
	MD5=$(cat $CURDIR/paramamr.md5)
	MD5=$(echo $MD5 | tr "[:lower:]" "[:upper:]")
	echo "$paramamr filemd5 $MD5" >> $CURDIR/md5check
	if [ "$TARMD5" != "$MD5" ]; then
		echo "paramamr md5check error" >> $CURDIR/md5check
	else
		echo "paramamr md5check ok" >> $CURDIR/md5check
		cp -rf paramamr.xin /data/app/ccoRouter/commFile/impdata/param/
		bSEC=1
	fi	
fi

#增加升级stAmr配置文件的功能
stAmrini=stAmr.ini
if [ -f "$stAmrini" ]; then
	TARMD5=`md5sum $CURDIR/stAmr.ini | awk '{ print $1}'`
	TARMD5=$(echo $TARMD5 | tr "[:lower:]" "[:upper:]")
	echo "$stAmrini md5sum $TARMD5" >> $CURDIR/md5check
	MD5=$(cat $CURDIR/stAmrini.md5)
	MD5=$(echo $MD5 | tr "[:lower:]" "[:upper:]")
	echo "$stAmrini filemd5 $MD5" >> $CURDIR/md5check
	if [ "$TARMD5" != "$MD5" ]; then
		echo "stAmrini md5check error" >> $CURDIR/md5check
	else
		echo "stAmrini md5check ok" >> $CURDIR/md5check
		cp -rf stAmr.ini /usr/local/extapps/stAmr/bin/
		bSEC=1
	fi	
fi


cp STM32F4x_MODULE_APP_JC.bin  /data/app/acMeter/jc_update_file
sleep 4
cp jc_update_file_ic  /data/app/acMeter/
cp -fr *.img /backup
if [ ${VER_SYS} -eq 1 ]; then
    echo "普通系统，替换hook.sh"
    cp $CURDIR/hook.sh /etc/
    chmod 755 /etc/hook.sh
fi
if [ ${VER_SYS} -eq 0 ]; then
    echo "正式系统，替换hook_1.sh"
    cp $CURDIR/hook_1.sh /etc/hook.sh
    chmod 755 /etc/hook.sh
	cp -fr $CURDIR/boot.sh /etc/
	chmod 755 /etc/boot.sh
fi
#写个升级标记
echo "2" > /data/app/backuptmp/updateflag

sync;sync
if [ "$bSEC" == "1" ]; then
	reboot
fi
