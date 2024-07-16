#!/bin/bash
CURDIR=$(cd $(dirname $0) && pwd )
#echo $CURDIR
cd $CURDIR
APPTarPath=/usr/local/extapps
PACKNAME=$1

CURR_TAR=`ls | grep .tar` 

# 文件系统同步
sync;sync

# 安装容器
install_container(){
	echo "--------------------------$1 insta1l start-------------------------"
    PID=$(ps |grep $2 |grep -v grep|grep -v '.sh'|awk '{print $1}')
	if [ ! $PID ]; then
		echo "no $2!"
	else
		echo "kill $2!"
		kill -9 $PID
	fi
	cp $1 $APPTarPath/
	tar -xvf $APPTarPath/$1 -C $APPTarPath
	if [ -f $APPTarPath/$2/bin/startup_app.sh ];then
		$APPTarPath/$2/bin/startup_app.sh
	fi
	echo -e "--------------------------$1 insta1l end---------------------------\n"	
}

#默认安装当前路径所有包
if [ -z "$PACKNAME" ];then
	for i in ${CURR_TAR[@]}
	do
		APPNAME=$(basename $i .tar)
		install_container $i $APPNAME
	done
else
    APPNAME=$(basename $PACKNAME .tar)
	install_container $PACKNAME $APPNAME
fi
