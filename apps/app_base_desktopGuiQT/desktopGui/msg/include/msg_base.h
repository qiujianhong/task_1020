/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   �߼�APP����ͨ��ģ��
* @date��    2019-12-24
* @history�� 
*********************************************************************
*/

#ifndef __MQTT_BASE_H__
#define __MQTT_BASE_H__

#include "framecomm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  APP_RECV_BUF_LEN           8192	/*recvfrom���ջ�������С*/

/* ͨ�ŷ�ʽѡ�� */
#define MSG_TYPE_UDP       0
#define MSG_TYPE_TCP       1
#define MSG_TYPE_MQTT      2
#define MSG_TYPE_CHOOSE    MSG_TYPE_MQTT

#define GUI_MSG_NAME   "M-desktopGui"
#define GUI_APP_NAME   "desktopGui"
#define GUI_APP_SUBTOPIC   "+/M-desktopGui"

#define GUI_DB_CLIENT_NUM   250

typedef int (*MsgFun)(int, uint8*, uint32);
typedef struct tagTaskMsgProc
{
    uint32             msgid;   /* �߳�ID */
    MsgFun             pMsgFunc;  /* �߳���ں��� */
}TASK_MSG_PROC_T;

#define DEBUG_MSG
#ifdef DEBUG_MSG
  #include <errno.h>
  extern int errno;
  #define DPRINT(x...) ({ if (getenv("DEBUG_MSG") != NULL)  printf(x);})
#else
  #define DPRINT(x...)
#endif

#define DPRINT_DBUPDATE(x...) ({ if (getenv("DPRINT_DBUPDATE") != NULL)  printf(x);})

#define MSG_LOG(x...) writelog("/mnt/log/monitermsg.log", x)

#define DPRINT_MSG(d, b, l) \
    do{\
        if(NULL != getenv("DEBUG_MSG"))\
        {\
            printf(d);\
            uint16 iii = 0;\
            for(; iii < l; iii++)\
            {\
                printf("%02x ", b[iii]);\
            }\
            printf("\n");\
        }\
    }while(0);


#ifdef __cplusplus
}
#endif

#endif //__MQTT_BASE_H__

