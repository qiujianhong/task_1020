#!/bin/bash
CURDIR=$(cd $(dirname $0) && pwd )
echo $CURDIR
cd $CURDIR

AREA=$(cat area.txt)
VERSION=$(cat version.txt)

VERSIONSYS=$(cat /proc/version)
echo $VERSIONSYS

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

secReinfPath=/usr/sec-app/package/work
APPTarPath=/usr/local/extapps
libpath=/lib
datapath=/data/app
OSBackPath=/backup
APPLIST=(smiOS wirelessDCM dbCenter ccoRouter uartManager desktopGui mapManager iotManager acMeter rspSample btManager usbManager scsMonitor lcMonitor taskManager stAmr sysUpdate)

updatetools 10 "解压程序"

tar -zxvf package*.tar.gz

# 先停止APP
updatetools 20 "停止程序"
echo "pkill All APPs!"
$APPTarPath/smiOS/bin/stop_app.sh

# 将喂狗时间调整到5分钟，防止关闭smiOS后立刻重启
wtd_set 300

# 停止容器
echo "------------- Remove All Containers -------------"
dks=`docker ps -a --format "{{.Names}}"`
for dk in $dks
do 
    container stop $dk
    container uninstall $dk
done

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

PID=$(ps |grep mosquitto |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
    echo "no mosquitto!"
else
    echo "kill mosquitto!"
    kill -9 $PID
fi

PID=$(ps |grep secReinf |grep -v grep|grep -v '.sh'|awk '{print $1}')
if [ ! $PID ]; then
    echo "no secReinf!"
else
    echo "kill secReinf!"
    kill -9 $PID
fi

echo "------------- update image -------------"
updatetools 40 "安装镜像"
cd $CURDIR/image
./setup_images.sh

updatetools 60 "拷贝软件"
# 拷贝APP和脚本
cd $CURDIR
chmod 755 ./openapp/*
cp -fr ./openapp/* /usr/bin/

# 拷贝APP到emmc中
chmod 755 *.tar
rm -rf $APPTarPath/*
mkdir -p $APPTarPath
for APPNAME in ${APPLIST[*]}
do
    cp -fr $APPNAME"".tar $APPTarPath/
done
chmod 777 -R $APPTarPath

# 解压远程升级脚本
tar -xvf $APPTarPath/sysUpdate.tar -C $APPTarPath/
chmod 755 -R $APPTarPath/sysUpdate/

cp -fr SecReinf_lic.tar $OSBackPath/
cp -fr SecReinf.tar $OSBackPath/
cp -fr script/applist.xml $OSBackPath/

rm -rf $secReinfPath/*
mkdir -p $secReinfPath
cp SecReinf.tar $secReinfPath/
cp SecReinf_lic.tar $secReinfPath/
chmod 755 -R $secReinfPath/
cd $secReinfPath
tar -xvf SecReinf.tar
chmod 755 -R SecReinf

updatetools 80 "拷贝软件"
# 拷贝hal
cd $CURDIR
tar -zxvf hal_lib.tar.gz
rm -rf $libpath/hal_lib
cp -rf hal_lib $libpath

# 拷贝脚本
cp script/hook.sh /etc/
chmod 755 /etc/hook.sh
cp script/smiOS.conf /etc/
chmod 755 /etc/smiOS.conf
cp script/setnetconf.sh /etc/
chmod 755 /etc/setnetconf.sh
cp script/os-version.yaml /etc/
chmod 755 /etc/os-version.yaml
cp script/os-version_1.1.yaml /backup/
chmod 755 /backup/os-version_1.1.yaml
cp script/os-version_1.2.yaml /backup/
chmod 755 /backup/os-version_1.2.yaml

cp -fr script/applist.xml $secReinfPath/SecReinf/cfg/
chmod 755 -R $secReinfPath/SecReinf/cfg/

rm -rf /data/app/uartManager/cfg/uartManager.ini
mkdir -p /data/app/uartManager/cfg
cp -rf $CURDIR/uartManager.ini /data/app/uartManager/cfg/

mkdir -p /data/app/mapManager/cfg
cp -rf $CURDIR/script/mapManager.ini /data/app/mapManager/cfg/

#拷贝其他(正式系统需要去掉)
cp -rf $CURDIR/*.img $OSBackPath/

cp $CURDIR/lcdctrl /usr/bin/
chmod 755 /usr/bin/lcdctrl
cp $CURDIR/liccheck /usr/bin/
chmod 755 /usr/bin/liccheck

# 删除安全加固容器结束
rm -rf /etc/init_finsh

# ip地址设置
if [ ! -f /backup/FE0 ]; then
    echo "[Match]" > /backup/FE0
    echo "Name=FE0" >> /backup/FE0
    echo "[Link]" >> /backup/FE0
    
    # 随机MAC地址
    mac="FF"
    while true
    do
        mac=`echo $RANDOM|md5sum|sed 's/../&:/g'|cut -c 1-17`
        if [ ${mac:1:1} = "E" -o ${mac:1:1} = "C" -o ${mac:1:1} = "A" -o ${mac:1:1} = "e" -o ${mac:1:1} = "c" -o ${mac:1:1} = "a" -o\
             ${mac:1:1} = "8" -o ${mac:1:1} = "6" -o ${mac:1:1} = "4" -o ${mac:1:1} = "2" -o ${mac:1:1} = "0" ]; then
            break
        fi
    done
    echo "MACAddress=$mac" >> /backup/FE0
    echo "[Network]" >> /backup/FE0
    echo "DHCP=no" >> /backup/FE0
    echo "Address=192.168.2.170/24" >> /backup/FE0
    echo "Gateway=192.168.2.1" >> /backup/FE0
    
    echo "Create FE0, mac: $mac, ip: 192.168.2.170"
fi
if [ ! -f /backup/FE1 ]; then
    echo "[Match]" > /backup/FE1
    echo "Name=FE1" >> /backup/FE1
    echo "[Link]" >> /backup/FE1
    
    # 随机MAC地址
    mac="FF"
    while true
    do
        mac=`echo $RANDOM|md5sum|sed 's/../&:/g'|cut -c 1-17`
        if [ ${mac:1:1} = "E" -o ${mac:1:1} = "C" -o ${mac:1:1} = "A" -o ${mac:1:1} = "e" -o ${mac:1:1} = "c" -o ${mac:1:1} = "a" -o\
             ${mac:1:1} = "8" -o ${mac:1:1} = "6" -o ${mac:1:1} = "4" -o ${mac:1:1} = "2" -o ${mac:1:1} = "0" ]; then
            break
        fi
    done
    echo "MACAddress=$mac" >> /backup/FE1
    echo "[Network]" >> /backup/FE1
    echo "DHCP=no" >> /backup/FE1
    echo "Address=192.168.1.1/24" >> /backup/FE1
    echo "Gateway=0.0.0.0" >> /backup/FE1
    
    echo "Create FE1, mac: $mac, ip: 192.168.1.1"
fi

# 删除旧参数
rm -rf /data/app/usbManager/*
rm -rf /data/app/smiOS/data/appRegisterList
rm -rf /data/app/scsMonitor/cfg/*

# 文件系统同步
sync;sync

#时钟检查
test_rtc

updatetools 90 "安装结束"
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

