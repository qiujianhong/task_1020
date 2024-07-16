/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� dockerManager.h
           docker managerͷ�ļ�
* @date�� 2019-12-2
*********************************************************************
*/

#ifndef __DOCKER_MANAGER_H__
#define __DOCKER_MANAGER_H__

#include "baseType.h"
#include "dockerManagerDefinition.h"

#ifdef __cplusplus
extern "C" {
#endif 

/* @brief:docker����ģ���ʼ�� */
sint32 docker_manager_init_system(void);

/* @brief:docker����ģ������ */
void docker_manager_clean_up_system(void);

/* @brief:docker�������� */
sint32 docker_manager_start_system(void);

/* @brief:docker����ֹͣ */
void docker_manager_stop_system(void);

#ifdef __cplusplus
}
#endif 

#endif
