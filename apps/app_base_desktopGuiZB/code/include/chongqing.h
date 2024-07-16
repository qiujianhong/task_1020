#ifndef _CHONGQING_H_
#define _CHONGQING_H_

#ifdef AREA_CHONGQING
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    CQ_TRANS_INIT = 0,  //初始态
    CQ_TRANS_SEND,      //发送
    CQ_TRANS_SUCC,      //成功
    CQ_TRANS_FAIL,      //失败
}CQ_TRANS_STATUS_E;

//指定测量点抄表用的结构体
typedef struct
{
    uint16 pn;      //测量点号
    uint8 prtl;         //规约 给搜表结果抄表用
    OOP_TSA_T tsa;  //对应的地址 接收时用来对比
    uint32 sendtime;    //透传发送的时间
    uint16 sendIndex;   //发送的消息索引
    CQ_TRANS_STATUS_E status;   //状态 0 初始 1已发送 2成功 3失败
    OOP_DATETIME_S_T meterTime;        //透传的电表时间
    uint32 meterData;           //透传的有功总示值
}CQ_METER_TRANS_T;  

//所有测量点抄表用的结构体
typedef struct
{
    uint8 meterindex;
    uint8 meternum;
    CQ_METER_TRANS_T transinfo[2040];  
}CQ_ALL_METER_TRANS_T;

//搜表结果用的 
typedef struct
{
    uint16 num;
    OOP_TSA_T tsa[2040];
    uint8 prtl[2040]; 
}CQ_METER_SRARCH_RESULT_T;



int meter_trans_amr_recv_check(MESSAGE_INFO_T *pMsgInfo);
int meter_trans_amr_recv_check_0010(MESSAGE_INFO_T *pMsgInfo);


#ifdef __cplusplus
}
#endif

#endif
#endif
