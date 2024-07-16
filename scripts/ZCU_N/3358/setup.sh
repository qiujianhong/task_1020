#!/bin/bash
CURDIR=$(cd $(dirname $0) && pwd )
echo $CURDIR
cd $CURDIR

APPTarPath=/usr/local/extapps
libpath=/lib
qtlibpath=/usr/lib
fontspath=/usr/share
datapath=/data/app
APPLIST=(smiOS dbCenter desktopGui wirelessDCM acMeter rspSample uartManager btManager usbManager taskManager lcMonitor stAmr sysUpdate)

tar -zxvf package*.tar.gz

# 先停止APP
echo "pkill All APPs!"
$APPTarPath/smiOS/bin/stop_app.sh

# 将喂狗时间调整到2分钟，防止关闭smiOS后立刻重启
wtd_set 120

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

PID=$(ps |grep mosquitto |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
	echo "no mosquitto!"
else
	echo "kill mosquitto!"
	kill -9 $PID
fi

# QT库操作
cd $CURDIR/qt
rm -rf $qtlibpath/qt5.12
tar -zxvf qt5.12.tar.gz -C $qtlibpath/
tar -zxvf tslib.tar.gz -C $qtlibpath/
cp libtgtsmlInputContextPlugin.so.1.0.0 $qtlibpath/qt5.12/plugins/platforminputcontexts/
tar -zxvf fonts.tar.gz -C $fontspath/
cp profile /etc/
chmod 755 /etc/profile

cd $CURDIR
# 拷贝APP和脚本
chmod 755 ./openapp/*
cp -fr ./openapp/* /bin/
chmod 755 ./lib/*
cp -fr ./lib/* /lib/

chmod 755 *.tar
rm -rf $APPTarPath/*
mkdir -p $APPTarPath
cp -fr *.tar $APPTarPath/

cp script/hook.sh /etc/
chmod 755 /etc/hook.sh

cp script/boot.sh /mnt/
chmod 755 /mnt/boot.sh

cp script/rcS /etc/init.d/
chmod 755 /etc/init.d/rcS

cp -fr script/mosquitto /etc/
chmod 755 -R /etc/mosquitto

# 拷贝
tar -zxvf hal_lib.tar.gz
rm -rf $libpath/hal_lib
cp -rf hal_lib $libpath

cd $APPTarPath
for APPNAME in ${APPLIST[*]}
do
	tar -xvf $APPNAME"".tar
done

chmod 755 -R $APPTarPath

# 文件系统同步
sync;sync

# 开狗
wtd_set 10

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
