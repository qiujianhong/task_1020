
/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/

#ifndef __AC_METER_MODULE_H__
#define __AC_METER_MODULE_H__

#include "ac_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif


/*******************************************************************************
* ȫ�ֱ���
*******************************************************************************/

//5��ģ�����Ϣ
extern DEV_HAL_INFO  gDevHalInfo[MAX_ALLOWED_DOWN_NUM];  //ң������ģ��HAL��Ϣ
extern BOOL gQueryAllModuleFlag;  //���������ʱ, ��ȡ����ģ����Ϣ
extern MODULE_INFO_T gModuleInfo[MAX_ALLOWED_DOWN_NUM];  //5��ģ�����Ϣ(ʵ��ģ��˳����Ϣ)
extern uint8		  gReloadModuleFlag[6];  //��λ��0��ʾ�ޱ仯, 1��ʾ�仯, 0xaa��ʾ��ʼ��, ����5��0/1��ʾģ��仯״̬

//ģ�������»�ȡ�豸
extern uint8 gReloadDevFlag[MAX_ALLOWED_DOWN_NUM];       //ģ�������»�ȡ�豸

/*******************************************************************************
* ö�ٺͽṹ��
*******************************************************************************/


/*******************************************************************************
* ��������
*******************************************************************************/
extern void ModuleInfoDeal(uint8 ModuleIndex, uint8 *RecvBuf, uint32 RecvLen);
extern int GetModuleInfo(uint8 ModuleIndex);
extern int GetAllModuleInfo();
extern int UpdateModuleInfo();
extern void ModuleEventFun(uint8* buf, uint16 len);
extern int ModuleEventDeal();


#ifdef __cplusplus
}
#endif

#endif // __AC_METER_MODULE_H__

