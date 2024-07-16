/*
*********************************************************************
* Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved. 
* @brief：   公共头文件
* @date：    2018/10/24
* @history： 
*********************************************************************
*/

#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
//-------------------标准的头文件--------------------------------
#include <stdio.h>       //标准输入输出定义
#include <stdlib.h>      //标准函数库定义
#include <unistd.h>      //Unix 标准函数定义
#include <sys/types.h>   //系统类型定义
#include <sys/stat.h>    //系统状态定义
#include <fcntl.h>       //文件控制定义
#include <termios.h>     //PPSIX 终端控制定义
#include <errno.h>       //错误号定义
#include <dlfcn.h>       //动态库控制定义
#include <string.h>      //字符处理函数定义
#include <sys/time.h>    //时间函数
#include <time.h>        //时间函数
#include <pthread.h>     //线程函数库定义
#include <sys/ipc.h>     //信号量
#include <signal.h>      //系统信号定义
#include <sys/file.h>    //文件系统定义
#include <semaphore.h>   //信号量
#include <sys/sem.h>     //SYSTEM V信号量
#include <linux/rtc.h>   //系统时间相关
#include <sys/shm.h>     //共享内存
#include <sys/mman.h>    //内存管理声明
#include <limits.h>      //定义各种数据类型最值的常量
#include <dirent.h>      //目录操作
#include <stdarg.h>      //标准参数头文件，以宏的形式定义变量参数列表。主要说明了一个类型（va_list）和3个宏（va_start
#include <assert.h>      //提供断言，assert(表达式)
#include <libgen.h>      //模式匹配函数定义
#include <ctype.h>       //字符测试函数
#include <getopt.h>      //处理命令行参数
#include <netinet/in.h>  //INTERNET地址族
#include <arpa/inet.h>   //INTERNET定义
#include <sys/socket.h>  //套接字
#include <sys/wait.h>    //进程控制
#include <sys/select.h>  //Select函数
#include <sys/reboot.h>  //重启
#include <sys/msg.h>     //消息队列
#include <math.h>        //定义数学函数
#include <sys/ioctl.h>   //杂项操作
#include <sched.h>       //执行调度
#include <linux/netlink.h>//内核态和用户态进程之间进行数据传输的特殊的IPC机制
#include <asm/types.h>    //对一些特殊类型的定义
#include <linux/types.h>   //对一些特殊类型的定义
#include <stdint.h>
#include <stdbool.h>
#include <sys/sysinfo.h>  //获取系统相关信息的结构体
#define LINUX
#endif


#include "dataCommon.h"     // 通用数据文件
#include "errCode.h"        // 通用错误码
#endif
