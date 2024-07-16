#ifndef _MDL_COMMON_H_
#define _MDL_COMMON_H_

#include "mdlmgr.h"
#ifdef __cplusplus
extern "C"{
#endif

//22版新协议(2022年6月份)
#define PRTL_NEW_ENABEL 1

#define POWER_ON  1
#define POWER_OFF 0

typedef struct 
{
    uint16  year;
    uint8   month;
    uint8   day_of_month;
    uint8   day_of_week;
    uint8   hour;
    uint8   minute;
    uint8   second;
    uint16  millisec;
}MDL_DATE_TIME_T;

typedef struct
{
    char  deviceName[30];       /*设备名 */
    void *pDev;
}MDL_COMM_PARA;

typedef struct
{
    uint8   ctrlflag;      //开启标志
    time_t  ctrlime;       //开启秒数
}MOD_CTRL_BUZZER_T;

extern INFO_DATA_LIST_T  gInfoDataList_mdl;                             //收到的消息缓存区列表
extern pthread_mutex_t   ActiveInfoIndexLk;
extern InfoDataQueue_T   InfoDataQueue;                                 //已处理mqtt消息帧信息
extern LockQueue_T       LockRecvBuf ;                                  //mqtt接收缓冲区
extern LockQueue_T       LockSendBuf[mdlnum];                           //mqtt发送缓冲区

extern uint8  BINtoASCII(uint8* InBuf, uint16 len, char* OutBuf, uint16* outlen);

//extern void memcpy_r(uint8 *de,uint8 *sr,uint16 len);
extern void PrintInfoDetail(MSG_INFO_T *stMsgInfoSend);

extern void writePlclog(const char * filepath, const char * fmt, ...);

extern void ComposeBuf(uint8 *buf, int len, uint8 *comm_log);

extern void PrintBuf(uint8 *buf, int len);

extern uint16 GetInfoIndex();

extern int8 CreateTaskByInfo(void);

extern void PrintBuf(uint8 *buf, int len);

extern int32 PlcWriteQueue(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len);

extern int32 PlcReadQueue(LockQueue_T *pLockQueue, uint8 *buf);

extern int32 GetTimeNow(int32 usbNo, uint8* pData, uint8* dataLen);

extern int ModulePowerOff(int moduleNo, uint8 OnorOff);

extern void mdl_open_buzzer(uint32 delay);

extern void refresh_status_bar(uint32 time, char *str);

extern int mdl_comm_init(MDL_COMM_PARA *pCommPara);

extern void mdl_comm_uninit(MDL_COMM_PARA *pCommPara);

extern int mdl_comm_send(MDL_COMM_PARA *pCommPara, uint8* pBuffer, int nBufLen);

extern int mdl_comm_recv(MDL_COMM_PARA *pCommPara, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime, uint8 endbyte);

#ifdef __cplusplus
}
#endif


#endif