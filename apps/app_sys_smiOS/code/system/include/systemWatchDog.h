/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� ͷ�ļ�
* @date�� 2019-11-20
*********************************************************************
*/
#ifndef __SYSTEM_WATCHT_DOG_H__
#define __SYSTEM_WATCHT_DOG_H__
#ifdef __cplusplus
extern "C" {
#endif
#define hw_wdg_open()
#define hw_wdg_clear() //(printf("wd is running.\n"))
#define hw_wdg_stop()

/* @brief: ���Ź���ʼ�� */
sint32 sys_wd_init(void);

/* @brief: ���Ź����� */
sint32 sys_wd_start(void);

/* @brief: �˳����Ź� */
sint32 sys_wd_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_WATCHT_DOG_H__ */