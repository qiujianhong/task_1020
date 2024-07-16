#!/bin/bash -e
BOARD_NAME="$1"
AREA_NAME="$2"
PROG_TYPE="$3"
PROG_NAME="$4"
CLEAN_FLAG="$5"

CURR_DIR=$(cd $(dirname $0) && pwd )
CODE_DIR=${CURR_DIR}/../../apps
LIB_DIR=${CURR_DIR}/../../lib/code

# 需要手动添加编译内容及其路径
LIB_ARRAY=(appgui msgassist msgbus security storage framecomm algo iniparser mapmsg mxml mqttclient template cJson mingather ble radio uart)
LIB_DIR=($LIB_DIR/appgui $LIB_DIR/msgassist $LIB_DIR/msgbus $LIB_DIR/security $LIB_DIR/storage $LIB_DIR/commonfun $LIB_DIR/algorithm $LIB_DIR/iniparser \
         $LIB_DIR/mapmsg $LIB_DIR/mxml $LIB_DIR/mqttclient $LIB_DIR/template $LIB_DIR/cJson $LIB_DIR/mingather $LIB_DIR/ble $LIB_DIR/radio $LIB_DIR/uart)

# yc和stamr是一套代码, 需要后编译yc
APP_ARRAY=(wirelessDCM dbCenter)
APP_DIR=($CODE_DIR/app_base_wirelessDCM/code $CODE_DIR/app_base_dbCenter/code) 

printfLibFun(){
    #lib不存在提示支持的lib
	Out_String="lib: all hal"
	for LIB_NAME in ${LIB_ARRAY[*]}
	do
		Out_String=$Out_String" $LIB_NAME"
	done
	echo $Out_String
}

printfAppFun(){
    #app不存在提示支持的app
	Out_String="app: all"
	for LIB_NAME in ${APP_ARRAY[*]}
	do
		Out_String=$Out_String" $LIB_NAME"
	done
	echo $Out_String
}

# 清除编译
clean() {

	if [ -e "$1" ]; then
		cd $1
		if [ -e "Makefile" -o -e "makefile" ]; then	
			make clean
			echo "$1 make clean."
		fi	
		
		cd ${CURR_DIR} && rm -rf $1
		echo "$1 delete $1."
	fi
	if [ -e "$2" ]; then
		cd $2
	
		if [ -e "Makefile" -o -e "makefile" ]; then	
			make clean
			echo "$2 make clean."
		fi
        
		#rm -f cmake_install.cmake && rm -f CMakeCache.txt && rm -f Makefile && rm -f makefile && rm -rf CMakeFiles
        #echo "$2 delete cmake_install.cmake CMakeCache.txt Makefile makefile CMakeFiles/."
        
		if [ -e "cmake_install.cmake" ]; then
			rm -rf cmake_install.cmake 
			echo "$2 delete cmake_install.cmake."
		fi

		if [ -e "CMakeCache.txt" ]; then
			rm -rf CMakeCache.txt 
			echo "$2 delete CMakeCache.txt."
		fi
		
		if [ -e "Makefile" ]; then
			rm -rf Makefile 
			echo "$2 delete Makefile."
		fi
		
		if [ -e "makefile" ]; then
			rm -rf makefile 
			echo "$2 delete makefile."
		fi
		
		if [ -e "CMakeFiles" ]; then
			echo "$2 delete CMakeFiles."
			rm -rf CMakeFiles 
		fi	
	fi		
}

# 编译单个lib
makelib(){
	echo ""
	echo -e "\033[32mStart to make lib ${LIB_ARRAY[$1]}:\033[0m"
	echo -e "\033[32m============================start============================\033[0m"
	if [ -e "${LIB_DIR[$1]}" ]; then	
		cd ${LIB_DIR[$1]}
        
        # 防止进入子目录编译的残留导致out-of-source方式失败
		if [ -e "CMakeCache.txt" ]; then
			rm -rf CMakeCache.txt 
		fi
        
		cd $CURR_DIR
		echo ${LIB_DIR[$1]}
		mkdir -p ${LIB_ARRAY[$1]} && cd ${LIB_ARRAY[$1]} && cmake ${LIB_DIR[$1]} && make -j4
	fi
	echo -e "\033[32m=============================end=============================\033[0m"
	cd $CURR_DIR
}

# 编译所有lib
makealllib(){
	for i in ${!LIB_ARRAY[@]}
	do
		makelib $i
	done
}

# 清除单个lib
cleanlib(){
	echo -e "\033[32mClean lib ${LIB_ARRAY[$1]}\033[0m"

    clean ${LIB_ARRAY[$1]} ${LIB_DIR[$1]}
	cd $CURR_DIR
}

# 清除所有lib
cleanalllib(){
	echo ""
	echo -e "\033[32mStart to clean all lib:\033[0m"
	echo -e "\033[32m============================start============================\033[0m"
	for i in ${!LIB_ARRAY[@]}
	do
		cleanlib $i
	done
	echo -e "\033[32m=============================end=============================\033[0m"
}

# 编译单个app
makeapp(){
	echo ""
	echo -e "\033[32mStart to make app ${APP_ARRAY[$1]}:\033[0m"
	echo -e "\033[32m============================start============================\033[0m"
	if [ -e "${APP_DIR[$1]}" ]; then
		cd ${APP_DIR[$1]}
        
        # 防止进入子目录编译的残留导致out-of-source方式失败
		if [ -e "CMakeCache.txt" ]; then
			rm -rf CMakeCache.txt 
		fi
     
        #检查依赖的库，并且提前编译     
        for i in ${!LIB_ARRAY[@]}
        do
            #从库的编译文件中获取库名
            if [ -e "${LIB_DIR[i]}/CMakeLists.txt" ];then
                LIB_NAME=`grep -Po 'add_library\(\K[^)]*' ${LIB_DIR[i]}/CMakeLists.txt | awk '{print $1}'`
            fi
            
            #遍历库名，因为可能存在多目标CMakeLists
            for j in ${LIB_NAME[@]}
            do   
                if [ `grep -c "$j" ${APP_DIR[$1]}/CMakeLists.txt` -ne 0 ];then
                    echo -e "\033[33mStart to make dependent lib $j \033[0m"
                    makelib $i
                fi
            done
        done
        
		cd $CURR_DIR
		mkdir -p ${APP_ARRAY[$1]} && cd ${APP_ARRAY[$1]} && cmake ${APP_DIR[$1]} && make -j4
	fi
    echo -e "\033[32m=============================end=============================\033[0m"	
	cd $CURR_DIR
}
	
# 编译所有app且打包APP及相关脚本文件
makeallapp(){
	for i in ${!APP_ARRAY[@]}
	do
		makeapp $i
	done

	echo -e "$CURR_DIR"	
	./package_$BOARD_NAME".sh" $AREA_NAME
}

# 清除单个app
cleanapp(){
	echo ""
	echo -e "\033[32mStart to clean app ${APP_ARRAY[$1]}:\033[0m"
	echo -e "\033[32m--------------------start--------------------\033[0m"
	
	clean ${APP_ARRAY[$1]} ${APP_DIR[$1]}

    if [ -e "${APP_DIR[$1]}/../target/bin" ];then 
        if [ "`ls -A ${APP_DIR[$1]}/../target/bin`" != "" ]; then
            rm -rf ${APP_DIR[$1]}/../target/bin/* 
        fi
    fi
    
    #目录存在且不为空，则清空目录内容
    if [ -e "${APP_DIR[$1]}/../target/lib" ];then 
        if [ "`ls -A ${APP_DIR[$1]}/../target/lib`" != "" ]; then
            rm -rf ${APP_DIR[$1]}/../target/lib/* 
            echo "${APP_DIR[$1]} delete dependent libs."
        fi
    fi
    
    #检查依赖的库，并且清除编译     
    for i in ${!LIB_ARRAY[@]}
    do
        #从库的编译文件中获取库名
        if [ -e "${LIB_DIR[i]}/CMakeLists.txt" ];then
            LIB_NAME=`grep -Po 'add_library\(\K[^)]*' ${LIB_DIR[i]}/CMakeLists.txt | awk '{print $1}'`
        fi
        
        #遍历库名，因为可能存在多目标CMakeLists
        for j in ${LIB_NAME[@]}
        do   
            if [ `grep -c "$j" ${APP_DIR[$1]}/CMakeLists.txt` -ne 0 ];then
                #echo -e "\033[33mStart to clean dependent lib $j \033[0m"
                cleanlib $i
            fi
        done
    done

    echo -e "\033[32m-------------------- end --------------------\033[0m"
	cd $CURR_DIR
	rm -rf ${APP_ARRAY[$1]}
}

# 清除所有app
cleanallapp(){		

	echo ""
	echo -e "\033[32mStart to clean all apps :\033[0m"
	echo -e "\033[32m============================start============================\033[0m"
	
	for i in ${!APP_ARRAY[@]}
	do
		cleanapp $i
	done
	
	if [ -e "cmake_install.cmake" ]; then
		rm -rf cmake_install.cmake 
		echo "delete cmake_install.cmake."
	fi

	if [ -e "CMakeCache.txt" ]; then
		rm -rf CMakeCache.txt 
		echo "delete CMakeCache.txt."
	fi
	
	if [ -e "Makefile" ]; then
		rm -rf Makefile 
		echo "delete Makefile."
	fi
	
	if [ -e "makefile" ]; then
		rm -rf makefile 
		echo "delete makefile."
	fi
	
	if [ -e "CMakeFiles" ]; then
		echo "delete CMakeFiles."
		rm -rf CMakeFiles 
	fi	

    echo -e "\033[32m============================ end ============================\033[0m"
	cd $CURR_DIR	
}

NAME_FLAG=0
MODULE_FLAG=0

#入参判断执行
if [ "3568" != "$BOARD_NAME" -a "3358" != "$BOARD_NAME" ]; then
	# 核心板判断
	echo "No support for "$BOARD_NAME
elif [ -z "$PROG_TYPE" -o "all" == "$PROG_TYPE" ]; then
	echo -e "\033[32mStart to make all:\033[0m"
	makeallapp
elif [ "lib" = "$PROG_TYPE" ]; then
	#编译lib
	if [ -z "$PROG_NAME" -o "all" == "$PROG_NAME" ]; then
		echo -e "\033[32mStart to make lib all:\033[0m"
		#编译所有lib
		makealllib
	elif [ "clean" == "$PROG_NAME" ]; then
	    echo -e "\033[32mStart to clean lib all\033[0m"
		#清除所有lib
		cleanalllib
	else
		for i in ${!LIB_ARRAY[@]}
		do
			if [ "$PROG_NAME" == "${LIB_ARRAY[$i]}" ]; then
				NAME_FLAG=1
                if [ "clean" == "$MODULE_NAME" ];then
                    #清除单个lib
                    cleanlib $i
                else 
                    #编译单个lib
                    makelib $i
                fi
				break
			fi
		done
		if [ 0 -eq $NAME_FLAG ]; then
			printfLibFun
		fi
	fi
elif [ "app" = "$PROG_TYPE" ]; then
	#编译app
	if [ -z "$PROG_NAME" -o "all" == "$PROG_NAME" ]; then
		echo -e "\033[32mStart to make app all:\033[0m"
		#编译所有app
		makeallapp
	elif [ "clean" == "$PROG_NAME" ]; then
	    echo -e "\033[32mStart to clean app all:\033[0m"
		#清除所有app
		cleanallapp		
	else
		for i in ${!APP_ARRAY[@]}
		do
			if [ "$PROG_NAME" == "${APP_ARRAY[$i]}" ]; then
				NAME_FLAG=1
                 if [ -z "$CLEAN_FLAG" ];then
                    #编译单个app
                    makeapp $i
                elif [ "clean" == "$CLEAN_FLAG" ];then
                    #清除单个app
                    cleanapp $i
                fi
				break
			fi
		done
		if [ 0 -eq $NAME_FLAG ]; then
			printfAppFun
		fi
	fi
elif [ "clean" = "$PROG_TYPE" ]; then
	echo -e "\033[32mStart to clean all:\033[0m"
	#清除所有lib和app
	cleanalllib
	cleanallapp	
else
	echo "Error input"
fi

cd $WORK_DIR