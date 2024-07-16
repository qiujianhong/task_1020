/*********************************************
 * Copyright(c) 2007,南京新联电子仪器有限公司
 *              All rights reserved.
 * 程序名称: interface.h
 * 版本号: 1.0
 * 功能: 声明了库函数用到的公共头文件
 * 其他:
***********************************************/

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/file.h> 
#include <fcntl.h>
#include <errno.h>

/* 自定义类型定义 */
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;


#include "ad.h"
#include "diproc.h"
#include "misc.h"
#include "ykyd.h"
#include "rtc.h"
#include "gpio.h"
#include "eeprom.h"
#include "vertion.h"
#include "filelock.h"
#include "att7022.h"
#include "ad73360.h"
#include "tc.h"
#include "wavplay.h"

#define printt(fmt, args...) do{printf("#%04d" fmt, getpid() , ## args); \
				fflush(stdout); } while(0)

#endif

