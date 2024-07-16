/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级app用采对外消息处理
* @date：    2020-1-6
* @history： 
*********************************************************************
*/

#ifndef __OUT_MSG_H__
#define __OUT_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    ADVANCE_STATUS_INIT = 0,  //初始化 APP注册
    ADVANCE_STATUS_SEEK,      //查询APP信息
    ADVANCE_STATUS_WORK,      //正常工作
}APP_ADVANCE_STATUS_E;

int advance_outmsg_init(uint8 type, uint32 ip, uint16 port);
DB_AGENT advance_get_dbagent();
void advance_outmsg_uninit();
uint16 advance_get_next_index();
int advance_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);
int advance_recv_msg(uint8 *pBuf, uint16 buflen);
int advance_login_to_smios();
int advance_heart_bear(MESSAGE_INFO_T *pInMsg);
int advance_get_version(MESSAGE_INFO_T *pInMsg);
BOOLEAN advance_recv_iotManager(MESSAGE_INFO_T *pInMsg);
int advance_get_appidmsg(MESSAGE_INFO_T *pInMsg);
int advance_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex);
int advance_send_ctsmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advance_send_wirelessmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advance_send_modmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advance_send_scsmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advance_send_proxymsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint8 response);
int advance_send_smios(uint16 label, uint16 iip, uint16 iop, uint8* playload, uint16 length);
int advance_send_iotManager(uint16 label, uint16 iip, uint16 iop, uint8* playload, uint16 length);
int advance_send_acmeter(uint16 label, uint16 iip, uint16 iop, uint8* playload, uint16 length);
int advance_send_taskManager(uint16 label, uint16 iip, uint16 iop, uint8* playload, uint16 length);
int advance_send_ccomsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advance_send_guimsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advance_send_uartmsg(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length);
int advance_send_btmsg(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length);
int advance_send_radiomsg(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length);
int advance_send_scs(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length);
int advance_send_rspmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advance_send_appmsg(MESSAGE_INFO_T *pMsgInfo);
int advance_send_pdsManager(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advance_send_commmsg(char* destaddr, uint16 label, uint16 iop, uint8* playload, uint16 length);
int advance_seek_app();
BOOLEAN advance_recv_regapp(MESSAGE_INFO_T *pInMsg);
BOOLEAN advance_start_goon();
void advance_mqtt_debug();
int advance_mqtt_reproc(void *pMsg, int msglen);

#ifdef __cplusplus
}
#endif

#endif //__OUT_MSG_H__

