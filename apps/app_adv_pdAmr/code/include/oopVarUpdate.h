/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief：参数变更处理
* @date： 2019.12.03
*********************************************************************
*/
#ifndef __COMM_OOP_VAR_UPDATE_H__
#define __COMM_OOP_VAR_UPDATE_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "sys_cfg.h"
#include "framecomm.h"

/** @brief 档案数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;                           //锁 多线程访问
    uint16                 oopMeterNum;                     //电表总个数
    BOOL                   isVaild[NO_VIR_MTR_MAX_NUM + 1]; //档案是否有效
    OOP_METER_T            oopMeter[NO_VIR_MTR_MAX_NUM + 1];//每个电表的档案
}OOP_METER_P_LIST_T;

extern OOP_METER_P_LIST_T oopMeterList;

/** @brief 任务数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;        //锁 多线程访问
    uint16                 oopTaskNum;   //任务个数
    BOOLEAN                   isChanged;    //变更标记
    BOOLEAN                   isVaild[256]; //任务是否有效
    OOP_TASK_T             oopTask[256]; //任务参数
}OOP_TASK_LIST_T;

extern OOP_TASK_LIST_T oopTaskList;

/** @brief 上报方案数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;              //锁 多线程访问
    uint16                 oopPlanReportNum;   //方案个数
    BOOLEAN                   isChanged;          //变更标记
    BOOLEAN                   isVaild[256];       //方案是否有效
    OOP_PLAN_REPORT_T      oopPlanReport[256]; //方案参数
}OOP_PLAN_REPORT_LIST_T;

extern OOP_PLAN_REPORT_LIST_T oopPlanReportList;

/** @brief 普通采集方案数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;              //锁 多线程访问
    uint16                 oopPlanNormalNum;   //方案个数
    BOOLEAN                   isChanged;          //变更标记
    BOOLEAN                   isVaild[256];       //方案是否有效
    OOP_PLAN_NORMAL_T      oopPlanNormal[256]; //方案参数
}OOP_PLAN_NORMAL_LIST_T;

extern OOP_PLAN_NORMAL_LIST_T oopPlanNormalList;

/**********************************************************************
* @name      : app_get_meter_list
* @brief     ：获取档案数据
* @param[in] ：i        - 档案序号
* @param[out]：oopMeter - 档案
* @return    : 错误码
* @Create    :
* @Date      ：2019-12-16
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