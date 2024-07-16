#!/bin/bash

#os
cp /etc/os-version.yaml /tmp/vsys		#系统没有提供，启动时，拷贝

#kernel
cat /sys/class/version/vkernel |awk -F '.' '{print $1}' > /tmp/vtmp
cat /sys/class/version/vkernel |awk -F '_' '{print $2}' >> tmp/vtmp
cat tmp/vtmp | tr '\n' '.' | sed 's/.$/ /' > /tmp/vkernel