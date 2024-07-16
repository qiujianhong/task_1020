#ifndef __COMMFUN_H__
#define __COMMFUN_H__

#include "CtsAll.h"
#include "OopAll.h"

#define PATH_MAX_LEN 256
#define MINS_IN_DAY             86400           //每日的秒数
typedef struct
{
    uint8   Second;                             //BIN or BCD
    uint8   Minute;                             //BIN or BCD
    uint8   Hour;                               //BIN or BCD
    uint8   Day;                                //BIN or BCD
    uint8   Month;                              //BIN or BCD
    uint8   Year;                               //BIN or BCD
}DateTime_T;                                    //日期时间结构

typedef struct
{    
    uint8    ss;
    uint8    nn;
    uint8    hh;
    uint8    WW;
    uint8    DD;
    uint8    MM;
    uint8    YY;
}__attribute__ ((packed))YYMMDDWWhhnnss_TYPE;     // YYMMDDWWhhnnss结构体声明

//数据转换结构
typedef struct 
{   
    uint8   *srcBuf;    //数据
    uint32  srcLen;    //长度
    uint32  pipedelay; //10ms
 
    uint8   *dstBuf;    //数据
    uint32  dstLen;    //长度
    uint8   prtl;
    LinkLayerCtrl_t ctrl; //控制码
    uint8   num645;        //如果是多帧645报文则记录下有多少个报文
    uint8   datalen645[20];  //记录下每个645报文的长度
}FRAME_CONVERT_T;

extern void taskmng_cur_bin_time_get(DateTime_T *pTime);
extern void taskmng_cur_bcd_time_get(DateTime_T *pTime);
extern uint8 HexToBcd(uint8 hex);
extern void DateTimeSToChar(OOP_DATETIME_S_T *pDataTime_s,char *pTime);
//extern void memcpy_r(uint8 *de,uint8 *sr,uint16 len);
extern char commfun_HexToBcd(int hex);
extern int commfun_BcdToHex(char compress_bcd);
extern void DT_TimeSubInv(time_t *time,OOP_TI_T ti);
extern BOOL DT_Time2DateTimeHex(time_t linuxTime,DateTimeHex_t *dateTimeHex);
extern BOOL DT_DateTimeHex2Time(DateTimeHex_t *dateTimeHex,time_t *linuxTim);
extern BOOL DT_DateTimeHexIsValid(DateTimeHex_t *DateTime);
extern BOOL DT_IsLeap(uint16 year);
extern void DT_TimeAddInv(time_t *time,OOP_TI_T ti);
extern BOOL DT_DateTimeBCD2Time(DateTimeBcd_t *dateTimeBCD,time_t *linuxTime);
extern BOOL DT_DateTimeBCDIsValid(DateTimeBcd_t *DateTime);
extern int GetEnvValue(const char *env_name, int default_value);
extern uint16 XADR_GetEleNum(const uint8 *indata,uint8 *bytes);

extern BOOL CheckFF(uint8 *buf, uint16 len);
extern BOOL DT_Time2DateTimeBCD(time_t linuxTime,DateTimeBcd_t *dateTimeBCD);
extern BOOL DT_DateTimeHex2BCD(DateTimeHex_t *dateTimeHex,DateTimeBcd_t *dateTimeBCD);
extern time_t GetDaycntTime(uint8 DayCnt);
extern uint8 BcdToHex(uint8 bcd);
extern int GetDistanceBinTimeByDay(DateTime_T dt1, DateTime_T dt2);
extern time_t DateTime_TToSec(DateTime_T PointTime);
extern time_t commfun_rtc2time2(rtc_t src_rtc);
extern BOOL DT_DateTimeBCD2Hex(DateTimeBcd_t *dateTimeBCD,DateTimeHex_t *dateTimeHex);
extern void InterTaskReadRTC(rtc_t *prtc);
extern uint8  CalCs(uint8 *pBuf, uint16 len);
extern void AddGap(uint8 *pRecvBuf, uint8 offs, uint8  gapValue);
extern char *commfun_645_frame_get(char *buf, uint8 sync, uint32 buf_len,uint16 *reallen);
extern BOOL cmpOad(OOP_OAD_U sOad,OOP_OAD_U dOad);
extern void memrev(uint8* buff,uint16 len);
extern BOOL taskmng_oop_next_read_data_analysis(const uint8 *inData,uint16 *offset,uint16 inDataLen,enum tag_OOP_DATATYPE DataType,BOOL bWithType);

extern BOOL taskmng_oop_next_read_data_get(const uint8 *inData,uint16 *offset,uint16 inDataLen,enum tag_OOP_DATATYPE DataType,BOOL bWithType);

extern void ComposeBuf(uint8 *buf, int len, uint8 *comm_log);
extern uint16 get_meter_read_speed(uint8 baud);
extern void frame_printf(uint8* bufffer,uint32 len);
extern void WriteTimeLog(uint8 pipe,char* Time);
extern void commfun_printstring(uint8 *buf, int16 len);
extern long DT_TI2Seconds(OOP_TI_T ti);
extern OOP_DATETIME_S_T Time_tToDataTimeS(time_t tm);
extern OOP_COMDCB_T taskmng_com_dcb_get(uint8 comm);

extern void Pro_ReduceMeterGap(uint8 *pRecvBuf, uint8 offs, uint8 gapValue);
extern BOOL check_is_all_ch(uint8 *buf ,uint8 ch,uint16 len);
extern void MakeDescriptor(uint8 type,DESCRIPTOR_T Descriptor,char* buf);
extern DESCRIPTOR_T UnpackDescriptor(uint8 type,char* buf);
extern char* GetFileName(char* filepath,int8 pipe);
extern uint8 CharToNum(uint8 str);
extern void TimeT2RTC(time_t time,rtc_t *prtc);
uint8 oop_pipe_get(uint8 pipe,uint8 index);
uint8 Hex2Bcd(uint8 HexNum);
uint8 Bcd2Hex(uint8 BcdNum);
void CaculateWeekDay(YYMMDDWWhhnnss_TYPE *DesTime);
uint8 CalWeekday(int y, int m, int d);
OOP_COMDCB_T taskmng_com_dcb_get_new(uint8* comm);
BOOL DealRequestFrameFormat11(FRAME_CONVERT_T *frame);
BOOL DealRequestFrameFormat12(FRAME_CONVERT_T *frame);
BOOL DealRequestFrameFormat13(FRAME_CONVERT_T *frame,OOP_TSA_T *tsa);
BOOL DealRequestFrameFormat14(FRAME_CONVERT_T *frame,OOP_TSA_T *tsa);
BOOL DealRequestFrameFormat15(FRAME_CONVERT_T *frame,OOP_TSA_T *tsa);
BOOL DealRequestFrameFormat51(FRAME_CONVERT_T *frame,OOP_TSA_T *tsa);
BOOL DealRequestFrameFormat52(uint8 pipe,FRAME_CONVERT_T *frame,OOP_TSA_T *tsa);
BOOL DealRequestFrameFormat53(uint8 pipe,FRAME_CONVERT_T *frame);
BOOL Convert_Request_Frame(uint8 pipe,FRAME_CONVERT_T *frame,uint8 frametype,OOP_TSA_T *tsa);
void Convert_Response_Frame(uint8 pipe,FRAME_CONVERT_T *frame,uint8 frmetype);
int uart_baud_get(uint8 baud);
BOOL DealRequestFrameFormat31(FRAME_CONVERT_T *frame);
void taskmng_current_time_get(struct timeval *tv);
void taskmng_stat_time_write(uint8 *addr,struct timeval *msgsendtv,struct timeval *downsendtv,struct timeval *downrecvtv);
uint16 taskmng_stat_fun_0038_unpack(uint8 *buf,uint16 len,struct timeval *sendtv);
void taskmng_comdcb_default(OOP_COMDCB_T *comdcb);

#endif
