/*-
****************************************************************************************
 * Copyright (c) 2006,南京新联电子仪器有限责任公司
 *            All rights reserved.
 * 程序名称：metlgr.h
 * 版本号：1.0
 * 功能：国标表动态库函数声明
 * 开发人：尤斌
 * 开发时间：2006年12月22日
 * 修改者:
 * 修改时间:
 * 修改简要说明:
 * 其他：
*****************************************************************************************
-*/
//#ifndef _METSTD2007_H
#define _METSTD2007_H
 #include "CtsAll.h"
/*解帧函数宏定义*/
#define AMR_MT_PRTL_MIN_LEN_188             ((uint8)15)         //188规约水表最小帧长   
#define AMR_MT_PRTL_CTL_188             ((uint8)0x81)       //188规约累计流量功能码(固定)
#define AMR_LED_DATALEN_188             ((uint8)22)        //利尔达水表数据域长度
#define AMR_MT_RECV_OVER_TIME_188       ((int16)5000)     /* 接收等到超时时间 150 * 20ms */
#define AMR_INVALID_CHILD_INDEX_188     ((uint8)0xFF)    /* 188规约子数据标识无效索引 */
#define AMR_MT_MAX_RETRY_CNT_188        ((uint8)3)       /* 最大重试次数 */
#define AMR_MAX_FAILED_DI_NUM_188       ((uint8)3)       /* 测量点抄表失败数据标志个数 */




/* 解帧函数返回值定义宏 */
#define AMR_CHECK_RESULT_SUCCEED_188        ((int8)0)
#define AMR_CHECK_RESULT_FAILUER_188        ((int8)-1)
#define AMR_CHECK_RESULT_CONFIRM_188         ((int8)1)
#define AMR_CHECK_RESULT_DENY_188            ((int8)2)

/* 数据处理定义宏 */
#define AMR_LF_ADDR_SUCCEED_188             ((int8)1)  //所解帧地址有效
#define AMR_LF_ADDR_FAILUER_188             ((int8)-1) //所解帧地址无效

extern uint16 CaclBiasByOad_188(uint8 type, OOP_OAD_U subOad, uint8 *pLen);

extern CB_STATE Cb_State_Init_Std188(S_COMM_INFO_MET *Comm_Info_Oop,uint8 pipe,cbMtrPara   *MtrPara);

extern void Meter_188_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff);
extern void Prepare_std188_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara);
extern void Meter_Std188_Read_Item(SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 *DI07Num,uint32 *DI07Buff);
extern  uint16 STD188_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
extern int8 CJT188_CheckRspFrame(uint8 pipe, uint8 *pBuf, uint16 rcvLen, uint16 DI, S_COMM_INFO_MET* pCommInfo, uint16 *offset,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo);
void Meter_Std188_Read_Item_New(SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 *DI07Num,uint32 *DI07Buff);
void Meter_188_Recvive_Data_New(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff);
uint16 STD188_AUTK_Recv_New(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
void STD188_MakeFrameEx_New(uint8 *pMetAddr, uint32 fraDI, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo);
uint16 CJT188_AUTK_Recv_Deal_New(uint8 pipe, uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo);
uint16 CaclBiasByOad_188_new(uint8 type, OOP_OAD_U subOad, uint8 *pLen, OOP_OAD_U mainoad);




//#endif

