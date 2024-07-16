#!/bin/bash
CURDIR=$(cd $(dirname $0) && pwd )
echo $CURDIR
cd $CURDIR

# ÈÝÆ÷¾µÏñ¼ì²é
iamgeIDs=`docker images -a --format "{{.ID}}"`
echo "uninstall image!"
for iamgeID in $iamgeIDs
do 
	docker rmi $iamgeID
done

mkdir -p /etc/docker
cp daemon.json /etc/docker/

echo "install new image!"
cat basic-img-arm32-1.0.img |docker import - basic-img-arm32:1.0

chmod 755 ./bin/*
cp -fr ./bin/* /usr/bin/

cd libs
chmod 755 *
libsos=`ls`
for libname in $libsos
do 
	cp -fr $libname /usr/lib/
done



