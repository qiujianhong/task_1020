
/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#ifndef __AC_METER_LIB_H__
#define __AC_METER_LIB_H__

#include "ac_redef.h"



#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

#define AC_LEN_TYPE_1    (1)               //交采模块应答报文长度1字节(标准645)
#define AC_LEN_TYPE_2    (2)               //交采模块应答报文长度2字节(内部协议)

/*******************************************************************************
* 调节日志路径
*******************************************************************************/

//调节日志路径
extern char	   *gDownRecordPath[MAX_ALLOWED_DOWN_NUM];  //调节日志路径

/*******************************************************************************
* 共享缓存队列
*******************************************************************************/
extern LockQueue_T *pDealRecvDown[RUN_DOWN_NUM]; 					 //抄表线程下行接收共享内存指针
extern LockQueue_T *pDealSendDown[RUN_DOWN_NUM]; 					 //抄表线程下行发送共享内存指针

extern LockQueue_T *pUdpRecvDown;					     //UDP透传下行接收共享内存指针
extern LockQueue_T *pUdpSendDown;					     //UDP透传下行发送共享内存指针

extern LockQueue_T *pTimeRecvDown;					     //对时下行接收共享内存指针
extern LockQueue_T *pTimeSendDown;					     //对时下行发送共享内存指针

extern LockQueue_T *pModuleRecvUp;	                     //模组信息上行接收共享内存指针
extern LockQueue_T *pConfirmDown; 					     //回复交采模块应答共享内存指针

//485串口通信报文
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define AC485COM_FMT_TRACE(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 33, gDownRecordPath[pipe], 0, NULL, 0, fmt, ##__VA_ARGS__)
#define AC485COM_FMT_DEBUG(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 33, gDownRecordPath[pipe], 1, NULL, 0, fmt, ##__VA_ARGS__)
#define AC485COM_FMT_LOGCC(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 33, gDownRecordPath[pipe], 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define AC485COM_BUF_TRACE(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 33, gDownRecordPath[pipe], 0, buf, len, fmt, ##__VA_ARGS__)
#define AC485COM_BUF_DEBUG(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 33, gDownRecordPath[pipe], 1, buf, len, fmt, ##__VA_ARGS__)
#define AC485COM_BUF_LOGCC(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 33, gDownRecordPath[pipe], 2, buf, len, fmt, ##__VA_ARGS__)



/*******************************************************************************
* 全局变量
*******************************************************************************/
extern PAST_TIME_FLAG_TYPE	  gTimeFlag;		  // 过时间标志
extern PAST_TIME_FLAG_TYPE   gSaveTimeFlag;		  // 过时间标志
extern const uint8 HaveDays[13];


/*******************************************************************************
* 函数声明
*******************************************************************************/

extern void ACDataSave(uint8 bFlag);
extern void printfBuff(uint8 *buff, uint16 len, uint8 flag);
extern uint8  CalCs(uint8 *pBuf, uint16 len);
extern char *commfun_memchr_gb(char *buf, uint8 sync, uint32 buf_len);
extern char *commfun_memchr_gb_send(char *buf, uint8 sync, uint32 buf_len);
extern char *commfun_memchr_gb_head(char *buf, uint8 sync, uint32 buf_len);
extern uint8* get_07ptr_from_buf(uint8* buf, uint32 maxLen, uint32* offset, uint32* outLen);
extern int32 PlcWriteQueue(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len);
extern int32 PlcReadQueue(LockQueue_T *pLockQueue, uint8 *buf);

/*******************************************************************************
*
*************************************原有的分界线******************************************
*
*******************************************************************************/


// 函数原型的声明
extern uint32 BinTransToBCD(uint32 BinData, uint8 *Bcdbuff, uint32 Binlength);
extern uint32 Byte4BcdToBin(uint8 *bcdpoint);
extern void AddCRCFun(uint8 *pData, uint16 Len);
extern uint8 GetCRCFun(uint8 *pData, uint16 Len);
extern int CheckCRCFun(uint8 *pData, uint16 Len);
extern void BcdDecSubtract(uint8 *Minuend, uint8 *Subtrahend,uint8 *Difference,uint8 srcLen);
extern void BcdAddition(uint8 *summand, uint8 *addend,uint8 *sum,uint8 srcLen);
extern uint8 BCDtoBIN(uint8 BCDnum);
extern uint16 DoubleBinToBCD(uint16 BinNum);
extern uint8 HexToBcdFun(uint8 hex);
extern uint32 MakeTimeFun(REAL_TIME_TYPE *ptm);

extern uint8 Bcd2Hex(uint8 BcdNum);        // BCD转HEX
extern uint16 Bcd2Hex2(uint16 BcdNum);     // 2字节BCD转HEX
extern uint32 Bcd2Hex4(uint32 BcdNum);     // 4字节BCD转HEX
extern uint8 Hex2Bcd(uint8 HexNum);        // HEX转BCD
extern uint16 Hex2Bcd2(uint16 HexNum);     // 2字节HEX转BCD
extern uint32 Hex2Bcd4(uint32 HexNum);     // 4字节HEX转BCD
extern void HexToBcdSign(uint8 *DesBCD, BYTE4_INT32_TYPE SrcHEX, uint8 DataLen, uint8 DataStyle); // 带方向位HEX到BCD码的转换
extern void BcdToHexSign(BYTE4_INT32_TYPE *DesHex, uint8 *SrcBCD, uint8 DataLen, uint8 DataStyle);// 带方向BCD位到HEX码的转换
extern void CaculateWeekDay(YYMMDDWWhhnnss_TYPE *DesTime);            // 从指定时间获得周信息
extern int8 CompareTime(YYMMDDhhnnss_TYPE DesTime, YYMMDDhhnnss_TYPE SrcTime);
extern void TimeManageInit(void);          // 时间管理初始化
extern void TimeManageStart(void);         // 时间标志管理开始
extern void TimeManageEnd(void);          // 时间标志管理结束
extern void SaveTimeManageInit(void);          // 时间管理初始化
extern void SaveTimeManageStart(void);         // 时间标志管理开始
extern void SaveTimeManageEnd(void);          // 时间标志管理结束
extern uint8 CheckBcdValidity(uint8 *SrcData, uint8 DataLen);      // 检查BCD码有效性
extern uint8 CheckTimeValidity(uint8 *SrcData, uint8 DataLen);     // 检查时间有效性
extern uint32 Time2Sec(YYMMDDWWhhnnss_TYPE SrcData);               // YYMMDDWWhhnnss_TYPE转换为秒数
extern time_t TimetoSec(YYMMDDWWhhnnss_TYPE SrcData);               // YYMMDDWWhhnnss_TYPE转换为秒数
extern void GetNextHour(YYMMDDhhnnss_TYPE *DesDate, uint32 HourCnt, uint8 BcdFlag); // 获得若干时后时间
extern void GetLastMin(YYMMDDhhnnss_TYPE *DesDate, uint32 MinCnt, uint8 BcdFlag);
extern void GetNextMin(YYMMDDhhnnss_TYPE *DesDate, uint32 MinCnt, uint8 BcdFlag);
extern uint8 CalWeek(const uint8 *const pTime, uint8 Format);      // 根据时间计算星期几
extern void BcdX_To_BinY(uint8 X, uint8 Y, uint8 *DesBin, uint8 *SrcBcd, uint16 Multi);
extern void BcdX_To_SBinY(uint8 X, uint8 Y, uint8 *DesBin, uint8 *SrcBcd, uint16 Multi);
extern uint32 ChangeLenToBuff(const uint16 len, uint16 *IndexID, uint8 *buff);
extern uint32 BuffToChangeLen(const uint8 *buff, uint16 *IndexID, uint16 *len);
extern void systm_to_ooptm(time_t sys_tm, OOP_DATETIME_S_T *poop_time);
extern void ooptm_to_systm(OOP_DATETIME_S_T oop_time, time_t *psys_tm);
extern uint32 Uint16To32(uint16 front, uint16 rear);
extern void current_tm_to_ooptm(OOP_DATETIME_S_T *poop_time);

#ifdef __cplusplus
}
#endif

#endif // __AC_METER_LIB_H__

