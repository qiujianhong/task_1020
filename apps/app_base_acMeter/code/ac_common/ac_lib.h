
/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
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

#define AC_LEN_TYPE_1    (1)               //����ģ��Ӧ���ĳ���1�ֽ�(��׼645)
#define AC_LEN_TYPE_2    (2)               //����ģ��Ӧ���ĳ���2�ֽ�(�ڲ�Э��)

/*******************************************************************************
* ������־·��
*******************************************************************************/

//������־·��
extern char	   *gDownRecordPath[MAX_ALLOWED_DOWN_NUM];  //������־·��

/*******************************************************************************
* ���������
*******************************************************************************/
extern LockQueue_T *pDealRecvDown[RUN_DOWN_NUM]; 					 //�����߳����н��չ����ڴ�ָ��
extern LockQueue_T *pDealSendDown[RUN_DOWN_NUM]; 					 //�����߳����з��͹����ڴ�ָ��

extern LockQueue_T *pUdpRecvDown;					     //UDP͸�����н��չ����ڴ�ָ��
extern LockQueue_T *pUdpSendDown;					     //UDP͸�����з��͹����ڴ�ָ��

extern LockQueue_T *pTimeRecvDown;					     //��ʱ���н��չ����ڴ�ָ��
extern LockQueue_T *pTimeSendDown;					     //��ʱ���з��͹����ڴ�ָ��

extern LockQueue_T *pModuleRecvUp;	                     //ģ����Ϣ���н��չ����ڴ�ָ��
extern LockQueue_T *pConfirmDown; 					     //�ظ�����ģ��Ӧ�����ڴ�ָ��

//485����ͨ�ű���
//��ӡ��ʽ����Ϣ��TRACE�ǲ���¼��־��,DEBUG�Ǳؼ���־��,LOGCC���ܿؼ���־��
#define AC485COM_FMT_TRACE(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 33, gDownRecordPath[pipe], 0, NULL, 0, fmt, ##__VA_ARGS__)
#define AC485COM_FMT_DEBUG(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 33, gDownRecordPath[pipe], 1, NULL, 0, fmt, ##__VA_ARGS__)
#define AC485COM_FMT_LOGCC(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 33, gDownRecordPath[pipe], 2, NULL, 0, fmt, ##__VA_ARGS__)
//��ӡ��������buffer����,��ʽΪÿ2λ16��������һ��
#define AC485COM_BUF_TRACE(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 33, gDownRecordPath[pipe], 0, buf, len, fmt, ##__VA_ARGS__)
#define AC485COM_BUF_DEBUG(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 33, gDownRecordPath[pipe], 1, buf, len, fmt, ##__VA_ARGS__)
#define AC485COM_BUF_LOGCC(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 33, gDownRecordPath[pipe], 2, buf, len, fmt, ##__VA_ARGS__)



/*******************************************************************************
* ȫ�ֱ���
*******************************************************************************/
extern PAST_TIME_FLAG_TYPE	  gTimeFlag;		  // ��ʱ���־
extern PAST_TIME_FLAG_TYPE   gSaveTimeFlag;		  // ��ʱ���־
extern const uint8 HaveDays[13];


/*******************************************************************************
* ��������
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
*************************************ԭ�еķֽ���******************************************
*
*******************************************************************************/


// ����ԭ�͵�����
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

extern uint8 Bcd2Hex(uint8 BcdNum);        // BCDתHEX
extern uint16 Bcd2Hex2(uint16 BcdNum);     // 2�ֽ�BCDתHEX
extern uint32 Bcd2Hex4(uint32 BcdNum);     // 4�ֽ�BCDתHEX
extern uint8 Hex2Bcd(uint8 HexNum);        // HEXתBCD
extern uint16 Hex2Bcd2(uint16 HexNum);     // 2�ֽ�HEXתBCD
extern uint32 Hex2Bcd4(uint32 HexNum);     // 4�ֽ�HEXתBCD
extern void HexToBcdSign(uint8 *DesBCD, BYTE4_INT32_TYPE SrcHEX, uint8 DataLen, uint8 DataStyle); // ������λHEX��BCD���ת��
extern void BcdToHexSign(BYTE4_INT32_TYPE *DesHex, uint8 *SrcBCD, uint8 DataLen, uint8 DataStyle);// ������BCDλ��HEX���ת��
extern void CaculateWeekDay(YYMMDDWWhhnnss_TYPE *DesTime);            // ��ָ��ʱ��������Ϣ
extern int8 CompareTime(YYMMDDhhnnss_TYPE DesTime, YYMMDDhhnnss_TYPE SrcTime);
extern void TimeManageInit(void);          // ʱ������ʼ��
extern void TimeManageStart(void);         // ʱ���־����ʼ
extern void TimeManageEnd(void);          // ʱ���־�������
extern void SaveTimeManageInit(void);          // ʱ������ʼ��
extern void SaveTimeManageStart(void);         // ʱ���־����ʼ
extern void SaveTimeManageEnd(void);          // ʱ���־�������
extern uint8 CheckBcdValidity(uint8 *SrcData, uint8 DataLen);      // ���BCD����Ч��
extern uint8 CheckTimeValidity(uint8 *SrcData, uint8 DataLen);     // ���ʱ����Ч��
extern uint32 Time2Sec(YYMMDDWWhhnnss_TYPE SrcData);               // YYMMDDWWhhnnss_TYPEת��Ϊ����
extern time_t TimetoSec(YYMMDDWWhhnnss_TYPE SrcData);               // YYMMDDWWhhnnss_TYPEת��Ϊ����
extern void GetNextHour(YYMMDDhhnnss_TYPE *DesDate, uint32 HourCnt, uint8 BcdFlag); // �������ʱ��ʱ��
extern void GetLastMin(YYMMDDhhnnss_TYPE *DesDate, uint32 MinCnt, uint8 BcdFlag);
extern void GetNextMin(YYMMDDhhnnss_TYPE *DesDate, uint32 MinCnt, uint8 BcdFlag);
extern uint8 CalWeek(const uint8 *const pTime, uint8 Format);      // ����ʱ��������ڼ�
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

