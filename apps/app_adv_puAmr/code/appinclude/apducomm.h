/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程APDU公共部分
* @date：    2019-12-5
* @history： 
*********************************************************************
*/
#ifndef	__APDUCOMM_H
#define __APDUCOMM_H


#ifdef __cplusplus
        extern "C" {
#endif

uint32 protocol_securityrequest(FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint32 outsize, APP_CONTEXT_t *pContext, 
                                uint8 *pSecurity);
uint16 protocol_analyze_apdu(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pOutMsg, uint16 msgmaxlen, 
                             uint8 *pProto, uint8 *pApduBreak);
OOP_DAR_E transresult_to_frame(uint8 *inData,uint16 inLen,uint8 *outData, uint16 *outLen,uint16 outMaxlen);
extern uint8 rpt_taskstate(int8 RunStat,float  *ClockLimt);
extern void rpt_ansertime_deal(uint8 *Inbuf,uint16 buflen);
OOP_DAR_E transresult_to_frame(uint8 *inData,uint16 inLen,uint8 *outData, uint16 *outLen,uint16 outMaxlen);
typedef struct
{
    BOOLEAN bSend;
    BOOLEAN bAck;
    uint8 retry;
}APPLY_PROXY_ACK;



#ifdef __cplusplus
}
#endif

#endif


