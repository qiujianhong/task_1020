/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� dbCenterModule.h
           ��������ģ��
* @date�� 2019-11-9
*********************************************************************
*/

#ifndef __DB_CENTER_MODULE_H__
#define __DB_CENTER_MODULE_H__

#include "baseType.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// typdef sint32 (*initFp)(void);
/* @brief:ö�٣���Ϣͨ������  */
enum COMMUNICATION_TYPE_E {
  MOD_TCP = 0,
  MOD_UDP,
  MOD_IPC,
  MOD_MQTT,
  MOD_MSG,
  MOD_SAFE,
  MOD_DB,
  // MOD_MAX������������ģ����󣬷�ֹ����Խ��
  MOD_MAX
};

/* @brief:�ṹ�壺ģ�黯����  */
typedef struct {
  sint8 *name;             //ģ������
  sint32 (*initFp)(void);  //ģ��ĳ�ʼ���ص�����
  sint32 (*startFp)(void); //ģ��������ص�����
  void (*stopFp)(void);    //ģ���ֹͣ�ص�����
  sint32 enable;           //ģ��Ŀ�ʼ��ر�ʹ�ܱ�־λ
} SERVICE_MODULE_T;

/* @brief:ϵͳ��ģ���ʼ��  */
sint32 init_system(void);

/* @brief:ϵͳ��ģ������  */
sint32 start_system(void);

/* @brief:�ͷŸ���ģ�����ⲿ��Դ */
void de_init_system(void);

#ifdef __cplusplus
}
#endif
#endif