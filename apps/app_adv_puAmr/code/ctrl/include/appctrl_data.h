/*
*********************************************************************
* Copyright(C) 2021 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��չ����ģ�� ��������
* @date��    2021-7-8
* @history�� 
*********************************************************************
*/

#ifndef __CTRL_DATA_H__
#define __CTRL_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PORT_NUM 8

//������־·��
char *gLogPath[MAX_PORT_NUM];

//485����ͨ�ű���
//��ӡ��ʽ����Ϣ��TRACE�ǲ���¼��־��,DEBUG�Ǳؼ���־��,LOGCC���ܿؼ���־��
#define TRANS_FMT_TRACE(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 54, gLogPath[pipe], 0, NULL, 0, fmt, ##__VA_ARGS__)
#define TRANS_FMT_DEBUG(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 54, gLogPath[pipe], 1, NULL, 0, fmt, ##__VA_ARGS__)
#define TRANS_FMT_LOGCC(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 54, gLogPath[pipe], 2, NULL, 0, fmt, ##__VA_ARGS__)
//��ӡ��������buffer����,��ʽΪÿ2λ16��������һ��
#define TRANS_BUF_TRACE(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 54, gLogPath[pipe], 0, buf, len, fmt, ##__VA_ARGS__)
#define TRANS_BUF_DEBUG(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 54, gLogPath[pipe], 1, buf, len, fmt, ##__VA_ARGS__)
#define TRANS_BUF_LOGCC(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 54, gLogPath[pipe], 2, buf, len, fmt, ##__VA_ARGS__)

typedef struct CTRLDevMsg
{
    struct CTRLDevMsg *pNext;
    
    uint8             priority;   //���ȼ�
    OOP_TSA_T         tsa;
    OOP_OAD_U         portOAD;
    uint8             baud;
    uint16            apdulen;
    uint8             apdu[0];
}CTRL_DEV_MSG_T;

typedef struct tagCTRLQueue	    /*�����������ṹ*/
{
	pthread_mutex_t f_lock;
	CTRL_DEV_MSG_T  *pHead;
}CTRL_QUEUE_T;

void ctrl_clean_queue(CTRL_QUEUE_T *pQueue);
CTRL_DEV_MSG_T *ctrl_read_queue(CTRL_QUEUE_T *pQueue);
void ctrl_write_queue(CTRL_QUEUE_T *pQueue, CTRL_DEV_MSG_T *pMsgProc, uint8 bOrder);


#ifdef __cplusplus
}
#endif

#endif //__BLE_DATA_H__


