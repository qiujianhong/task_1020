/*********************************************************************
 * Copyright(c) 2007,南京新联电子仪器有限公司
 * 		All rights reserved.
 * 程序名称: eeprom.h
 * 版本号: 1.0
 * 功能: EEPROM接口说明
 * 其他: 
 **********************************************************************/

#ifndef _EEPROM_H_
#define _EEPROM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "filelock.h"

/* function delaration */

/***********************************************************************
  所属功能单元: eeprom
  功能描述: 初始化EEPROM，设置EEPROM保护管脚并使能写保护
  输入参数: pro 为1表示写保护，为0表示去保护
  输出参数: 无
  函数返回值: 0为成功，负数表示出错
************************************************************************/
int WriteProtectEEPROM(uint8 pro);

/***********************************************************************
  所属功能单元: eeprom
  功能描述: 读铁电或EEPROM
  输入参数: eeprom_id	要读的是哪一片eeprom芯片(0为铁电,1为EEPROM)
	   eeprom_add 片内区域起始地址	  
	   len	读取区域的长度（以Byte计）
  输出参数: ram_add 存放读取数据的内存缓冲区的起始地址 
  函数返回值: 正数为实际读取的字节数,负数表示出错
************************************************************************/
int ReadDataFromEEPROM(uint8 eeprom_id,uint8 *ram_add,uint32 eeprom_add,uint32 len);

/***********************************************************************
  所属功能单元: eeprom
  功能描述: 写铁电或EEPROM
  输入参数: eeprom_id	要写的是哪一片eeprom芯片(0为铁电,1为EEPROM)
	   eeprom_add 片内区域起始地址	  
	   len	写区域的长度（以Byte计）
	   ram_add 存放写入数据的内存缓冲区的起始地址	  
  输出参数: 无 
  函数返回值: 正数实际写入的字节数，负数表示出错
************************************************************************/
int WriteDataToEEPROM(uint8 eeprom_id,const uint8 *ram_add,uint32 eeprom_add,uint32 len);

#ifdef __cplusplus
}
#endif

#endif

