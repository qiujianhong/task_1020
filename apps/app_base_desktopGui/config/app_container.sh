#!/bin/bash

echo "------------- Remove All Containers -------------"
dks=`docker ps -a --format "{{.Names}}"`
for dk in $dks
do 
	container stop $dk
	container uninstall $dk
done

# ɾ����ȫ�ӹ���������
rm -rf /etc/init_finsh

# �ļ�ϵͳͬ��
sync;sync

reboot
sleep 5
#��ֹrebootʧ�ܣ��Ӳ�������
echo "rebootʧ�� ִ��reboot -nf"
reboot -nf