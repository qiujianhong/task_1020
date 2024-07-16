
/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#ifndef __AC_METER_MODULE_H__
#define __AC_METER_MODULE_H__

#include "ac_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif


/*******************************************************************************
* 全局变量
*******************************************************************************/

//5个模组的信息
extern DEV_HAL_INFO  gDevHalInfo[MAX_ALLOWED_DOWN_NUM];  //遥信脉冲模块HAL信息
extern BOOL gQueryAllModuleFlag;  //程序刚启动时, 读取所有模块信息
extern MODULE_INFO_T gModuleInfo[MAX_ALLOWED_DOWN_NUM];  //5个模组的信息(实际模组顺序及信息)
extern uint8		  gReloadModuleFlag[6];  //首位：0表示无变化, 1表示变化, 0xaa表示初始化, 后面5个0/1表示模组变化状态

//模块插拔重新获取设备
extern uint8 gReloadDevFlag[MAX_ALLOWED_DOWN_NUM];       //模块插拔重新获取设备

/*******************************************************************************
* 枚举和结构体
*******************************************************************************/


/*******************************************************************************
* 函数声明
*******************************************************************************/
extern void ModuleInfoDeal(uint8 ModuleIndex, uint8 *RecvBuf, uint32 RecvLen);
extern int GetModuleInfo(uint8 ModuleIndex);
extern int GetAllModuleInfo();
extern int UpdateModuleInfo();
extern void ModuleEventFun(uint8* buf, uint16 len);
extern int ModuleEventDeal();


#ifdef __cplusplus
}
#endif

#endif // __AC_METER_MODULE_H__

