/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 698交采升级
* @date： 2019-10-25
*********************************************************************
*/

#ifndef __AC_UPDATE698_H__
#define __AC_UPDATE698_H__

#include "ac_redef.h"

//升级时每帧的长度
#define   PACKLEN      2048

//07和698升级自适应
#define   UPDATE_SELF_ADAPTION   1

typedef enum
{
    CMD698_IDLE = 0,                            //开始初始化
    CMD698_START_SEND,                          //启动传输
    CMD698_START_RECV,                          //启动传输
    CMD698_SEND,                                //升级发送
    CMD698_RECV,                                //升级接收
    CMD698_RETRY,                               //发送尝试
    CMD698_DELAY,                               //升级后延时
    CMD698_FAIL,	                            //升级失败
    CMD698_SUC,	                                //升级成功
    CMD698_VER_SEND,                            //发版本号
    CMD698_VER_RECV,                            //收版本号
    CMD698_END,                                 //升级结束
}JC_UPDATE698_DEAL_T;                           //升级状态字

typedef struct _JC_UPDATE698_CFG_T
{
    uint8   iUpdateState;                       //升级状态: 0x00 没有升级任务 0x55 暂停升级 0xAA 正在升级
    uint16  CurJcIndex;                         //当前升级交采索引
    uint8   CollecterNum;                       //本次地址文件的采集器总数
    uint32  UpdateSeq;                          //当前升级的帧序列
    uint16  iUpdateSuccCnt;                     //升级成功个数
    int     iUpdateCnt;                         //升级轮次计数
    uint8   UpdateFlag;                         //升级失败标志   0:未配置；1:成功；2:失败
    uint8   ClearFlag;                          //首帧清除标志   0:未发送清除    1:已发送清除
    uint32  transStatusNum;                     //传输状态字长度
    uint8   transStatus[256];                   //传输状态字
}JC_UPDATE698_CFG_T;                            //升级配置结构体

extern JC_UPDATE698_CFG_T g_iUpdate698Cfg;

#if UPDATE_SELF_ADAPTION == 1
extern BOOL gIsUpdate698;
#endif

extern int ac_get_698ver(int32 *pfd);
extern int ac_update698_proc(int32 *pfd);
extern int ac_get_698ver_bak(int32 *pfd);

#if UPDATE_SELF_ADAPTION == 1
BOOL ac_is_support_update698(int32 fd_spi);
#endif

#endif