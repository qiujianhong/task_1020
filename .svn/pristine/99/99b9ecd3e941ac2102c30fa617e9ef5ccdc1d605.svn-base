#!/bin/bash
CURDIR=$(cd $(dirname $0) && pwd )
cd $CURDIR/package_upgrade/

upgrade=`ls *.tar`

for file in ${upgrade[*]}
do
    filename=${file%.*}
    echo "md5sum for $filename"
	MD5=`md5sum $CURDIR/package_upgrade/$filename.tar | awk '{ print $1}'`
	echo "md5 = $MD5"
	echo "$MD5" > $CURDIR/package_upgrade/$filename.md5
done

halfile=hal_lib.tar.gz
if [ -f "$halfile" ]; then
	echo "md5sum for hal"
	MD5=`md5sum $CURDIR/package_upgrade/hal_lib.tar.gz | awk '{ print $1}'`
	echo "halmd5 = $MD5"
	echo "$MD5" > $CURDIR/package_upgrade/hal.md5
fi

dockertools=docker_tools.tar.gz
if [ -f "$dockertools" ]; then
	echo "md5sum for docker_tools"
	MD5=`md5sum $CURDIR/package_upgrade/docker_tools.tar.gz | awk '{ print $1}'`
	echo "docker_toolsmd5 = $MD5"
	echo "$MD5" > $CURDIR/package_upgrade/docker_tools.md5
fi
dbCentercfg=dbCenterData.cfg
if [ -f "$dbCentercfg" ]; then
	echo "md5sum for dbCentercfg"
	MD5=`md5sum $CURDIR/package_upgrade/dbCenterData.cfg | awk '{ print $1}'`
	echo "dbCentercfg = $MD5"
	echo "$MD5" > $CURDIR/package_upgrade/dbcfg.md5
fi
udiskupdate=udisk_update.tar.gz
if [ -f "$udiskupdate" ]; then
	echo "md5sum for udiskupdate"
	MD5=`md5sum $CURDIR/package_upgrade/udisk_update.tar.gz | awk '{ print $1}'`
	echo "udiskupdate = $MD5"
	echo "$MD5" > $CURDIR/package_upgrade/udiskupdate.md5
fi
daemonjson=daemon.json
if [ -f "$daemonjson" ]; then
	echo "md5sum for daemonjson"
	MD5=`md5sum $CURDIR/package_upgrade/daemon.json | awk '{ print $1}'`
	echo "daemonjson = $MD5"
	echo "$MD5" > $CURDIR/package_upgrade/daemonjson.md5
fi
paramamr=paramamr.xin
if [ -f "$paramamr" ]; then
	echo "md5sum for paramamr"
	MD5=`md5sum $CURDIR/package_upgrade/paramamr.xin | awk '{ print $1}'`
	echo "paramamr = $MD5"
	echo "$MD5" > $CURDIR/package_upgrade/paramamr.md5
fi

stAmrini=stAmr.ini
if [ -f "$stAmrini" ]; then
	echo "md5sum for stAmrini"
	MD5=`md5sum $CURDIR/package_upgrade/stAmr.ini | awk '{ print $1}'`
	echo "stAmrini = $MD5"
	echo "$MD5" > $CURDIR/package_upgrade/stAmrini.md5
fi
cd $CURDIR
tar -zcvf upgrade.tar.gz package_upgrade/