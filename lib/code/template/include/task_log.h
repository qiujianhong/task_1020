/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   �߳�ģ���쳣��־
* @date��    2020-6-29
* @history�� 
* @User deatil:

*********************************************************************
*/
#ifndef    __TASK_LOG_H
#define __TASK_LOG_H
    
#ifdef __cplusplus
        extern "C" {
#endif

/* ��ѭ������ */
#define SIGDEADLOOP           (SIGRTMIN + 10)

void task_log_init(char *filename);

/* ��־�ӿ����������û�����/�߳�ģ���� ��¼�쳣��־ */
int TASK_ERROR_LOG(char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif

