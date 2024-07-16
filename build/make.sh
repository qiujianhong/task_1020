#!/bin/bash -e
PROG_TYPE="$1"
PROG_NAME="$2"
CLEAN_FLAG="$3"

# 执行脚本的路径
CURR_DIR=$(cd $(dirname $0) && pwd )
#echo "Build path: "$CURR_DIR
cd $CURR_DIR
CODE_DIR=$CURR_DIR/..
ARCH_DIR=$CODE_DIR/arch_board
DEIVE_DIR=$CODE_DIR/drivers
RELEASE_DIR=$CODE_DIR/release
BOARD_DIR=$CODE_DIR/arch_board

# 需要手动添加产品及其路径
PRODUCTS_ARRAY=(CCU          CCU_FB           ECU            ECU_FB           ZCU_1            ZCU_3)
PRODUCTS_DIR=($CURR_DIR/CCU  $CURR_DIR/CCU_FB $CURR_DIR/ECU  $CURR_DIR/ECU_FB $CURR_DIR/ZCU_1  $CURR_DIR/ZCU_3)
HALS_DIR=(    $DEIVE_DIR/CCU $DEIVE_DIR/CCU   $DEIVE_DIR/ECU $DEIVE_DIR/ECU   $DEIVE_DIR/ZCU_1 $DEIVE_DIR/ZCU_3)

# 地区名称 代码中实际宏定义是大写的
AREA_NAME=(default jiangsu zhejiang anhui hubei hunan jiangxi shandong gansu shaanxi fujian shanxi chongqing)

PRODUCTNAME=""
BORADNAME=""
AREANAME=""
SH_DIR=""
HAL_DIR=""

printfHelpFun(){
    echo "Usage: make.sh [<type>] [<project>] [clean]"
    echo "<type>:"  
	echo "  product             PreBuild for product."
	echo "  board               PreBuild for board type."
	echo "  area                PreBuild for borad area."
	echo "  lib                 Build for library."
    echo "  app                 Build for application."
	echo "  all,'NONE'          Build all library and apps, create package."    
	echo "<project>:"               
	echo "  all,'NONE'          Build all <type=lib, app> type."
    echo "  <name>              Build <type> projetct named by <name>."
	echo "                      <type>=product <name>=\"CCU CCU_FB ECU ECU_FB SCU ZCU_1 ZCU_3 ZCU_N...\"." 
	echo "                      <type>=board <name>=\"701 801 3358 3568 ...\"." 
	echo "                      <type>=area <name>=\"default jiangsu zhejiang anhui hubei jiangxi shandong gansu fujian chongqing...\"." 
	echo "                      <type>=lib <name>=\"all hal appgui msgassist msgbus security storage framecomm algo...\"." 
	echo "                      <type>=app <name>=\"all smiOS ccoRouter dbCenter desktopGUi mapManager wirelessDCM taskManager puAmr...\"."  
	echo "clean:                Clean the produced files."
}

printfAREAFun(){
    #module不存在提示支持的module
    Out_String="area:"
	for AREA_PLACE in ${AREA_NAME[*]}
	do
		Out_String=$Out_String" $AREA_PLACE"
	done
	echo $Out_String
}

#首先判断area.cmake是否存在
makeFirstCkeck(){
	if [ ! -f "product.cmake" ]; then
		echo "No set product, please set board type first(./make.sh product [type])! Use this command get product list:"
		echo "./make.sh cpu ?"
		exit 0
	fi

	if [ ! -f "product.txt" ]; then
		echo "No set product, please set board type first(./make.sh product [type])! Use this command get product list:"
		echo "./make.sh product ?"
		exit 0
	fi
	PRODUCTNAME=$(cat product.txt)
	
	if [ ! -f "board.cmake" ]; then
		echo "No set board, please set board type first(./make.sh board [type])! Use this command get board list:"
		echo "./make.sh board ?"
		exit 0
	fi

	if [ ! -f "board.txt" ]; then
		echo "No set board, please set board type first(./make.sh board [type])! Use this command get board list:"
		echo "./make.sh board ?"
		exit 0
	fi
	BORADNAME=$(cat board.txt)
	# 拷贝环境变量
	if [ ! -f $BOARD_DIR/$BORADNAME/env.cmake ]; then
		echo "No support gcc for "$BORADNAME
		exit 0
	fi
	cp $BOARD_DIR/$BORADNAME/env.cmake $CURR_DIR/
	
	if [ ! -f "area.cmake" ]; then
		echo "No set area, please set area first(./make.sh area [palce])! Use this command get area list:"
		echo "./make.sh area ?"
		exit 0
	fi

	if [ ! -f "area.txt" ]; then
		echo "No set area, please set area first(./make.sh area [palce])! Use this command get area list:"
		echo "./make.sh area ?"
		exit 0
	fi
	AREANAME=$(cat area.txt)
	
	MODULE_FLAG=0
	# 获取APP编译脚本路径
	for i in ${!PRODUCTS_ARRAY[@]}
	do
		if [ "$PRODUCTNAME" == "${PRODUCTS_ARRAY[$i]}" ]; then
			MODULE_FLAG=1
			SH_DIR=${PRODUCTS_DIR[$i]}
			HAL_DIR=${HALS_DIR[$i]}/$BORADNAME"_hal"
			break
		fi
	done  
	if [ 0 -eq $MODULE_FLAG ]; then
		echo "No support product for "$PRODUCTNAME
		exit 0
	fi

	# 获取HAL代码路径
	if [ ! -d $HAL_DIR ]; then
		echo "No support hal for "$PRODUCTNAME" "$BORADNAME" hal"
		exit 0
	fi
	
}

# 编译APP之前需要编译一下 编译本省不费时间
makehallib()
{
	echo "Make hal in "$HAL_DIR
	# 判断是否需要编译hal
	rm -rf $RELEASE_DIR/hal_lib/*
	cd $HAL_DIR
	make -j4 && cp -rf ./target/lib/hal_lib/* $RELEASE_DIR/hal_lib/
	cd $CURR_DIR
}

makehallibclean()
{
	echo "Clean hal in "$HAL_DIR
	# 判断是否需要编译hal
	cd $HAL_DIR
	make clean
	cd $CURR_DIR
	rm -rf $RELEASE_DIR/hal_lib/*
}

makehallibclean_all()
{
	echo "Clean all hal "
	ALLHALS_DIR=($DEIVE_DIR/CCU/3358_hal $DEIVE_DIR/CCU/801_hal $DEIVE_DIR/ECU/3358_hal)
	for i in ${!ALLHALS_DIR[@]}
	do
		if [ -f ${ALLHALS_DIR[$i]}/Makefile ]; then
			HAL_DIR=${ALLHALS_DIR[$i]}
			makehallibclean
		fi
	done  
}

makeproductclean_all()
{
	echo "Clean all product "
	for i in ${!PRODUCTS_DIR[@]}
	do
		PEO_DIR=${PRODUCTS_DIR[$i]}
		echo "Clean product ${PRODUCTS_ARRAY[$i]}" 
		cd $PEO_DIR && ./make_apps.sh 3358 default clean 
		cd $PEO_DIR && ./make_apps.sh 801 default clean 
	done  
}

#入参判断执行
if [ -z "$PROG_TYPE" -o "all" == "$PROG_TYPE" ]; then
	# 全部编译
	makeFirstCkeck
	makehallib
	cd $SH_DIR
	./make_apps.sh $BORADNAME $AREANAME lib all 
	cd $SH_DIR
	./make_apps.sh $BORADNAME $AREANAME app all 
elif [ "lib" = "$PROG_TYPE" ]; then
	# 编译lib库
	makeFirstCkeck
	if [ "all" == "$PROG_NAME" -o -z "$PROG_NAME" ]; then
		#编译所有lib
		makehallib
		cd $SH_DIR && ./make_apps.sh $BORADNAME $AREANAME lib all 
	elif [ "clean" == "$PROG_NAME" ]; then
		#清除所有lib
		makehallibclean
		cd $SH_DIR && ./make_apps.sh $BORADNAME $AREANAME lib clean 
	elif [ "hal" == "$PROG_NAME" ]; then
		#hal
		if [ -z "$CLEAN_FLAG"]; then
			makehallib
		else
			makehallibclean
		fi
	else
		cd $SH_DIR && ./make_apps.sh $BORADNAME $AREANAME lib $PROG_NAME $CLEAN_FLAG
	fi
elif [ "app" = "$PROG_TYPE" ]; then
	# 编译app
	makeFirstCkeck
	if [ "all" == "$PROG_NAME" -o -z "$PROG_NAME" ]; then
		#编译hal
		makehallib
		cd $SH_DIR && ./make_apps.sh $BORADNAME $AREANAME app all 
	else
		cd $SH_DIR && ./make_apps.sh $BORADNAME $AREANAME app $PROG_NAME $CLEAN_FLAG
	fi
elif [ "product" = "$PROG_TYPE" ]; then
	#找到指定产品
	for i in ${!PRODUCTS_ARRAY[@]}
	do
		if [ "$PROG_NAME" == "${PRODUCTS_ARRAY[$i]}" ]; then
			rm -rf FB.txt
			MODULE_FLAG=1
			if [ "$PROG_NAME" == "ECU_FB" ]; then
				PRODUCT_TRAL_NAME="ECU"
				echo $PROG_NAME > FB.txt
			elif [ "$PROG_NAME" == "CCU_FB" ]; then
				PRODUCT_TRAL_NAME="CCU"
				echo $PROG_NAME > FB.txt
			else
				PRODUCT_TRAL_NAME=$PROG_NAME
			fi
			echo "set product "$PRODUCT_TRAL_NAME
			echo "add_definitions(-DPRODUCT_"$PRODUCT_TRAL_NAME" -DPRODUCT_NAME=\"$PRODUCT_TRAL_NAME\")" > product.cmake
			echo "set(PRODUCT_NAME $PRODUCT_TRAL_NAME)" >> product.cmake
			echo $PROG_NAME > product.txt
			break
		fi
	done  
	if [ 0 -eq $MODULE_FLAG ]; then
		printfAREAFun
	fi
	exit 0
elif [ "board" = "$PROG_TYPE" ]; then
	if [ "701" = "$PROG_NAME" ]; then
		echo "add_definitions(-DBOARD_TYPE=\"701\" -DBOARD_701)" > board.cmake
		echo "set(BOARD_TYPE 701)" >> board.cmake
		echo "701" > board.txt
		echo "board type 701"
	elif [ "801" = "$PROG_NAME" ]; then
		echo "add_definitions(-DBOARD_TYPE=\"801\" -DBOARD_801)" > board.cmake
		echo "set(BOARD_TYPE 801)" >> board.cmake
		echo "801" > board.txt
		echo "board type 801"
	elif [ "3358" = "$PROG_NAME" ]; then
		echo "add_definitions(-DBOARD_TYPE=\"3358\" -DBOARD_3358)" > board.cmake
		echo "set(BOARD_TYPE 3358)" >> board.cmake
		echo "3358" > board.txt
		echo "board type 3358"
	elif [ "3568" = "$PROG_NAME" ]; then
		echo "add_definitions(-DBOARD_TYPE=\"3568\" -DBOARD_3568)" > board.cmake
		echo "set(BOARD_TYPE 3568)" >> board.cmake
		echo "3568" > board.txt
		echo "board type 3568"
	else
		echo -e "Board type support 701 801 3358 3568!"
	fi
	exit 0
elif [ "area" = "$PROG_TYPE" ]; then
	MODULE_FLAG=0
	for i in ${!AREA_NAME[@]}
	do
		if [ "$PROG_NAME" == "${AREA_NAME[$i]}" ]; then
			MODULE_FLAG=1
			AREA_BIG=$(echo $PROG_NAME | tr "[:lower:]" "[:upper:]")
			echo "place "$PROG_NAME
			echo "add_definitions(-DAREA_"$AREA_BIG" -DAREANAME_PLACE=\"$PROG_NAME\")" > area.cmake
			echo "set(AREA_DIR "$PROG_NAME")" >> area.cmake
			echo $PROG_NAME > area.txt
			break
		fi
	done  
	if [ 0 -eq $MODULE_FLAG ]; then
		printfAREAFun
	fi
	exit 0
elif [ "clean" = "$PROG_TYPE" ]; then
	echo -e "Start to clean all:"
	#清除所有lib和app
	makehallibclean_all
	makeproductclean_all
	rm -rf $RELEASE_DIR/apps/*
	rm -rf $RELEASE_DIR/hal_lib/*
	rm -rf $RELEASE_DIR/lib/*
	rm -rf $RELEASE_DIR/package/*
else
	printfHelpFun
	exit 0
fi

if [ "?" != "$PROG_NAME" ]; then
	echo "------------------------------------------------"
	echo "Finish for make "$PROG_TYPE" "$PROG_NAME" "$CLEAN_FLAG
	echo "product "$PRODUCTNAME", board "$BORADNAME", area "$AREANAME
	echo "------------------------------------------------"
fi
cd $CURR_DIR
