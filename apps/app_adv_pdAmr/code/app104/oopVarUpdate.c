/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief：参数变更处理
* @date： 2019.12.03
*********************************************************************
*/
#include "sys_cfg.h"
#include "oopVarUpdate.h"
#include "db_api.h"

/* 档案的缓存 */
OOP_METER_P_LIST_T oopMeterList;

/* 任务的缓存 */
OOP_TASK_LIST_T oopTaskList;

/* 上报方案的缓存 */
OOP_PLAN_REPORT_LIST_T oopPlanReportList;

//普通采集方案的缓存
OOP_PLAN_NORMAL_LIST_T oopPlanNormalList;


#if DESC("档案参数变更" ,1)

/**********************************************************************
* @name      : meter_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void meter_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopMeterList, 0, sizeof(OOP_METER_P_LIST_T));
    pthread_mutex_init(&oopMeterList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_meter_list_init
* @brief：     档案参数变更初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_meter_list_init(int clientid)
{
    int             ret = ERR_NORMAL;
    uint32          i = 0;
    uint32          oopMeterNum = 0;
    NOMAL_OAD_T     tOAD = {0};
    OOP_METER_T     oopMeter = {0};
    uint32          readlen = 0;

    memset(&oopMeterList, 0, sizeof(OOP_METER_P_LIST_T));

    tOAD.logicId   = 0;
    tOAD.oad.value = 0x60000200;

    printf("Initializing meters. please wait......\n");
    for (i = 1; i <= NO_VIR_MTR_MAX_NUM; i++)
    {
        tOAD.infoNum   = i;
        ret = db_read_nomal(clientid, &tOAD, sizeof(NOMAL_OAD_T), (uint8 *)&oopMeter, &readlen);
        if(0 != ret || readlen != sizeof(OOP_METER_T))
        {
            oopMeterList.isVaild[i] = FALSE;
            continue;
        }
        else
        {
            oopMeterNum++;

            //档案序号从1开始
            oopMeterList.isVaild[i] = TRUE;
            oopMeterList.oopMeter[i] = oopMeter;
        }
    }
    printf("Initialize meters end.\n");

    /* 记录档案数量 */
    oopMeterList.oopMeterNum = oopMeterNum;

    PRTL_FMT_LOGCC("read meter num: %d\n", oopMeterList.oopMeterNum);

    return;
}

/**********************************************************************
* @name      : app_set_meter_list
* @brief     ：设置档案数据
* @param[in] ：oopMeter - 档案
               i        - 档案序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_meter_list(uint32 i, OOP_METER_T *oopMeter)
{
    if(NULL == oopMeter)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    oopMeterList.oopMeter[i] = *oopMeter;
    oopMeterList.isVaild[i] = TRUE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);

    return ERR_OK;
}

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
int app_get_meter_list(uint32 i, OOP_METER_T *oopMeter)
{
    int32 ret = ERR_NORMAL;

    if(NULL == oopMeter)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    *oopMeter = oopMeterList.oopMeter[i];

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);

    if (oopMeterList.isVaild[i])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_set_meter_list
* @brief     ：设置档案数据无效
* @param[in] ：oopMeter - 档案
               i        - 档案序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_meter_invalid(uint32 i)
{
    if ((i >= 1) && (i <= NO_VIR_MTR_MAX_NUM))
    {
        return ERR_OPTION;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

    memset(&oopMeterList.oopMeter[i], 0, sizeof(OOP_METER_T));
    oopMeterList.isVaild[i] = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_meter_list
* @brief     ：获取档案列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_meter_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_METER_T        oopMeter = {0};

    if(0x60000200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PRTL_FMT_LOGCC("appdb_get_meter_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    ret = db_read_nomal(clientid, &tOAD, sizeof(NOMAL_OAD_T), (uint8 *)&oopMeter, &readlen);
    if(0 != ret || readlen != sizeof(OOP_METER_T))
    {
        if (app_set_meter_invalid(infoNum) < ERR_OK)
        {
            PRTL_FMT_DEBUG("app_set_meter_invalid err\n");
        }
        PRTL_FMT_DEBUG("db_read_nomal err = %d\n", ret);
        return ERR_NORMAL;
    }

    ret = app_set_meter_list(infoNum, &oopMeter);
    if (ret != ERR_OK)
    {
        if (app_set_meter_invalid(infoNum) < ERR_OK)
        {
            PRTL_FMT_DEBUG("app_set_meter_invalid err\n");
        }
        PRTL_FMT_DEBUG("app_set_meter_list err\n");
        return ERR_NORMAL;
    }

    return 0;
}

#endif

#if DESC("任务参数变更" ,1)

/**********************************************************************
* @name      : task_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void task_list_lock_init(void)
{
    /* 初始化锁 */
     memset(&oopTaskList, 0, sizeof(OOP_TASK_LIST_T));
    pthread_mutex_init(&oopTaskList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_task_list_init
* @brief：     档案参数变更初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_task_list_init(int clientid)
{
    int             ret = ERR_NORMAL;
    uint16          taskID = 0;
    uint32          oopTaskNum = 0;
    NOMAL_OAD_T     tOAD = {0};
    OOP_TASK_T      oopTask = {0};
    uint32          readlen = 0;

    memset(&oopTaskList, 0, sizeof(OOP_TASK_LIST_T));

    tOAD.logicId   = 0;
    tOAD.oad.value = 0x60120200;

    printf("Initializing tasks. please wait......\n");
    for (taskID = 1; taskID <= 255; taskID++)
    {
        tOAD.infoNum = taskID;
        ret = db_read_nomal(clientid, &tOAD, sizeof(NOMAL_OAD_T), (uint8 *)&oopTask, &readlen);
        if(0 != ret || readlen != sizeof(OOP_TASK_T))
        {
            oopTaskList.isVaild[taskID] = FALSE;
            continue;
        }
        else
        {
            oopTaskNum++;

            //任务序号从1开始
            oopTaskList.isVaild[taskID] = TRUE;
            oopTaskList.oopTask[taskID] = oopTask;
        }
    }

    printf("Initialize tasks end.\n");

    /* 记录任务数量 */
    oopTaskList.oopTaskNum = oopTaskNum;
    oopTaskList.isChanged = TRUE;

    PRTL_FMT_LOGCC("read task num: %d\n", oopTaskList.oopTaskNum);

    return;
}


/**********************************************************************
* @name      : app_get_task_list
* @brief     ：从缓存中获取任务数据
* @param[in] ：taskID  - 任务序号
* @param[out]: oopTask - 任务数据
* @return    : 错误码
**********************************************************************/
int app_get_task_list(uint8 taskID, OOP_TASK_T *oopTask)
{
    int32 ret = ERR_NORMAL;

    if(NULL == oopTask)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopTaskList.mutex);

    *oopTask = oopTaskList.oopTask[taskID];
    oopTaskList.isChanged = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopTaskList.mutex);

    if (oopTaskList.isVaild[taskID])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_set_task_list
* @brief     ：将更新的任务数据写入缓存
* @param[in] ：taskID  - 任务序号
               oopTask - 任务数据
* @return    ：错误码
**********************************************************************/
int app_set_task_list(uint8 taskID, OOP_TASK_T *oopTask)
{
    if(NULL == oopTask)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopTaskList.mutex);

    oopTaskList.oopTask[taskID] = *oopTask;
    oopTaskList.isVaild[taskID] = TRUE;
    oopTaskList.isChanged = TRUE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopTaskList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_task_invalid
* @brief     ：设置任务数据无效
* @param[in] : taskID - 任务序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_task_invalid(uint8 taskID)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopTaskList.mutex);

    memset(&oopTaskList.oopTask[taskID], 0, sizeof(OOP_TASK_T));
    oopTaskList.isVaild[taskID] = FALSE;
    oopTaskList.isChanged = TRUE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopTaskList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_task_list
* @brief     ：任务列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_task_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_TASK_T         oopTask = {0};

    if(0x60120200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PRTL_FMT_LOGCC("appdb_get_task_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    ret = db_read_nomal(clientid, &tOAD, sizeof(NOMAL_OAD_T), (uint8 *)&oopTask, &readlen);
    if(0 != ret || readlen != sizeof(OOP_TASK_T))
    {
        app_set_task_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_TASK_T));
        return ERR_NORMAL;
    }

    PRTL_FMT_LOGCC("oopTask.taskID = %d\n", oopTask.id);
    PRTL_FMT_LOGCC("oopTask.acqFreq.value = %d\n", oopTask.acqFreq.value);
    PRTL_FMT_LOGCC("oopTask.acqFreq.unit = %d\n", oopTask.acqFreq.unit);
    PRTL_FMT_LOGCC("oopTask.planType = %d\n", oopTask.planType);
    PRTL_FMT_LOGCC("oopTask.planID = %d\n", oopTask.planID);
    PRTL_FMT_LOGCC("oopTask.startTime = %d\n", oopTask.startTime);
    PRTL_FMT_LOGCC("oopTask.endTime = %d\n", oopTask.endTime);
    PRTL_FMT_LOGCC("oopTask.acqDelay = %d\n", oopTask.acqDelay);
    PRTL_FMT_LOGCC("oopTask.state = %d\n", oopTask.state);

    ret = app_set_task_list(infoNum, &oopTask);
    if (ret != ERR_OK)
    {
        app_set_task_invalid(infoNum);
        PRTL_FMT_DEBUG("app_set_task_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }

    return 0;
}

#endif

#if DESC("上报方案参数变更" ,1)

/**********************************************************************
* @name      : plan_report_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
void plan_report_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopPlanReportList, 0, sizeof(OOP_PLAN_REPORT_LIST_T));
    pthread_mutex_init(&oopPlanReportList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_plan_report_list_init
* @brief：     档案参数变更初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_plan_report_list_init(int clientid)
{
    int             ret = ERR_NORMAL;
    uint16          planID = 0;
    uint32          oopPlanReportNum = 0;
    NOMAL_OAD_T     tOAD = {0};
    OOP_PLAN_REPORT_T      oopPlanReport = {0};
    uint32          readlen = 0;

    memset(&oopPlanReportList, 0, sizeof(OOP_PLAN_REPORT_LIST_T));

    tOAD.logicId   = 0;
    tOAD.oad.value = 0x601C0200;

    printf("Initializing plan reports. please wait......\n");
    for (planID = 1; planID <= 255; planID++)
    {
        tOAD.infoNum = planID;
        ret = db_read_nomal(clientid, &tOAD, sizeof(NOMAL_OAD_T), (uint8 *)&oopPlanReport, &readlen);
        if(0 != ret || readlen != sizeof(OOP_PLAN_REPORT_T))
        {
            oopPlanReportList.isVaild[planID] = FALSE;
            continue;
        }
        else
        {
            oopPlanReportNum++;

            //上报方案序号从1开始
            oopPlanReportList.isVaild[planID] = TRUE;
            oopPlanReportList.oopPlanReport[planID] = oopPlanReport;
        }
    }

    printf("Initialize plan reports end.\n");

    /* 记录上报方案数量 */
    oopPlanReportList.oopPlanReportNum = oopPlanReportNum;
    oopPlanReportList.isChanged = TRUE;

    PRTL_FMT_LOGCC("read plan_report num: %d\n", oopPlanReportList.oopPlanReportNum);

    return;
}


/**********************************************************************
* @name      : app_get_plan_report_list
* @brief     ：从缓存中获取上报方案数据
* @param[in] ：plan_reportID  - 上报方案序号
* @param[out]: oopPlanReport - 上报方案数据
* @return    : 错误码
**********************************************************************/
int app_get_plan_report_list(uint8 planID, OOP_PLAN_REPORT_T *oopPlanReport)
{
    int32 ret = ERR_NORMAL;

    if(NULL == oopPlanReport)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanReportList.mutex);

    *oopPlanReport = oopPlanReportList.oopPlanReport[planID];
    oopPlanReportList.isChanged = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanReportList.mutex);

    if (oopPlanReportList.isVaild[planID])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_set_plan_report_list
* @brief     ：将更新的上报方案数据写入缓存
* @param[in] ：plan_reportID  - 上报方案序号
               oopPlanReport - 上报方案数据
* @return    ：错误码
**********************************************************************/
int app_set_plan_report_list(uint8 planID, OOP_PLAN_REPORT_T *oopPlanReport)
{
    if(NULL == oopPlanReport)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanReportList.mutex);

    oopPlanReportList.oopPlanReport[planID] = *oopPlanReport;
    oopPlanReportList.isVaild[planID] = TRUE;
    oopPlanReportList.isChanged = TRUE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanReportList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_plan_report_invalid
* @brief     ：设置上报方案数据无效
* @param[in] : planID - 上报方案序号
* @param[out]：
* @return    ：错误码
**********************************************************************/
int app_set_plan_report_invalid(uint8 planID)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanReportList.mutex);

    memset(&oopPlanReportList.oopPlanReport[planID], 0, sizeof(OOP_PLAN_REPORT_T));
    oopPlanReportList.isVaild[planID] = FALSE;
    oopPlanReportList.isChanged = TRUE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanReportList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_plan_report_list
* @brief     ：上报方案列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int appdb_get_plan_report_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_PLAN_REPORT_T         oopPlanReport = {0};
    uint32 i = 0;
    uint32 j = 0;

    if(0x601C0200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    PRTL_FMT_LOGCC("appdb_get_plan_report_list logicId(%d) infoNum(%d) value(0x%x) running.....\n", logicId, infoNum, value);

    ret = db_read_nomal(clientid, &tOAD, sizeof(NOMAL_OAD_T), (uint8 *)&oopPlanReport, &readlen);
    if(0 != ret || readlen != sizeof(OOP_PLAN_REPORT_T))
    {
        app_set_plan_report_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), oad(0x%08x), infoNum(%d), readlen(%d), explen(%d)\n", ret, tOAD.oad.value, infoNum, readlen, sizeof(OOP_PLAN_REPORT_T));
        return ERR_NORMAL;
    }

    PRTL_FMT_LOGCC("planID = %d\n", oopPlanReport.planID);
    PRTL_FMT_LOGCC("timeout.unit = %d\n", oopPlanReport.timeout.unit);
    PRTL_FMT_LOGCC("timeout.value = %d\n", oopPlanReport.timeout.value);
    PRTL_FMT_LOGCC("retry = %d\n", oopPlanReport.retry);
    PRTL_FMT_LOGCC("data.choice = %d\n", oopPlanReport.data.choice);

    if(oopPlanReport.data.choice == 0)
    {
        PRTL_FMT_LOGCC("data.oad = 0x%08x\n", oopPlanReport.data.oad.value);
    }
    else
    {
        //OAD
        PRTL_FMT_LOGCC("data.record.oad = %d\n", oopPlanReport.data.record.oad.value);

        //RCSD
        for(i = 0; i < oopPlanReport.data.record.tRCSD.nNum; i++)
        {
            PRTL_FMT_LOGCC("data.record.tRCSD.cols[%d].choice = %d\n", i , oopPlanReport.data.record.tRCSD.cols[i].choice);
            if(oopPlanReport.data.record.tRCSD.cols[i].choice == 0)
            {
                PRTL_FMT_LOGCC("data.record.tRCSD.cols[%d].oad = 0x%08x\n", i, oopPlanReport.data.record.tRCSD.cols[i].oad.value);
            }
            else
            {
                PRTL_FMT_TRACE(".data.record.tRCSD.cols[%d].road.oadMain = 0x%08x\n", j, oopPlanReport.data.record.tRCSD.cols[i].road.oadMain.value);
                for(j = 0; j < oopPlanReport.data.record.tRCSD.cols[i].road.oadCols.nNum; j++)
                {
                    PRTL_FMT_TRACE(".data.record.tRCSD.cols[%d].road.oadCols.oad[%d] = 0x%08x\n", i, j, oopPlanReport.data.record.tRCSD.cols[i].road.oadCols.oad[j].value);
                }
            }
        }

        //RSD
        PRTL_FMT_LOGCC("data.record.tRSD.choice = %d\n", oopPlanReport.data.record.tRSD.choice);
    }

    ret = app_set_plan_report_list(infoNum, &oopPlanReport);
    if (ret != ERR_OK)
    {
        app_set_plan_report_invalid(infoNum);
        PRTL_FMT_DEBUG("app_set_plan_report_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }

    return 0;
}

#endif

#if DESC("普通采集方案参数变更" ,1)

/**********************************************************************
* @name      : plan_normal_list_lock_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
void plan_normal_list_lock_init(void)
{
    /* 初始化锁 */
    memset(&oopPlanNormalList, 0, sizeof(OOP_PLAN_NORMAL_LIST_T));
    pthread_mutex_init(&oopPlanNormalList.mutex, NULL);
}

/**
*********************************************************************
* @name：      appdb_plan_normal_list_init
* @brief：     普通采集方案参数变更初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
*********************************************************************
*/
void appdb_plan_normal_list_init(int clientid)
{
    int                 ret              = ERR_NORMAL;
    uint16              planID           = 0;
    uint32              oopPlanNormalNum = 0;
    NOMAL_OAD_T         tOAD             = {0};
    OOP_PLAN_NORMAL_T   oopPlanNormal    = {0};
    uint32              readlen          = 0;

    memset(&oopPlanNormalList, 0, sizeof(OOP_PLAN_NORMAL_LIST_T));

    tOAD.logicId   = 0;
    tOAD.oad.value = 0x60140200;

    printf("Initializing plan normals. please wait......\n");
    for (planID = 1; planID <= 255; planID++)
    {
        tOAD.infoNum = planID;
        ret = db_read_nomal(clientid, &tOAD, sizeof(NOMAL_OAD_T), (uint8 *)&oopPlanNormal, &readlen);
        if(0 != ret || readlen != sizeof(OOP_PLAN_NORMAL_T))
        {
            oopPlanNormalList.isVaild[planID] = FALSE;
            continue;
        }
        else
        {
            oopPlanNormalNum++;

            //普通采集方案序号从1开始
            oopPlanNormalList.isVaild[planID] = TRUE;
            oopPlanNormalList.oopPlanNormal[planID] = oopPlanNormal;
        }
    }

    printf("Initialize plan normals end.\n");

    /* 记录上报方案数量 */
    oopPlanNormalList.oopPlanNormalNum = oopPlanNormalNum;
    oopPlanNormalList.isChanged = TRUE;

    PRTL_FMT_LOGCC("read plan_normal num: %d\n", oopPlanNormalList.oopPlanNormalNum);

    return;
}


/**********************************************************************
* @name      : app_get_plan_normal_list
* @brief     ：从缓存中获取普通采集方案数据
* @param[in] ：plan_normalID  - 方案序号
* @param[out]: oopPlanNormal  - 方案数据
* @return    : 错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :

**********************************************************************/
int app_get_plan_normal_list(uint8 planID, OOP_PLAN_NORMAL_T *oopPlanNormal)
{
    int32 ret = ERR_NORMAL;

    if(NULL == oopPlanNormal)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanNormalList.mutex);

    *oopPlanNormal = oopPlanNormalList.oopPlanNormal[planID];
    oopPlanReportList.isChanged = FALSE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanNormalList.mutex);

    if (oopPlanNormalList.isVaild[planID])
    {
        ret = ERR_OK;
    }
    else
    {
        ret = ERR_INVALID;
    }

    return ret;
}

/**********************************************************************
* @name      : app_set_plan_normal_list
* @brief     ：将更新的普通采集方案数据写入缓存
* @param[in] ：planID          - 方案序号
* @param[out]: oopPlanNormal  - 方案数据
* @return    ：错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int app_set_plan_normal_list(uint8 planID, OOP_PLAN_NORMAL_T *oopPlanNormal)
{
    if(NULL == oopPlanNormal)
    {
        return ERR_PNULL;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanNormalList.mutex);

    oopPlanNormalList.oopPlanNormal[planID] = *oopPlanNormal;
    oopPlanNormalList.isVaild[planID] = TRUE;
    oopPlanNormalList.isChanged = TRUE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanNormalList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : app_set_plan_normal_invalid
* @brief     ：设置普通采集方案数据无效
* @param[in] : planID - 上报方案序号
* @param[out]：
* @return    ：错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int app_set_plan_normal_invalid(uint8 planID)
{

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopPlanNormalList.mutex);

    memset(&oopPlanNormalList.oopPlanNormal[planID], 0, sizeof(OOP_PLAN_NORMAL_T));
    oopPlanNormalList.isVaild[planID] = FALSE;
    oopPlanNormalList.isChanged = TRUE;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopPlanNormalList.mutex);

    return ERR_OK;
}

/**********************************************************************
* @name      : appdb_get_plan_normal_list
* @brief     ：普通采集方案列表到缓存
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int appdb_get_plan_normal_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD = {0};
    OOP_PLAN_NORMAL_T  oopPlanNormal = {0};

    if(0x60140200 != value)
    {
        return -1;
    }

    tOAD.logicId   = logicId;
    tOAD.infoNum   = infoNum;
    tOAD.oad.value = value;

    ret = db_read_nomal(clientid, &tOAD, sizeof(NOMAL_OAD_T), (uint8 *)&oopPlanNormal, &readlen);
    if(0 != ret || readlen != sizeof(OOP_PLAN_NORMAL_T))
    {
        app_set_plan_normal_invalid(infoNum);
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), infoNum(%d), readlen(%d), explen(%d)\n", ret, infoNum, readlen, sizeof(OOP_PLAN_NORMAL_T));
        return ERR_NORMAL;
    }

    ret = app_set_plan_normal_list(infoNum, &oopPlanNormal);
    if (ret != ERR_OK)
    {
        app_set_plan_normal_invalid(infoNum);
        PRTL_FMT_DEBUG("app_set_plan_normal_list failed. ret(%d), infoNum(%d)\n", ret, infoNum);
        return ERR_NORMAL;
    }

    return 0;
}

#endif

#if DESC("参数变更初始化" ,1)
/**
*********************************************************************
* @name：      var_update_init
* @brief：     规约解析模板的参数变更初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @author    :
* @Date      ：2019-12-6
*********************************************************************
*/
void appdb_var_update_init(int clientid)
{
    appdb_meter_list_init(clientid);

    appdb_task_list_init(clientid);

    appdb_plan_report_list_init(clientid);

    appdb_plan_normal_list_init(clientid);

    return;
}

#endif


