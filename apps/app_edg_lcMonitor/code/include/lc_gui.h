/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·�����ʾ
* @date��    2020-6-30
* @history�� 
* @User deatil:

*********************************************************************
*/
#ifndef	__LC_GUI_H
#define __LC_GUI_H
    
#ifdef __cplusplus
        extern "C" {
#endif

/** @brief ���� */
typedef struct tag_LC_EVT_FIFO
{
    int nCount;                       //��Ա���� 
    OOP_ABNORMALCTLOG_T  fifo[10];       //����
}LC_EVT_FIFO_T;

void lc_app_stop();
int lc_get_dbClient();

#ifdef __cplusplus
}
#endif

#endif


