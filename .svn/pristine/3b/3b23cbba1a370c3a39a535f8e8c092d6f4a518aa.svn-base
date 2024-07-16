#!/bin/bash
MASK_ARRAY=(0 128 192 224 240 248 252 254 255)

setmac(){
	FENAME=$1
	MACADDR=$2
	VALID=0
	echo "$MACADDR" > /tmp/FEMAC
	mac=`grep -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}' /tmp/FEMAC`
	rm -rf /tmp/FEMAC
	if [ ${#mac} -eq 17 ]; then
		if [ ${mac:1:1} = "E" -o ${mac:1:1} = "C" -o ${mac:1:1} = "A" -o ${mac:1:1} = "e" -o ${mac:1:1} = "c" -o ${mac:1:1} = "a" -o\
		     ${mac:1:1} = "8" -o ${mac:1:1} = "6" -o ${mac:1:1} = "4" -o ${mac:1:1} = "2" -o ${mac:1:1} = "0" ]; then
			VALID=1
		fi
	fi
	
	if [ "0" -eq "$VALID" ]; then
		mac="FF"
		while true
		do
			mac=`echo $RANDOM|md5sum|sed 's/../&:/g'|cut -c 1-17`
			if [ ${mac:1:1} = "E" -o ${mac:1:1} = "C" -o ${mac:1:1} = "A" -o ${mac:1:1} = "e" -o ${mac:1:1} = "c" -o ${mac:1:1} = "a" -o\
				 ${mac:1:1} = "8" -o ${mac:1:1} = "6" -o ${mac:1:1} = "4" -o ${mac:1:1} = "2" -o ${mac:1:1} = "0" ]; then
				break
			fi
		done
	fi
	mac=$(echo $mac | tr "[:lower:]" "[:upper:]")
	echo "set $FENAME $mac"
	ifconfig $FENAME down 
	sleep 1s
	ifconfig $FENAME hw ether $mac 
	sleep 1s
	ifconfig $FENAME up
	sleep 1s
}

FE0PATH=`cat /etc/smiOS.conf | grep 'FE0_PATH' | awk -F '=' '{print $2}'`
echo "$FE0PATH"
if [ ! -z "$FE0PATH" ]; then
	etDevice=`cat $FE0PATH | grep 'Name' | awk -F '=' '{print $2}'`
	MACADDR=`cat $FE0PATH | grep 'MACAddress'| awk -F '=' '{print $2}'`
	echo "$etDevice: $MACADDR"
	setmac $etDevice $MACADDR
	
	DHCP=`cat $FE0PATH | grep 'DHCP' |grep -v '#DHCP' | awk -F '=' '{print $2}'`
	if [ "yes" == "$DHCP" -o "YES" == "$DHCP" ]; then
		echo "$etDevice: DHCP"
		udhcpc -i $etDevice &
	else
		addr=`cat $FE0PATH | grep 'Address' |grep -v 'MACAddress' | awk -F '=' '{print $2}'| awk -F '/' '{print $1}'`
		maskno=$((`cat $FE0PATH | grep 'Address' |grep -v 'MACAddress' | awk -F '=' '{print $2}'| awk -F '/' '{print $2}'`))
		maskaddr=""
		if [ 0 -eq $maskno ]; then
			maskaddr="0.0.0.0"
		elif [ $maskno -lt 9 ]; then
			maskaddr="${MASK_ARRAY[$maskno]}"".0.0.0"
		elif [ $maskno -lt 17 ]; then
			maskaddr="255."${MASK_ARRAY[$(( $maskno - 8 ))]}".0.0"
		elif [ $maskno -lt 25 ]; then
			maskaddr="255.255."${MASK_ARRAY[$(( $maskno - 16 ))]}".0"
		elif [ $maskno -lt 33 ]; then
			maskaddr="255.255.255."${MASK_ARRAY[$(( $maskno - 24 ))]}
		else
			maskaddr="255.255.255.255"
		fi
		getway=`cat $FE0PATH | grep 'Gateway' | awk -F '=' '{print $2}'`
		echo "$etDevice: $addr $maskaddr $getway"
		if [ ! -z "$addr" -a "0.0.0.0" != "$addr" ]; then
			if [ "0.0.0.0" != "$maskaddr" ]; then
				ifconfig $etDevice $addr netmask $maskaddr up
			else
				ifconfig $etDevice $addr up
			fi
			sleep 1s
		fi
		
		route del default $etDevice
		if [ ! -z "$getway" -a "0.0.0.0" != "$getway" ]; then
			route add default gw $getway $etDevice
		fi
	fi
else
	# MAC随机
	setmac FE0 FF:FF:FF:FF:FF:FF
fi
	
FE1PATH=`cat /etc/smiOS.conf | grep 'FE1_PATH' | awk -F '=' '{print $2}'`
echo "$FE1PATH"
if [ ! -z "$FE1PATH" ]; then
	etDevice=`cat $FE1PATH | grep 'Name' | awk -F '=' '{print $2}'`
	MACADDR=`cat $FE1PATH | grep 'MACAddress'| awk -F '=' '{print $2}'`
	echo "$etDevice: $MACADDR"
	setmac $etDevice $MACADDR
	
	DHCP=`cat $FE1PATH | grep 'DHCP' |grep -v '#DHCP' | awk -F '=' '{print $2}'`
	if [ "yes" == "$DHCP" -o "YES" == "$DHCP" ]; then
		echo "$etDevice: DHCP"
		udhcpc -i $etDevice &
	else
		addr=`cat $FE1PATH | grep 'Address' |grep -v 'MACAddress' | awk -F '=' '{print $2}'| awk -F '/' '{print $1}'`
		maskno=$((`cat $FE1PATH | grep 'Address' |grep -v 'MACAddress' | awk -F '=' '{print $2}'| awk -F '/' '{print $2}'`))
		maskaddr=""
		if [ 0 -eq $maskno ]; then
			maskaddr="0.0.0.0"
		elif [ $maskno -lt 9 ]; then
			maskaddr="${MASK_ARRAY[$maskno]}"".0.0.0"
		elif [ $maskno -lt 17 ]; then
			maskaddr="255."${MASK_ARRAY[$(( $maskno - 8 ))]}".0.0"
		elif [ $maskno -lt 25 ]; then
			maskaddr="255.255."${MASK_ARRAY[$(( $maskno - 16 ))]}".0"
		elif [ $maskno -lt 33 ]; then
			maskaddr="255.255.255."${MASK_ARRAY[$(( $maskno - 24 ))]}
		else
			maskaddr="255.255.255.255"
		fi
		getway=`cat $FE1PATH | grep 'Gateway' | awk -F '=' '{print $2}'`
		echo "$etDevice: $addr $maskaddr $getway"
		if [ ! -z "$addr" -a "0.0.0.0" != "$addr" ]; then
			if [ "0.0.0.0" != "$maskaddr" ]; then
				ifconfig $etDevice $addr netmask $maskaddr up
			else
				ifconfig $etDevice $addr up
			fi
			sleep 1s
		fi
		
		route del default $etDevice
		if [ ! -z "$getway" -a "0.0.0.0" != "$getway" ]; then
			route add default gw $getway $etDevice
		fi
	fi
else
	# MAC随机
	setmac FE1 FF:FF:FF:FF:FF:FF
fi

