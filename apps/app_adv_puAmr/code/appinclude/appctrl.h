/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   控制模块头文件
* @date：    2020-3-31
* @history： 
*********************************************************************
*/
#ifndef	__APPCTRL_H_
#define __APPCTRL_H_


#ifdef __cplusplus
        extern "C" {
#endif

#define CTRL_LABEL_MODPORT       0
#define CTRL_LABEL_WORK          1

typedef struct tagAppCtrlThread
{
    uint8        *pRunFlag;
}CTRL_THREAD_T;

void appctrl_getmsg_port(int sockedfd);
int appctrl_modport_ack(int cid, uint16 label, uint8* pMsg, uint16 msglen, uint32* nextStep);
int appctrl_mod_event(int cid, uint16 label, uint8* pMsg, uint16 msglen, uint32* nextStep);
int appctrl_gui_msg(int cid, uint16 label, uint8* pMsg, uint16 msglen, uint32* nextStep);
int appctrl_if_msg(int cid, uint16 label, uint8* pMsg, uint16 msglen, uint32* nextStep);

void app_ctrl_init(uint8 port, uint8 turnMax);
void *app_ctrl_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif


