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
#include "CtsRedef.h"


#ifndef _MET_DLMS_H
#define _MET_DLMS_H

#if OS_METDLMS_EN == 1

//#ifdef FIRST_DEFINE_METPARAM

//#else
//extern cbMetPara SD_DLMSPara;
//#endif

//
//  BER 编码器
//
#define MAX_BER_CODE_LEN          100

typedef struct{
   unsigned int   pos;
   unsigned int   len;
   unsigned char  bercode[MAX_BER_CODE_LEN];
}BER;

//
// A-XDR编码器
//

typedef struct{
   unsigned int  pos;  //当前编码位置
   unsigned int  len;
   unsigned char code[MAX_BER_CODE_LEN];
}A_XDR;

//
// BITSTRING
//
typedef struct{
  unsigned char unused;  //BITSTRING的最后字节中不使用的比特数
  unsigned char bits[5]; //最大40个比特位
  unsigned char bytelen;  //使用字节数
}BITSTRING;

//
//COSEM OBJECT_IDENTIFIER
//
typedef struct{
   unsigned char object_identifier_tag;
   unsigned char object_identifier_len;
   unsigned char object_id[7];
}COSEM_OID;

typedef struct{
	uint8	year_h;
	uint8	year_l;
	uint8	month;
	uint8	day_of_month;
	uint8	day_of_week;
	uint8	hour;
	uint8	min;
	uint8	sec;
	uint8  hundredths;
	uint8  deviation_h;
	uint8  deviation_l;//Min. from local time to GMT 0x8000 = not specified 
	uint8  clockstatus;//Always equal to attribute status 
}__attribute__ ((packed))Date_Time_t_dlms;

typedef BER * PBER;
typedef A_XDR *PAXDR;


//uint32 DLMS_PERIOD;//负荷曲线捕获周期

typedef struct 
{
    OOP_OAD_U      oad;//oad_id.c 中 OopObjectList[]的索引号,索引号的名字  enum meter_oad_id
    uint8       oBis[6];        //OBIS
    uint8       offset;         //偏移
    uint16      ShortName;      //短名
    uint8       ClassNo;        //自定义的类号
    uint8       DINum;/*-表示一帧报文能够读取的数据ID数量-*/
    uint8       dataNum;
    uint8       srcDataLen;//dlms规约数据项的长度
    uint8       srcDecLen;//小数位数 ，具体使用见 MET_DLMS_EN_BIN2_To_WHOLENUM
    uint8       rate;
}MET_PRTL_DLMS;




#define CLASS_UNIVERSAL           0x00

#define CLASS_APPLICATION         0x40
#define CLASS_CONTEXT_SPECIFIC    0x80
#define ENCODE_CONSTRUCTED        0x20


//CB_RET btasknew_DlmsMetAutkDealEx(S_cbTaskQueue *pPipeCbTaskQueue, enum_cbtask_id taskid); 
void JFPGtoFPGJ(uint16 metsn,uint8 WriteFlag,uint16 srcid);

#endif

#endif
//void Prepare_Dlms_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara);
extern void Meter_Dlms_Hnadsend(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara);
extern void Meter_Dlms_Hnadrecv(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara);
extern void Prepare_dlms_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara);

extern uint8 DLMS_AUTK_Cmd(uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, cbMtrPara *MtrPara, S_COMM_INFO_MET *Comm_Info_dlms,SCHEME_TASK_RUN_INFO *pCbTask,CSD_TASK_INFO  *csd);
void Meter_Dlms_Recv(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara);
void Meter_Dlms_Send(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara);
void  DLMS_HDSK_Cmd_Init(S_COMM_INFO_MET *Comm_Info_dlms);
void AnalysisIfFrzDataFromMeter(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *pdlms_comm);
void Meter_Dlms_Frzsend(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara);
uint16 get_DLMS_Association_OBJ(S_COMM_INFO_MET *pdlms_comm,const uint8 *in_obis);
void DLMS_MakeFrzFrame(uint8 *pMetAddr, uint8 Class, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, S_COMM_INFO_MET *pdlms_comm);
int DLMS_saveFrzParaAndData(S_COMM_INFO_MET *pdlms_comm,uint8 *recvBuf, uint16 recvLen, uint8 *met_addr, uint8 * RRFlag,SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo);
void  DLMS_Frz_Cmd_Init(S_COMM_INFO_MET *pdlms_comm,SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo);
void Meter_Dlms_Frzrcv(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara);
void DLMS_AUTK_Recv(uint8 pipe, uint8* pbuf,uint16 revlen,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,cbMtrPara *MtrPara,DateTimeBcd_t *StoreTime,OOP_OAD_U oad,uint8 dlsnum);
CB_STATE Deal_Receive_Result_Dlms(S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, BOOL actResult,uint8 pipe,OOP_OAD_U Suboad);
void  Dlms_ToNextReadItem(BOOL NextCSDFlg,S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe,OOP_OAD_U Suboad);

