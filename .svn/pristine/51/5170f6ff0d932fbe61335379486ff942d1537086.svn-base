#!/bin/bash
CURDIR=$(cd $(dirname $0) && pwd )
echo $CURDIR
cd $CURDIR

IMAGE_VERSION=$(cat images_version.txt)

# 容器镜像检查
iamgeIDs=`docker images -a --format "{{.ID}}"|grep -v $IMAGE_VERSION`
echo "uninstall image!"
for iamgeID in $iamgeIDs
do 
	docker rmi $iamgeID
done

mkdir -p /etc/docker
cp daemon.json /etc/docker/

image=`docker images -a |grep $IMAGE_VERSION`
if [ -z "$image"  ]; then
	echo "install image!"
	docker load < basic_img-arm64.tar
	# 文件系统同步
	chmod 755 ./bin/*
	cp -fr ./bin/* /usr/bin/
	chmod 755 ./lib/*
	cp -fr ./lib/* /usr/lib/
	sync;sync
fi


