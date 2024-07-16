#!/bin/bash
CURDIR=$(cd $(dirname $0) && pwd )
echo $CURDIR
cd $CURDIR

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
		echo "      U盘升级" >> /tmp/showlcdtxt
		echo "" >> /tmp/showlcdtxt
		echo " "$AREA"  "$VERSION >> /tmp/showlcdtxt
		echo "" >> /tmp/showlcdtxt
		echo "进度: "$step"%" >> /tmp/showlcdtxt
		
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
	updatetools 0 "程序异常，请检查U盘"
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

updatetools 10 "解压程序"

tar -zxvf package*.tar.gz

# 拷贝ko
#if [ ${VER_SYS} -eq 0 ]; then
#    echo "正式系统，替换ko"
#
#	tar -zxvf modules.tar.gz
#	rm -rf $libpath/modules
#	cp -rf modules $libpath
#	
#	if [ -f /tmp/lcdctrl ]; then
#		# 新系统U盘强制升级，先加载驱动
#		CURR_TAR=`ls /lib/modules | grep .ko` 
#		for i in ${CURR_TAR[@]}
#		do
#			insmod /lib/modules/$i
#		done
#	fi
#fi

# 先停止APP
updatetools 20 "停止程序"
echo "pkill All APPs!"
$APPTarPath/smiOS/bin/stop_app.sh

# 将喂狗时间调整到5分钟，防止关闭smiOS后立刻重启
wtd_set 300

rm -rf /home/sysadm/app_package/*
rm -rf /backup/app_back/*

# 停止容器
echo "------------- Remove All Containers -------------"
dks=`docker ps -a --format "{{.Names}}"`
for dk in $dks
do 
	container stop $dk
	container uninstall $dk
done

updatetools 30 "停止程序"

echo "------------- Kill All APPs -------------"
for APPNAME in ${APPLIST[*]}
do
	PID=$(ps |grep $APPNAME |grep -v grep|grep -v '.sh'|awk '{print $1}')
	if [ ! $PID ]; then
		echo "no $APPNAME!"
	else
		echo "kill $APPNAME!"
		kill -9 $PID
	fi
done

PID=$(ps |grep secReinf |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	echo "no secReinf!"
else
	echo "kill secReinf!"
	kill -9 $PID
fi

echo "------------- update image -------------"
updatetools 40 "安装镜像"
if [ ${VER_SYS} -eq 1 ]; then
    cd $CURDIR/image
else
	cd $CURDIR/gwimage
fi
./setup_images.sh

updatetools 60 "拷贝软件"
# 拷贝APP到emmc中
cd $CURDIR
chmod 755 *.tar
rm -rf $APPTarPath/*
mkdir -p $APPTarPath
for APPNAME in ${APPLIST[*]}
do
	cp -fr $APPNAME"".tar $APPTarPath/
done
chmod 777 -R $APPTarPath

rm -rf $secReinfPath/*
mkdir -p $secReinfPath
if [ ${VER_SYS} -eq 0 ]; then
	echo "正式系统使用正式安全加固SecReinf"
    cp -fr SecReinf_lic.tar $secReinfPath/SecReinf.tar
fi
if [ ${VER_SYS} -eq 1 ]; then
    cp SecReinf.tar $secReinfPath/
	cp SecReinf_lic.tar $secReinfPath/
	cp -fr SecReinf_lic.tar $OSBackPath/
	cp -fr SecReinf.tar $OSBackPath/
	cp -fr script/applist.xml $OSBackPath/
fi
chmod 755 -R $secReinfPath/
cd $secReinfPath
tar -xvf SecReinf.tar
chmod 755 -R SecReinf

# 解压远程升级脚本
tar -xvf $APPTarPath/sysUpdate.tar -C $APPTarPath/
chmod 755 -R $APPTarPath/sysUpdate/

updatetools 80 "拷贝软件"
# 拷贝hal
cd $CURDIR
tar -zxvf hal_lib.tar.gz
rm -rf $libpath/hal_lib
cp -rf hal_lib $libpath

#拷贝工具集
if [ ${VER_SYS} -eq 0 ]; then
	cp -rf docker_tools/* /usr/bin
	chmod 755 /usr/bin/appm
	chmod 755 /usr/bin/appm_docker
	chmod 755 /usr/bin/ResMonitor
	chmod 755 /usr/bin/container 
fi
# 拷贝脚本
cp wtd_set /usr/bin/
chmod 755 /usr/bin/wtd_set

#cp version /usr/bin/
#chmod 755 /usr/bin/version

cp esam_no /usr/bin/
chmod 755 /usr/bin/esam_no

if [ ${VER_SYS} -eq 1 ]; then
    cp update /usr/bin/
	chmod 755 /usr/bin/update
fi

if [ ${VER_SYS} -eq 1 ]; then
    echo "普通系统，替换hook.sh"
    cp script/hook.sh /etc/
    chmod 755 /etc/hook.sh
fi
if [ ${VER_SYS} -eq 0 ]; then
    echo "正式系统，替换hook_1.sh"
    cp script/hook_1.sh /etc/hook.sh
    chmod 755 /etc/hook.sh
fi

cp script/version.sh /etc/version.sh
chmod 755 /etc/version.sh

cp -fr script/applist.xml $secReinfPath/SecReinf/cfg/
cp -fr script/applist.xml /backup/
chmod 755 -R $secReinfPath/SecReinf/cfg/

cp script/os-version.yaml /etc/
chmod 755 /etc/os-version.yaml
cp script/os-version_1.1.yaml /backup/
chmod 755 /backup/os-version_1.1.yaml
cp script/os-version_1.2.yaml /backup/
chmod 755 /backup/os-version_1.2.yaml

rm -rf /data/app/ccoRouter/cfg/ccoRouter.ini
rm -rf /data/app/uartManager/cfg/uartManager.ini
mkdir -p /data/app/uartManager/cfg/
cp -rf $CURDIR/uartManager.ini /data/app/uartManager/cfg/
#拷一下数据中心配置文件
mkdir -p /data/app/dbCenter/cfg
cp -fr $CURDIR/script/dbCenterData.cfg /data/app/dbCenter/cfg
chmod 755 /data/app/dbCenter/cfg/dbCenterData.cfg
#拷贝其他(正式系统需要去掉)
#新增 不区分系统 都考一下
if [ ${VER_SYS} -eq 1 ]; then
    cp -rf $CURDIR/*.img $OSBackPath/
fi
if [ ${VER_SYS} -eq 0 ]; then
    #rm $OSBackPath/*.img
	cp -rf $CURDIR/*.img $OSBackPath/
fi

# 删除旧参数
rm -rf /data/app/usbManager/*
rm -rf /data/app/smiOS/data/appRegisterList
rm -rf /data/app/scsMonitor/cfg/*

cp $CURDIR/lcdctrl /usr/bin/
chmod 755 /usr/bin/lcdctrl
cp $CURDIR/liccheck /usr/bin/
chmod 755 /usr/bin/liccheck

# 删除安全加固容器结束
rm -rf /etc/init_finsh

mkdir -p /data/devinfo
# 文件系统同步
sync;sync

updatetools 90 "安装结束"
LICSTATUS=0
ESAM_NO=""
if [ ${LICSTATUS} -eq 0 ]; then
    # 没有许可文件下发许可
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
		updatetools 95 "安装许可文件"
		
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
		
		# 文件系统同步
		sync;sync
	fi
fi

# 检查是否有许可
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
	echo "许可缺失"
    if [ -f /tmp/lcdctrl -a ${VER_SYS} -eq 0 ]; then
		updatetools 95 "许可缺失" "$ESAM_NO"
		wtd_set 300
		sleep 2s
		exit 0
	fi
fi

# 开狗
wtd_set 10

updatetools 100 "升级成功  即将重启"
sleep 1

echo "安装完毕，准备reboot!!!"

if [ "$1" = "noReboot" ];then
    echo "noReboot"
else
    echo "reboot"
    reboot
    sleep 5
    #防止reboot失败，加参数重启
    echo "reboot失败 执行reboot -nf"
    reboot -nf
fi
