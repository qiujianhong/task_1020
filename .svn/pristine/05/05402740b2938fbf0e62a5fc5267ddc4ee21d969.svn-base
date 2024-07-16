#!/bin/bash
CURDIR=$(cd $(dirname $0) && pwd )
echo $CURDIR
cd $CURDIR

AREA=$(cat area.txt)
VERSION=$(cat version.txt)

updatetools(){
	step="$1"
	Info="$2"
	
	AREA_T=$(cat area.txt)
	VERSION_T=$(cat version.txt)
	if [ ! -z "$AREA_T" ]; then
		AREA=$AREA_T
	fi
	if [ ! -z "$VERSION_T" ]; then
		VERSION=$VERSION_T
	fi

	if [ -f /tmp/lcdctrl ]; then
		echo "" > /tmp/showlcdtxt
		echo "      U盘升级" >> /tmp/showlcdtxt
		echo "" >> /tmp/showlcdtxt
		echo " "$AREA"  "$VERSION >> /tmp/showlcdtxt
		echo "" >> /tmp/showlcdtxt
		echo "进度: "$step"%" >> /tmp/showlcdtxt
		
		num=$(($step / 4))
		stepshow=
		for i in `seq 1 25`; do
			if [ $i -le $num ]; then
				stepshow=$stepshow"-"
			fi
		done
		echo $stepshow >> /tmp/showlcdtxt
		echo "" >> /tmp/showlcdtxt
		infilen=${#Info}
		spacepass="                    "
		if [ $infilen -ge 30 ]; then
			echo $infilen >> /tmp/showlcdtxt
		else
			spacelen=$((30 - $infilen))
			spacelen=$(($spacelen / 2))
			echo "${spacepass:0:$spacelen}"$Info >> /tmp/showlcdtxt
		fi
		/tmp/lcdctrl 240 128 0 -f /tmp/showlcdtxt
		/tmp/lcdctrl 240 128 0 -f /tmp/showlcdtxt
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

APPTarPath=/usr/local/extapps
libpath=/lib
qtlibpath=/usr/lib
fontspath=/usr/share
datapath=/data/app
APPLIST=(smiOS dbCenter desktopGui acMeter rspSample radioManager scsMonitor uartManager wirelessDCM taskManager stAmr sysUpdate)

updatetools 10 "解压程序"

tar -zxvf package*.tar.gz

updatetools 40 "停止程序"
# 先停止APP
echo "pkill All APPs!"
$APPTarPath/smiOS/bin/stop_app.sh

# 将喂狗时间调整到2分钟，防止关闭smiOS后立刻重启
wtd_set 120

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

updatetools 70 "拷贝软件"

# 拷贝APP和脚本
cd $CURDIR

chmod 755 *.tar
rm -rf $APPTarPath/*
mkdir -p $APPTarPath
cp -fr *.tar $APPTarPath/

cp script/run.sh /usr/bin/
chmod 755 /usr/bin/run.sh

# 拷贝
tar -zxvf hal_lib.tar.gz
rm -rf $libpath/hal_lib
cp -rf hal_lib $libpath

cp -rf hwInit /usr/bin/
chmod 755 /usr/bin/hwInit
cd $APPTarPath
for APPNAME in ${APPLIST[*]}
do
	tar -xvf $APPNAME"".tar
done

chmod 755 -R $APPTarPath

# 配置文件
mkdir -p /data/app/wirelessDCM/data
cp -rf $CURDIR/script/default_conf.ini /data/app/wirelessDCM/data/
chmod 755 /data/app/wirelessDCM/data/default_conf.ini

rm -rf /data/app/uartManager/cfg/uartManager.ini
mkdir -p /data/app/uartManager/cfg/
cp -rf $CURDIR/uartManager.ini /data/app/uartManager/cfg/

#拷一下数据中心配置文件
mkdir -p /data/app/dbCenter/cfg
cp -fr $CURDIR/script/dbCenterData.cfg /data/app/dbCenter/cfg
chmod 755 /data/app/dbCenter/cfg/dbCenterData.cfg

# 删除旧参数
rm -rf /data/app/usbManager/*
rm -rf /data/app/scsMonitor/cfg/*
updatetools 90 "安装结束"
# 文件系统同步
sync;sync

# 开狗
wtd_set 10

updatetools 100 "升级成功  即将重启"

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
