/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ���ݿ�ײ���������ͷ�ļ�
* @date�� 2019-11-9
*********************************************************************
*/

#ifndef _DB_MODULE_H_
#define _DB_MODULE_H_
#include "baseType.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* @brief:���ݿ��ʼ��  */
sint32 db_init(void);
/* @brief: ���ݿ�ģ������ */
sint32 db_start(void);
/* @brief: ���ݿ�ֹͣ */
void db_stop(void);
sint32 db_data_init(uint8 logi_dev );
sint32 db_spec_data_(uint8 logi_dev , sint16 inf_addr_no, sint32 prim_di, sint32 sub_di);
/* @brief:  �ָ��������� */
sint32 db_recovery_factory(uint8 logi_dev);

#ifdef __cplusplus
}
#endif

#endif /* _DB_CDRIVER_H_ */
