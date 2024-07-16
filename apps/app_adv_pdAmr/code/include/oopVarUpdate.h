/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�������������
* @date�� 2019.12.03
*********************************************************************
*/
#ifndef __COMM_OOP_VAR_UPDATE_H__
#define __COMM_OOP_VAR_UPDATE_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "sys_cfg.h"
#include "framecomm.h"

/** @brief �������ݻ���ṹ*/
typedef struct
{
    pthread_mutex_t        mutex;                           //�� ���̷߳���
    uint16                 oopMeterNum;                     //����ܸ���
    BOOL                   isVaild[NO_VIR_MTR_MAX_NUM + 1]; //�����Ƿ���Ч
    OOP_METER_T            oopMeter[NO_VIR_MTR_MAX_NUM + 1];//ÿ�����ĵ���
}OOP_METER_P_LIST_T;

extern OOP_METER_P_LIST_T oopMeterList;

/** @brief �������ݻ���ṹ*/
typedef struct
{
    pthread_mutex_t        mutex;        //�� ���̷߳���
    uint16                 oopTaskNum;   //�������
    BOOLEAN                   isChanged;    //������
    BOOLEAN                   isVaild[256]; //�����Ƿ���Ч
    OOP_TASK_T             oopTask[256]; //�������
}OOP_TASK_LIST_T;

extern OOP_TASK_LIST_T oopTaskList;

/** @brief �ϱ��������ݻ���ṹ*/
typedef struct
{
    pthread_mutex_t        mutex;              //�� ���̷߳���
    uint16                 oopPlanReportNum;   //��������
    BOOLEAN                   isChanged;          //������
    BOOLEAN                   isVaild[256];       //�����Ƿ���Ч
    OOP_PLAN_REPORT_T      oopPlanReport[256]; //��������
}OOP_PLAN_REPORT_LIST_T;

extern OOP_PLAN_REPORT_LIST_T oopPlanReportList;

/** @brief ��ͨ�ɼ��������ݻ���ṹ*/
typedef struct
{
    pthread_mutex_t        mutex;              //�� ���̷߳���
    uint16                 oopPlanNormalNum;   //��������
    BOOLEAN                   isChanged;          //������
    BOOLEAN                   isVaild[256];       //�����Ƿ���Ч
    OOP_PLAN_NORMAL_T      oopPlanNormal[256]; //��������
}OOP_PLAN_NORMAL_LIST_T;

extern OOP_PLAN_NORMAL_LIST_T oopPlanNormalList;

/**********************************************************************
* @name      : app_get_meter_list
* @brief     ����ȡ��������
* @param[in] ��i        - �������
* @param[out]��oopMeter - ����
* @return    : ������
* @Create    :
* @Date      ��2019-12-16
* @Update    :
**********************************************************************/
int app_get_meter_list(uint32 i, OOP_METER_T *oopMeter);
int app_get_task_list(uint8 taskID, OOP_TASK_T *oopTask);
int app_get_plan_normal_list(uint8 planID, OOP_PLAN_NORMAL_T *oopPlanNormal);

int appdb_get_task_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);

void appdb_var_update_init(int clientid);
#ifdef __cplusplus
}
#endif

#endif