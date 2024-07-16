
#include "CtsAll.h"
#include "area.h"

extern uint8 total_col_flg[AMR_THREAD_NUM]; 
extern uint8 patch_policy_flg[AMR_THREAD_NUM];

extern OOP_PLAN_Transparent_LIST_T oopPlanTransparentList;//透明方案方案集合
/*******************************************************************************
* 函数名称: taskmng_task_need_mtr_num_get
* 函数功能: //获取每个任务需要抄读的表个数
* 输入参数: 
            taskindex 任务索引
* 输出参数: 无
* 返 回 值:  任务需要抄读的电表个数 
*******************************************************************************/
uint16 taskmng_task_need_mtr_num_get(uint8 taskindex)
{
    uint16 count=0;
    pthread_rwlock_rdlock(&TaskRWLock);

    count=SchemeTaskRunQueue[taskindex].TotalNum;
    pthread_rwlock_unlock(&TaskRWLock);
    return count;

}
/*******************************************************************************
* 函数名称: taskmng_task_info_get
* 函数功能:估计任务id获取任务具体抄表参数
* 输入参数: TaskId
* 输出参数: TaskInfo
* 返 回 值: 
*******************************************************************************/
void taskmng_task_info_get(uint8 TaskId,uint8 logicid,SCHEME_TASK_RUN_INFO *TaskInfo)
{
    uint8 i;
    pthread_rwlock_rdlock(&TaskRWLock);
    for(i=0;i<SCHEME_MAX_NUM;i++)
    {
        if(TaskId == SchemeTaskRunQueue[i].TaskId && logicid == SchemeTaskRunQueue[i].logicid)
        {
            memcpy(TaskInfo,&SchemeTaskRunQueue[i],sizeof(SCHEME_TASK_RUN_INFO));
            break;
        }
    }
    pthread_rwlock_unlock(&TaskRWLock);
    
}

/*******************************************************************************
* 函数名称: taskmng_task_finish
* 函数功能://任务本轮结束时间到 置任务结束状态
* 输入参数:
        TaskId 任务号

* 输出参数: 
* 返 回 值:
*******************************************************************************/
void taskmng_task_finish(DB_CLIENT clientid,uint16 TaskId)
{
    uint8 i;
    OOP_ACQ_MONITOR_T* taskMonitorUnit = NULL;
    
    pthread_mutex_lock(&TaskMonitor.MonitorLock);
    for(i=0;i<TaskMonitor.TaskCount;i++)
    {
        if(TaskId == TaskMonitor.TaskMonitorInfo[i].id)
        {
            taskMonitorUnit = &TaskMonitor.TaskMonitorInfo[i];
            break;
        }
    }
    if(NULL == taskMonitorUnit)
    {
        pthread_mutex_unlock(&TaskMonitor.MonitorLock);
        return;
    }
    
    if(taskMonitorUnit->taskState!=finish)
    {
        taskMonitorUnit->taskState = finish;
        NOMAL_OAD_T NormalOad;
        NormalOad.logicId = 0;
        NormalOad.oad.value = 0x60340200;
        NormalOad.infoNum = TaskId;
        NormalOad.classtag = 5;
        db_write_nomal(clientid, &NormalOad,(uint8*)taskMonitorUnit,sizeof(OOP_ACQ_MONITOR_T));
    }
    pthread_mutex_unlock(&TaskMonitor.MonitorLock);
}
/*******************************************************************************
* 函数名称: taskmng_task_monitor_update
* 函数功能://更新任务监控参数
* 输入参数:pipe 端口号
        TaskId 任务号
        cmd 更新命令
        indata 更新命令带的参数
* 输出参数: 
* 返 回 值:
*******************************************************************************/
void taskmng_task_monitor_update(uint8 pipe,uint16 taskID,uint8 logicid,UpdateTaskCmd_t cmd,void *indata)
{
    uint8 i;
    int ret;
    static uint8 cnt[AMR_THREAD_NUM] = {0};     //频繁写数据中心很耗时 攒一波再一起写
    OOP_ACQ_MONITOR_T* taskMonitorUnit = NULL;
    uint8 flag = 0;//标记是否需要更新数据中心
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    if(pipe!=AMR_THREAD_NUM)
    {
        if(pAmrThreadArg->TmnAmrInfo.MtrCnt == 0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, " 当前端口没有电表，无需更新任务监控参数\n");
            return;
        }

    }
    pthread_mutex_lock(&TaskMonitor.MonitorLock);

    for(i=0;i<TaskMonitor.TaskCount;i++)
    {
        if(taskID == TaskMonitor.TaskMonitorInfo[i].id && logicid ==TaskMonitor.logicid[i] )
        {
            taskMonitorUnit = &TaskMonitor.TaskMonitorInfo[i];
            break;
        }
    }
    if(taskMonitorUnit==NULL)
    {
        pthread_mutex_unlock(&TaskMonitor.MonitorLock);
        if(pipe!=AMR_THREAD_NUM)
            TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_task_monitor_update 未找到监控单元 \n");
        return;
    }
   
    if(pipe!=AMR_THREAD_NUM)
       TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_task_monitor_update 任务ID-%d,更新命令:%d taskMonitorUnit->taskState  %d \n",taskID,cmd,taskMonitorUnit->taskState );
    switch (cmd)
    {
    case UDTASK_INIT:
    {
        if(NULL==indata)
            break;
        CTS_RUN_TASK_T * pTaskInfo = (CTS_RUN_TASK_T *)indata;
        rtc_t rtc_tmp;
        time_t monitorstarttime;
        memset(&rtc_tmp, 0, sizeof(rtc_t));
        memcpy(&rtc_tmp, pTaskInfo->ThisTurnBeginTime, 6);
        time_t starttime = commfun_rtc2time2(rtc_tmp);
        DT_DateTimeShort2Time(&taskMonitorUnit->startTime,&monitorstarttime);
        if(pipe!=AMR_THREAD_NUM)
            TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_task_monitor_update UDTASK_INIT taskState %d monitorstarttime %ld starttime %ld \n",taskMonitorUnit->taskState,monitorstarttime,starttime);
        //刚启动时 从数据中心加载参数 会出现taskState是doing 但是启动时间已经是前面的轮次了
        if(taskMonitorUnit->taskState==finish || monitorstarttime <starttime)
        {
            memset(taskMonitorUnit,0x00,sizeof(OOP_ACQ_MONITOR_T));
            taskMonitorUnit->id = taskID;
            taskMonitorUnit->taskState = undo;
            
            flag =1;
        }
    }
        break;
    case UDTASK_START:
    {
        if(NULL==indata)
            break;
        if(taskMonitorUnit->taskState == undo)
        {
            CTS_RUN_TASK_T * pTaskInfo = (CTS_RUN_TASK_T *)indata;
            DateTimeBcd_t taskStartTime;
            time_t colStartTm;
            memcpy_r((uint8*)&taskStartTime,(uint8*)pTaskInfo->ThisTurnBeginTime,7);//采集启动时间
            DT_DateTimeBCD2Time(&taskStartTime,&colStartTm);
            //taskMonitorUnit->startTime = Time_tToDataTimeS(time(NULL)+GetTaskDelay(taskID,logicid));
            if(time(NULL)>colStartTm)
            {
                taskMonitorUnit->startTime = Time_tToDataTimeS(time(NULL));
            }
            else
            {
                taskMonitorUnit->startTime = Time_tToDataTimeS(colStartTm);    
            }
            taskMonitorUnit->taskState = doing;
            taskMonitorUnit->sum=pTaskInfo->TotalMtrNum;
            flag = 1;
        }
    }
        break;
    case UDTASK_SUCCESSNUM:
    {
        if(taskMonitorUnit->taskState == doing)
        {
            taskMonitorUnit->success++;
            if(taskMonitorUnit->success>=taskMonitorUnit->sum)
            {
                taskMonitorUnit->taskState = finish;
                taskMonitorUnit->endTime=Time_tToDataTimeS(time(NULL));
            }
            flag = 1;
        }
    }
        break;
    case UDTASK_SENDFRMNUM:
    {
        if(taskMonitorUnit->taskState == doing)
        {
            taskMonitorUnit->send++;
            flag = 1;
        }
    }
        break;
    case UDTASK_RECVFRMNUM:
    {
        if(taskMonitorUnit->taskState == doing)
        {
            taskMonitorUnit->receive++;
            flag = 1;
        }
    }
        break;

    default:
        break;
    }


    if(flag==1)
    {
        TASK_FMT_TRACE( pipe, REC_LOG, "TaskID-%d , allnum %d succ %d send %d recv %d\n",taskMonitorUnit->id,taskMonitorUnit->sum,
                            taskMonitorUnit->success,taskMonitorUnit->send,taskMonitorUnit->receive);
        cnt[pipe]++;
        if((cnt[pipe] < 100) && (taskMonitorUnit->taskState != finish)&&((cmd==UDTASK_SENDFRMNUM)||(cmd==UDTASK_RECVFRMNUM)||(cmd==UDTASK_SUCCESSNUM))&&(pAmrThreadArg->TmnAmrInfo.MtrCnt>20))
        {
            pthread_mutex_unlock(&TaskMonitor.MonitorLock);
            return;
        }

        if(total_col_flg[pipe] != 0 || patch_policy_flg[pipe] != 0)
        {
            pthread_mutex_unlock(&TaskMonitor.MonitorLock);
            return;
        }
        
        cnt[pipe]=0;
        NOMAL_OAD_T NormalOad;
        NormalOad.logicId = logicid;
        NormalOad.oad.value = 0x60340200;
        NormalOad.infoNum = taskID;
        NormalOad.classtag = 5;
        if(pipe !=AMR_THREAD_NUM)
        {
            ret = db_write_nomal(gAmrThreadArg[pipe].DBClientHandle, &NormalOad,(uint8*)taskMonitorUnit,sizeof(OOP_ACQ_MONITOR_T));
            TASK_FMT_TRACE( pipe, REC_LOG, "写任务监控单元 ret %d \n",ret);

            //TASK_FMT_TRACE( pipe, REC_LOG, "TaskID-%d , allnum %d succ %d send %d recv %d\n",taskMonitorUnit->id,taskMonitorUnit->sum,
                          //      taskMonitorUnit->success,taskMonitorUnit->send,taskMonitorUnit->receive);
            //TASK_FMT_TRACE( pipe, REC_LOG, "任务启动时间:\n");
            //TASK_BUF_TRACE( pipe, REC_LOG, (uint8*)&taskMonitorUnit->startTime,7);
           // TASK_FMT_TRACE( pipe, REC_LOG, "任务结束时间:\n");
           // TASK_BUF_TRACE( pipe, REC_LOG, (uint8*)&taskMonitorUnit->endTime,7);
            //TASK_FMT_TRACE( pipe, REC_LOG, "采集总数量:%d,线程号pipe is %d,cmd is %d\n",taskMonitorUnit->sum,pipe,cmd);
            //TASK_FMT_TRACE( pipe, REC_LOG, "采集成功数量:%d\n",taskMonitorUnit->success);
            //TASK_FMT_TRACE( pipe, REC_LOG, "发送帧数量:%d\n",taskMonitorUnit->send);
            //TASK_FMT_TRACE( pipe, REC_LOG, "接收帧数量:%d\n",taskMonitorUnit->receive);
        }
        else
        {
            db_write_nomal(TGThreadArg.DBClientHandle, &NormalOad,(uint8*)taskMonitorUnit,sizeof(OOP_ACQ_MONITOR_T));

            TASK_FMT_TRACE( -1, TG_LOG, "任务ID-%d,任务更新命令:%d,任务状态:%d\n",taskMonitorUnit->id,cmd,taskMonitorUnit->taskState);
            TASK_FMT_TRACE( -1, TG_LOG, "任务启动时间:\n");
            TASK_BUF_TRACE( -1, TG_LOG, (uint8*)&taskMonitorUnit->startTime,7);
            TASK_FMT_TRACE( -1, TG_LOG, "任务结束时间:\n");
            TASK_BUF_TRACE( -1, TG_LOG, (uint8*)&taskMonitorUnit->endTime,7);
            TASK_FMT_TRACE( -1, TG_LOG, "采集总数量:%d,cmd is %d\n",taskMonitorUnit->sum,cmd);
            TASK_FMT_TRACE( -1, TG_LOG, "采集成功数量:%d\n",taskMonitorUnit->success);
            TASK_FMT_TRACE( -1, TG_LOG, "发送帧数量:%d\n",taskMonitorUnit->send);
            TASK_FMT_TRACE( -1, TG_LOG, "接收帧数量:%d\n",taskMonitorUnit->receive);

        }
    }

    pthread_mutex_unlock(&TaskMonitor.MonitorLock);
}

/*******************************************************************************
* 函数名称: taskmng_task_in_period_check
* 函数功能: 检查任务是否在允许运行时段内
* 输入参数: index 任务索引
            RealTime_t  当前时间
* 输出参数: 无
* 返 回 值:  
*******************************************************************************/
BOOL taskmng_task_in_period_check(uint8 index,time_t RealTime_t)
{
    uint8        PeriordNum = 0;
    uint8        i = 0,flag=0;
    REGION_TYPE  PeriordType;
    uint32       PeriordStart;
    uint32       PeriordEnd;
    uint32       RtTime = 0;
    uint8         *pBuf = NULL;
    //uint8        RunSegFlag = 0;  //bit0:允许执行时段  bit1:后续是否存在允许执行的时段
    DateTime_T   CurBinTime;
    time_t          TaskRunBeginTime_t=0;   //方案开始执行转换后的秒
    time_t          TaskRunEndTime_t=0;   //方案结束执行转换后的秒

    OOP_TASK_CFG *pTaskCfg = &SchemeTaskCfgList.OopTaskCfg[index];

    DT_DateTimeShort2Time(&pTaskCfg->RunBeginTime,&TaskRunBeginTime_t);
    if(pTaskCfg->RunEndTime.year<2038)
    {
        DT_DateTimeShort2Time(&pTaskCfg->RunEndTime,&TaskRunEndTime_t);
        flag = 1;
    }
    
    if(((RealTime_t > TaskRunEndTime_t)&&(1==flag))||(RealTime_t<TaskRunBeginTime_t))
    {
        return FALSE;
    }
    PeriordNum  = pTaskCfg->TimePeriordNum;   //时段个数
    PeriordType = pTaskCfg->TimePeriordType;  //时段类型
    
    if(PeriordNum == 0)
        return TRUE;
    taskmng_cur_bin_time_get ( &CurBinTime);
    RtTime = CurBinTime.Hour*3600 + CurBinTime.Minute*60 + CurBinTime.Second;
    for(i = 0; i < PeriordNum; i++)
    {
        pBuf = &pTaskCfg->Periordhhmm[4*i];

        PeriordStart = 0;
        PeriordEnd = 0;

        switch(PeriordType)
        {
        case FRONTCLOSE_BACKOPEN:    //前闭后开
            PeriordStart = pBuf[0]*3600 + pBuf[1]*60;
            PeriordEnd = pBuf[2]*3600 + pBuf[3]*60;
            PeriordEnd -= 1;
            break;

        case FRONTOPEN_BACKCLOSE:    //前开后闭
            PeriordStart = pBuf[0]*3600 + pBuf[1]*60;
            PeriordEnd = pBuf[2]*3600 + pBuf[3]*60;
            //适配台体 分钟级任务配置这个会导致到下个月0秒的时候不计算轮次到1秒的时候才计算轮次导致第一分钟的任务就无法执行
            //PeriordStart += 1;
            PeriordEnd+=59;
            break;

        case FRONTCLOSE_BACKCLOSE:   //前闭后闭
            PeriordStart = pBuf[0]*3600 + pBuf[1]*60;
            PeriordEnd = pBuf[2]*3600 + pBuf[3]*60;
            PeriordEnd+=59;
            break;

        case FRONTOPEN_BACKOPEN:     //前开后开
            PeriordStart = pBuf[0]*3600 + pBuf[1]*60;
            PeriordEnd = pBuf[2]*3600 + pBuf[3]*60;
            PeriordEnd -= 1;
            PeriordStart += 1;
            break;

        default:
            return FALSE;
        }

        //判断在此时段是允许执行
        if(RtTime >= PeriordStart && RtTime <= PeriordEnd)
        {
            //TASK_FMT_TRACE(-1,REC_LOG,"判断在此时段是允许执行 \n");
            return TRUE;
        }

    }
    //TASK_FMT_TRACE(-1,REC_LOG,"判断在此时段是不允许执行允许执行 \n");
    return FALSE;
}

/*******************************************************************************
* 函数名称: taskmng_task_run_in_period
* 函数功能: 判断任务是否在允许运行时段
* 输入参数: TaskInfo 任务运行时参数
            pTaskCfg  任务参数
* 输出参数: 无
* 返 回 值:  
*******************************************************************************/
void taskmng_task_run_in_period(CTS_RUN_TASK_T * TaskInfo,OOP_TASK_CFG *pTaskCfg)
{
    uint8        PeriordNum = 0;
    uint8        i = 0;
    REGION_TYPE  PeriordType;
    uint32       PeriordStart;
    uint32       PeriordEnd;
    uint32       RtTime = 0;
    uint8         *pBuf = NULL;
    uint8        RunSegFlag = 0;  //bit0:允许执行时段  bit1:后续是否存在允许执行的时段
    DateTime_T   CurBinTime;
    PeriordNum  = pTaskCfg->TimePeriordNum;   //时段个数
    PeriordType = pTaskCfg->TimePeriordType;  //时段类型

    if(PeriordNum == 0)
        return;
    taskmng_cur_bin_time_get ( &CurBinTime);
    RtTime = CurBinTime.Hour*3600 + CurBinTime.Minute*60 + CurBinTime.Second;
    for(i = 0; i < PeriordNum; i++)
    {
        pBuf = &pTaskCfg->Periordhhmm[4*i];

        PeriordStart = 0;
        PeriordEnd = 0;

        switch(PeriordType)
        {
        case FRONTCLOSE_BACKOPEN:    //前闭后开
            PeriordStart = pBuf[0]*3600 + pBuf[1]*60;
            PeriordEnd = pBuf[2]*3600 + pBuf[3]*60;
            PeriordEnd -= 1;
            break;

        case FRONTOPEN_BACKCLOSE:    //前开后闭
            PeriordStart = pBuf[0]*3600 + pBuf[1]*60;
            PeriordEnd = pBuf[2]*3600 + pBuf[3]*60;
            //适配台体 分钟级任务配置这个会导致到下个月0秒的时候不计算轮次到1秒的时候才计算轮次导致第一分钟的任务就无法执行
            //PeriordStart += 1;
            //后闭的时候需要把这一分钟也加上
            PeriordEnd+=59;
            break;

        case FRONTCLOSE_BACKCLOSE:   //前闭后闭
            PeriordStart = pBuf[0]*3600 + pBuf[1]*60;
            PeriordEnd = pBuf[2]*3600 + pBuf[3]*60;
            PeriordEnd+=59;
            break;

        case FRONTOPEN_BACKOPEN:     //前开后开
            PeriordStart = pBuf[0]*3600 + pBuf[1]*60;
            PeriordEnd = pBuf[2]*3600 + pBuf[3]*60;
            PeriordEnd -= 1;
            PeriordStart += 1;
            break;

        default:
            return;
        }

        if(RtTime < PeriordStart)//存在后续允许执行的时段
        {
            RunSegFlag |= 0x02;
        }

        //判断在此时段是允许执行
        if(RtTime >= PeriordStart && RtTime <= PeriordEnd)
        {
            RunSegFlag |= 0x01;
        }

    }

    if(RunSegFlag & 0x01)//bit0:判断是否在允许执行时段
    {
        TaskInfo->ThisTurnPeriodStat = AMR_IN_PERIOD;
        return;
    }

    if(RunSegFlag & 0x02)//bit1:判断是否后续存在允许的执行时段
    {
        TaskInfo->ThisTurnPeriodStat = AMR_PRE_PERIOD;
        return;
    }

    TaskInfo->ThisTurnPeriodStat = AMR_AFT_PERIOD;
}

/*********************************************************************
所属功能单元：
功能描述：根据指定时间和采集存储时间类型获取采集存储时间
输入参数：
        pipe 端口号
        pTmp_NormalLink 任务参数
输出参数：
    无
函数返回值：
       采集存储时间(DateTimeBcd格式)
*********************************************************************/
const uint8 MonTab[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
DateTimeBcd_t taskmng_scheme_task_store_time_get(uint8 pipe,SCHEME_TASK_RUN_INFO* pTmp_NormalLink)
{
    DateTimeBcd_t      StoreTm;
    uint8              tmpbuff[7];
    time_t             ThisTurnTime_t;
    OOP_TI_T               DelayTi;
    DateTimeHex_t      tmpTm;
    uint16             year;
    CTS_RUN_TASK_T*      RunTask=NULL;
    taskmng_run_task_get(pipe, pTmp_NormalLink->TaskId,pTmp_NormalLink->logicid, &RunTask);
    //将本轮次开始时间转换为1970秒数
    memcpy_r(tmpbuff,(uint8*)RunTask->ThisTurnBeginTime,7);
    DT_DateTimeBCD2Time((DateTimeBcd_t*)tmpbuff,&ThisTurnTime_t);

    //本轮次开始时间减去任务延时
    DelayTi.unit = pTmp_NormalLink->DelayTimeUnit;
    DelayTi.value = pTmp_NormalLink->DelayTime;
    DT_TimeSubInv(&ThisTurnTime_t, DelayTi);

    switch(pTmp_NormalLink->StoreTimeType)
    {
    case STRTMTYP_NULL:              //未定义，使用当前时间
    case STRTMTYP_STARTTM:           //任务启动时间
    {
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        break;
    }
    case STRTMTYP_DAYSTART:          //相对当日0点0分
    {
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        StoreTm.hour = 0;
        StoreTm.min = 0;
        StoreTm.sec = 0;
        break;
    }
    case STRTMTYP_LASTDAYEND:        //相对上日23点59分
    {
        ThisTurnTime_t -= 24*3600;
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        StoreTm.hour = 0x23;
        StoreTm.min = 0x59;
        StoreTm.sec = 0x00;
        break;
    }
    case STRTMTYP_LASTDAYSTART:      //相对上日0点0分
    {
        ThisTurnTime_t -= 24*3600;
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        StoreTm.hour = 0;
        StoreTm.min = 0;
        StoreTm.sec = 0;
        break;
    }
    case STRTMTYP_MONTHSTART:       //相对当月1日0点0分
    {
        //将本轮次开始时间转换为1970秒数
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        StoreTm.day = 1;
        StoreTm.hour = 0;
        StoreTm.min = 0;
        StoreTm.sec = 0;
        break;
    }

    case STRTMTYP_LASTMONTHEND:     //相对上月月末0点0分
    {
        DelayTi.unit = 4;
        DelayTi.value = 1;
        DT_TimeSubInv(&ThisTurnTime_t,DelayTi);
        DT_Time2DateTimeHex(ThisTurnTime_t,&tmpTm);
        tmpTm.day = MonTab[tmpTm.month-1];
        year = ((uint16)tmpTm.year_h<<8)+(uint16)tmpTm.year_l;
        if(tmpTm.month==2 && (TRUE == DT_IsLeap(year)))
            tmpTm.day += 1;

        tmpTm.hour = 23;
        tmpTm.min = 59;

        tmpTm.sec = 0;
        DT_DateTimeHex2BCD(&tmpTm,&StoreTm);
        break;
    }
    default:
    {
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        break;
    }

    }
    return StoreTm;
}
/*******************************************************************************
* 函数名称: taskmng_task_store_time_get_tg
* 函数功能:总加组任务获取采集存储时间
* 输入参数: RunTask             任务运行时参数
          pTmp_NormalLink     任务信息

* 输出参数:
* 返 回 值: 任务存储时间
*******************************************************************************/
DateTimeBcd_t taskmng_task_store_time_get_tg(CTS_RUN_TASK_T *RunTask,SCHEME_TASK_RUN_INFO* pTmp_NormalLink)
{
    DateTimeBcd_t      StoreTm;
    uint8              tmpbuff[7];
    time_t             ThisTurnTime_t;
    OOP_TI_T               DelayTi;
    DateTimeHex_t      tmpTm;
    uint16             year;

    //将本轮次开始时间转换为1970秒数
    memcpy_r(tmpbuff,(uint8*)RunTask->ThisTurnBeginTime,7);
    DT_DateTimeBCD2Time((DateTimeBcd_t*)tmpbuff,&ThisTurnTime_t);

    //本轮次开始时间减去任务延时
    DelayTi.unit = pTmp_NormalLink->DelayTimeUnit;
    DelayTi.value = pTmp_NormalLink->DelayTime;
    DT_TimeSubInv(&ThisTurnTime_t, DelayTi);

    switch(pTmp_NormalLink->StoreTimeType)
    {
    case STRTMTYP_NULL:              //未定义，使用当前时间
    case STRTMTYP_STARTTM:           //任务启动时间
    {
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        break;
    }
    case STRTMTYP_DAYSTART:          //相对当日0点0分
    {
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        StoreTm.hour = 0;
        StoreTm.min = 0;
        StoreTm.sec = 0;
        break;
    }
    case STRTMTYP_LASTDAYEND:        //相对上日23点59分
    {
        ThisTurnTime_t -= 24*3600;
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        StoreTm.hour = 0x23;
        StoreTm.min = 0x59;
        StoreTm.sec = 0x00;
        break;
    }
    case STRTMTYP_LASTDAYSTART:      //相对上日0点0分
    {
        ThisTurnTime_t -= 24*3600;
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        StoreTm.hour = 0;
        StoreTm.min = 0;
        StoreTm.sec = 0;
        break;
    }
    case STRTMTYP_MONTHSTART:       //相对当月1日0点0分
    {
        //将本轮次开始时间转换为1970秒数
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        StoreTm.day = 1;
        StoreTm.hour = 0;
        StoreTm.min = 0;
        StoreTm.sec = 0;
        break;
    }

    case STRTMTYP_LASTMONTHEND:     //相对上月月末0点0分
    {
        DelayTi.unit = 4;
        DelayTi.value = 1;
        DT_TimeSubInv(&ThisTurnTime_t,DelayTi);
        DT_Time2DateTimeHex(ThisTurnTime_t,&tmpTm);
        tmpTm.day = MonTab[tmpTm.month-1];
        year = ((uint16)tmpTm.year_h<<8)+(uint16)tmpTm.year_l;
        if(tmpTm.month==2 && (TRUE == DT_IsLeap(year)))
            tmpTm.day += 1;

        tmpTm.hour = 23;
        tmpTm.min = 59;

        tmpTm.sec = 0;
        DT_DateTimeHex2BCD(&tmpTm,&StoreTm);
        break;
    }
    default:
    {
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        break;
    }

    }
    return StoreTm;
}

/*******************************************************************************
* 函数名称: taskmng_patch_scheme_task_store_time_get
* 函数功能:补抄获取存储时间
* 输入参数: pipe 端口号
        MtrIndex    电表索引
        pTmp_NormalLink 任务参数

* 输出参数:
* 返 回 值: 补抄存储时间
*******************************************************************************/
DateTimeBcd_t taskmng_patch_scheme_task_store_time_get(uint8 pipe,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink)
{
    DateTimeBcd_t      StoreTm;

    time_t             ThisTurnTime_t;
    OOP_TI_T               DelayTi;
    DateTimeHex_t      tmpTm;
    uint16             year;
   
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    ThisTurnTime_t = pAmrThreadArg->PatchMtrInfo[MtrIndex].EndTime;

    switch(pTmp_NormalLink->StoreTimeType)
    {
    case STRTMTYP_NULL:              //未定义，使用当前时间
    case STRTMTYP_STARTTM:           //任务启动时间
    {
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        break;
    }
    case STRTMTYP_DAYSTART:          //相对当日0点0分
    {
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        StoreTm.hour = 0;
        StoreTm.min = 0;
        StoreTm.sec = 0;
        break;
    }
    case STRTMTYP_LASTDAYEND:        //相对上日23点59分
    {
        ThisTurnTime_t -= 24*3600;
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        StoreTm.hour = 0x23;
        StoreTm.min = 0x59;
        StoreTm.sec = 0x00;
        break;
    }
    case STRTMTYP_LASTDAYSTART:      //相对上日0点0分
    {
        ThisTurnTime_t -= 24*3600;
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        StoreTm.hour = 0;
        StoreTm.min = 0;
        StoreTm.sec = 0;
        break;
    }
    case STRTMTYP_MONTHSTART:       //相对当月1日0点0分
    {
        //将本轮次开始时间转换为1970秒数
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        StoreTm.day = 1;
        StoreTm.hour = 0;
        StoreTm.min = 0;
        StoreTm.sec = 0;
        break;
    }

    case STRTMTYP_LASTMONTHEND:     //相对上月月末0点0分
    {
        DelayTi.unit = 4;
        DelayTi.value = 1;
        DT_TimeSubInv(&ThisTurnTime_t,DelayTi);
        DT_Time2DateTimeHex(ThisTurnTime_t,&tmpTm);
        tmpTm.day = MonTab[tmpTm.month-1];
        year = ((uint16)tmpTm.year_h<<8)+(uint16)tmpTm.year_l;
        if(tmpTm.month==2 && (TRUE == DT_IsLeap(year)))
            tmpTm.day += 1;

        tmpTm.hour = 23;
        tmpTm.min = 59;

        tmpTm.sec = 0;
        DT_DateTimeHex2BCD(&tmpTm,&StoreTm);
        break;
    }
    default:
    {
        DT_Time2DateTimeBCD(ThisTurnTime_t,&StoreTm);
        break;
    }

    }
    return StoreTm;
}

uint8 Compare_Trans_Scheme(OOP_PLAN_TRANS_T *TransPlanInfo)
{
    uint8 SchemeId=0;
    uint8 i = 0;
    uint8 j=0;
    uint8 addflag=0;
    SchemeId = TransPlanInfo->planID;
    if(oopPlanTransparentList.oopPlanTransparent[SchemeId].planID == TransPlanInfo->planID)
    {
        for(i=0;i<TransPlanInfo->value.nNum;i++)
        {
            if(TransPlanInfo->value.info[i].nIndex == oopPlanTransparentList.oopPlanTransparent[SchemeId].value.info[i].nIndex)
            {
               if((TransPlanInfo->value.info[i].cmd.nNum > oopPlanTransparentList.oopPlanTransparent[SchemeId].value.info[i].cmd.nNum)&&(oopPlanTransparentList.oopPlanTransparent[SchemeId].value.info[i].cmd.nNum>=1))
               {
                    for(j=0;j<TransPlanInfo->value.info[i].cmd.nNum;j++)   
                    {
                        if(TransPlanInfo->value.info[i].cmd.value[j].nIndex == oopPlanTransparentList.oopPlanTransparent[SchemeId].value.info[i].cmd.value[j].nIndex)
                        {
                            if(0==memcmp(TransPlanInfo->value.info[i].cmd.value[j].data.value,oopPlanTransparentList.oopPlanTransparent[SchemeId].value.info[i].cmd.value[j].data.value,TransPlanInfo->value.info[i].cmd.value[j].data.nNum))
                            {
                                addflag =1;
                                continue;
                            }
                      
                        }
                        if(addflag ==1)
                        {
                            return j;
                        }
                   }
        
               }
            }
        }

    }

    return 0xFF;
}

/*******************************************************************************
* 函数名称: taskmng_scheme_task_link_add
* 函数功能: 将任务按照优先级插入链表
* 输入参数: pAddrSchemeTaskRunInfo 需要插入的任务
            HeadSchemeTaskRunInfo 当前优先级最高的链表头
* 输出参数: 
          
* 返 回 值: 优先级最高的链表头
*******************************************************************************/
CTS_RUN_TASK_T *taskmng_scheme_task_link_add(CTS_RUN_TASK_T *pAddrSchemeTaskRunInfo,CTS_RUN_TASK_T *HeadSchemeTaskRunInfo)
{
    CTS_RUN_TASK_T *PTmpHeadTaskLink = NULL;//轮询链表时使用
    CTS_RUN_TASK_T *ScollLink = NULL;//轮询链表时使用

    if(pAddrSchemeTaskRunInfo == NULL)
        return HeadSchemeTaskRunInfo;

    if(HeadSchemeTaskRunInfo == NULL)
    {
        HeadSchemeTaskRunInfo = pAddrSchemeTaskRunInfo;
        HeadSchemeTaskRunInfo->next = NULL;
        return HeadSchemeTaskRunInfo; //添加成功
    }

    if(pAddrSchemeTaskRunInfo->Priority < HeadSchemeTaskRunInfo->Priority)
    {
        pAddrSchemeTaskRunInfo->next = HeadSchemeTaskRunInfo;
        return pAddrSchemeTaskRunInfo;
    }

    PTmpHeadTaskLink = HeadSchemeTaskRunInfo;
    while(PTmpHeadTaskLink != NULL)
    {
        //如果插入的节点任务优先级小于节点的任务优先级
        //如果优先级一样 把日冻结的放前面
        if(pAddrSchemeTaskRunInfo->Priority >= PTmpHeadTaskLink->Priority)
        {
            if(pAddrSchemeTaskRunInfo->MainOad.value == 0x50040200 && pAddrSchemeTaskRunInfo->Priority == PTmpHeadTaskLink->Priority)
            {
            
                break;
            }
            ScollLink = PTmpHeadTaskLink;
            PTmpHeadTaskLink = PTmpHeadTaskLink->next;
        }
        else
            break;
    }
    pAddrSchemeTaskRunInfo->next = PTmpHeadTaskLink;
    if(NULL == ScollLink)
    {
        pAddrSchemeTaskRunInfo->next = HeadSchemeTaskRunInfo;
        return pAddrSchemeTaskRunInfo;
    }
    ScollLink->next = pAddrSchemeTaskRunInfo;

    return  HeadSchemeTaskRunInfo;
}

void taskmng_task_db_update_check(DB_UPDATE_T *dbupdate)
{
    int Recvlen = 0;
    Recvlen = taskmng_db_update_queue_read(&pTaskDBUpdate, dbupdate);
    if(Recvlen>0)
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "收到mqtt发送的更新消息 logicid %d  infonum %d oad %08x !\n",dbupdate->logicid,dbupdate->infonum,dbupdate->oad);
        SchemeTaskCfgList.TaskCfgState = TASK_CFG_MODIFY;
    }
}

/*******************************************************************************
* 函数名称: taskmng_task_monitor_init
* 函数功能://任务监控初始化
* 输入参数:TaskId 任务号

* 输出参数: 
* 返 回 值:
*******************************************************************************/
void taskmng_task_monitor_init(uint16 TaskId,uint8 logicid)
{
    uint8 i;
    OOP_ACQ_MONITOR_T* taskMonitorUnit = NULL;
    pthread_mutex_lock(&TaskMonitor.MonitorLock);
    for(i=0;i<TaskMonitor.TaskCount;i++)
    {
        if(TaskId == TaskMonitor.TaskMonitorInfo[i].id && logicid ==TaskMonitor.logicid[i] )
        {
            taskMonitorUnit = &TaskMonitor.TaskMonitorInfo[i];
            break;
        }
    }
    if(taskMonitorUnit==NULL)
    {
        pthread_mutex_unlock(&TaskMonitor.MonitorLock);
        return;
    }
    memset(taskMonitorUnit,0x00,sizeof(OOP_ACQ_MONITOR_T));
    taskMonitorUnit->id = TaskId;
    taskMonitorUnit->taskState = undo;
    
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = logicid;
    NormalOad.oad.value = 0x60340200;
    NormalOad.infoNum = TaskId;
    NormalOad.classtag = 5;
    db_write_nomal(gDBClientTask, &NormalOad,(uint8*)taskMonitorUnit,sizeof(OOP_ACQ_MONITOR_T));

    pthread_mutex_unlock(&TaskMonitor.MonitorLock);
            
}

/*******************************************************************************
* 函数名称: taskmng_scheme_task_init
* 函数功能: 采集方案任务遍历
* 输入参数: 
            pSchemeTaskParaList   //载波方案任务参数列表
            SchemeTaskRunQueue    //载波方案任务队列
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
void taskmng_scheme_task_init_with_logicid(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue,uint8 logicid)
{

    uint16          TaskNum = 0;
    OOP_TASK_T      TaskInfo;
    ePlanType       planType; 
    uint8           SchemeType = 0;
    uint8           FixPriority = 0;
    int           ret;
    uint16 i;
    SCHEME_TASK_RUN_INFO  *pSchemeTaskTaskRunInfo = NULL;

    OOP_PLAN_NORMAL_T NormalPlanInfo;
    OOP_PLAN_EVENT_T EventPlanInfo;
    OOP_PLAN_TRANS_T TransPlanInfo;
    OOP_PLAN_ENCRYPT_T EncryptPlanInfo;
    
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    memset(&TaskInfo,0x00,sizeof(OOP_TASK_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    inReadRecord.recordOAD.logicId = logicid;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 255;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60120200;
    stepReadRecord.MAC.nNum = 0;
    

    pthread_rwlock_wrlock(&pSchemeTaskParaList->rwLock);

    ret = db_read_record_start(gDBClientTask, &inReadRecord, &handle, &recNum);
    if(ret!=0 ||recNum==0)
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "开始读任务失败 ret %d recNum %d logicid %d \n",ret,recNum,logicid);
    }
    else
    {
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(gDBClientTask, handle, &stepReadRecord, &stepRecordOut);
            if(ret!=0 )
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "单步读任务失败 ret %d i %d \n",ret,i);
                continue;
            }
            if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=0x60120200
                ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_TASK_T))
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "单步读取的任务记录有问题 \n");
                continue;
            }
            memcpy(&TaskInfo,stepRecordOut.record[0].outDataBuf,sizeof(OOP_TASK_T));
            TASK_FMT_DEBUG(-1,TASK_LOG, "读取任务 %d ret = %d SchemeType  %d \n",TaskInfo.id,ret,TaskInfo.planType);
            TaskNum =pSchemeTaskParaList->TaskNum;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].logicid = logicid;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].TaskId = TaskInfo.id;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycle = TaskInfo.acqFreq.value;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycleUnit = TaskInfo.acqFreq.unit;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].SchemeType = TaskInfo.planType;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].SchemeId = TaskInfo.planID;

            pSchemeTaskParaList->OopTaskCfg[TaskNum].RunBeginTime = TaskInfo.startTime;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].RunEndTime = TaskInfo.endTime;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].DelayTime = TaskInfo.acqDelay.value;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].DelayTimeUnit = TaskInfo.acqDelay.unit;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].Priority = TaskInfo.priority;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].TaskState = TaskInfo.state;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].PreRunScript = TaskInfo.startScriptID;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].OverRunScript = TaskInfo.endScriptID;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].TimePeriordType = TaskInfo.workPeriod.type;
            pSchemeTaskParaList->OopTaskCfg[TaskNum].TimePeriordNum = TaskInfo.workPeriod.nNum;
            memcpy(pSchemeTaskParaList->OopTaskCfg[TaskNum].Periordhhmm,TaskInfo.workPeriod.period,sizeof(TASKPERIOD)*OOP_MAX_WORKPERIOD);

            planType = pSchemeTaskParaList->OopTaskCfg[TaskNum].SchemeType-1;
            TASK_FMT_DEBUG(-1,TASK_LOG, "planType %d 任务状态state %d \n",planType, TaskInfo.state);
            TASK_FMT_DEBUG(-1,TASK_LOG, "任务开始结束时间如下： \n");
            TASK_BUF_DEBUG(-1,TASK_LOG,&TaskInfo.startTime,8);
            TASK_BUF_DEBUG(-1,TASK_LOG,&TaskInfo.endTime,8);
            switch(planType)
            {
                case NORMALPLAN:   // 普通采集方案
                    SchemeType = 1;
                    break;

                case EVENTPLAN:    // 事件采集方案
                    SchemeType = 1;
                    break;
                case TRANSPARENTPLAN:
                    SchemeType = 1;
                    break;
                case ENCRYPTPLAN:
                    SchemeType = 1;
                    break;
                default:
                    SchemeType = 0;
                    break;
            }
            if(SchemeType == 0)//目前支持普通方案以及事件方案
                continue;
                
            //SchemeId = TaskInfo.planID;
            
            memset(pSchemeTaskParaList->OopTaskCfg[TaskNum].ThisTurnBeginTime, 0, 7);
            memset(pSchemeTaskParaList->OopTaskCfg[TaskNum].ThisTurnEndTime, 0, 7);

            //初始化各个任务运行状态信息
            pSchemeTaskTaskRunInfo = &SchemeTaskRunQueue[TaskNum];
            memset(pSchemeTaskTaskRunInfo, 0, sizeof(SCHEME_TASK_RUN_INFO));

            if(pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycle == 5 && pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycleUnit == 1)
            {
                pSchemeTaskTaskRunInfo->FiveMinTaskFlag = 1;
            }
            pSchemeTaskTaskRunInfo->logicid       = logicid;
            pSchemeTaskTaskRunInfo->TaskId        = TaskInfo.id;
            pSchemeTaskTaskRunInfo->SchemeType    = pSchemeTaskParaList->OopTaskCfg[TaskNum].SchemeType-1;  //采集方案类型
            pSchemeTaskTaskRunInfo->SchemeId      = pSchemeTaskParaList->OopTaskCfg[TaskNum].SchemeId;
            pSchemeTaskTaskRunInfo->DelayTime     = pSchemeTaskParaList->OopTaskCfg[TaskNum].DelayTime;
            pSchemeTaskTaskRunInfo->DelayTimeUnit  = pSchemeTaskParaList->OopTaskCfg[TaskNum].DelayTimeUnit;
            pSchemeTaskTaskRunInfo->RunResult     = SCHEME_UNDONE;
            pSchemeTaskTaskRunInfo->Priority      = pSchemeTaskParaList->OopTaskCfg[TaskNum].Priority;
            //感觉方案id和方案类型从数据中心读取方案信息

            switch (planType)
            {
            case NORMALPLAN:    //普通采集方案
            {
                //根据方案编号读取方案数据

                //printf("任务 %d 读取方案 %d  \n",tmpId,pSchemeTaskTaskRunInfo->SchemeId);
                //ret = nomal_data_read(TaskHandle, NomalOad, sizeof(OOP_PLAN_NORMAL_T),(uint8*)&NormalPlanInfo, &len);
                memset(&NormalPlanInfo,0x00,sizeof(OOP_PLAN_NORMAL_T));
                ret = taskmng_normal_plan_info_read(pSchemeTaskTaskRunInfo->SchemeId, &NormalPlanInfo,logicid);
                TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取方案 %d ret = %d 方案大小 %d \n",TaskInfo.id,pSchemeTaskTaskRunInfo->SchemeId,ret,sizeof(OOP_PLAN_NORMAL_T));
                if(ret<0)
                {
                    pSchemeTaskParaList->OopTaskCfg[TaskNum].TaskState = 3;
                    break;
                }
                TASK_FMT_DEBUG(-1,TASK_LOG, "NormalPlanInfo.ms.choice %d NormalPlanInfo.tmFlag %d\n",NormalPlanInfo.ms.choice,NormalPlanInfo.tmFlag);
                pSchemeTaskTaskRunInfo->AcqInfo.Acqtype = NormalPlanInfo.recdRow.choice;
                pSchemeTaskTaskRunInfo->StoreTimeType = NormalPlanInfo.tmFlag;
                if(ACQTYP_LASTN == pSchemeTaskTaskRunInfo->AcqInfo.Acqtype)
                {
                    pSchemeTaskTaskRunInfo->AcqInfo.LastN = NormalPlanInfo.recdRow.num;
                }
                else if(ACQTYP_BC == pSchemeTaskTaskRunInfo->AcqInfo.Acqtype)
                {
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.value = NormalPlanInfo.recdRow.rd.ti.value;
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = NormalPlanInfo.recdRow.rd.ti.unit;
                    pSchemeTaskTaskRunInfo->AcqInfo.LastN =NormalPlanInfo.recdRow.rd.last;

                }
                else if(ACQTYP_TI == pSchemeTaskTaskRunInfo->AcqInfo.Acqtype)
                {   
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.value = NormalPlanInfo.recdRow.ti.value;
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = NormalPlanInfo.recdRow.ti.unit;             
                }

                pSchemeTaskTaskRunInfo->TotalNum= taskmng_ms_list_get(&NormalPlanInfo.ms,pSchemeTaskTaskRunInfo->MsInfo,logicid);
                TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取方案 %d  抄读电表个数 %d \n",TaskInfo.id,pSchemeTaskTaskRunInfo->SchemeId,pSchemeTaskTaskRunInfo->TotalNum);
                //分钟级采集标记再判下表个数是否大于100个
                if(pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycle == 1 && pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycleUnit == 1 
                        && MeterFileAll.MeterCount > 100)
                {
                    pSchemeTaskTaskRunInfo->MinTaskFlag = 1;
                }
#ifdef PRODUCT_ECU
                //if(pSchemeTaskTaskRunInfo->AcqInfo.Acqtype == ACQTYP_NULL && pSchemeTaskTaskRunInfo->TotalNum == 300 && pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycle == 15 && pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycleUnit == 1)
                //{
                //    TASK_FMT_DEBUG(-1,TASK_LOG, "能源控制器采集压力测试15分钟抄不完，把任务周期改成20分钟，优先级改为1 \n");
                //    pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycle = 20;
                //    pSchemeTaskParaList->OopTaskCfg[TaskNum].Priority = 1;
                //}
                //if(pSchemeTaskTaskRunInfo->TotalNum == 300 && pSchemeTaskParaList->OopTaskCfg[TaskNum].DelayTime == 5)
                //{
                //    TASK_FMT_DEBUG(-1,TASK_LOG, "能源控制器采集压力测试15分钟抄不完，把冻结任务优先级改为2，先把实时抄完 \n");
                //    pSchemeTaskParaList->OopTaskCfg[TaskNum].Priority = 2;
                //}
#endif 

                uint16 i;
//                MS_INFO tmpms;
//                for(i=0;i<pSchemeTaskTaskRunInfo->TotalNum;i++)
//                {
//                    tmpms = pSchemeTaskTaskRunInfo->MsInfo[i];
//                    TASK_FMT_TRACE(-1,TASK_LOG, "电表 %d port =%d prtl = %d  \n",tmpms.Sn ,tmpms.Pipe,tmpms.Prtl);
//                }
                taskmng_oad_list_get(&NormalPlanInfo.recdCol,&pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0],&pSchemeTaskTaskRunInfo->AcqInfo);
                TASK_FMT_DEBUG(-1,TASK_LOG, "mainoad %08x  \n",pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.value);
                for(i=0;i<pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].SubOadNum;i++)
                {
                    TASK_FMT_DEBUG(-1,TASK_LOG, "taskmng_oad_list_get %d oad %08x  \n",i,pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].OadListInfo[i].value);
                }
                pSchemeTaskTaskRunInfo->CsdTask.RoadNum=1;
                pSchemeTaskTaskRunInfo->CsdTask.TotalSubOadNum = pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].SubOadNum;
#ifdef AREA_SHANDONG
                //山东光伏表配的任务是5分钟或者1分钟 先注释掉采当前数据的逻辑
                Change5min1minToReal(pSchemeTaskTaskRunInfo, &pSchemeTaskParaList->OopTaskCfg[TaskNum]);
                pSchemeTaskTaskRunInfo->NILMFlag = taskmng_taskinfo_4800_check(&pSchemeTaskTaskRunInfo->CsdTask);
#endif
                //对于5002的分钟曲线任务,按照采集间隔采集做一些容错处理；
                if((pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)&&(pSchemeTaskTaskRunInfo->AcqInfo.Acqtype == ACQTYP_TI))
                {   //将时间间隔转化为分钟单位
                    if(pSchemeTaskTaskRunInfo->AcqInfo.TI.unit ==0)
                    {
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = 1;
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.value = pSchemeTaskTaskRunInfo->AcqInfo.TI.value/60;
                    }
                    else if(pSchemeTaskTaskRunInfo->AcqInfo.TI.unit ==2)
                    {
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = 1;
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.value = pSchemeTaskTaskRunInfo->AcqInfo.TI.value*60;
                    }
                    else if(pSchemeTaskTaskRunInfo->AcqInfo.TI.unit >2)
                    {
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.value =0;
                    }
                    //异常情况下使用默认值
                    if(pSchemeTaskTaskRunInfo->AcqInfo.TI.value ==0)
                    {
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = 1;
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.value =15;
                    }
                }
                //发现有现场配置5002曲线任务时配置成按冻结时标采集，没有时标间隔无法正常抄读，需要强制修改
                if(pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002 && pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_TI&& 
                    pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_BC && pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_NULL &&
                    pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_LASTN)
                {
                    pSchemeTaskTaskRunInfo->AcqInfo.Acqtype = ACQTYP_TI;
                    if((pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycleUnit >= TASK_CYCLE_FLAG_HOUR)
                        ||((pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycleUnit == TASK_CYCLE_FLAG_MIN)&&(pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycle > 15)))
                    {
                        //任务频率大于15分钟，时标间隔设为15分钟
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = TASK_CYCLE_FLAG_MIN;
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.value = 15;
                    }
                    else
                    {
                        //时标间隔与任务周期一致
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.value = pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycle;
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = pSchemeTaskParaList->OopTaskCfg[TaskNum].RunCycleUnit;
                    }
                }
                else if(pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003 && pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_TI && pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_NULL)
                {
                    pSchemeTaskTaskRunInfo->AcqInfo.Acqtype = ACQTYP_TI;
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = TASK_CYCLE_FLAG_HOUR;//小时
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.value = 1;//默认1小时
                }
                //优先级修正
                FixPriority = taskmng_priority_fix_by_mainoi(pSchemeTaskTaskRunInfo, pSchemeTaskParaList->OopTaskCfg[TaskNum]);
                if(FixPriority > 0)
                {
                    pSchemeTaskTaskRunInfo->Priority = FixPriority;
                }
            }
                break;
            case EVENTPLAN:
            {
                //根据方案号从数据中心读取事件方案信息
                
                //ret = nomal_data_read(TaskHandle, NomalOad,sizeof(OOP_PLAN_EVENT_T), (uint8*)&EventPlanInfo, &len);
                memset(&EventPlanInfo,0x00,sizeof(OOP_PLAN_EVENT_T));
                ret = taskmng_eventl_plan_info_read(pSchemeTaskTaskRunInfo->SchemeId, &EventPlanInfo,logicid);
                TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取事件方案 %d ret = %d 方案大小 %d \n",TaskInfo.id,pSchemeTaskTaskRunInfo->SchemeId,ret,sizeof(OOP_PLAN_EVENT_T));
                if(ret<0)
                {
                    pSchemeTaskParaList->OopTaskCfg[TaskNum].TaskState = 3;
                    break;
                }
                pSchemeTaskTaskRunInfo->rptFlag = EventPlanInfo.bReport;
                pSchemeTaskTaskRunInfo->AcqInfo.Acqtype = ACQTYP_LASTN;//事件采集方案通过采集上1次执行
                pSchemeTaskTaskRunInfo->AcqInfo.LastN = 1;
                pSchemeTaskTaskRunInfo->StoreTimeType = STRTMTYP_STARTTM;//按照任务启动时间执行
                pSchemeTaskTaskRunInfo->TotalNum = taskmng_ms_list_get(&EventPlanInfo.ms, pSchemeTaskTaskRunInfo->MsInfo,logicid);
                TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取事件方案 %d  抄读电表个数 %d \n",TaskInfo.id,pSchemeTaskTaskRunInfo->SchemeId,pSchemeTaskTaskRunInfo->TotalNum);
                uint16 i;
//                MS_INFO tmpms;
//                for(i=0;i<pSchemeTaskTaskRunInfo->TotalNum;i++)
//                {
//                    tmpms = pSchemeTaskTaskRunInfo->MsInfo[i];
//                    TASK_FMT_TRACE(-1,TASK_LOG, "电表 %d port =%d prtl = %d  \n",tmpms.Sn ,tmpms.Pipe,tmpms.Prtl);
//                }
                taskmng_road_list_add(&EventPlanInfo.events,&pSchemeTaskTaskRunInfo->CsdTask);
                uint8 j;
                TASK_FMT_DEBUG(-1,TASK_LOG, "事件采集方案 road个数 %d  \n",pSchemeTaskTaskRunInfo->CsdTask.RoadNum);
                for(j=0;j<pSchemeTaskTaskRunInfo->CsdTask.RoadNum;j++)
                {
                    TASK_FMT_DEBUG(-1,TASK_LOG, "mainoad %08x  SubOadNum %d \n",pSchemeTaskTaskRunInfo->CsdTask.RoadTask[j].MainOad.value,pSchemeTaskTaskRunInfo->CsdTask.RoadTask[j].SubOadNum);
                    for(i=0;i<pSchemeTaskTaskRunInfo->CsdTask.RoadTask[j].SubOadNum;i++)
                    {
                        TASK_FMT_DEBUG(-1,TASK_LOG, "OadList %d oad %08x  \n",i,pSchemeTaskTaskRunInfo->CsdTask.RoadTask[j].OadListInfo[i].value);
                    }
                }
                
                //优先级修正
                FixPriority = taskmng_priority_fix_by_mainoi(pSchemeTaskTaskRunInfo, pSchemeTaskParaList->OopTaskCfg[TaskNum]);
                if(FixPriority > 0)
                {
                    pSchemeTaskTaskRunInfo->Priority = FixPriority;
                }
            }
                break;
            case TRANSPARENTPLAN:
            {
                memset(&TransPlanInfo,0x00,sizeof(OOP_PLAN_TRANS_T));
                uint8 SchemeId;
                SchemeId = pSchemeTaskTaskRunInfo->SchemeId;
                ret = taskmng_trans_plan_info_read(pSchemeTaskTaskRunInfo->SchemeId,&TransPlanInfo,logicid);
                TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取透明方案 %d ret = %d 方案大小 %d \n",TaskInfo.id,pSchemeTaskTaskRunInfo->SchemeId,ret,sizeof(OOP_PLAN_TRANS_T));
                if(ret<0)
                {
                    pSchemeTaskParaList->OopTaskCfg[TaskNum].TaskState = 3;
                    break;
                }
                oopPlanTransparentList.oopPlanTransparentNum++;
                oopPlanTransparentList.oopPlanTransparent[SchemeId] = TransPlanInfo;
                
                TASK_FMT_DEBUG(-1,TASK_LOG, "透明方案个数 %d \n",oopPlanTransparentList.oopPlanTransparentNum);
                pSchemeTaskTaskRunInfo->TotalNum = taskmng_trans_plan_info_get(&TransPlanInfo.value , pSchemeTaskTaskRunInfo->MsInfo,pSchemeTaskTaskRunInfo->TransInfo,logicid);
                TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取透明方案 %d  抄读电表个数 %d \n",TaskInfo.id,pSchemeTaskTaskRunInfo->SchemeId,pSchemeTaskTaskRunInfo->TotalNum);
                uint16 i;
                MS_INFO tmpms;
                for(i=0;i<pSchemeTaskTaskRunInfo->TotalNum;i++)
                {
                    tmpms = pSchemeTaskTaskRunInfo->MsInfo[i];
                    TASK_FMT_DEBUG(-1,TASK_LOG, "电表 %d port =%d prtl = %d  \n",tmpms.Sn ,tmpms.Pipe,tmpms.Prtl);
                }
            }
                break;
            case ENCRYPTPLAN:
            {
                memset(&EncryptPlanInfo,0x00,sizeof(OOP_PLAN_ENCRYPT_T));
                ret = taskmng_encrypt_plan_info_read(pSchemeTaskTaskRunInfo->SchemeId,&EncryptPlanInfo);
                if(ret<0)
                {
                    pSchemeTaskParaList->OopTaskCfg[TaskNum].TaskState = 3;
                    break;
                }
                pSchemeTaskTaskRunInfo->EncryptPlanID = EncryptPlanInfo.planID;
                pSchemeTaskTaskRunInfo->EncryptPlanType = EncryptPlanInfo.planType;
                pSchemeTaskTaskRunInfo->TotalNum = taskmng_encrypt_plan_info_get(&EncryptPlanInfo,pSchemeTaskTaskRunInfo->MsInfo,&pSchemeTaskTaskRunInfo->EncryptInfo);
                TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取转加密方案 %d 方案类型 %d 抄读电表个数 %d \n",TaskInfo.id,pSchemeTaskTaskRunInfo->SchemeId,
                    EncryptPlanInfo.planType,pSchemeTaskTaskRunInfo->TotalNum);
                uint16 i;
                MS_INFO tmpms;
                for(i=0;i<pSchemeTaskTaskRunInfo->TotalNum;i++)
                {
                    tmpms = pSchemeTaskTaskRunInfo->MsInfo[i];
                    TASK_FMT_DEBUG(-1,TASK_LOG, "电表 %d port =%d prtl = %d  \n",tmpms.Sn ,tmpms.Pipe,tmpms.Prtl);
                }
                ret = taskmng_encrypt_result_first_write(gDBClientTask,EncryptPlanInfo.planID,EncryptPlanInfo.planType,EncryptPlanInfo.config.nNum,pSchemeTaskTaskRunInfo->MsInfo);
                if(ret <0 )
                {
                    TASK_FMT_DEBUG(-1,TASK_LOG,"写转加密执行结果集失败");
                }
            }
                break;
            default:
                break;
            }
//            if(pSchemeTaskTaskRunInfo->TotalNum==0)
//                continue;
            pSchemeTaskParaList->TaskNum++;
        }

    }
    db_read_record_end(gDBClientTask, handle);

    TASK_FMT_DEBUG(-1,TASK_LOG, "logicid %d 读取任务完成 任务总个数为 %d \n",logicid,pSchemeTaskParaList->TaskNum);
    pthread_rwlock_unlock(&pSchemeTaskParaList->rwLock);
}

void taskmng_scheme_task_init(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue)
{
    //memset((char*)pSchemeTaskParaList,0x00,sizeof(OOP_TASK_CFG_LIST));
    pSchemeTaskParaList->TaskCfgState =TASK_CFG_UNINIT;
    pSchemeTaskParaList->TaskNum = 0;
    memset(pSchemeTaskParaList->OopTaskCfg,0x00,sizeof(OOP_TASK_CFG)*255);
    memset((char*)SchemeTaskRunQueue,0x00,SCHEME_MAX_NUM*sizeof(SCHEME_TASK_RUN_INFO));
    //pthread_rwlock_init ( &pSchemeTaskParaList->rwLock, NULL );
    taskmng_scheme_task_init_with_logicid(pSchemeTaskParaList,SchemeTaskRunQueue,LOGIC_ID_YC);
    //taskmng_scheme_task_init_with_logicid(pSchemeTaskParaList,SchemeTaskRunQueue,LOGIC_ID_PD);
}

/*******************************************************************************
* 函数名称: taskmng_task_monitor_load
* 函数功能://从数据中心加载任务监控内容
* 输入参数:

* 输出参数: 
* 返 回 值:
*******************************************************************************/
void taskmng_task_monitor_load()
{
    uint8 i;
    uint16 TaskId;
    int   ret;
    uint32 Len;
    OOP_ACQ_MONITOR_T taskMonitorUnit;
    NOMAL_OAD_T NormalOad;
    NormalOad.oad.value = 0x60340200;
    pthread_mutex_lock(&TaskMonitor.MonitorLock);
    pthread_rwlock_rdlock(&SchemeTaskCfgList.rwLock);
    TaskMonitor.TaskCount = SchemeTaskCfgList.TaskNum;
    TASK_FMT_DEBUG(-1,TASK_LOG, "TaskMonitor.TaskCount %d \n",TaskMonitor.TaskCount);
    for(i=0;i<SchemeTaskCfgList.TaskNum;i++)
    {
        TaskId = SchemeTaskCfgList.OopTaskCfg[i].TaskId;
        NormalOad.logicId = SchemeTaskCfgList.OopTaskCfg[i].logicid;
        NormalOad.infoNum = TaskId;
        memset(&taskMonitorUnit,0x00,sizeof(OOP_ACQ_MONITOR_T));
        ret = db_read_nomal(gDBClientTask,&NormalOad,sizeof(OOP_ACQ_MONITOR_T),(uint8*)&taskMonitorUnit,&Len);
        if(ret!=0 ||Len!=sizeof(OOP_ACQ_MONITOR_T))
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "读取任务%d 监控状态失败 ret %d \n",TaskId,ret);
            memset(&TaskMonitor.TaskMonitorInfo[i],0x00,sizeof(OOP_ACQ_MONITOR_T));
            TaskMonitor.TaskMonitorInfo[i].id = TaskId;
            TaskMonitor.TaskMonitorInfo[i].taskState = undo;
            TaskMonitor.logicid[i] = SchemeTaskCfgList.OopTaskCfg[i].logicid;
            continue;
        }
        memcpy(&TaskMonitor.TaskMonitorInfo[i],&taskMonitorUnit,sizeof(OOP_ACQ_MONITOR_T));
        TaskMonitor.logicid[i] = SchemeTaskCfgList.OopTaskCfg[i].logicid;

    }
    pthread_rwlock_unlock(&SchemeTaskCfgList.rwLock);
    pthread_mutex_unlock(&TaskMonitor.MonitorLock);
}

/*******************************************************************************
* 函数名称: taskmng_event_plan_list_load 
* 函数功能://从数据中心读取采集类型为2的所有事件采集方案
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_event_plan_list_load()
{
    int ret ;
    uint16 i;
    uint16 count=0;
    OOP_PLAN_EVENT_T EventPlanInfo;
    memset(&EventPlanInfo,0x00,sizeof(OOP_PLAN_EVENT_T));
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 255;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60160200;
    stepReadRecord.MAC.nNum = 0;

    pthread_rwlock_wrlock(&EventPlanList.rwLock);
    EventPlanList.plancount = 0;
    memset(EventPlanList.PlanEvent,0x00,sizeof(PLAN_EVENT_INTO_T)*MAX_EVENT_PLAN_NUM);
    ret = db_read_record_start(gDBClientTask, &inReadRecord, &handle, &recNum);
    if(ret !=0 ||recNum==0)
    {
        TASK_FMT_TRACE(-1,TASK_LOG, "taskmng_event_plan_list_load 开始读失败 ret %d recNum %d  \n",ret,recNum);
    }
    else
    {
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(gDBClientTask, handle, &stepReadRecord, &stepRecordOut);
            if (ret != 0)
            {
                TASK_FMT_TRACE(-1,TASK_LOG, "taskmng_event_plan_list_load 单步读失败 ret %d i %d  \n",ret,i);
                continue;
            }
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60160200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_PLAN_EVENT_T)))
            {
                continue;
            }
            memcpy(&EventPlanInfo,stepRecordOut.record[0].outDataBuf,sizeof(OOP_PLAN_EVENT_T));
            if(EventPlanInfo.nType!=2)
                continue;
            EventPlanList.PlanEvent[count].planID = EventPlanInfo.planID;
            memcpy(&EventPlanList.PlanEvent[count].events,&EventPlanInfo.events,sizeof(OOP_EVENTSELECT_T));
            EventPlanList.PlanEvent[count].msNum = taskmng_ms_list_get(&EventPlanInfo.ms,EventPlanList.PlanEvent[count].msInfo,0);
            TASK_FMT_TRACE(-1,TASK_LOG, "taskmng_event_plan_list_load 事件采集方案 %d msnum %d \n",EventPlanInfo.planID,EventPlanList.PlanEvent[count].msNum);

            count++;
            if(count >= MAX_EVENT_PLAN_NUM)
                break;
        }

    }
    db_read_record_end(gDBClientTask, handle);

    EventPlanList.plancount = count;
    TASK_FMT_TRACE(-1,TASK_LOG, "taskmng_event_plan_list_load 事件采集方案个数为 %d  \n",count);
    pthread_rwlock_unlock(&EventPlanList.rwLock);
}

/*******************************************************************************
* 函数名称: taskmng_task_info_read
* 函数功能: //根据任务信息点号获取任务信息
* 输入参数: Taskid 任务号
* 输出参数: TaskInfo 获取的任务内容
          
* 返 回 值: -1 获取失败
        0  获取成功
*******************************************************************************/
int taskmng_task_info_read(uint16 Taskid,OOP_TASK_T *TaskInfo,uint8 logicid)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = logicid;
    NormalOad.oad.value = 0x60120200;
    NormalOad.infoNum = Taskid;

    ret = db_read_nomal(gDBClientTask,&NormalOad,sizeof(OOP_TASK_T),(uint8*)TaskInfo,&len);

    
    if(ret !=0 ||len!=sizeof(OOP_TASK_T))
        return -1 ;
    return 0;
}


/*******************************************************************************
* 函数名称: taskmng_normal_plan_info_read
* 函数功能: 根据普通方案信息点号获取普通采集方案
* 输入参数: id 普通方案编号
* 输出参数: PlanInfo 获取的普通方案内容
          
* 返 回 值: -1 获取失败
        0  获取成功
*******************************************************************************/
int taskmng_normal_plan_info_read(uint16 id,OOP_PLAN_NORMAL_T *PlanInfo,uint8 logicid)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = logicid;
    NormalOad.oad.value = 0x60140200;
    NormalOad.infoNum = id;

    ret = db_read_nomal(gDBClientTask,&NormalOad,sizeof(OOP_PLAN_NORMAL_T),(uint8*)PlanInfo,&len);

    if(ret !=0 ||len!=sizeof(OOP_PLAN_NORMAL_T))
        return -1 ;
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_eventl_plan_info_read
* 函数功能: 根据事件方案信息点号获取事件采集方案
* 输入参数: id 事件方案编号
* 输出参数: PlanInfo 获取的事件方案内容
          
* 返 回 值: -1 获取失败
        0  获取成功
*******************************************************************************/
int taskmng_eventl_plan_info_read(uint16 id,OOP_PLAN_EVENT_T *PlanInfo,uint8 logicid)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = logicid;
    NormalOad.oad.value = 0x60160200;
    NormalOad.infoNum = id;

    ret = db_read_nomal(gDBClientTask,&NormalOad,sizeof(OOP_PLAN_EVENT_T),(uint8*)PlanInfo,&len);


    if(ret !=0 ||len!=sizeof(OOP_PLAN_EVENT_T))
        return -1 ;
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_trans_plan_info_read
* 函数功能: 根据透明任务方案信息点号获取透明方案
* 输入参数: id 透明方案编号
* 输出参数: PlanInfo 获取的透明方案内容
          
* 返 回 值: -1 获取失败
        0  获取成功
*******************************************************************************/
int taskmng_trans_plan_info_read(uint16 id,OOP_PLAN_TRANS_T *PlanInfo,uint8 logicid)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = logicid;
    NormalOad.oad.value = 0x60180200;
    NormalOad.infoNum = id;

    ret = db_read_nomal(gDBClientTask,&NormalOad,sizeof(OOP_PLAN_TRANS_T),(uint8*)PlanInfo,&len);

    if(ret !=0 ||len!=sizeof(OOP_PLAN_TRANS_T))
        return -1 ;
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_encrypt_plan_info_read
* 函数功能: 根据转加密任务方案信息点号获取转加密方案
* 输入参数: id 转加密方案编号
* 输出参数: PlanInfo 获取的透明方案内容
          
* 返 回 值: -1 获取失败
        0  获取成功
*******************************************************************************/
int taskmng_encrypt_plan_info_read(uint16 id,OOP_PLAN_ENCRYPT_T *PlanInfo)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    memset(&NormalOad,0x00,sizeof(NOMAL_OAD_T));
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x601E0200;
    NormalOad.infoNum = id;

    ret = db_read_nomal(gDBClientTask,&NormalOad,sizeof(OOP_PLAN_ENCRYPT_T),(uint8*)PlanInfo,&len);

    if(ret !=0 ||len!=sizeof(OOP_PLAN_ENCRYPT_T))
        return -1 ;
    return 0;
}


/*******************************************************************************
* 函数名称: taskmng_task_record_file_delete 
* 函数功能://删除任务记录文件
* 输入参数: 
            TaskId 任务号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_task_record_file_delete(uint16 TaskId,uint8 logicid)
{
    //char      pfile[100]={0};
    
    uint8 pipe;
    int ret;
    DB_UPDATE_T dbupdate;
    //修改为将任务id通知到各个抄表线程
    for(pipe=0;pipe<AMR_THREAD_NUM+1;pipe++)
    {
        //ret = task_send_msg(gTaskSock,MSG_RECORD_FILE_DEL,(uint8*)&TaskId,2,IPC_AMR+pipe);
        //task_send_msg这个接口会阻塞导致线程加锁卡死
        dbupdate.logicid = logicid;
        dbupdate.infonum = TaskId;
        dbupdate.oad.value = 0;
        ret = taskmng_db_update_queue_write( &pTaskRecordFileDel[pipe], &dbupdate);
        TASK_FMT_DEBUG(-1,TASK_LOG, "通知抄表线程删除任务 logicid %d id %d 记录文件 ret %d \n",logicid,TaskId,ret);
    }
    
}
//删除指定逻辑设备号的任务
void taskmng_task_delete_with_logicid(uint8 logicid,OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue)
{
    uint16 i;
    //uint16 leftcount = 0;
    //OOP_TASK_CFG lefttask[255];
    //SCHEME_TASK_RUN_INFO lefttaskruninfo[SCHEME_MAX_NUM];
    //memset(lefttask,0x00,sizeof(OOP_TASK_CFG)*255);
    //结构体超大 会栈溢出
    //memset(lefttaskruninfo,0x00,sizeof(SCHEME_TASK_RUN_INFO)*SCHEME_MAX_NUM);
    for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
    {
            taskmng_task_record_file_delete(pSchemeTaskParaList->OopTaskCfg[i].TaskId,pSchemeTaskParaList->OopTaskCfg[i].logicid);
            taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,pSchemeTaskParaList->OopTaskCfg[i].logicid);
        
    }
    pSchemeTaskParaList->TaskNum = 0;
    memset(pSchemeTaskParaList->OopTaskCfg,0x00,sizeof(OOP_TASK_CFG)*255);
    memset(SchemeTaskRunQueue,0x00,sizeof(SCHEME_TASK_RUN_INFO)*SCHEME_MAX_NUM);

}

/*******************************************************************************
* 函数名称: taskmng_ms_list_get_by_mettype
* 函数功能: 根据一组用户类型获取对应的电表MS
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
uint16 taskmng_ms_list_get_by_mettype(OOP_MSP_T *inMS, MS_INFO *pMsInfo,uint8 logicid)
{
    uint16 i,j;
    uint8 MtrType;
    uint16 MsNum = 0;
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {   
        if(MeterFileAll.MeterLogicid[i] == logicid)
        {
            MtrType = MeterFileAll.MeterFile[i].basic.userType;
            for(j=0;j<inMS->mst.size;j++)
            {
                if(MtrType == inMS->mst.type[j])
                {   
                    pMsInfo[MsNum].Prtl = MeterFileAll.MeterFile[i].basic.protocol;
                    pMsInfo[MsNum].Pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
                    pMsInfo[MsNum].Sn = MeterFileAll.MeterFile[i].nIndex;    
                    MsNum++;
                    break;
                }
            }
        }
    }
    return MsNum;
}
/*******************************************************************************
* 函数名称: taskmng_ms_list_get_by_metaddr
* 函数功能: 根据一组用户地址获取对应的电表MS
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
uint16 taskmng_ms_list_get_by_metaddr(OOP_MSP_T *inMS, MS_INFO *pMsInfo,uint8 logicid)
{
    uint16 i,j;
    OOP_TSA_T MtrAddr;
    //OOP_TSA_T tem_MtrAddr;
    for(i=0;i<inMS->msa.size;i++)
    {
        OOP_TSA_T *tsa = &inMS->msa.add[i];
        TASK_FMT_TRACE(-1,TASK_LOG,"i %d flag %d oldvxd %d vxd %d 地址 %02x%02x%02x%02x%02x%02x \n",i,tsa->flag,tsa->oldvxd,tsa->vxd,
            tsa->add[0],tsa->add[1],tsa->add[2],tsa->add[3],tsa->add[4],tsa->add[5]);

    }
    uint16 MsNum = 0;
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {   
        if(MeterFileAll.MeterLogicid[i] == logicid)
        {
            MtrAddr = MeterFileAll.MeterFile[i].basic.tsa;
            for(j=0;j<inMS->msa.size;j++)
            {
                if(IsMtrAddrMatch(&inMS->msa.add[j],&MtrAddr))
                {   
                    pMsInfo[MsNum].Prtl = MeterFileAll.MeterFile[i].basic.protocol;
                    pMsInfo[MsNum].Pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
                    pMsInfo[MsNum].Sn = MeterFileAll.MeterFile[i].nIndex;    
                    MsNum++;
                    break;
                }
            }
        }
    }
    return MsNum;
}
/*******************************************************************************
* 函数功能: 根据一组配置序号获取对应的电表MS
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
uint16 taskmng_ms_list_get_by_metsn(OOP_MSP_T *inMS, MS_INFO *pMsInfo,uint8 logicid)
{
    uint16 i,j;
    uint16 Sn;
    uint16 MsNum = 0;
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {   
        if(MeterFileAll.MeterLogicid[i] == logicid)
        {
            Sn = MeterFileAll.MeterFile[i].nIndex;
            for(j=0;j<inMS->msi.size;j++)
            {
                if(Sn == inMS->msi.idx[j])
                {   
                    pMsInfo[MsNum].Prtl = MeterFileAll.MeterFile[i].basic.protocol;
                    pMsInfo[MsNum].Pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
                    pMsInfo[MsNum].Sn = MeterFileAll.MeterFile[i].nIndex;    
                    MsNum++;
                    break;
                }
            }
        }
    }
    return MsNum;
}

/*******************************************************************************
* 函数功能: 根据一组用户类型区间获取系列用户类型
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
uint16 taskmng_ms_list_get_by_mettype_region(OOP_MSP_T *inMS, MS_INFO *pMsInfo,uint8 logicid)
{
    uint16 i,j;
    REGION_TYPE    RegionType;
    uint8 start=0;
    uint8 end=0;
    uint16 MsNum = 0;
    uint8 MtrType;
    for(j=0;j<inMS->msrt.size;j++)
    {
        RegionType = inMS->msrt.region[j].nType;
        start = inMS->msrt.region[j].start;
        end = inMS->msrt.region[j].end;
        switch(RegionType)
        {
        case FRONTCLOSE_BACKOPEN://前闭后开
        {
            if(end == 0)
                return 0;
            end -= 1;
            break;
        }

        case FRONTOPEN_BACKCLOSE://前开后闭
        {
            start += 1;
            break;
        }

        case FRONTCLOSE_BACKCLOSE://前闭后闭
        {
            break;
        }

        case FRONTOPEN_BACKOPEN://前开后开
        {
            start += 1;
            if(end == 0)
                return 0;
            end -= 1;
            break;
        }
        default:
            break;
        }
        if(start>end)
            return 0;
        for(i=0;i<MeterFileAll.MeterCount;i++)
        {
            if(MeterFileAll.MeterLogicid[i]==logicid)
            {
                MtrType = MeterFileAll.MeterFile[i].basic.userType;
                if(MtrType>=start && MtrType<=end)
                {
                    //判断当前的序号是否存在于当前序号列表中
                    if(IsMtrSnInList(MeterFileAll.MeterFile[i].nIndex, MsNum, pMsInfo))
                        continue;
                    pMsInfo[MsNum].Prtl = MeterFileAll.MeterFile[i].basic.protocol;
                    pMsInfo[MsNum].Pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
                    pMsInfo[MsNum].Sn = MeterFileAll.MeterFile[i].nIndex;    
                    MsNum++;

                }
            }
        }
    }

    return MsNum;
}

/*******************************************************************************
* 函数功能: 根据一组用户地址区间获取系列用户类型
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
uint16 taskmng_ms_list_get_by_metaddr_region(OOP_MSP_T *inMS, MS_INFO *pMsInfo,uint8 logicid)
{
    uint16 i,j;
    REGION_TYPE    RegionType;
    OOP_TSA_T start;
    OOP_TSA_T end;
    int64         tmpaddr = 0;
    int64         startaddr64 = 0;
    int64         endaddr64 = 0;
    uint16 MsNum = 0;

    for(j=0;j<inMS->msra.size;j++)
    {
        RegionType = inMS->msra.region[j].nType;
        start = inMS->msra.region[j].start;
        end = inMS->msra.region[j].end;
        switch(RegionType)
        {
        case FRONTCLOSE_BACKOPEN://前闭后开
        {
            TsaCalcfun(&end,-1);//对地址进行减1操作
            break;
        }

        case FRONTOPEN_BACKCLOSE://前开后闭
        {
            TsaCalcfun(&start,1);//对地址进行加1操作
            break;
        }

        case FRONTCLOSE_BACKCLOSE://前闭后闭
        {
            break;
        }

        case FRONTOPEN_BACKOPEN://前开后开
        {
            TsaCalcfun(&start,1);//对地址进行加1操作
            TsaCalcfun(&end,-1);//对地址进行减1操作
        }

        default:
            break;
        }
        memcpy_r((uint8*)&startaddr64, start.add, start.len+1);
        memcpy_r((uint8*)&endaddr64, end.add, end.len+1);
        for(i=0;i<MeterFileAll.MeterCount;i++)
        {
            if(MeterFileAll.MeterLogicid[i] == logicid)
            {
                memcpy_r((uint8*)&tmpaddr, MeterFileAll.MeterFile[i].basic.tsa.add, MeterFileAll.MeterFile[i].basic.tsa.len+1);
                if(tmpaddr>=startaddr64 && tmpaddr<=endaddr64)
                {
                    //判断当前的序号是否存在于当前序号列表中
                    if(IsMtrSnInList(MeterFileAll.MeterFile[i].nIndex, MsNum, pMsInfo))
                        continue;
                    pMsInfo[MsNum].Prtl = MeterFileAll.MeterFile[i].basic.protocol;
                    pMsInfo[MsNum].Pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
                    pMsInfo[MsNum].Sn = MeterFileAll.MeterFile[i].nIndex;    
                    MsNum++;
                }
            }
        }
    }
    return MsNum;
}

/*******************************************************************************
* 函数功能: 根据一组用户序号区间获取系列用户类型
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
uint16 taskmng_ms_list_get_by_metsn_region(OOP_MSP_T *inMS, MS_INFO *pMsInfo,uint8 logicid)
{
    uint16 i,j;
    REGION_TYPE    RegionType;
    uint16 start=0;
    uint16 end=0;
    uint16 MsNum = 0;
    uint16 Sn;
    for(j=0;j<inMS->msri.size;j++)
    {
        RegionType = inMS->msri.region[j].nType;
        start = inMS->msri.region[j].start;
        end = inMS->msri.region[j].end;
        switch(RegionType)
        {
        case FRONTCLOSE_BACKOPEN://前闭后开
        {
            if(end == 0)
                return 0;
            end -= 1;
            break;
        }

        case FRONTOPEN_BACKCLOSE://前开后闭
        {
            start += 1;
            break;
        }

        case FRONTCLOSE_BACKCLOSE://前闭后闭
        {
            break;
        }

        case FRONTOPEN_BACKOPEN://前开后开
        {
            start += 1;
            if(end == 0)
                return 0;
            end -= 1;
            break;
        }
        default:
            break;
        }

        if(start > end)
            return 0;
        for(i=0;i<MeterFileAll.MeterCount;i++)
        {
            if(MeterFileAll.MeterLogicid[i]==logicid)
            {
                Sn = MeterFileAll.MeterFile[i].nIndex;
                if(Sn>=start && Sn<=end)
                {
                    //判断当前的序号是否存在于当前序号列表中
                    if(IsMtrSnInList(MeterFileAll.MeterFile[i].nIndex, MsNum, pMsInfo))
                        continue;
                    pMsInfo[MsNum].Prtl = MeterFileAll.MeterFile[i].basic.protocol;
                    pMsInfo[MsNum].Pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
                    pMsInfo[MsNum].Sn = MeterFileAll.MeterFile[i].nIndex;    
                    MsNum++;
                }
            }
        }
    }
    return MsNum;
}

/*******************************************************************************
* 函数功能: 根据一组配置序号获取对应的电表MS
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
uint16 taskmng_ms_list_get_by_devicetype(OOP_MSP_T *inMS, MS_INFO *pMsInfo,uint8 logicid)
{
    uint16 i,j;
    uint16 DeviceType = 0;
    uint16 MsNum = 0;
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {   
        if(MeterFileAll.MeterLogicid[i] == logicid)
        {
            for(j=0;j<MeterFileAll.MeterFile[i].annex.nNum;j++)
            {
                if(MeterFileAll.MeterFile[i].annex.annexObj[j].oad.value== 0x40360200)
                {
                    memcpy(&DeviceType,MeterFileAll.MeterFile[i].annex.annexObj[j].value,sizeof(uint16));
                    break;
                }
            }

            for(j=0;j<inMS->msdt.size;j++)
            {
                if(DeviceType == inMS->msdt.type[j])
                {   
                    pMsInfo[MsNum].Prtl = MeterFileAll.MeterFile[i].basic.protocol;
                    pMsInfo[MsNum].Pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
                    pMsInfo[MsNum].Sn = MeterFileAll.MeterFile[i].nIndex;    
                    MsNum++;
                    break;
                }
            }
        }
    }
    return MsNum;
}

/*******************************************************************************
* 函数名称: taskmng_ms_list_get
* 函数功能: 根据下发的MS信息遍历需要抄读的电表
* 输入参数: 
            inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
            
* 输出参数: 无
* 返 回 值: 抄读电表数量
*******************************************************************************/
uint16 taskmng_ms_list_get(OOP_MSP_T *inMS, MS_INFO *pMsInfo,uint8 logicid)
{
    uint16 MsNum = 0;
    uint16 i;
    TASK_FMT_DEBUG(-1,TASK_LOG, "taskmng_ms_list_get choice %d logicid %d \n",inMS->choice ,logicid);
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    switch (inMS->choice)
    {
    case 1:     //全部用户地址
        for(i=0;i<MeterFileAll.MeterCount;i++)
        {           
            if(MeterFileAll.MeterLogicid[i] == logicid)
            {
                pMsInfo[MsNum].Prtl = MeterFileAll.MeterFile[i].basic.protocol;
                pMsInfo[MsNum].Pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
                pMsInfo[MsNum].Sn = MeterFileAll.MeterFile[i].nIndex;    
                MsNum++;
            }
        }
        break;
    case 2:     //一组用户类型
        MsNum = taskmng_ms_list_get_by_mettype(inMS,pMsInfo,logicid);
        break;
    case 3:     //一组用户地址
        MsNum = taskmng_ms_list_get_by_metaddr(inMS,pMsInfo,logicid);
        break;
    case 4:     //一组配置序号
        MsNum = taskmng_ms_list_get_by_metsn(inMS,pMsInfo,logicid);
        break;
    case 5:     //一组用户类型区间
        MsNum =  taskmng_ms_list_get_by_mettype_region(inMS,pMsInfo,logicid);
        break;
    case 6:     //一组用户地址区间
        MsNum = taskmng_ms_list_get_by_metaddr_region(inMS,pMsInfo,logicid);
        break;
    case 7:     //一组配置序号区间
        MsNum = taskmng_ms_list_get_by_metsn_region(inMS,pMsInfo,logicid);
        break;

    case 8:     //一组设备类型
        MsNum = taskmng_ms_list_get_by_devicetype(inMS,pMsInfo,logicid);
        break;

    default:
        pthread_rwlock_unlock(&MeterFileAll.rwLock);
        return 0;
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return MsNum;
}

/*******************************************************************************
* 函数名称: taskmng_csd_task_add
* 函数功能: 添加任务
* 输入参数: MainOad                  //主OAD信息(数据限定词,实时数据为0000)
            SubOadNum                //从OAD数量
            SubOad                   //从OAD数组
            pCsdInfo                 //pCsdInfo转换后的CSD任务
            Acqtype                  //任务类型为时标时需要自动采集数据时标
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
void taskmng_csd_task_add(OOP_OAD_U MainOad,uint8 SubOadNum, OOP_OAD_U *SubOad,CSD_TASK_INFO *pCsdInfo,AcqInfo_t *Acqtype)
{
    uint16 i,j;
    uint8 IsNewCsd;
    //第一次添加CSD抄表任务
    if(pCsdInfo->MainOad.nObjID==0xffff)
    {
        pCsdInfo->MainOad = MainOad;
        if((pCsdInfo->MainOad.nObjID&0xf000) == 0x3000)//事件
        {
            pCsdInfo->OadListInfo[0].nObjID = 0x2022;//事件记录序号
            pCsdInfo->OadListInfo[0].attID = 02;
            pCsdInfo->OadListInfo[0].nIndex = 00;

            
            pCsdInfo->SubOadNum = 1;
        }else
        {
            //如果CSD类型不为实时数据或事件数据
            if(pCsdInfo->MainOad.nObjID != 0x0000)
            {
                //判断如果按照冻结时标采集或者按照时标间隔采集那么优先抄读冻结时标(防止现场不配置抄读时标)
                if(ACQTYP_FRZTM == Acqtype->Acqtype || ACQTYP_TI == Acqtype->Acqtype)
                {
                    pCsdInfo->OadListInfo[0].nObjID = 0x2021;
                    pCsdInfo->OadListInfo[0].attID = 02;
                    pCsdInfo->OadListInfo[0].nIndex = 00;


                    pCsdInfo->SubOadNum = 1;
                }
            }
            
        }
    }
     //目前任务不支持一个方案多个任务类型
    if(pCsdInfo->MainOad.nObjID != MainOad.nObjID)
    {
        return;
    }
    for(i = 0; i < SubOadNum; i++)
    {
        IsNewCsd  = TRUE;

        //判断添加的OAD是否已经存在,如果存在不再重复进行添加
        for(j = 0; j < pCsdInfo->SubOadNum; j++)
        {
            if(SubOad[i].value== pCsdInfo->OadListInfo[j].value)                 
            {
                IsNewCsd = FALSE;
                break;
            }
        }

        if(IsNewCsd)//如果此CSD无重复
        {
            j = pCsdInfo->SubOadNum;
            //pCsdInfo->OadListInfo[j].nObjID  = SubOad[i].nObjID;
           // pCsdInfo->OadListInfo[j].attID   = SubOad[i].attID;
           // pCsdInfo->OadListInfo[j].nIndex  = SubOad[i].nIndex;
            pCsdInfo->OadListInfo[j] = SubOad[i];
            pCsdInfo->SubOadNum++;
        }
    }
}

/*******************************************************************************
* 函数功能: 根据传递进来的CSD参数转换成对应的CSD以及OAD字典信息
* 输入参数: inData                  //配置的参数
            offset                  //偏移信息
            isWithType              //数据类型
            pCsdInfo                //传递进来的CSD参数
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
uint16 taskmng_oad_list_get(OOP_RECORD_COL_T *inRecdCol,CSD_TASK_INFO *pCsdInfo,AcqInfo_t *Acqtype)
{
    uint8       i,j;
    uint8       CSDNum;
    OOP_OAD_U        MainOad;
    uint8       SubOadNum;
    OOP_OAD_U        SubOad[MAX_OAD_NUM];
    OOP_CSD_T   CSD;
    CSDNum = inRecdCol->num;
    
    pCsdInfo->MainOad.nObjID = 0xffff;
    for(i=0;i<CSDNum;i++)
    {
        CSD = inRecdCol->csd[i];
        //设计依据:每个CSD应配置采集为一种数据类型,不应该配置为两种及以上的数据类型
        if(CSD.choice==0)   //类型为OAD即表示实时数据
        {
            //主OAD为0
            memset(&MainOad,0x0,sizeof(OOP_OAD_U));

            SubOadNum = 1;
            SubOad[0] = CSD.oad;
            
         }
        else if(CSD.choice==1)
        {
            MainOad = CSD.road.oadMain;

            SubOadNum = CSD.road.oadCols.nNum;
            if(SubOadNum >= MAX_OAD_NUM)
                SubOadNum = MAX_OAD_NUM;
            for(j=0;j<SubOadNum;j++)
            {
                SubOad[j] = CSD.road.oadCols.oad[j];
            }
        }
        else
            break;
            
        //将CSD添加到抄表任务中
        taskmng_csd_task_add(MainOad,SubOadNum,SubOad,pCsdInfo,Acqtype);
    }
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_road_list_add
* 函数功能: 根据传递进来的Road参数转换成对应的CSD以及OAD字典信息
* 输入参数: inData                  //配置的参数
            offset                  //偏移信息
            isWithType              //数据类型
            pCsdInfo                //传递进来的CSD参数
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/

uint16 taskmng_road_list_add(OOP_EVENTSELECT_T *inEvents,TASK_INFO *pTaskInfo)
{
    uint8       i,j;
    uint8       ROADNum;
    OOP_OAD_U        MainOad;
    uint8       SubOadNum;
    OOP_OAD_U        SubOad[MAX_OAD_NUM];
    OOP_ROAD_T  Road;
    AcqInfo_t   Acqtype;
    Acqtype.Acqtype = ACQTYP_NULL;
    ROADNum = inEvents->nNum;
    
    for(i=0;i<ROADNum;i++)
    {
        Road = inEvents->cols[i];
        MainOad = Road.oadMain;
        
        SubOadNum = Road.oadCols.nNum;
        TASK_FMT_DEBUG(-1,TASK_LOG, "taskmng_road_list_add roadindex %d  mainoad %08x suboadnum %d  \n",i,Road.oadMain.value,SubOadNum);
        if(SubOadNum >= MAX_OAD_NUM)
            SubOadNum = MAX_OAD_NUM;
        pTaskInfo->RoadTask[i].MainOad.nObjID = 0xffff;
        for(j=0;j<SubOadNum;j++)
        {
            SubOad[j] = Road.oadCols.oad[j];
            TASK_FMT_DEBUG(-1,TASK_LOG, "SubOad %d %08x \n",j,Road.oadCols.oad[j].value);
        }
        taskmng_csd_task_add( MainOad,SubOadNum, SubOad, &pTaskInfo->RoadTask[i],&Acqtype);
        TASK_FMT_DEBUG(-1,TASK_LOG, "MainOad %08x  SubOadNum %d \n",pTaskInfo->RoadTask[i].MainOad.value,pTaskInfo->RoadTask[i].SubOadNum);
        pTaskInfo->RoadNum++;
        pTaskInfo->TotalSubOadNum+=SubOadNum;
        if(pTaskInfo->RoadNum >= MAX_EVENT_ROAD_NUM)
            break;
    }
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_priority_fix_by_mainoi
* 函数功能: 根据任务的主OAD修正参数中配置的优先级，以保证正常完成各项抄表任务
* 输入参数: pSchemeTaskTaskRunInfo                  //载波方案任务结构体
*           OopTaskCfg                              //任务配置结构体
* 输出参数: 无
* 返 回 值: >0: 该任务要修正的优先级 =0: 不进行修正
*******************************************************************************/
uint8 taskmng_priority_fix_by_mainoi(SCHEME_TASK_RUN_INFO  *pSchemeTaskTaskRunInfo, OOP_TASK_CFG OopTaskCfg)
{
    uint32 dwPeriod = OopTaskCfg.RunCycle;
    uint16 MainOI = pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID;

    switch (OopTaskCfg.RunCycleUnit)    //执行频率单位
    {
    case TASK_CYCLE_FLAG_SEC:       //秒
        break;

    case TASK_CYCLE_FLAG_MIN:       //分钟
        dwPeriod *= 60L;
        break;

    case TASK_CYCLE_FLAG_HOUR:      //小时
        dwPeriod *= 3600L;
        break;

    case TASK_CYCLE_FLAG_DAY:       //日
    case TASK_CYCLE_FLAG_MON:       //月
    case TASK_CYCLE_FLAG_YEAR:      //月
        dwPeriod = 86400L;
        break;

    default:
        dwPeriod = 86400L;
        break;
    }

    if(pSchemeTaskTaskRunInfo->SchemeType == EVENTPLAN)
    {
        //事件采集任务，属于频繁任务时优先级高于月冻结，否则优先级等于月冻结
        if(dwPeriod < 86400L)
            return 3;
        else
            return 4;
    }

    switch(MainOI)
    {
    case 0:          //实时,属于频繁任务时优先级高于日冻结，否则优先级低于日冻结
        if(dwPeriod < 86400L)
            return 2;
        else
            return 3;
        break;

    case 0x5002:     //分钟冻结
    case 0x5003:     //分钟冻结
        return 1;
        break;

    case 0x5004:     //日冻结
        return 2;
        break;

    case 0x5005:     //抄表日冻结
    case 0x5006:     //月冻结
        return 4;
        break;

    default:
        return 4;
        break;
    }
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_trans_plan_info_get
* 函数功能: 获取透明方案需要抄的电表
* 输入参数: TransArray 透明方案内容集合
* 输出参数: pMsInfo 透明方案要抄的电表集合
          TransInfo 透明方案内容
* 返 回 值: 透明方案需要抄的电表个数
*******************************************************************************/
int taskmng_trans_plan_info_get(OOP_TRANS_ARRAY_T *TransArray,MS_INFO *pMsInfo,OOP_TRANS_INFO_T *TransInfo,uint8 logicid)
{
    uint8 i;
    uint16 j;
    OOP_TSA_T Tsa;
    uint8 MsNum = 0;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<TransArray->nNum;i++)
    {
        memset(&Tsa,0x00,sizeof(OOP_TSA_T));
        Tsa = TransArray->info[i].addr;
        for(j=0;j<MeterFileAll.MeterCount;j++)
        {
            if(MeterFileAll.MeterLogicid[j] == logicid && memcmp(Tsa.add,MeterFileAll.MeterFile[j].basic.tsa.add,6)==0)
            {
                pMsInfo[MsNum].Prtl = MeterFileAll.MeterFile[j].basic.protocol;
                pMsInfo[MsNum].Pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[j].basic.port);
                pMsInfo[MsNum].Sn = MeterFileAll.MeterFile[j].nIndex;    
                memcpy(&TransInfo[MsNum],&TransArray->info[i],sizeof(OOP_TRANS_INFO_T));
                MsNum++;
                break;
            }
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return MsNum;
}

/*******************************************************************************
* 函数名称: taskmng_trans_plan_info_get
* 函数功能: 获取透明方案需要抄的电表
* 输入参数: TransArray 透明方案内容集合
* 输出参数: pMsInfo 透明方案要抄的电表集合
          TransInfo 透明方案内容
* 返 回 值: 透明方案需要抄的电表个数
*******************************************************************************/
int taskmng_encrypt_plan_info_get(OOP_PLAN_ENCRYPT_T *planinfo,MS_INFO *pMsInfo,OOP_ENCRYPT_DATA_T *encryptData)
{
    uint8 i;
    uint16 j;
   
    uint8 MsNum = 0;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<planinfo->config.nNum;i++)
    {
        for(j=0;j<MeterFileAll.MeterCount;j++)
        {
            if(MeterFileAll.MeterLogicid[j] == 0 && MeterFileAll.MeterFile[j].nIndex == planinfo->config.meter[i].metid)
            {
                pMsInfo[MsNum].Prtl = MeterFileAll.MeterFile[j].basic.protocol;
                pMsInfo[MsNum].Pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[j].basic.port);
                pMsInfo[MsNum].Sn = MeterFileAll.MeterFile[j].nIndex;    
                memcpy(&pMsInfo[MsNum].key,&planinfo->config.meter[i].key,sizeof(OOP_OCTETVAR256_T));
                MsNum++;
                break;
            }
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    memcpy(encryptData,&planinfo->endata,sizeof(OOP_ENCRYPT_DATA_T));
    return MsNum;
}


/*******************************************************************************
* 函数名称: taskmng_task_change_fun 
* 函数功能:任务变更处理
* 输入参数: pSchemeTaskParaList 所有任务内容
          SchemeTaskRunQueue 所有任务具体信息
          choice 0 删除 1 变更
          TaskInfo 变更的任务信息
* 输出参数: 
* 返 回 值: TRUE 任务发生改变 FALSE 任务无需改变
*******************************************************************************/
BOOL taskmng_task_change_fun(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue,uint8 choice,OOP_TASK_T *TaskInfo,uint8 logicid)
{
    uint16 i;
    int ret;
    int index = -1;
    ePlanType       planType; 
    uint8           SchemeType = 0;
    uint8           FixPriority = 0;
    uint8 transindex;
    uint8 SchemeId;
    if(choice == 0)
    {
        //未读取到任务 表示任务删除
        for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
        {
            if(pSchemeTaskParaList->OopTaskCfg[i].TaskId == TaskInfo->id &&pSchemeTaskParaList->OopTaskCfg[i].logicid == logicid)
            {
                index = i;
                memset(&pSchemeTaskParaList->OopTaskCfg[i],0x00,sizeof(OOP_TASK_CFG));
                memset(&SchemeTaskRunQueue[i],0x00,sizeof(SCHEME_TASK_RUN_INFO));
                break;
            }
        }
        if(index!=-1)
        {
            for(i=index;i<pSchemeTaskParaList->TaskNum-1;i++)
            {
                pSchemeTaskParaList->OopTaskCfg[i] = pSchemeTaskParaList->OopTaskCfg[i+1];
                SchemeTaskRunQueue[i] = SchemeTaskRunQueue[i+1];
            }
            memset(&pSchemeTaskParaList->OopTaskCfg[pSchemeTaskParaList->TaskNum-1],0x00,sizeof(OOP_TASK_CFG));
            memset(&SchemeTaskRunQueue[pSchemeTaskParaList->TaskNum-1],0x00,sizeof(SCHEME_TASK_RUN_INFO));
            pSchemeTaskParaList->TaskNum--;
            return TRUE;
        }
        return FALSE;
    }
    else
    {
        //读取到任务 

        for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
        {
            if(pSchemeTaskParaList->OopTaskCfg[i].TaskId == TaskInfo->id &&pSchemeTaskParaList->OopTaskCfg[i].logicid == logicid)
            {
                index = i;
                
                taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,logicid);
                memset(&pSchemeTaskParaList->OopTaskCfg[i],0x00,sizeof(OOP_TASK_CFG));
                memset(&SchemeTaskRunQueue[i],0x00,sizeof(SCHEME_TASK_RUN_INFO));
                break;
            }
        }
        //如果没有相同的任务 则更新到最后一个位置
        if(index==-1)
        {
            index = pSchemeTaskParaList->TaskNum;
        }

        pSchemeTaskParaList->OopTaskCfg[index].logicid = logicid;
        pSchemeTaskParaList->OopTaskCfg[index].TaskId = TaskInfo->id;
        pSchemeTaskParaList->OopTaskCfg[index].RunCycle = TaskInfo->acqFreq.value;
        pSchemeTaskParaList->OopTaskCfg[index].RunCycleUnit = TaskInfo->acqFreq.unit;
        pSchemeTaskParaList->OopTaskCfg[index].SchemeType = TaskInfo->planType;
        pSchemeTaskParaList->OopTaskCfg[index].SchemeId = TaskInfo->planID;
        //DateTimeSToChar(&TaskInfo->startTime,pSchemeTaskParaList->OopTaskCfg[index].RunBeginTime);
        //DateTimeSToChar(&TaskInfo->endTime,pSchemeTaskParaList->OopTaskCfg[index].RunEndTime);
        pSchemeTaskParaList->OopTaskCfg[index].RunBeginTime = TaskInfo->startTime;
        pSchemeTaskParaList->OopTaskCfg[index].RunEndTime = TaskInfo->endTime;
        pSchemeTaskParaList->OopTaskCfg[index].DelayTime = TaskInfo->acqDelay.value;
        pSchemeTaskParaList->OopTaskCfg[index].DelayTimeUnit = TaskInfo->acqDelay.unit;
        pSchemeTaskParaList->OopTaskCfg[index].Priority = TaskInfo->priority;
        pSchemeTaskParaList->OopTaskCfg[index].TaskState = TaskInfo->state;
        pSchemeTaskParaList->OopTaskCfg[index].PreRunScript = TaskInfo->startScriptID;
        pSchemeTaskParaList->OopTaskCfg[index].OverRunScript = TaskInfo->endScriptID;
        pSchemeTaskParaList->OopTaskCfg[index].TimePeriordType = TaskInfo->workPeriod.type;
        pSchemeTaskParaList->OopTaskCfg[index].TimePeriordNum = TaskInfo->workPeriod.nNum;
        memcpy(pSchemeTaskParaList->OopTaskCfg[index].Periordhhmm,TaskInfo->workPeriod.period,sizeof(TASKPERIOD)*OOP_MAX_WORKPERIOD);

        planType = pSchemeTaskParaList->OopTaskCfg[index].SchemeType-1;
        TASK_FMT_DEBUG(-1,TASK_LOG, "planType %d 任务状态 %d \n",planType, TaskInfo->state);
        switch(planType)
        {
            case NORMALPLAN:   // 普通采集方案
                SchemeType = 1;
                break;
        
            case EVENTPLAN:    // 事件采集方案
                SchemeType = 1;
                break;
            case TRANSPARENTPLAN:
                SchemeType = 1;
                break;
            case ENCRYPTPLAN:
                SchemeType = 1;
                break;
            default:
                SchemeType = 0;
                break;
        }
        if(SchemeType == 0)//目前支持普通方案以及事件方案
            return FALSE;
            
        memset(pSchemeTaskParaList->OopTaskCfg[index].ThisTurnBeginTime, 0, 7);
        memset(pSchemeTaskParaList->OopTaskCfg[index].ThisTurnEndTime, 0, 7);

        //初始化各个任务运行状态信息
        SCHEME_TASK_RUN_INFO  *pSchemeTaskTaskRunInfo = NULL;
        pSchemeTaskTaskRunInfo = &SchemeTaskRunQueue[index];
        memset(pSchemeTaskTaskRunInfo, 0, sizeof(SCHEME_TASK_RUN_INFO));

        if(pSchemeTaskParaList->OopTaskCfg[index].RunCycle == 5 && pSchemeTaskParaList->OopTaskCfg[index].RunCycleUnit == 1)
        {
            pSchemeTaskTaskRunInfo->FiveMinTaskFlag = 1;
        }
        pSchemeTaskTaskRunInfo->logicid       = logicid;
        pSchemeTaskTaskRunInfo->TaskId        = TaskInfo->id;
        pSchemeTaskTaskRunInfo->SchemeType    = pSchemeTaskParaList->OopTaskCfg[index].SchemeType-1;  //采集方案类型
        pSchemeTaskTaskRunInfo->SchemeId      = pSchemeTaskParaList->OopTaskCfg[index].SchemeId;
        pSchemeTaskTaskRunInfo->DelayTime     = pSchemeTaskParaList->OopTaskCfg[index].DelayTime;
        pSchemeTaskTaskRunInfo->DelayTimeUnit  = pSchemeTaskParaList->OopTaskCfg[index].DelayTimeUnit;
        pSchemeTaskTaskRunInfo->RunResult     = SCHEME_UNDONE;
        pSchemeTaskTaskRunInfo->Priority      = pSchemeTaskParaList->OopTaskCfg[index].Priority;
        //感觉方案id和方案类型从数据中心读取方案信息

        switch (planType)
        {
        case NORMALPLAN:    //普通采集方案
        {
            //根据方案编号读取方案数据
            OOP_PLAN_NORMAL_T NormalPlanInfo;
            memset(&NormalPlanInfo,0x00,sizeof(OOP_PLAN_NORMAL_T));
            ret = taskmng_normal_plan_info_read(pSchemeTaskTaskRunInfo->SchemeId, &NormalPlanInfo,logicid);
            TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取方案 %d ret = %d 方案大小 %d \n",TaskInfo->id,pSchemeTaskTaskRunInfo->SchemeId,ret,sizeof(OOP_PLAN_NORMAL_T));
            if(ret<0)
            {
                pSchemeTaskParaList->OopTaskCfg[index].TaskState = 3;
                break;   //如果没有对应的方案也加载
            }
            TASK_FMT_DEBUG(-1,TASK_LOG, "NormalPlanInfo.ms.choice %d \n",NormalPlanInfo.ms.choice);
            pSchemeTaskTaskRunInfo->AcqInfo.Acqtype = NormalPlanInfo.recdRow.choice;
            pSchemeTaskTaskRunInfo->StoreTimeType = NormalPlanInfo.tmFlag;
            if(ACQTYP_LASTN == pSchemeTaskTaskRunInfo->AcqInfo.Acqtype)
            {
                pSchemeTaskTaskRunInfo->AcqInfo.LastN = NormalPlanInfo.recdRow.num;
            }
            else if(ACQTYP_TI == pSchemeTaskTaskRunInfo->AcqInfo.Acqtype)
            {   
                pSchemeTaskTaskRunInfo->AcqInfo.TI.value = NormalPlanInfo.recdRow.ti.value;
                pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = NormalPlanInfo.recdRow.ti.unit;             

            }
            else if(ACQTYP_BC == pSchemeTaskTaskRunInfo->AcqInfo.Acqtype)
            {
                pSchemeTaskTaskRunInfo->AcqInfo.TI.value = NormalPlanInfo.recdRow.rd.ti.value;
                pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = NormalPlanInfo.recdRow.rd.ti.unit;
                pSchemeTaskTaskRunInfo->AcqInfo.LastN =NormalPlanInfo.recdRow.rd.last;
            }
            
            pSchemeTaskTaskRunInfo->TotalNum= taskmng_ms_list_get(&NormalPlanInfo.ms,pSchemeTaskTaskRunInfo->MsInfo,logicid);
            TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取方案 %d  抄读电表个数 %d \n",TaskInfo->id,pSchemeTaskTaskRunInfo->SchemeId,pSchemeTaskTaskRunInfo->TotalNum);

            if(pSchemeTaskParaList->OopTaskCfg[index].RunCycle == 1 && pSchemeTaskParaList->OopTaskCfg[index].RunCycleUnit == 1 
                        && MeterFileAll.MeterCount > 100)
            {
                pSchemeTaskTaskRunInfo->MinTaskFlag = 1;
            }

#ifdef PRODUCT_ECU
            //if(pSchemeTaskTaskRunInfo->AcqInfo.Acqtype == ACQTYP_NULL && pSchemeTaskTaskRunInfo->TotalNum == 300 && pSchemeTaskParaList->OopTaskCfg[index].RunCycle == 15 && pSchemeTaskParaList->OopTaskCfg[index].RunCycleUnit == 1)
            //{
            //    TASK_FMT_DEBUG(-1,TASK_LOG, "能源控制器采集压力测试15分钟抄不完，把任务周期改成20分钟，优先级改为1 \n");
            //    pSchemeTaskParaList->OopTaskCfg[index].RunCycle = 20;
            //    pSchemeTaskParaList->OopTaskCfg[index].Priority = 1;
            //}
            //if(pSchemeTaskTaskRunInfo->TotalNum == 300 && pSchemeTaskParaList->OopTaskCfg[index].DelayTime == 5)
            //{
            //    TASK_FMT_DEBUG(-1,TASK_LOG, "能源控制器采集压力测试15分钟抄不完，把冻结任务优先级改为2，先把实时抄完 \n");
            //    pSchemeTaskParaList->OopTaskCfg[index].Priority = 2;
            //}
#endif
//            MS_INFO tmpms;
//            for(i=0;i<pSchemeTaskTaskRunInfo->TotalNum;i++)
//            {
//                tmpms = pSchemeTaskTaskRunInfo->MsInfo[i];
//                TASK_FMT_DEBUG(-1,TASK_LOG, "电表 %d port =%d prtl = %d  \n",tmpms.Sn ,tmpms.Pipe,tmpms.Prtl);
//            }
            taskmng_oad_list_get(&NormalPlanInfo.recdCol,&pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0],&pSchemeTaskTaskRunInfo->AcqInfo);
            TASK_FMT_DEBUG(-1,TASK_LOG, "mainoad %08x  \n",pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.value);
            for(i=0;i<pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].SubOadNum;i++)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "taskmng_oad_list_get %d oad %08x  \n",i,pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].OadListInfo[i].value);
            }
            pSchemeTaskTaskRunInfo->CsdTask.RoadNum=1;
            pSchemeTaskTaskRunInfo->CsdTask.TotalSubOadNum = pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].SubOadNum;
#ifdef AREA_SHANDONG
            Change5min1minToReal(pSchemeTaskTaskRunInfo, &pSchemeTaskParaList->OopTaskCfg[index]);
            pSchemeTaskTaskRunInfo->NILMFlag = taskmng_taskinfo_4800_check(&pSchemeTaskTaskRunInfo->CsdTask);

#endif
            //对于5002的分钟曲线任务,按照采集间隔采集做一些容错处理；
            if((pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)&&(pSchemeTaskTaskRunInfo->AcqInfo.Acqtype == ACQTYP_TI))
            {   //将时间间隔转化为分钟单位
                if(pSchemeTaskTaskRunInfo->AcqInfo.TI.unit ==0)
                {
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = 1;
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.value = pSchemeTaskTaskRunInfo->AcqInfo.TI.value/60;
                }
                else if(pSchemeTaskTaskRunInfo->AcqInfo.TI.unit ==2)
                {
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = 1;
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.value = pSchemeTaskTaskRunInfo->AcqInfo.TI.value*60;
                }
                else if(pSchemeTaskTaskRunInfo->AcqInfo.TI.unit >2)
                {
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.value =0;
                }
                //异常情况下使用默认值
                if(pSchemeTaskTaskRunInfo->AcqInfo.TI.value ==0)
                {
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = 1;
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.value =15;
                }
            }
            //发现有现场配置5002曲线任务时配置成按冻结时标采集，没有时标间隔无法正常抄读，需要强制修改
            if(pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002 && pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_TI&& 
                pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_BC && pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_NULL &&
                pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_LASTN)
            {
                pSchemeTaskTaskRunInfo->AcqInfo.Acqtype = ACQTYP_TI;
                if((pSchemeTaskParaList->OopTaskCfg[index].RunCycleUnit >= TASK_CYCLE_FLAG_HOUR)
                    ||((pSchemeTaskParaList->OopTaskCfg[index].RunCycleUnit == TASK_CYCLE_FLAG_MIN)&&(pSchemeTaskParaList->OopTaskCfg[index].RunCycle > 15)))
                {
                    //任务频率大于15分钟，时标间隔设为15分钟
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = TASK_CYCLE_FLAG_MIN;
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.value = 15;
                }
                else
                {
                    //时标间隔与任务周期一致
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.value = pSchemeTaskParaList->OopTaskCfg[index].RunCycle;
                    pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = pSchemeTaskParaList->OopTaskCfg[index].RunCycleUnit;
                }
            }
            else if(pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003 && pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_TI && pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_NULL)
            {
                pSchemeTaskTaskRunInfo->AcqInfo.Acqtype = ACQTYP_TI;
                pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = TASK_CYCLE_FLAG_HOUR;//小时
                pSchemeTaskTaskRunInfo->AcqInfo.TI.value = 1;//默认1小时
            }
            //优先级修正
            FixPriority = taskmng_priority_fix_by_mainoi(pSchemeTaskTaskRunInfo, pSchemeTaskParaList->OopTaskCfg[index]);
            if(FixPriority > 0)
            {
                pSchemeTaskTaskRunInfo->Priority = FixPriority;
            }
        }
            break;
        case EVENTPLAN:
        {
            //根据方案号从数据中心读取事件方案信息
            
            OOP_PLAN_EVENT_T EventPlanInfo;
            memset(&EventPlanInfo,0x00,sizeof(OOP_PLAN_EVENT_T));
            ret = taskmng_eventl_plan_info_read(pSchemeTaskTaskRunInfo->SchemeId, &EventPlanInfo,logicid);
            TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取事件方案 %d ret = %d 方案大小 %d \n",TaskInfo->id,pSchemeTaskTaskRunInfo->SchemeId,ret,sizeof(OOP_PLAN_EVENT_T));
            if(ret<0)
            {
                pSchemeTaskParaList->OopTaskCfg[index].TaskState = 3;
                break;   //如果没有对应的方案也加载
            }
            pSchemeTaskTaskRunInfo->rptFlag = EventPlanInfo.bReport;
            pSchemeTaskTaskRunInfo->AcqInfo.Acqtype = ACQTYP_LASTN;//事件采集方案通过采集上1次执行
            pSchemeTaskTaskRunInfo->AcqInfo.LastN = 1;
            pSchemeTaskTaskRunInfo->StoreTimeType = STRTMTYP_STARTTM;//按照任务启动时间执行
            pSchemeTaskTaskRunInfo->TotalNum = taskmng_ms_list_get(&EventPlanInfo.ms, pSchemeTaskTaskRunInfo->MsInfo,logicid);
            TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取事件方案 %d  抄读电表个数 %d \n",TaskInfo->id,pSchemeTaskTaskRunInfo->SchemeId,pSchemeTaskTaskRunInfo->TotalNum);

//            MS_INFO tmpms;
//            for(i=0;i<pSchemeTaskTaskRunInfo->TotalNum;i++)
//            {
//                tmpms = pSchemeTaskTaskRunInfo->MsInfo[i];
//                TASK_FMT_DEBUG(-1,TASK_LOG, "电表 %d port =%d prtl = %d  \n",tmpms.Sn ,tmpms.Pipe,tmpms.Prtl);
//            }
            taskmng_road_list_add(&EventPlanInfo.events,&pSchemeTaskTaskRunInfo->CsdTask);
            uint8 j;
            TASK_FMT_DEBUG(-1,TASK_LOG, "事件采集方案 road个数 %d  \n",pSchemeTaskTaskRunInfo->CsdTask.RoadNum);
            for(j=0;j<pSchemeTaskTaskRunInfo->CsdTask.RoadNum;j++)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "mainoad %08x  SubOadNum %d \n",pSchemeTaskTaskRunInfo->CsdTask.RoadTask[j].MainOad.value,pSchemeTaskTaskRunInfo->CsdTask.RoadTask[j].SubOadNum);
                for(i=0;i<pSchemeTaskTaskRunInfo->CsdTask.RoadTask[j].SubOadNum;i++)
                {
                    TASK_FMT_DEBUG(-1,TASK_LOG, "OadList %d oad %08x  \n",i,pSchemeTaskTaskRunInfo->CsdTask.RoadTask[j].OadListInfo[i].value);
                }
            }
            
            //优先级修正
            FixPriority = taskmng_priority_fix_by_mainoi(pSchemeTaskTaskRunInfo, pSchemeTaskParaList->OopTaskCfg[index]);
            if(FixPriority > 0)
            {
                pSchemeTaskTaskRunInfo->Priority = FixPriority;
            }
        }
            break;
        case TRANSPARENTPLAN:
        {
            OOP_PLAN_TRANS_T TransPlanInfo;
            memset(&TransPlanInfo,0x00,sizeof(OOP_PLAN_TRANS_T));
            ret = taskmng_trans_plan_info_read(pSchemeTaskTaskRunInfo->SchemeId,&TransPlanInfo,logicid);
            TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取透明方案 %d ret = %d 方案大小 %d \n",TaskInfo->id,pSchemeTaskTaskRunInfo->SchemeId,ret,sizeof(OOP_PLAN_TRANS_T));
            if(ret<0)
            {
                pSchemeTaskParaList->OopTaskCfg[index].TaskState = 3;
                break;   //如果没有对应的方案也加载
            }
            transindex = Compare_Trans_Scheme(&TransPlanInfo);
            if(transindex !=0xFF)
            {
               pSchemeTaskTaskRunInfo->transindex =  transindex;
                
              TASK_FMT_DEBUG(-1,TASK_LOG, "添加一组透明报文 transindex %d \n",transindex);
            }
            else
            {
                pSchemeTaskTaskRunInfo->transindex = 0;
            } 
            SchemeId = TransPlanInfo.planID;
            memcpy((uint8*)&oopPlanTransparentList.oopPlanTransparent[SchemeId],(uint8*)&TransPlanInfo,sizeof(OOP_PLAN_TRANS_T));
            pSchemeTaskTaskRunInfo->TotalNum = taskmng_trans_plan_info_get(&TransPlanInfo.value , pSchemeTaskTaskRunInfo->MsInfo,pSchemeTaskTaskRunInfo->TransInfo,logicid);
            TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取透明方案 %d  抄读电表个数 %d \n",TaskInfo->id,pSchemeTaskTaskRunInfo->SchemeId,pSchemeTaskTaskRunInfo->TotalNum);
            uint16 i;
            MS_INFO tmpms;
            for(i=0;i<pSchemeTaskTaskRunInfo->TotalNum;i++)
            {
                tmpms = pSchemeTaskTaskRunInfo->MsInfo[i];
                TASK_FMT_DEBUG(-1,TASK_LOG, "电表 %d port =%d prtl = %d  \n",tmpms.Sn ,tmpms.Pipe,tmpms.Prtl);
            }
        }
            break;
        case ENCRYPTPLAN:
        {
            OOP_PLAN_ENCRYPT_T EncryptPlanInfo;
            memset(&EncryptPlanInfo,0x00,sizeof(OOP_PLAN_ENCRYPT_T));
            ret = taskmng_encrypt_plan_info_read(pSchemeTaskTaskRunInfo->SchemeId,&EncryptPlanInfo);
            if(ret<0)
            {
                pSchemeTaskParaList->OopTaskCfg[index].TaskState = 3;
                break;
            }
            pSchemeTaskTaskRunInfo->EncryptPlanID = EncryptPlanInfo.planID;
            pSchemeTaskTaskRunInfo->EncryptPlanType = EncryptPlanInfo.planType;
            pSchemeTaskTaskRunInfo->TotalNum = taskmng_encrypt_plan_info_get(&EncryptPlanInfo,pSchemeTaskTaskRunInfo->MsInfo,&pSchemeTaskTaskRunInfo->EncryptInfo);
            TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取转加密方案 %d  抄读电表个数 %d \n",TaskInfo->id,pSchemeTaskTaskRunInfo->SchemeId,pSchemeTaskTaskRunInfo->TotalNum);
            uint16 i;
            MS_INFO tmpms;
            for(i=0;i<pSchemeTaskTaskRunInfo->TotalNum;i++)
            {
                tmpms = pSchemeTaskTaskRunInfo->MsInfo[i];
                TASK_FMT_DEBUG(-1,TASK_LOG, "电表 %d port =%d prtl = %d  \n",tmpms.Sn ,tmpms.Pipe,tmpms.Prtl);
            }
            ret = taskmng_encrypt_result_first_write(gDBClientTask,EncryptPlanInfo.planID,EncryptPlanInfo.planType,EncryptPlanInfo.config.nNum,pSchemeTaskTaskRunInfo->MsInfo);
            if(ret <0 )
            {
                TASK_FMT_DEBUG(-1,TASK_LOG,"写转加密执行结果集失败");
            }
        }
            break;
        default:
            break;
        }
        if(index == pSchemeTaskParaList->TaskNum )
            pSchemeTaskParaList->TaskNum++;
        return TRUE;

    }

}

/*******************************************************************************
* 函数名称: taskmng_normal_plan_change_fun 
* 函数功能:普通方案变更处理
* 输入参数: pSchemeTaskParaList 所有任务内容
          SchemeTaskRunQueue 所有任务具体信息
          choice 0 删除 1 变更
          NormalPlanInfo 变更的普通方案信息
* 输出参数: 
* 返 回 值: TRUE 任务发生改变 FALSE 任务无需改变
*******************************************************************************/
BOOL taskmng_normal_plan_change_fun(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue,uint8 choice,OOP_PLAN_NORMAL_T *NormalPlanInfo,uint8 logicid)
{
    uint16 i;
    uint8 flag =0 ;//以防多个任务公用一个方案
    if(choice == 0 )
    {
        //方案删除
        for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
        {
            if(pSchemeTaskParaList->OopTaskCfg[i].SchemeType == 1 && pSchemeTaskParaList->OopTaskCfg[i].SchemeId == NormalPlanInfo->planID 
                &&pSchemeTaskParaList->OopTaskCfg[i].logicid == logicid )
            {
                if(pSchemeTaskParaList->OopTaskCfg[i].TaskState == 1)
                {
                    pSchemeTaskParaList->OopTaskCfg[i].TaskState = 3; //方案删除后 任务停用 3表示是因为方案无效而停用
                    flag =1;
                    taskmng_task_record_file_delete(pSchemeTaskParaList->OopTaskCfg[i].TaskId,logicid);
                    taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,logicid);
                }
            }
        }
    }
    else
    {
        //方案变更
        for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
        {
            if(pSchemeTaskParaList->OopTaskCfg[i].SchemeType == 1 && pSchemeTaskParaList->OopTaskCfg[i].SchemeId == NormalPlanInfo->planID 
                &&pSchemeTaskParaList->OopTaskCfg[i].logicid == logicid)
            {
                //如果任务因为方案删除而变成停用 方案有了就启用
                if(pSchemeTaskParaList->OopTaskCfg[i].TaskState == 1||pSchemeTaskParaList->OopTaskCfg[i].TaskState == 3)
                {
                    SCHEME_TASK_RUN_INFO  *pSchemeTaskTaskRunInfo = NULL;
                    pSchemeTaskTaskRunInfo = &SchemeTaskRunQueue[i];
                    memset(pSchemeTaskTaskRunInfo, 0, sizeof(SCHEME_TASK_RUN_INFO));
                    pSchemeTaskTaskRunInfo->logicid        = logicid;
                    pSchemeTaskTaskRunInfo->TaskId        = pSchemeTaskParaList->OopTaskCfg[i].TaskId;
                    pSchemeTaskTaskRunInfo->SchemeType    = pSchemeTaskParaList->OopTaskCfg[i].SchemeType-1;  //采集方案类型
                    pSchemeTaskTaskRunInfo->SchemeId      = pSchemeTaskParaList->OopTaskCfg[i].SchemeId;
                    pSchemeTaskTaskRunInfo->DelayTime     = pSchemeTaskParaList->OopTaskCfg[i].DelayTime;
                    pSchemeTaskTaskRunInfo->DelayTimeUnit  = pSchemeTaskParaList->OopTaskCfg[i].DelayTimeUnit;
                    pSchemeTaskTaskRunInfo->RunResult     = SCHEME_UNDONE;
                    pSchemeTaskTaskRunInfo->Priority      = pSchemeTaskParaList->OopTaskCfg[i].Priority;

                    pSchemeTaskTaskRunInfo->AcqInfo.Acqtype = NormalPlanInfo->recdRow.choice;
                    pSchemeTaskTaskRunInfo->StoreTimeType = NormalPlanInfo->tmFlag;
                    if(ACQTYP_LASTN == pSchemeTaskTaskRunInfo->AcqInfo.Acqtype)
                    {
                        pSchemeTaskTaskRunInfo->AcqInfo.LastN = NormalPlanInfo->recdRow.num;
                    }
                    else if(ACQTYP_TI == pSchemeTaskTaskRunInfo->AcqInfo.Acqtype)
                    {   
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.value = NormalPlanInfo->recdRow.ti.value;
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = NormalPlanInfo->recdRow.ti.unit;             
//                        //将时间间隔转化为分钟单位
//                        if(pSchemeTaskTaskRunInfo->AcqInfo.TI.unit ==0)
//                        {
//                            pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = 1;
//                            pSchemeTaskTaskRunInfo->AcqInfo.TI.value = pSchemeTaskTaskRunInfo->AcqInfo.TI.value/60;
//                        }
//                        else if(pSchemeTaskTaskRunInfo->AcqInfo.TI.unit ==2)
//                        {
//                            pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = 1;
//                            pSchemeTaskTaskRunInfo->AcqInfo.TI.value = pSchemeTaskTaskRunInfo->AcqInfo.TI.value*60;
//                        }
//                        else if(pSchemeTaskTaskRunInfo->AcqInfo.TI.unit >2)
//                        {
//                            pSchemeTaskTaskRunInfo->AcqInfo.TI.value =0;
//                        }
//                        //异常情况下使用默认值
//                        if(pSchemeTaskTaskRunInfo->AcqInfo.TI.value ==0)
//                        {
//                            pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = 1;
//                            pSchemeTaskTaskRunInfo->AcqInfo.TI.value =15;
//                        }
                    }
                    else if(ACQTYP_BC == pSchemeTaskTaskRunInfo->AcqInfo.Acqtype)
                    {
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.value = NormalPlanInfo->recdRow.rd.ti.value;
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = NormalPlanInfo->recdRow.rd.ti.unit;
                        pSchemeTaskTaskRunInfo->AcqInfo.LastN =NormalPlanInfo->recdRow.rd.last;
        
                    }
                    
                    pSchemeTaskTaskRunInfo->TotalNum= taskmng_ms_list_get(&NormalPlanInfo->ms,pSchemeTaskTaskRunInfo->MsInfo,logicid);
                    TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取方案 %d  抄读电表个数 %d \n",pSchemeTaskTaskRunInfo->TaskId,pSchemeTaskTaskRunInfo->SchemeId,pSchemeTaskTaskRunInfo->TotalNum);
                    uint16 j;
                    MS_INFO tmpms;
                    for(j=0;j<pSchemeTaskTaskRunInfo->TotalNum;j++)
                    {
                        tmpms = pSchemeTaskTaskRunInfo->MsInfo[j];
                        TASK_FMT_DEBUG(-1,TASK_LOG, "电表 %d port =%d prtl = %d  \n",tmpms.Sn ,tmpms.Pipe,tmpms.Prtl);
                    }
                    taskmng_oad_list_get(&NormalPlanInfo->recdCol,&pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0],&pSchemeTaskTaskRunInfo->AcqInfo);
                    TASK_FMT_DEBUG(-1,TASK_LOG, "mainoad %08x  \n",pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.value);
                    for(j=0;j<pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].SubOadNum;j++)
                    {
                        TASK_FMT_DEBUG(-1,TASK_LOG, "taskmng_oad_list_get %d oad %08x  \n",j,pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].OadListInfo[j].value);
                    }
                    pSchemeTaskTaskRunInfo->CsdTask.RoadNum=1;
                    pSchemeTaskTaskRunInfo->CsdTask.TotalSubOadNum = pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].SubOadNum;
                    //对于5002的分钟曲线任务,按照采集间隔采集做一些容错处理；
                    if((pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)&&(pSchemeTaskTaskRunInfo->AcqInfo.Acqtype == ACQTYP_TI))
                    {   //将时间间隔转化为分钟单位
                        if(pSchemeTaskTaskRunInfo->AcqInfo.TI.unit ==0)
                        {
                            pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = 1;
                            pSchemeTaskTaskRunInfo->AcqInfo.TI.value = pSchemeTaskTaskRunInfo->AcqInfo.TI.value/60;
                        }
                        else if(pSchemeTaskTaskRunInfo->AcqInfo.TI.unit ==2)
                        {
                            pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = 1;
                            pSchemeTaskTaskRunInfo->AcqInfo.TI.value = pSchemeTaskTaskRunInfo->AcqInfo.TI.value*60;
                        }
                        else if(pSchemeTaskTaskRunInfo->AcqInfo.TI.unit >2)
                        {
                            pSchemeTaskTaskRunInfo->AcqInfo.TI.value =0;
                        }
                        //异常情况下使用默认值
                        if(pSchemeTaskTaskRunInfo->AcqInfo.TI.value ==0)
                        {
                            pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = 1;
                            pSchemeTaskTaskRunInfo->AcqInfo.TI.value =15;
                        }
                    }
                    //发现有现场配置5002曲线任务时配置成按冻结时标采集，没有时标间隔无法正常抄读，需要强制修改
                    if(pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002 && pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_TI&& 
                        pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_BC && pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_NULL &&
                        pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_LASTN)
                    {
                        pSchemeTaskTaskRunInfo->AcqInfo.Acqtype = ACQTYP_TI;
                        if((pSchemeTaskParaList->OopTaskCfg[i].RunCycleUnit >= TASK_CYCLE_FLAG_HOUR)
                            ||((pSchemeTaskParaList->OopTaskCfg[i].RunCycleUnit == TASK_CYCLE_FLAG_MIN)&&(pSchemeTaskParaList->OopTaskCfg[i].RunCycle > 15)))
                        {
                            //任务频率大于15分钟，时标间隔设为15分钟
                            pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = TASK_CYCLE_FLAG_MIN;
                            pSchemeTaskTaskRunInfo->AcqInfo.TI.value = 15;
                        }
                        else
                        {
                            //时标间隔与任务周期一致
                            pSchemeTaskTaskRunInfo->AcqInfo.TI.value = pSchemeTaskParaList->OopTaskCfg[i].RunCycle;
                            pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = pSchemeTaskParaList->OopTaskCfg[i].RunCycleUnit;
                        }
                    }
                    else if(pSchemeTaskTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003 && pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_TI && pSchemeTaskTaskRunInfo->AcqInfo.Acqtype != ACQTYP_NULL)
                    {
                        pSchemeTaskTaskRunInfo->AcqInfo.Acqtype = ACQTYP_TI;
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.unit = TASK_CYCLE_FLAG_HOUR;//小时
                        pSchemeTaskTaskRunInfo->AcqInfo.TI.value = 1;//默认1小时
                    }
                    pSchemeTaskParaList->OopTaskCfg[i].TaskState=1;
                    flag = 1;       
                    taskmng_task_record_file_delete(pSchemeTaskParaList->OopTaskCfg[i].TaskId,logicid);
                    taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,logicid);
                }

            }
        }
    }
    if(flag == 1)
        return TRUE;
    return FALSE;

}

/*******************************************************************************
* 函数名称: taskmng_event_plan_change_fun 
* 函数功能:事件方案变更处理
* 输入参数: pSchemeTaskParaList 所有任务内容
          SchemeTaskRunQueue 所有任务具体信息
          choice 0 删除 1 变更
          EventPlanInfo 变更的事件方案信息
* 输出参数: 
* 返 回 值: TRUE 任务发生改变 FALSE 任务无需改变
*******************************************************************************/
BOOL taskmng_event_plan_change_fun(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue,uint8 choice,OOP_PLAN_EVENT_T *EventPlanInfo,uint8 logicid)
{
    uint16 i;
    uint8 flag =0;
    if(choice==0)
    {
        //方案删除
        for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
        {
            if(pSchemeTaskParaList->OopTaskCfg[i].SchemeType == 2 && pSchemeTaskParaList->OopTaskCfg[i].SchemeId == EventPlanInfo->planID 
                && pSchemeTaskParaList->OopTaskCfg[i].logicid == logicid)
            {
                if(pSchemeTaskParaList->OopTaskCfg[i].TaskState == 1)
                {
                    pSchemeTaskParaList->OopTaskCfg[i].TaskState = 3; //方案删除后 任务停用 3表示是因为方案无效而停用
                    flag =1;
                    taskmng_task_record_file_delete(pSchemeTaskParaList->OopTaskCfg[i].TaskId,logicid);
                    taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,logicid);
                }
            }
        }

    }
    else
    {
        //方案变更
        for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
        {

            if(pSchemeTaskParaList->OopTaskCfg[i].SchemeType == 2 && pSchemeTaskParaList->OopTaskCfg[i].SchemeId == EventPlanInfo->planID 
                && pSchemeTaskParaList->OopTaskCfg[i].logicid == logicid)
            {
                //如果任务因为方案删除而变成停用 方案有了就启用
                if(pSchemeTaskParaList->OopTaskCfg[i].TaskState == 1||pSchemeTaskParaList->OopTaskCfg[i].TaskState == 3)
                {
                    SCHEME_TASK_RUN_INFO  *pSchemeTaskTaskRunInfo = NULL;
                    pSchemeTaskTaskRunInfo = &SchemeTaskRunQueue[i];
                    memset(pSchemeTaskTaskRunInfo, 0, sizeof(SCHEME_TASK_RUN_INFO));
                    pSchemeTaskTaskRunInfo->logicid         =logicid;
                    pSchemeTaskTaskRunInfo->TaskId        = pSchemeTaskParaList->OopTaskCfg[i].TaskId;
                    pSchemeTaskTaskRunInfo->SchemeType    = pSchemeTaskParaList->OopTaskCfg[i].SchemeType-1;  //采集方案类型
                    pSchemeTaskTaskRunInfo->SchemeId      = pSchemeTaskParaList->OopTaskCfg[i].SchemeId;
                    pSchemeTaskTaskRunInfo->DelayTime     = pSchemeTaskParaList->OopTaskCfg[i].DelayTime;
                    pSchemeTaskTaskRunInfo->DelayTimeUnit  = pSchemeTaskParaList->OopTaskCfg[i].DelayTimeUnit;
                    pSchemeTaskTaskRunInfo->RunResult     = SCHEME_UNDONE;
                    pSchemeTaskTaskRunInfo->Priority      = pSchemeTaskParaList->OopTaskCfg[i].Priority;

                    pSchemeTaskTaskRunInfo->rptFlag = EventPlanInfo->bReport;
                    pSchemeTaskTaskRunInfo->AcqInfo.Acqtype = ACQTYP_LASTN;//事件采集方案通过采集上1次执行
                    pSchemeTaskTaskRunInfo->AcqInfo.LastN = 1;
                    pSchemeTaskTaskRunInfo->StoreTimeType = STRTMTYP_STARTTM;//按照任务启动时间执行
                    pSchemeTaskTaskRunInfo->TotalNum = taskmng_ms_list_get(&EventPlanInfo->ms, pSchemeTaskTaskRunInfo->MsInfo,logicid);
                    TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取事件方案 %d  抄读电表个数 %d \n",pSchemeTaskTaskRunInfo->TaskId,pSchemeTaskTaskRunInfo->SchemeId,pSchemeTaskTaskRunInfo->TotalNum);
    
                    MS_INFO tmpms;
                    uint16 j,k;
                    for(j=0;j<pSchemeTaskTaskRunInfo->TotalNum;j++)
                    {
                        tmpms = pSchemeTaskTaskRunInfo->MsInfo[j];
                        TASK_FMT_DEBUG(-1,TASK_LOG, "电表 %d port =%d prtl = %d  \n",tmpms.Sn ,tmpms.Pipe,tmpms.Prtl);
                    }
                    taskmng_road_list_add(&EventPlanInfo->events,&pSchemeTaskTaskRunInfo->CsdTask);
                    
                    TASK_FMT_DEBUG(-1,TASK_LOG, "事件采集方案 road个数 %d  \n",pSchemeTaskTaskRunInfo->CsdTask.RoadNum);
                    for(j=0;j<pSchemeTaskTaskRunInfo->CsdTask.RoadNum;j++)
                    {
                        TASK_FMT_DEBUG(-1,TASK_LOG, "mainoad %08x  SubOadNum %d \n",pSchemeTaskTaskRunInfo->CsdTask.RoadTask[j].MainOad.value,pSchemeTaskTaskRunInfo->CsdTask.RoadTask[j].SubOadNum);
                        for(k=0;k<pSchemeTaskTaskRunInfo->CsdTask.RoadTask[k].SubOadNum;k++)
                        {
                            TASK_FMT_DEBUG(-1,TASK_LOG, "OadList %d oad %08x  \n",k,pSchemeTaskTaskRunInfo->CsdTask.RoadTask[j].OadListInfo[k].value);
                        }
                    }
                    flag = 1;
                    taskmng_task_record_file_delete(pSchemeTaskParaList->OopTaskCfg[i].TaskId,logicid);
                    taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,logicid);
                }
           }
       }

    }
    if(flag == 1)
        return TRUE;
    return FALSE;

}
/*******************************************************************************
* 函数名称: taskmng_trans_plan_change_fun
* 函数功能:透明方案变更处理
* 输入参数:
* 输出参数: 
* 返 回 值:
*******************************************************************************/
BOOL taskmng_trans_plan_change_fun(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue,uint8 choice,OOP_PLAN_TRANS_T *TransPlanInfo)
{
    uint16 i;
    uint8 flag =0 ;//以防多个任务公用一个方案
    uint8 SchemeId=0;
    uint8 transindex=0;
    if(choice == 0 )
    {
        //方案删除
        for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
        {
            if(pSchemeTaskParaList->OopTaskCfg[i].SchemeType == 3 && pSchemeTaskParaList->OopTaskCfg[i].SchemeId == TransPlanInfo->planID )
            {
                if(pSchemeTaskParaList->OopTaskCfg[i].TaskState == 1)
                {
                    pSchemeTaskParaList->OopTaskCfg[i].TaskState = 3; //方案删除后 任务停用 3表示是因为方案无效而停用
                    flag =1;
                    taskmng_task_record_file_delete(pSchemeTaskParaList->OopTaskCfg[i].TaskId,LOGIC_ID_YC);
                    taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,LOGIC_ID_YC);
                    SchemeId = TransPlanInfo->planID;
                    //oopPlanTransparentList.oopPlanTransparentNum--;
                    memset((uint8*)&oopPlanTransparentList.oopPlanTransparent[SchemeId],0x00,sizeof(OOP_PLAN_TRANS_T));
                }
            }
        }
    }
    else
    {
        //方案变更
        for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
        {
            if(pSchemeTaskParaList->OopTaskCfg[i].SchemeType == 3 && pSchemeTaskParaList->OopTaskCfg[i].SchemeId == TransPlanInfo->planID )
            {
                //如果任务因为方案删除而变成停用 方案有了就启用
                if(pSchemeTaskParaList->OopTaskCfg[i].TaskState == 1||pSchemeTaskParaList->OopTaskCfg[i].TaskState == 3)
                {
                    SCHEME_TASK_RUN_INFO  *pSchemeTaskTaskRunInfo = NULL;
                    pSchemeTaskTaskRunInfo = &SchemeTaskRunQueue[i];
                    memset(pSchemeTaskTaskRunInfo, 0, sizeof(SCHEME_TASK_RUN_INFO));
                    pSchemeTaskTaskRunInfo->logicid     =LOGIC_ID_YC;
                    pSchemeTaskTaskRunInfo->TaskId        = pSchemeTaskParaList->OopTaskCfg[i].TaskId;
                    pSchemeTaskTaskRunInfo->SchemeType    = pSchemeTaskParaList->OopTaskCfg[i].SchemeType-1;  //采集方案类型
                    pSchemeTaskTaskRunInfo->SchemeId      = pSchemeTaskParaList->OopTaskCfg[i].SchemeId;
                    pSchemeTaskTaskRunInfo->DelayTime     = pSchemeTaskParaList->OopTaskCfg[i].DelayTime;
                    pSchemeTaskTaskRunInfo->DelayTimeUnit  = pSchemeTaskParaList->OopTaskCfg[i].DelayTimeUnit;
                    pSchemeTaskTaskRunInfo->RunResult     = SCHEME_UNDONE;
                    pSchemeTaskTaskRunInfo->Priority      = pSchemeTaskParaList->OopTaskCfg[i].Priority;
                    transindex = Compare_Trans_Scheme(TransPlanInfo);
                    if(transindex !=0xFF)
                    {
                       pSchemeTaskTaskRunInfo->transindex =  transindex;
                        
                      TASK_FMT_DEBUG(-1,TASK_LOG, "添加一组透明报文 transindex %d \n",transindex);
                    }
                    else
                    {
                        pSchemeTaskTaskRunInfo->transindex = 0;
                    }      
                    //oopPlanTransparentList.oopPlanTransparentNum++;
                    SchemeId = TransPlanInfo->planID;
                    memcpy((uint8*)&oopPlanTransparentList.oopPlanTransparent[SchemeId],TransPlanInfo,sizeof(OOP_PLAN_TRANS_T));
                    //oopPlanTransparentList.oopPlanTransparent[SchemeId] = *TransPlanInfo;
                    pSchemeTaskTaskRunInfo->TotalNum = taskmng_trans_plan_info_get(&TransPlanInfo->value , pSchemeTaskTaskRunInfo->MsInfo,pSchemeTaskTaskRunInfo->TransInfo,LOGIC_ID_YC);
                    TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取透明方案 %d  抄读电表个数 %d \n",pSchemeTaskTaskRunInfo->TaskId,pSchemeTaskTaskRunInfo->SchemeId,pSchemeTaskTaskRunInfo->TotalNum);
                    uint16 i;
                    MS_INFO tmpms;
                    for(i=0;i<pSchemeTaskTaskRunInfo->TotalNum;i++)
                    {
                        tmpms = pSchemeTaskTaskRunInfo->MsInfo[i];
                        TASK_FMT_DEBUG(-1,TASK_LOG, "电表 %d port =%d prtl = %d  \n",tmpms.Sn ,tmpms.Pipe,tmpms.Prtl);
                    }
                    pSchemeTaskParaList->OopTaskCfg[i].TaskState=1;
                    flag = 1;       
                    //透明方案变更后直接删除任务记录文件 重新抄读
                    taskmng_task_record_file_delete(pSchemeTaskTaskRunInfo->TaskId,LOGIC_ID_YC);
                    taskmng_task_monitor_init(pSchemeTaskTaskRunInfo->TaskId,LOGIC_ID_YC);
               }

            }
        }
    }
    if(flag == 1)
        return TRUE;
    return FALSE;


}

/*******************************************************************************
* 函数名称: taskmng_encrypt_plan_change_fun
* 函数功能:转加密方案变更处理
* 输入参数:
* 输出参数: 
* 返 回 值:
*******************************************************************************/
BOOL taskmng_encrypt_plan_change_fun(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue,uint8 choice,OOP_PLAN_ENCRYPT_T *PlanInfo)
{
    uint16 i;
    uint8 flag =0 ;//以防多个任务公用一个方案
    int ret;
    if(choice == 0 )
    {
        //方案删除
        for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
        {
            if(pSchemeTaskParaList->OopTaskCfg[i].SchemeType == 6 && pSchemeTaskParaList->OopTaskCfg[i].SchemeId == PlanInfo->planID )
            {
                if(pSchemeTaskParaList->OopTaskCfg[i].TaskState == 1)
                {
                    pSchemeTaskParaList->OopTaskCfg[i].TaskState = 3; //方案删除后 任务停用 3表示是因为方案无效而停用
                    flag =1;
                    taskmng_task_record_file_delete(pSchemeTaskParaList->OopTaskCfg[i].TaskId,LOGIC_ID_YC);
                    taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,LOGIC_ID_YC);

                }
            }
        }
    }
    else
    {
        //方案变更
        for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
        {
            if(pSchemeTaskParaList->OopTaskCfg[i].SchemeType == 6 && pSchemeTaskParaList->OopTaskCfg[i].SchemeId == PlanInfo->planID )
            {
                //如果任务因为方案删除而变成停用 方案有了就启用
                if(pSchemeTaskParaList->OopTaskCfg[i].TaskState == 1||pSchemeTaskParaList->OopTaskCfg[i].TaskState == 3)
                {
                    SCHEME_TASK_RUN_INFO  *pSchemeTaskTaskRunInfo = NULL;
                    pSchemeTaskTaskRunInfo = &SchemeTaskRunQueue[i];
                    memset(pSchemeTaskTaskRunInfo, 0, sizeof(SCHEME_TASK_RUN_INFO));
                    pSchemeTaskTaskRunInfo->logicid     =LOGIC_ID_YC;
                    pSchemeTaskTaskRunInfo->TaskId        = pSchemeTaskParaList->OopTaskCfg[i].TaskId;
                    pSchemeTaskTaskRunInfo->SchemeType    = pSchemeTaskParaList->OopTaskCfg[i].SchemeType-1;  //采集方案类型
                    pSchemeTaskTaskRunInfo->SchemeId      = pSchemeTaskParaList->OopTaskCfg[i].SchemeId;
                    pSchemeTaskTaskRunInfo->DelayTime     = pSchemeTaskParaList->OopTaskCfg[i].DelayTime;
                    pSchemeTaskTaskRunInfo->DelayTimeUnit  = pSchemeTaskParaList->OopTaskCfg[i].DelayTimeUnit;
                    pSchemeTaskTaskRunInfo->RunResult     = SCHEME_UNDONE;
                    pSchemeTaskTaskRunInfo->Priority      = pSchemeTaskParaList->OopTaskCfg[i].Priority;
                    pSchemeTaskTaskRunInfo->EncryptPlanID = PlanInfo->planID;
                    pSchemeTaskTaskRunInfo->EncryptPlanType = PlanInfo->planType;
                    pSchemeTaskTaskRunInfo->TotalNum = taskmng_encrypt_plan_info_get(PlanInfo,pSchemeTaskTaskRunInfo->MsInfo,&pSchemeTaskTaskRunInfo->EncryptInfo);
                    TASK_FMT_DEBUG(-1,TASK_LOG, "任务 %d 读取转加密方案 %d  抄读电表个数 %d \n",pSchemeTaskTaskRunInfo->TaskId,pSchemeTaskTaskRunInfo->SchemeId,pSchemeTaskTaskRunInfo->TotalNum);
                    uint16 i;
                    MS_INFO tmpms;
                    for(i=0;i<pSchemeTaskTaskRunInfo->TotalNum;i++)
                    {
                        tmpms = pSchemeTaskTaskRunInfo->MsInfo[i];
                        TASK_FMT_DEBUG(-1,TASK_LOG, "电表 %d port =%d prtl = %d  \n",tmpms.Sn ,tmpms.Pipe,tmpms.Prtl);
                    }
                    ret = taskmng_encrypt_result_first_write(gDBClientTask,PlanInfo->planID,PlanInfo->planType,PlanInfo->config.nNum,pSchemeTaskTaskRunInfo->MsInfo);
                    if(ret <0 )
                    {
                        TASK_FMT_DEBUG(-1,TASK_LOG,"写转加密执行结果集失败");
                    }

                    pSchemeTaskParaList->OopTaskCfg[i].TaskState=1;
                    flag = 1;       

                    taskmng_task_record_file_delete(pSchemeTaskTaskRunInfo->TaskId,LOGIC_ID_YC);
                    taskmng_task_monitor_init(pSchemeTaskTaskRunInfo->TaskId,LOGIC_ID_YC);
               }

            }
        }
    }
    if(flag == 1)
        return TRUE;
    return FALSE;


}


/*******************************************************************************
* 函数名称: taskmng_task_modify_fun 
* 函数功能:任务线程收到任务方案变更消息后进行处理
* 输入参数: dbupdate 变更的信息点号和数据标识
          pSchemeTaskParaList 所有任务内容
          SchemeTaskRunQueue 所有任务具体信息
* 输出参数: 
* 返 回 值: TRUE 任务发生改变 FALSE 任务无需改变
*******************************************************************************/
BOOL taskmng_task_modify_fun(DB_UPDATE_T dbupdate,OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue)
{
    int ret;
    TASK_FMT_DEBUG(-1,TASK_LOG, "任务方案变更 oad %08x  logicid %d infonum %d \n",dbupdate.oad.value,dbupdate.logicid,dbupdate.infonum);
    switch(dbupdate.oad.value)
    {
        case 0x60120200:    //任务更新
        {
            //任务初始化
            if(dbupdate.infonum == 0xffff)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "logicid %d 所有任务删除 \n",dbupdate.logicid);
                taskmng_task_delete_with_logicid(dbupdate.logicid,pSchemeTaskParaList,SchemeTaskRunQueue);
                return TRUE;
            }
            OOP_TASK_T      TaskInfo;
            memset(&TaskInfo,0x00,sizeof(OOP_TASK_T));
            
            ret = taskmng_task_info_read(dbupdate.infonum,&TaskInfo,dbupdate.logicid);
            TASK_FMT_DEBUG(-1,TASK_LOG, "开始读取任务 %d  ret %d \n",dbupdate.infonum,ret);
            //任务修改了就把任务记录文件删了
            taskmng_task_record_file_delete(dbupdate.infonum,dbupdate.logicid);
            if(ret!=0)
            {
                TaskInfo.id = dbupdate.infonum;
                return taskmng_task_change_fun(pSchemeTaskParaList,SchemeTaskRunQueue,0,&TaskInfo,dbupdate.logicid);
            }
            else
            {
                return taskmng_task_change_fun(pSchemeTaskParaList,SchemeTaskRunQueue,1,&TaskInfo,dbupdate.logicid);
            }
            
        }
            break;
        case 0x60140200:        //普通采集方案
        {
            if(dbupdate.infonum==0xffff)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "logicid %d 所有普通采集方案删除 \n",dbupdate.logicid);
                uint16 i;
                for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
                {
                    if(pSchemeTaskParaList->OopTaskCfg[i].SchemeType ==1 &&pSchemeTaskParaList->OopTaskCfg[i].logicid ==dbupdate.logicid )
                    {
                        pSchemeTaskParaList->OopTaskCfg[i].TaskState = 3;
                        taskmng_task_record_file_delete(pSchemeTaskParaList->OopTaskCfg[i].TaskId,dbupdate.logicid);
                        taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,dbupdate.logicid);
                    }
                }
                return TRUE;
            }
            OOP_PLAN_NORMAL_T NormalPlanInfo;
            memset(&NormalPlanInfo,0x00,sizeof(OOP_PLAN_NORMAL_T));
            ret = taskmng_normal_plan_info_read(dbupdate.infonum, &NormalPlanInfo,dbupdate.logicid);
            if(ret!=0)
            {
                //方案删除
                NormalPlanInfo.planID = dbupdate.infonum;
                return taskmng_normal_plan_change_fun(pSchemeTaskParaList,SchemeTaskRunQueue,0,&NormalPlanInfo,dbupdate.logicid);
            }
            else
            {
                return taskmng_normal_plan_change_fun(pSchemeTaskParaList,SchemeTaskRunQueue,1,&NormalPlanInfo,dbupdate.logicid);
            }
        }
            break;
        case 0x60160200:
        {
            if(dbupdate.infonum==0xffff)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "logicid %d 所有事件采集方案删除 \n",dbupdate.logicid);
                uint16 i;
                for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
                {
                    if(pSchemeTaskParaList->OopTaskCfg[i].SchemeType ==2&&pSchemeTaskParaList->OopTaskCfg[i].logicid ==dbupdate.logicid )
                    {
                        pSchemeTaskParaList->OopTaskCfg[i].TaskState = 3;
                        taskmng_task_record_file_delete(pSchemeTaskParaList->OopTaskCfg[i].TaskId,dbupdate.logicid);
                        taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,dbupdate.logicid);
                    }
                }
                pthread_rwlock_wrlock(&EventPlanList.rwLock);
                EventPlanList.plancount = 0;
                memset(EventPlanList.PlanEvent,0x00,sizeof(PLAN_EVENT_INTO_T)*MAX_EVENT_PLAN_NUM);
                pthread_rwlock_unlock(&EventPlanList.rwLock);
                return TRUE;
            }
            taskmng_event_plan_list_load();
            OOP_PLAN_EVENT_T EventPlanInfo;
            memset(&EventPlanInfo,0x00,sizeof(OOP_PLAN_EVENT_T));
            ret = taskmng_eventl_plan_info_read(dbupdate.infonum, &EventPlanInfo,dbupdate.logicid);
            if(ret!=0)
            {
                //方案删除
                EventPlanInfo.planID = dbupdate.infonum;
                return taskmng_event_plan_change_fun(pSchemeTaskParaList,SchemeTaskRunQueue,0,&EventPlanInfo,dbupdate.logicid);
            }
            else
            {
                return taskmng_event_plan_change_fun(pSchemeTaskParaList,SchemeTaskRunQueue,1,&EventPlanInfo,dbupdate.logicid);
            }
            
        }
            break;
        case 0x60180200:
        {
            if(dbupdate.infonum==0xffff)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "logicid %d 所有透明方案删除 \n",dbupdate.logicid);
                if(dbupdate.logicid!=LOGIC_ID_YC)
                    return FALSE;
                uint16 i;
                for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
                {
                    if(pSchemeTaskParaList->OopTaskCfg[i].SchemeType ==3 )
                    {
                        pSchemeTaskParaList->OopTaskCfg[i].TaskState = 3;
                        taskmng_task_record_file_delete(pSchemeTaskParaList->OopTaskCfg[i].TaskId,LOGIC_ID_YC);
                        taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,LOGIC_ID_YC);
                    }
                }
                memset((uint8*)&oopPlanTransparentList.oopPlanTransparent,0x00,sizeof(OOP_PLAN_TRANS_T)*256);
                return TRUE;
            }
            OOP_PLAN_TRANS_T TransPlanInfo;
            memset(&TransPlanInfo,0x00,sizeof(OOP_PLAN_TRANS_T));
            ret = taskmng_trans_plan_info_read(dbupdate.infonum,&TransPlanInfo,LOGIC_ID_YC);
            if(ret!=0)
            {
                //方案删除
                TransPlanInfo.planID = dbupdate.infonum;
                return taskmng_trans_plan_change_fun(pSchemeTaskParaList,SchemeTaskRunQueue,0,&TransPlanInfo);
            }
            else
            {
                return taskmng_trans_plan_change_fun(pSchemeTaskParaList,SchemeTaskRunQueue,1,&TransPlanInfo);
            }
        }
            break;
        case 0x601E0200:
        {
            if(dbupdate.infonum==0xffff)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "logicid %d 所有转加密方案删除 \n",dbupdate.logicid);
                if(dbupdate.logicid!=LOGIC_ID_YC)
                    return FALSE;
                uint16 i;
                for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
                {
                    if(pSchemeTaskParaList->OopTaskCfg[i].SchemeType ==6 )
                    {
                        pSchemeTaskParaList->OopTaskCfg[i].TaskState = 3;
                        taskmng_task_record_file_delete(pSchemeTaskParaList->OopTaskCfg[i].TaskId,LOGIC_ID_YC);
                        taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,LOGIC_ID_YC);
                    }
                }
                return TRUE;
            }
            OOP_PLAN_ENCRYPT_T EncryptPlanInfo;
            memset(&EncryptPlanInfo,0x00,sizeof(OOP_PLAN_ENCRYPT_T));
            ret = taskmng_encrypt_plan_info_read(dbupdate.infonum,&EncryptPlanInfo);
            if(ret!=0)
            {
                //方案删除
                EncryptPlanInfo.planID = dbupdate.infonum;
                return taskmng_encrypt_plan_change_fun(pSchemeTaskParaList,SchemeTaskRunQueue,0,&EncryptPlanInfo);
            }else
            {
                if(EncryptPlanInfo.planType == 3)
                {
                    pthread_mutex_lock(&gFileUpdateEncrypt.lock);
                    memcpy(&gFileUpdateEncrypt.planinfo,&EncryptPlanInfo,sizeof(OOP_PLAN_ENCRYPT_T));
                    gFileUpdateEncrypt.msnum = taskmng_encrypt_plan_info_get(&EncryptPlanInfo,gFileUpdateEncrypt.msinfo,&EncryptPlanInfo.endata);
                    pthread_mutex_unlock(&gFileUpdateEncrypt.lock);
                }
                return taskmng_encrypt_plan_change_fun(pSchemeTaskParaList,SchemeTaskRunQueue,1,&EncryptPlanInfo);
            }
        }
            break;
        default:
            break;
    }
    return FALSE;
}


/*******************************************************************************
* 函数名称: taskmng_scheme_task_ergodic
* 函数功能: 采集方案任务遍历
* 输入参数: 
            pSchemeTaskParaList   //载波方案任务参数列表
            SchemeTaskRunQueue    //载波方案任务队列
* 输出参数: 无
* 返 回 值:   无
*******************************************************************************/
void taskmng_scheme_task_ergodic(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue)
{
    time_t  RealTime_t ;
    uint8   i;
    BOOL ischange;//抄表任务是否需要变更
    static BOOL ischangelast = FALSE;

    if( ReloadTaskFlag ==1)
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "档案变更后重新任务初始化 \n");
        pSchemeTaskParaList->TaskCfgState = TASK_CFG_UNINIT;
        //将采集任务监控状态重新初始化
        for(i=0;i<pSchemeTaskParaList->TaskNum;i++)
        {
            taskmng_task_monitor_init(pSchemeTaskParaList->OopTaskCfg[i].TaskId,pSchemeTaskParaList->OopTaskCfg[i].logicid);
        }
        ReloadTaskFlag = 0;
    }
    //根据方案参数变更状态执行相关操作
    switch(pSchemeTaskParaList->TaskCfgState)
    {
    case TASK_CFG_UNINIT:
        TASK_FMT_DEBUG(-1,TASK_LOG, "开始任务初始化 \n");
        pthread_rwlock_wrlock(&TaskRWLock);
        taskmng_scheme_task_init(pSchemeTaskParaList,SchemeTaskRunQueue);

        pthread_rwlock_unlock(&TaskRWLock);
        taskmng_task_monitor_load();
        taskmng_event_plan_list_load();
        taskmng_file_update_encrypt_plan_list_load();
        for(i=0;i<AMR_THREAD_NUM;i++)
        {
            gAmrThreadArg[i].TaskNeedInit = 1;
        }
        TGThreadArg.TaskNeedInit = 1;
        pSchemeTaskParaList->TaskCfgState = TASK_CFG_INITED;
        return;

    case TASK_CFG_MODIFY:
        //收到任务方案变更的消息进行处理
        TASK_FMT_DEBUG(-1,TASK_LOG, "Task线程开始进行处理任务方案变更 \n");
        pthread_rwlock_wrlock(&pSchemeTaskParaList->rwLock);
        pthread_rwlock_wrlock(&TaskRWLock);
        ischange = taskmng_task_modify_fun(dbupdateTask,pSchemeTaskParaList,SchemeTaskRunQueue);
        memset(&dbupdateTask,0x00,sizeof(DB_UPDATE_T));
        pthread_rwlock_unlock(&TaskRWLock);
        pthread_rwlock_unlock(&pSchemeTaskParaList->rwLock);
        //如果任务改变需要通知抄表线程重新初始化 如果没改变则不需要

        if(ischangelast==FALSE)
        {
            ischangelast=ischange;
        }
        if((ischange==FALSE)&&(ischangelast==FALSE))
        {
            pSchemeTaskParaList->TaskCfgState = TASK_CFG_INITED;
            return ;
        }
        TASK_FMT_DEBUG(-1,TASK_LOG, "任务变更确认！ \n");
        pSchemeTaskParaList->ProCfgModifyDelaySec = 5;//原来30秒，现在先给缩短到5秒;
        RealTime_t = time(NULL);
        pSchemeTaskParaList->FunLastRunTime = RealTime_t;
        pSchemeTaskParaList->TaskCfgState = TASK_CFG_MODIFY_DELAY;
        return;

    case TASK_CFG_MODIFY_DELAY:
        RealTime_t = time(NULL);
        if(RealTime_t != pSchemeTaskParaList->FunLastRunTime)
        {
            if(pSchemeTaskParaList->ProCfgModifyDelaySec % 10 == 0)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG,"任务配置修改延时 : %d\n",pSchemeTaskParaList->ProCfgModifyDelaySec);
            }
            pSchemeTaskParaList->ProCfgModifyDelaySec--;
            pSchemeTaskParaList->FunLastRunTime = RealTime_t;
        }
        if(pSchemeTaskParaList->ProCfgModifyDelaySec <= 0)
        {
            TASK_FMT_DEBUG(-1,TASK_LOG,"任务配置修改延时已经完成 : %d\n",pSchemeTaskParaList->ProCfgModifyDelaySec);
            taskmng_task_monitor_load();
            for(i=0;i<AMR_THREAD_NUM;i++)
            {
                //TASK_FMT_TRACE(-1,TASK_LOG,"测试打印gAmrThreadArg[i].TaskModifyFlag is %d,i is %d\n",gAmrThreadArg[i].TaskModifyFlag,i);
                gAmrThreadArg[i].TaskNeedInit = 1;
            }
            TGThreadArg.TaskNeedInit = 1;
            pSchemeTaskParaList->TaskCfgState = TASK_CFG_INITED;
            ischangelast = FALSE;
        }
        return;

    default:
        break;
    }

}

/**********************************************************************
名称 taskmng_task_period_get
功能 计算任务执行周期，
参数  无
返回值: 任务执行周期 单位秒
***********************************************************************/
uint32 taskmng_task_period_get(uint16 TaskId,uint8 logicid)
{
    uint32         dwPeriod;
    int            i;
    pthread_rwlock_rdlock(&SchemeTaskCfgList.rwLock);
    //获取任务执行周期，根据日冻结成功率计算允许执行时间
    for(i=0; i<SchemeTaskCfgList.TaskNum; i++)
    {
        if(SchemeTaskCfgList.OopTaskCfg[i].TaskId == TaskId &&SchemeTaskCfgList.OopTaskCfg[i].logicid == logicid )
            break;
    }
    if(i == SchemeTaskCfgList.TaskNum)
    {
        pthread_rwlock_unlock(&SchemeTaskCfgList.rwLock);
        return 0;   //容错
    }

    dwPeriod = SchemeTaskCfgList.OopTaskCfg[i].RunCycle;
    switch (SchemeTaskCfgList.OopTaskCfg[i].RunCycleUnit)   //执行频率单位
    {
    case TASK_CYCLE_FLAG_SEC:       //秒
        break;

    case TASK_CYCLE_FLAG_MIN:       //分钟
        dwPeriod *= 60L;
        break;

    case TASK_CYCLE_FLAG_HOUR:      //小时
        dwPeriod *= 3600L;
        break;

    case TASK_CYCLE_FLAG_DAY:       //日
    case TASK_CYCLE_FLAG_MON:       //月
    case TASK_CYCLE_FLAG_YEAR:      //月
        dwPeriod = 86400L;
        break;

    default:
        return 0;
        break;
    }
    pthread_rwlock_unlock(&SchemeTaskCfgList.rwLock);
    return dwPeriod;
}

/*******************************************************************************
* 函数名称: taskmng_task_period_check 
* 函数功能: //判断任务执行周期是否和采集时标间隔相等
* 输入参数: Tmp_AmrLink 任务参数

* 输出参数:  
* 返 回 值:   TRUE 相同
        FALSE 不相同
*******************************************************************************/
BOOL taskmng_task_period_check(SCHEME_TASK_RUN_INFO* Tmp_AmrLink)
{

    uint32     TaskPeriod;
    uint32     TIPeriod;


    TaskPeriod = taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid);
    TIPeriod = Tmp_AmrLink->AcqInfo.TI.value;
    switch (Tmp_AmrLink->AcqInfo.TI.unit)   //执行频率单位
    {
    case TASK_CYCLE_FLAG_SEC:       //秒
        break;

    case TASK_CYCLE_FLAG_MIN:       //分钟
        TIPeriod *= 60L;
        break;

    case TASK_CYCLE_FLAG_HOUR:      //小时
        TIPeriod *= 3600L;
        break;

    case TASK_CYCLE_FLAG_DAY:       //日
    case TASK_CYCLE_FLAG_MON:       //月
    case TASK_CYCLE_FLAG_YEAR:      //月
        TIPeriod = 86400L;
        break;

    default:
        TIPeriod = 86400L;
        break;
    }

    if(TaskPeriod == TIPeriod)      //周期与时标间隔相同时仍用实时转存
        return TRUE;
    return FALSE;

}

/*********************************************************************
所属功能单元：
功能描述：根据指定时间和采集存储时间类型获取采集存储时间
输入参数：
        storeType 采集存储类型
        time 指定的时间
输出参数：
    无
函数返回值：
       采集存储时间(DateTimeBcd格式)
*********************************************************************/
DateTimeBcd_t taskmng_acq_store_time_get(StoreTimeType_t storeType,time_t startTime)
{
    const uint8 MonTab[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    DateTimeBcd_t storeTm;
    DateTimeHex_t tmpTm;
    uint16 year;
    OOP_TI_T ti;
    switch(storeType)
    {
    case STRTMTYP_NULL://未定义，使用当前时间
        //DT_Time2DateTimeBCD(time(NULL),&storeTm);
        //break;
    case STRTMTYP_STARTTM://未定义，使用当前时间
        DT_Time2DateTimeBCD(startTime,&storeTm);
        break;
    case STRTMTYP_DAYSTART:         //相对当日0点0分
        DT_Time2DateTimeBCD(startTime,&storeTm);
        storeTm.hour = 0;
        storeTm.min = 0;
        storeTm.sec = 0;
        break;
    case STRTMTYP_LASTDAYEND:       //相对上日23点59分
        ti.unit = 3;
        ti.value = 1;
        DT_TimeSubInv(&startTime,ti);
        DT_Time2DateTimeBCD(startTime,&storeTm);
        storeTm.hour = 0x23;
        storeTm.min = 0x59;
        storeTm.sec = 0x00;
        break;
    case STRTMTYP_LASTDAYSTART:     //相对上日0点0分
        ti.unit = 3;
        ti.value = 1;
        DT_TimeSubInv(&startTime,ti);
        DT_Time2DateTimeBCD(startTime,&storeTm);
        storeTm.hour = 0;
        storeTm.min = 0;
        storeTm.sec = 0;
        break;
    case STRTMTYP_MONTHSTART:       //相对当月1日0点0分
        DT_Time2DateTimeBCD(startTime,&storeTm);
        storeTm.day = 1;
        storeTm.hour = 0;
        storeTm.min = 0;
        storeTm.sec = 0;
        break;
    case STRTMTYP_LASTMONTHEND:     //相对上月月末0点0分
        ti.unit = 4;
        ti.value = 1;
        DT_TimeSubInv(&startTime,ti);
        DT_Time2DateTimeHex(startTime,&tmpTm);
        tmpTm.day = MonTab[tmpTm.month-1];
        year = ((uint16)tmpTm.year_h<<8)+(uint16)tmpTm.year_l;
        if(tmpTm.month==2 && (TRUE == DT_IsLeap(year)))
            tmpTm.day += 1;

        tmpTm.hour = 23;
        tmpTm.min = 59;
  
        tmpTm.sec = 0;
        DT_DateTimeHex2BCD(&tmpTm,&storeTm);
        break;
    default:
        DT_Time2DateTimeBCD(startTime,&storeTm);
        break;
    }
    return storeTm;
}

/*******************************************************************************
* 函数名称: taskmng_task_info_get
* 函数功能:估计任务id获取任务具体抄表参数
* 输入参数: TaskId
* 输出参数: TaskInfo
* 返 回 值: 
*******************************************************************************/
OOP_OAD_U taskmng_task_mainoad_get(uint8 taskindex)
{
    OOP_OAD_U mainoad;
    pthread_rwlock_rdlock(&TaskRWLock);
    mainoad = SchemeTaskRunQueue[taskindex].CsdTask.RoadTask[0].MainOad;
    pthread_rwlock_unlock(&TaskRWLock);
    return mainoad;
}


/*******************************************************************************
* 函数名称: taskmng_file_update_encrypt_plan_list_load 
* 函数功能://从数据中心读取方案类型为3的所有转加密方案
* 输入参数:
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_file_update_encrypt_plan_list_load()
{
    int ret ;
    uint16 i;

    OOP_PLAN_ENCRYPT_T PlanInfo;
    memset(&PlanInfo,0x00,sizeof(PlanInfo));
    
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 255;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x601E0200;
    stepReadRecord.MAC.nNum = 0;

    pthread_mutex_lock(&gFileUpdateEncrypt.lock);
    memset(&gFileUpdateEncrypt.planinfo,0x00,sizeof(gFileUpdateEncrypt.planinfo));
    gFileUpdateEncrypt.msnum = 0;
    memset(gFileUpdateEncrypt.msinfo,0x00,sizeof(gFileUpdateEncrypt.msinfo));
    
    ret = db_read_record_start(gDBClientTask, &inReadRecord, &handle, &recNum);
    if(ret !=0 ||recNum==0)
    {
        TASK_FMT_TRACE(-1,TASK_LOG, "taskmng_file_update_encrypt_plan_list_load 开始读失败 ret %d recNum %d  \n",ret,recNum);
    }
    else
    {
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(gDBClientTask, handle, &stepReadRecord, &stepRecordOut);
            if (ret != 0)
            {
                TASK_FMT_TRACE(-1,TASK_LOG, "taskmng_file_update_encrypt_plan_list_load 单步读失败 ret %d i %d  \n",ret,i);
                continue;
            }
            if ((stepRecordOut.recordnum != 1) 
            || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x601E0200)
            || (stepRecordOut.record[0].outDatalen != sizeof(OOP_PLAN_ENCRYPT_T)))
            {
                continue;
            }
            memcpy(&PlanInfo,stepRecordOut.record[0].outDataBuf,sizeof(OOP_PLAN_ENCRYPT_T));
            //只要转加密方案类型3 升级的
            if(PlanInfo.planType !=3)
                continue;
            memcpy(&gFileUpdateEncrypt.planinfo,&PlanInfo,sizeof(OOP_PLAN_ENCRYPT_T));
            gFileUpdateEncrypt.msnum = taskmng_encrypt_plan_info_get(&PlanInfo,gFileUpdateEncrypt.msinfo,&PlanInfo.endata);
            TASK_FMT_TRACE(-1,TASK_LOG, "加载到转加密升级方案  msnum %d \n",gFileUpdateEncrypt.msnum);
            break;
        }

    }
    db_read_record_end(gDBClientTask, handle);

    TASK_FMT_TRACE(-1,TASK_LOG, "转加密升级方案  msnum %d \n",gFileUpdateEncrypt.msnum);
    pthread_mutex_unlock(&gFileUpdateEncrypt.lock);
}

