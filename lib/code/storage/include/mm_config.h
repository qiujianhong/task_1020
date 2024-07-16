/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限公司
             All rights reserved.

  程序名称: utils.c
  功    能：存储子系统配置
  开 发 人：zhangyu
  开发时间：2006-11-06

  版本历史:

  2006-11-06 :
    v1.0, zhangyu, 新建

***************************************************************************
*/

#ifndef __MM_CONFIG_H__
#define __MM_CONFIG_H__

#include "../../include/dataType.h"

#define EEPROM_SIM_EN 0 /* eeprom存储器模拟开关:
                         *  1-用文件模拟eeprom,
                         *  0-读写实际eeprom(仅ARM终端, PC版始终使用文件模拟)
                         */

#define MEM_ERR_LOGFILE "/mnt/log/mem.log"

#define PROJECTID   0     //客户(方案)编号
#define MAJOR       1     //主版本号，在对动态库功能实现重大改动后更改，比如：
                          //接口函数有重大变化，或者内部实现机制有重大改变等等。
#define MINOR       0     //次版本号，一般如果对动态库内部实现了小修小补后可以更改。

/* 8k eeprom空间使用分布:
   =========================

        终端运行及功能参数
              (部分)              其他用途(AC)    谐波HR   eeprom私有类数据
 网卡Mac地址     |                    |              |           |
    /\   ________|_______   __________|__________   _|__   ______|______
   /  \ /                \ /                     \ /    \ /             \
  |----|--------^---------|-----------------------|------|---------------|
    1  +   3k - 128Bytes            3k            + 256  +  2k-256Bytes
    2   \                                                   \
    8    \                                               \
    B     \--XPARABASE                                    \--PDATABASE
*/
#define   XPARABASE         0x80      //eeprom参数区起始字节地址
//#define   PDATABASE       0x1900    //私有类数据在EEPROM中的基地址
#define   PDATABASE         0x1A00    //私有类数据在EEPROM中的基地址


#define   EEPROM_SIZE_TOTAL 0x2000    //eeprom容量, 8k
//#define   REG_MAX_LEN       0x0c00    //eeprom最大注册长度, 2k
#define   REG_MAX_LEN       0x0B00    //eeprom最大注册长度, 2k-256Bytes

#define   XPARACHK_SIZE     1         //交互类参数项校验字节数
#define   XPARA_CHECKTYPE   1         //校验类型, 1- XPARA_SUMCHECK, 2- XPARA_CRC16, 3- XPARA_CRC32

#define   SD_MAX_MONTHS     48        //SD卡中最大存放的月数据目录数

#define   ERASE_CHAR        0xff      // 擦除字符，用于清除数据

#ifdef ARM
  #define   RAMDISK_DEVID     0x0e      //ramdisk的设备号, 0x0e是目前终端的ramdisk号
  #define   SDCARD_DEVID      0x802     //sdcard的设备号
#else
  #define   RAMDISK_DEVID     0x100     //ramdisk的设备号
  #define   SDCARD_DEVID      0x802     //sdcard的设备号
#endif

#define   XPARASHM_KEYID    100       //参数映射数据访问同步信息共享内存区key
#define   XMTRSHM_KEYID     102       //参数映射数据访问同步信息共享内存区key
#define   XPARASEM_KEYID    101       //参数映射数据访问同步信息信号量key

#define   PATH_MAX_LEN      256       //定义路径最大长度

#define   XPARA_DEL_MINID  JLM_LOCATION + 1 //清除终端运行参数时的最小ID号

#define   LOG_MAX_NUM       10000      //日志文件最大行数

/*
 * 终端参数保存信息(需要保存在eeprom中的参数内容)
 */

// 系统为终端保留了大概3KB左右的eeprom空间用来保存部分终端运行及功能参数,
// 我们提取这两种参数的前面一部分内容保存到eeprom中，下面两个宏为这两种参
// 数分配字节空间并保留一定的空间裕量. 此两项需要结合???PARA_EEPROM_MAXID,
// 并根据qtables.c文件中的对应长度内容调整(保证能够放的下就行)!
//
#define   RUNPARA_SIZE   1000      //运行参数占用字节数
#define   FUNCPARA_SIZE  0          //功能参数占用字节数

#define   RUNPARABASE  XPARABASE  // 运行参数eeprom基地址
#define   FUNCPARABASE RUNPARABASE + RUNPARA_SIZE  // 功能参数eeprom基地址

#define   RUNPARA_EEPROM_MAXID    JLM_LOCATION      //在EEPROM中保存的终端运行参数项最大ID号
#define   FUNCPARA_EEPROM_MAXID Tariff_SEG_ID         //在EEPROM中保存的终端功能参数项最大ID号

#endif //__mm_config_h__
