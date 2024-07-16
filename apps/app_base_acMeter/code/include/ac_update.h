/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 交采升级
* @date： 2019-10-25
*********************************************************************
*/

#ifndef __AC_UPDATE_H__
#define __AC_UPDATE_H__

#include "ac_redef.h"

//用于拓扑模块升级的特殊逻辑地址
#define TOPO_LOGIC_ADDR       197

#define UPDATE_PATH_WITH_VER  "/data/app/acMeter/upFile/"
#define UPDATE_FILE_NAME      "jc_update_file"

extern uint8 gVerBuf[8];              // 存在交采版本信息
extern uint8 gVerBufLast[8];          // 存在上版交采版本信息 
extern uint8 gDateBuf[8];             // 存在交采版本日期信息
extern uint8 gDateBufLast[8];         // 存在上版交采版本日期信息
extern uint8 gVerSBuf[8];             // 存在交采小版本信息 
extern uint8 gVerSBufLast[8];         // 存在上版交采小版本信息 
extern uint8 gJCUpdateFileMark;       // 升级文件标识全局标记. 0x01:控制模块; 0x02:遥信模块;0x03:485模块;0x04:交采模块

extern int ac_update_file_close(int* Soft_fd);
extern int IsCloseInnerDelayState(int Condition, int* Cnt, int CntMax);
extern int JcUpdateDeal(int32 fd_spi, BOOL isGetVer);
extern BOOL isJCUpdateRunning(void);
extern BOOL isJCNeedGetVer(void);
extern void ClearCommRecvBuff(int32 fd);

#endif