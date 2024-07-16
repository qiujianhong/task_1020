/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   APP����ģ��
* @date��    2019-12-5
* @history�� 
*********************************************************************
*/
#ifndef __APPUPDATE_H
#define __APPUPDATE_H


#ifdef __cplusplus
        extern "C" {
#endif

typedef enum
{
    UP_IDLE = 0,
    UP_START = 1,
    UP_RUN,
    UP_END,
}APP_UPDATE_STATUS_E;                           //����״̬��

typedef struct tagFTP_STATUS
{
    uint8       tick;
    uint8       hash;
}FTP_STATUS_T;

#ifdef __cplusplus
}
#endif

#endif



