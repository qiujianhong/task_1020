
#include "CtsAll.h"
#include "task_msg.h"
#include "CtsACMeter.h"

//创建与交采APP交互的句柄和锁
int             g_UdpAcMeter = 0;

//unix udp 绑定的文件名称(全路径)，目标地址
#define TASKMANAGER_ACMETER_MSG_PATH       "/tmp/dev"
#define TASKMANAGER_ACMETER_APP_DEST       "/tmp/dev/acmeter"
#define ACMETER_BUFF_LEN       4096        //交互缓冲区长度
#define ACMETER_FILEPATH_LEN   256

PastTimeFlag_T             gPastTimeFlagTG;                           //过时间标志
DateTime_T                 gCurBinTimeTG;                             //当前BIN时间
DateTime_T                 gCurBcdTimeTG;                             //当前BCD时间
DateTime_T                 gPreBinTimeTG;                             //上次BIN时间

/*
*********************************************************************
* @name      : taskManager_acMeter_fd_init
* @brief     ：创建与交采APP交互的句柄
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int taskManager_acMeter_fd_init(void)
{
    if (access(TASKMANAGER_ACMETER_APP_DEST, F_OK) != 0)
    {
        TASK_FMT_TRACE(-1, TG_LOG, "%s is nonexistent\n", TASKMANAGER_ACMETER_APP_DEST);
        return ERR_NODEV;
    }

    if (g_UdpAcMeter <= 0)
    {
        char filename[ACMETER_FILEPATH_LEN] = {0};
        sprintf(filename, "%s/taskManager_acMeter", TASKMANAGER_ACMETER_MSG_PATH);
        g_UdpAcMeter = task_pfmsg_init(filename);
    }

    if (g_UdpAcMeter <= 0)
    {
        TASK_FMT_TRACE(-1, TG_LOG, "%d is invalid\n", g_UdpAcMeter);
        return ERR_NO_HANDLE;
    }

    return ERR_OK;
}


/*
*********************************************************************
* @name      : taskManager_acMeter_revsnd_unit
* @brief     ：与交采APP交互的同步接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int taskManager_acMeter_revsnd_unit(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen)
{
    int32 i = 0;
    int32 iRet = 0;
    uint8 czBuf[ACMETER_BUFF_LEN] = {0};
    uint8 sndBuf[ACMETER_BUFF_LEN] = {0};
    char  sender[ACMETER_FILEPATH_LEN] = {0};

    if (sndLen > sizeof(sndBuf))
    {
        return ERR_OPTION;
    }

    if (g_UdpAcMeter <= 0)
    {
        if (0 != taskManager_acMeter_fd_init())
        {
            return ERR_NO_HANDLE;
        }
    }

    i = 0;
    /* 1. 1ms内收到消息, 等待0.1s的时间清空队列 */
    task_msg_settime(g_UdpAcMeter, 0, 1000);
    iRet = task_pfmsg_recv(&g_UdpAcMeter, czBuf, ACMETER_BUFF_LEN, sender, ACMETER_FILEPATH_LEN);

    if (iRet > 0)
    {
        while (i < 100)
        {
            i++;
            task_pfmsg_recv(&g_UdpAcMeter, czBuf, ACMETER_BUFF_LEN, sender, ACMETER_FILEPATH_LEN);
        }
    }

    /* 2. 发送消息 */
    memcpy(sndBuf, sndData, sndLen);
    TASK_FMT_TRACE(-1, TG_LOG, "send acmeter \n");
    TASK_BUF_TRACE(-1, TG_LOG, sndBuf, sndLen);
    iRet = task_pfmsg_send(g_UdpAcMeter, sndBuf, sndLen, TASKMANAGER_ACMETER_APP_DEST);

    if (0 != iRet)
    {
        TASK_FMT_TRACE(-1, TG_LOG, "MSGAC send faild, ret %d\n", iRet);
        return ERR_NORESOURCE;
    }

    /* 3. 接收 5s超时 */
    task_msg_settime(g_UdpAcMeter, 5, 0);
    iRet = task_pfmsg_recv(&g_UdpAcMeter, czBuf, ACMETER_BUFF_LEN, sender, ACMETER_FILEPATH_LEN);

    if (iRet <= 0)
    {
        TASK_FMT_TRACE(-1, TG_LOG, "MSGAC recv faild, ret %d\n", iRet);
        return ERR_NORESOURCE;
    }

    TASK_FMT_TRACE(-1, TG_LOG, "recv acmeter \n");
    TASK_BUF_TRACE(-1, TG_LOG, czBuf, iRet);
    *rcvLen = iRet;
    memcpy(pOutBuf, czBuf, *rcvLen);
    return ERR_OK;
}

/*
*********************************************************************
* @name      : taskManager_acMeter_revsnd
* @brief     ：与交采APP交互的同步接口
* @param[in] ：
* @param[out]：
* @return    ：错误码
*********************************************************************
*/
int taskManager_acMeter_revsnd(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen)
{
    uint32 i = 0;
    int8   ret = ERR_NORMAL;

    while (i < 2)
    {
        ret = taskManager_acMeter_revsnd_unit(sndData, sndLen, pOutBuf, rcvLen);

        if (ERR_OK == ret)
        {
            break;
        }
        else
        {
            i++;
            usleep(10000);
        }
    }

    return ret;
}

/*
*********************************************************************
* @name      : taskmng_ac_sendapdu_get
* @brief     从交采app获取冻结数据组织apdu
* @param[in] ：
* @param[out]：
* @return    ：apdu长度
*********************************************************************
*/

uint16 taskmng_ac_sendapdu_get(OOP_OAD_U MainOad, OOP_OAD_U SubOad, CTS_RUN_TASK_T *RunTask, SCHEME_TASK_RUN_INFO *TaskInfo, uint8 *outbuf)
{
    uint16 index = 0;
    uint8 apdu[ACMETER_BUFF_LEN] = {0};
    OOP_OAD_U rsdoad = {0x20210200};
    DateTimeBcd_t           TempTimeBcd;
    time_t  tmpTime;
    OOP_TI_T Delay;
    apdu[index++] = 0x05;
    apdu[index++] = 0x03;
    apdu[index++] = 0x00;
    memcpy_r(&apdu[index], &MainOad.value, 4);
    index += 4;
    apdu[index++] = 2;      //方法2
    memcpy_r(&apdu[index], &rsdoad.value, 4);
    index += 4;
    //开始时间
    apdu[index++] = DT_DATETIME_S;
    memcpy_r((uint8*)&TempTimeBcd, (uint8*)RunTask->ThisTurnBeginTime, 7);//结束时间
    DT_DateTimeBCD2Time(&TempTimeBcd, &tmpTime);
    Delay.value = TaskInfo->DelayTime;
    Delay.unit = TaskInfo->DelayTimeUnit;
    DT_TimeSubInv(&tmpTime, Delay);
    //鼎信的不支持秒级筛选 再加一分钟
    //tmpTime+=60;
    //tmpTime++;
    DT_Time2DateTimeHex(tmpTime, (DateTimeHex_t*)(apdu + index));
    index += 7;
    //结束时间
    apdu[index++] = DT_DATETIME_S;
//    memcpy_r((uint8*)&TempTimeBcd, (uint8*)RunTask->ThisTurnBeginTime, 7);//结束时间
//    DT_DateTimeBCD2Time(&TempTimeBcd,&tmpTime);
//    Delay.value = TaskInfo->DelayTime;
//    Delay.unit = TaskInfo->DelayTimeUnit;
//    DT_TimeSubInv(&tmpTime, Delay);
    //tmpTime+=60;
    tmpTime++;
    DT_Time2DateTimeHex(tmpTime, (DateTimeHex_t*)(apdu + index));
    index += 7;
    //数据间隔
    apdu[index++] = DT_TI;

    if (TaskInfo->AcqInfo.Acqtype == ACQTYP_FRZTM || TaskInfo->AcqInfo.Acqtype == ACQTYP_NULL)
    {
        if (TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
        {
            TaskInfo->AcqInfo.TI.unit = TASK_CYCLE_FLAG_DAY;
        }
        else if (TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5006)
        {
            TaskInfo->AcqInfo.TI.unit = TASK_CYCLE_FLAG_MON;
        }
        else if (TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003)
        {
            TaskInfo->AcqInfo.TI.unit = TASK_CYCLE_FLAG_HOUR;
        }
        else if (TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)
        {
            TaskInfo->AcqInfo.TI.unit = TASK_CYCLE_FLAG_MIN;
        }

        TaskInfo->AcqInfo.TI.value = 0;
    }

    apdu[index++] = TaskInfo->AcqInfo.TI.unit;
    apdu[index++] = (uint8)(TaskInfo->AcqInfo.TI.value >> 8);
    apdu[index++] = (uint8)TaskInfo->AcqInfo.TI.value;
    apdu[index++] = 1; //一个子oad
    apdu[index++] = 0;
    memcpy_r(&apdu[index], &SubOad.value, 4);
    index += 4;
    apdu[index++] = 0;
    memcpy(outbuf, apdu, index);
    return index;
}
/*******************************************************************************
* 函数名称: taskmng_data_save_tg
* 函数功能:存储交采任务数据
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_data_save_tg(uint8* buf, uint16 len, OOP_OAD_U MainOad, OOP_OAD_U SubOad, CTS_RUN_TASK_T *RunTask, time_t StoreTime)
{
    int ret;
    uint8 tmpaddr[6];
    uint32 storeNo;
    DateTimeBcd_t  tmpTime;
    WRITE_RECORD_T Data;
    memset(&Data, 0x00, sizeof(WRITE_RECORD_T));
    Data.recordOAD.optional = 1;
    Data.recordOAD.logicId = 0;
    Data.recordOAD.infoNum = TGThreadArg.Pn;
    Data.recordOAD.road.oadMain = MainOad;
    Data.recordOAD.road.oadCols.nNum = 1;
    Data.recordOAD.road.oadCols.oad[0] = SubOad;
    Data.recordOAD.classtag = 5;
    memcpy_r((uint8*)&tmpTime, (uint8*)RunTask->ThisTurnBeginTime, 7);
    DT_DateTimeBCD2Time(&tmpTime, (time_t*)&Data.colStartTm);
    //存储时标放到外面处理
    Data.colStoreTm = StoreTime;
#if IS_NEED_ADD_8HOUR
    Data.colStartTm += 8 * 60 * 60;
    //Data.colStoreTm = StoreTime +8*60*60 ;
    Data.colEndTm = time(NULL) + 8 * 60 * 60;
#else
    Data.colEndTm = time(NULL);
#endif
    Data.MAC.nNum = 6;
    //memcpy(Data.MAC.value,LocalAddr,6);
    taskmng_mtr_addr_get(TGThreadArg.Pn, LOGIC_ID_YC, tmpaddr);
    memcpy_r(Data.MAC.value, tmpaddr, 6);
    //taskmng_mtr_addr_get(TGThreadArg.Pn,Data.MAC.value);
    Data.inDatalen = len;
    memcpy(Data.inDataBuf, buf, len);
    ret = db_write_record(TGThreadArg.DBClientHandle, &Data, &storeNo);

    if (ret != 0)
    {
        TASK_FMT_TRACE(-1, TG_LOG, " 存储交采数据失败 \n");
    }

    TASK_FMT_TRACE(-1, TG_LOG, " 存储交采数据成功 maioad %08x suboad %08x \n", MainOad.value, SubOad.value);
    TASK_FMT_TRACE(-1, TG_LOG, " 采集开始时间 存储时间 结束时间 分别为 \n");
    DT_Time2DateTimeBCD(Data.colStartTm, &tmpTime);
    TASK_BUF_TRACE(-1, TG_LOG, &tmpTime, 7);
    DT_Time2DateTimeBCD(Data.colStoreTm, &tmpTime);
    TASK_BUF_TRACE(-1, TG_LOG, &tmpTime, 7);
    DT_Time2DateTimeBCD(Data.colEndTm, &tmpTime);
    TASK_BUF_TRACE(-1, TG_LOG, &tmpTime, 7);
    TASK_FMT_TRACE(-1, TG_LOG, " 存储数据为 \n");
    TASK_BUF_TRACE(-1, TG_LOG, buf, len);
}

/*
*********************************************************************
* @name      : taskmng_acmeter_frozen_get
* @brief     从交采app获取冻结数据
* @param[in] ：
* @param[out]：
* @return    ：0 获取到数据后存储成功 -1 获取数据失败
*********************************************************************
*/

int taskmng_acmeter_frozen_get(OOP_OAD_U MainOad, OOP_OAD_U SubOad, CTS_RUN_TASK_T *RunTask, SCHEME_TASK_RUN_INFO *TaskInfo)
{
    uint16 sendlen;
    uint8 sendapdu[ACMETER_BUFF_LEN] = {0};
    uint8 recvapdu[ACMETER_BUFF_LEN] = {0};
    uint16 recvlen;
    int ret;
    uint16 index = 0;
    OOP_OAD_U recvmainoad ;
    OOP_OAD_U recvsuboad ;
    uint8 csdnum;
    uint8 recordnum;
    uint8 i;
    time_t StoreTime;
    DateTimeBcd_t  tmpTimeBcd;
    OadInfoMap pOadInfo;
    uint16 DataLen = 0;
    uint8 DataBuff[MAX_MSG_LEN] = {0};
    uint16 offset = 0;
    sendlen = taskmng_ac_sendapdu_get(MainOad, SubOad, RunTask, TaskInfo, sendapdu);
    ret = taskManager_acMeter_revsnd(sendapdu, sendlen, recvapdu, &recvlen);

    if (ret != ERR_OK)
    {
        TASK_FMT_TRACE(-1, TG_LOG, "taskmng_acmeter_frozen_get error \n");
        return -1;
    }

    //解析收到的apdu
    if (recvapdu[index++] != 0x85)
    {
        return -1;
    }

    if (recvapdu[index++] != 0x03)
    {
        return -1;
    }

    index++;
    memcpy_r(&recvmainoad, &recvapdu[index], 4);
    index += 4;

    if (recvmainoad.value != MainOad.value)
    {
        return -1;
    }

    csdnum = recvapdu[index++];

    //一个一个子oad招的数据
    if (csdnum != 1)
    {
        return -1;
    }

    if (recvapdu[index++] != 0)
    {
        return -1;
    }

    memcpy_r(&recvsuboad, &recvapdu[index], 4);
    index += 4;

    if (recvsuboad.value != SubOad.value)
    {
        return -1;
    }

    //三星的获取不到曲线冻结时标 需要特殊处理 85 03 00 50 02 02 00 01 00 20 21 02 00 00 03 00 00
    if (SubOad.value == 0x20210200 && recvapdu[index] == 0)
    {
        tmpTimeBcd = taskmng_task_store_time_get_tg(RunTask, TaskInfo);
        DT_DateTimeBCD2Time(&tmpTimeBcd, &StoreTime);
        OOP_DATETIME_S_T frozentime;
        memset(&frozentime, 0x00, sizeof(OOP_DATETIME_S_T));
        DT_Time2DateTimeShort(StoreTime, &frozentime);
        frozentime.week = 0;
#if IS_NEED_ADD_8HOUR
        StoreTime += 8 * 60 * 60;
#endif
        taskmng_data_save_tg((uint8*)&frozentime, sizeof(OOP_DATETIME_S_T), MainOad, SubOad, RunTask, StoreTime);
        return 0;
    }

    if (recvapdu[index++] != 1)
    {
        return -1;
    }

    recordnum = recvapdu[index++];

    if (recvapdu[index] == DT_NULL)
    {
        return -1;
    }

    if (recordnum > 8)
    {
        return -1;
    }

    //如果返回记录条数为0 则再读一次
    if (recordnum == 0)
    {
        return 1;
    }

    for (i = 0; i < recordnum; i++)
    {
        //记录下抄到的冻结时标
        if (SubOad.nObjID == 0x2021)
        {
            DateTimeHex_t        TmpDateTimeHex;
            memset(&TmpDateTimeHex, 0x0, sizeof(TmpDateTimeHex));
            TmpDateTimeHex.year_h = recvapdu[index + 1];
            TmpDateTimeHex.year_l = recvapdu[index + 2];
            TmpDateTimeHex.month  = recvapdu[index + 3];
            TmpDateTimeHex.day    = recvapdu[index + 4];
            TmpDateTimeHex.hour   = recvapdu[index + 5];
            TmpDateTimeHex.min    = recvapdu[index + 6];
            TmpDateTimeHex.sec    = recvapdu[index + 7];

            //鼎信的交采秒数回的是0XFF
            if (TmpDateTimeHex.sec == 0xff)
            {
                TmpDateTimeHex.sec = 0;
            }

            DT_DateTimeHex2Time(&TmpDateTimeHex, &RunTask->ACFrozenTime[i]);
            TASK_FMT_TRACE(-1, TG_LOG, "获取到的冻结时标\n");
            TASK_BUF_TRACE(-1, TG_LOG, &TmpDateTimeHex, 7);
        }

        //三星的终端无法获取曲线冻结时标 直接取任务开始时间
        if (TaskInfo->StoreTimeType == STRTMTYP_FRZTIME && RunTask->ACFrozenTime[i] != 0)
        {
            StoreTime = RunTask->ACFrozenTime[i];
        }
        else
        {
            tmpTimeBcd = taskmng_task_store_time_get_tg(RunTask, TaskInfo);
            DT_DateTimeBCD2Time(&tmpTimeBcd, &StoreTime);
        }

        if (StoreTime == 0)
        {
            TASK_FMT_TRACE(-1, TG_LOG, "未获取到冻结时标\n");
            return -1;
        }

#if IS_NEED_ADD_8HOUR
        StoreTime += 8 * 60 * 60;
#endif
        memset(&pOadInfo, 0x00, sizeof(OadInfoMap));
        ret = taskmng_oad_info_get(SubOad, &pOadInfo);

        if (ret == 0)
        {
            return -1;
        }

        if (recvapdu[index + 1] == 0)
        {
            continue;
        }

        Convert_Oop_Data_To_Struct(&recvapdu[index], &offset, recvlen, DataBuff, &pOadInfo, &DataLen, 3);
        index += offset;
        taskmng_data_save_tg(DataBuff, DataLen, MainOad, SubOad, RunTask, StoreTime);
    }

    return 0;
}



/*******************************************************************************
* 函数名称: taskmng_record_file_delete_tg
* 函数功能:收到任务管理线程发送的任务变更消息，删除任务记录文件
* 输入参数:
        TaskId 任务id
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_record_file_delete_tg(uint16 TaskId)
{
    CTS_RUN_TASK_T *tmpTask = NULL;
    char  pfile[100];
    GetTGRunTask(TaskId, &tmpTask);

    if (tmpTask != NULL)
    {
        tmpTask->TaskState = 2;    //收到任务变更的消息后暂时将该任务停用 延时过后会重新加载任务
    }

    memset(pfile, 0x00, sizeof(pfile));
    sprintf(pfile, "%s/%s%03d", PrivateDir, TASK_RECORD_FILE, TaskId);

    if (access(pfile, R_OK) == 0)
    {
        remove(pfile);
    }
}
/*******************************************************************************
* 函数名称: taskmng_data_init_check_tg
* 函数功能:检查是否需要删除任务记录文件
* 输入参数:

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_data_init_check_tg(void)
{
    if (DataInitFlag[AMR_THREAD_NUM] == 0)
    {
        return;
    }

    //删除任务记录文件
    char pfile[200];
    int ret;
    uint16 i;

    for (i = 0; i < TGThreadArg.TaskNum; i++)
    {
        memset(pfile, 0x00, sizeof(pfile));
        sprintf(pfile, "%s/%s%03d", PrivateDir, TASK_RECORD_FILE, TGThreadArg.pTaskRunInfo[i].TaskId);

        if (access(pfile, R_OK) == 0)
        {
            ret = remove(pfile);
            TASK_FMT_DEBUG(-1, TG_LOG, "删除任务记录文件%s ret is %d\n", pfile, ret);
        }
    }

    DataInitFlag[AMR_THREAD_NUM] = 0;
}

/*******************************************************************************
* 函数名称: taskmng_task_is_tg_check
* 函数功能://检查任务是否是抄交采的
* 输入参数:taskindex 任务索引
* 输出参数:
* 返 回 值:TRUE  FALSE
*******************************************************************************/
BOOL taskmng_task_is_tg_check(uint8 taskindex)
{
    BOOL ret = FALSE;
    uint16 i;
    pthread_rwlock_rdlock(&TaskRWLock);

    for (i = 0; i < SchemeTaskRunQueue[taskindex].TotalNum; i++)
    {
        if (SchemeTaskRunQueue[taskindex].MsInfo[i].Pipe == AMR_THREAD_NUM)
        {
            ret = TRUE;
            break;
        }
    }

    pthread_rwlock_unlock(&TaskRWLock);
    return ret;
}
/*******************************************************************************
* 函数名称: taskmng_task_run_sort_tg
* 函数功能://交采任务按照优先级排序
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_task_run_sort_tg()
{
    uint8 i;

    for (i = 0; i < TGThreadArg.TaskNum; i++)
    {
        TGThreadArg.pTaskHead = taskmng_scheme_task_link_add(&TGThreadArg.pTaskRunInfo[i], TGThreadArg.pTaskHead);
    }

    CTS_RUN_TASK_T *PTmpHeadTaskLink = NULL;//轮询链表时使用
    PTmpHeadTaskLink = TGThreadArg.pTaskHead;
    TASK_FMT_TRACE(-1, TG_LOG, "日常抄表任务队列   : \n");

    while (PTmpHeadTaskLink != NULL)
    {
        TASK_FMT_TRACE(-1, TG_LOG, " TaskId %d \n", PTmpHeadTaskLink->TaskId);
        TASK_FMT_TRACE(-1, TG_LOG, " NeedMtrNum %d  \n", PTmpHeadTaskLink->NeedMtrNum);
        TASK_FMT_TRACE(-1, TG_LOG, " Priority  %d  \n", PTmpHeadTaskLink->Priority);
        PTmpHeadTaskLink = PTmpHeadTaskLink->next;
    }
}


/*******************************************************************************
* 函数名称: taskmng_task_run_init_tg
* 函数功能://交采任务初始化
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
BOOL taskmng_task_run_init_tg(void)
{
    uint8 i;
    uint8 count = 0;
    memset(TGThreadArg.pTaskRunInfo, 0x00, sizeof(CTS_RUN_TASK_T)*SCHEME_MAX_NUM);
    TGThreadArg.pTaskHead = NULL;
    TGThreadArg.TaskNum = 0;
    TASK_FMT_TRACE(-1, TG_LOG, "开始任务初始化 ！\n");
    //如果任务队列还未初始化好 则不运行

    if (SchemeTaskCfgList.TaskCfgState != TASK_CFG_INITED)
    {
        return FALSE;
    }

    pthread_rwlock_rdlock(&SchemeTaskCfgList.rwLock);

    for (i = 0; i < SchemeTaskCfgList.TaskNum; i++)
    {
        if (taskmng_task_is_tg_check(i) == FALSE)
        {
            continue;
        }

        TGThreadArg.pTaskRunInfo[count].TaskId = SchemeTaskCfgList.OopTaskCfg[i].TaskId;
        TGThreadArg.pTaskRunInfo[count].Priority = SchemeTaskCfgList.OopTaskCfg[i].Priority;
        TGThreadArg.pTaskRunInfo[count].TaskState = SchemeTaskCfgList.OopTaskCfg[i].TaskState;
        TGThreadArg.pTaskRunInfo[count].TotalMtrNum = taskmng_task_need_mtr_num_get(i);
        TGThreadArg.pTaskRunInfo[count].NeedMtrNum = 1;
        TGThreadArg.pTaskRunInfo[count].RunResult = SCHEME_UNDONE;
        memset(TGThreadArg.pTaskRunInfo[count].MtrResult, 0x00, 128);
        count++;
    }

    TGThreadArg.TaskNum = count;
    TASK_FMT_TRACE(-1, TG_LOG, "任务初始化完成 任务个数 %d ！\n", count);
    pthread_rwlock_unlock(&SchemeTaskCfgList.rwLock);
    taskmng_task_run_sort_tg();
    return TRUE;
}

/*******************************************************************************
* 函数名称: taskmng_task_record_file_create_tg
* 函数功能://创建交采任务记录文件
* 输入参数:TaskId 任务号
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_task_record_file_create_tg(uint16 TaskId)
{
    char      pfile[100] = {0};
    memset(pfile, 0x00, sizeof(pfile));
    sprintf(pfile, "%s/%s%03d", PrivateDir, TASK_RECORD_FILE, TaskId);
    Task_Record_T TaskRecord;
    memset(&TaskRecord, 0X00, sizeof(Task_Record_T));
    SCHEME_TASK_RUN_INFO TaskInfo;
    memset(&TaskInfo, 0x00, sizeof(SCHEME_TASK_RUN_INFO));
    EVENT_REPORT_SEQ_T EventReportSeq[MAX_EVENT_ROAD_NUM];
    memset(EventReportSeq, 0x00, sizeof(EVENT_REPORT_SEQ_T)*MAX_EVENT_ROAD_NUM);
    taskmng_task_info_get(TaskId, LOGIC_ID_YC, &TaskInfo);
    TaskRecord.TaskId = TaskId;
    TaskRecord.MeterCount = 1;
    TaskRecord.MeterRecord[0].Pn = TGThreadArg.Pn;
    TaskRecord.MeterRecord[0].Result = 0;
    TASK_FMT_TRACE(-1, TG_LOG, "创建任务记录文件%s \n", pfile);
    write_pdata_xram(pfile, (char*)&TaskRecord, 0, sizeof(Task_Record_T));
}
/*******************************************************************************
* 函数名称: taskmng_task_record_file_check_tg
* 函数功能://检查交采任务记录文件，如果没有则创建
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_task_record_file_check_tg()
{
    char      pfile[100] = {0};
    uint8 i;
    uint8 TaskID;

    for (i = 0; i < TGThreadArg.TaskNum; i++)
    {
        TaskID = TGThreadArg.pTaskRunInfo[i].TaskId;
        memset(pfile, 0x00, sizeof(pfile));
        sprintf(pfile, "%s/%s%03d", PrivateDir, TASK_RECORD_FILE, TaskID);

        if (access(pfile, R_OK) == 0)    //如果该任务的记录文件存在则下一个
        {
            TASK_FMT_TRACE(-1, TG_LOG, "任务记录文件%s 存在\n", pfile);
            continue;
        }

        taskmng_task_record_file_create_tg(TaskID);
    }
}
/*******************************************************************************
* 函数名称: taskmng_task_run_time_update_tg
* 函数功能:交采任务轮次计算
* 输入参数:index 任务索引
        TaskInfo 任务运行时参数
* 输出参数:
* 返 回 值:
*******************************************************************************/
BOOL taskmng_task_run_time_update_tg(uint8 index, CTS_RUN_TASK_T *TaskInfo)
{
    //rtc_t            RealTime;                     //当前系统时间时间(RTC格式)
    time_t           RealTime_t;                   //当前系统时间的转换的秒数
    struct tm        tm_time;
    time_t           ThisTurnBeginTime_t;
    time_t           ThisTurnEndTime_t;
    //rtc_t            TaskRunBeginTime;             //方案开始执行时间(RTC格式)
    time_t           TaskRunBeginTime_t;           //方案开始执行转换后的秒
    //rtc_t            TaskRunEndTime;               //方案结束执行时间(RTC格式)
    //time_t           TaskRunEndTime_t;             //方案结束执行转换后的秒
    OOP_TASK_CFG *pTaskCfg = NULL;
    uint32           dwPeriod ;   //重复周期
    time_t           ProThisTurnTimeMark_t;
    time_t           ProThisTurnBeginTime_t;
    time_t           ProThisTurnEndTime_t;
    time_t           ProNextTurnBeginTime_t;
    time_t           ProLastTurnBeginTime_t;
    rtc_t            rtc_tmp;
    OOP_TI_T             Ti;
    uint8 i;

    for (i = 0; i < SchemeTaskCfgList.TaskNum; i++)
    {
        if (TaskInfo->TaskId == SchemeTaskCfgList.OopTaskCfg[i].TaskId)
        {
            pTaskCfg = &SchemeTaskCfgList.OopTaskCfg[i];
            break;
        }
    }

    if (NULL == pTaskCfg)
    {
        return FALSE;
    }

    dwPeriod = pTaskCfg->RunCycle;
    //系统当前时间
    RealTime_t = time(NULL);
    LOCALTIME_R(&RealTime_t, &tm_time);
    memset(&rtc_tmp, 0, sizeof(rtc_t));
    memcpy(&rtc_tmp, TaskInfo->ThisTurnBeginTime, 6);
    ThisTurnBeginTime_t = commfun_rtc2time2(rtc_tmp);
    memset(&rtc_tmp, 0, sizeof(rtc_t));
    memcpy(&rtc_tmp, TaskInfo->ThisTurnEndTime, 6);
    ThisTurnEndTime_t = commfun_rtc2time2(rtc_tmp);
    //减去任务开始时间里包含的延时
    Ti.unit = pTaskCfg->DelayTimeUnit;
    Ti.value = pTaskCfg->DelayTime;
    DT_TimeSubInv(&ThisTurnEndTime_t, Ti);
    DT_TimeSubInv(&ThisTurnBeginTime_t, Ti);

    if (RealTime_t >= ThisTurnBeginTime_t && RealTime_t < ThisTurnEndTime_t) //还在本轮无需更新
    {
        return FALSE;
    }

    uint8 inittime[7] = {0};

    if (RealTime_t >= ThisTurnEndTime_t && memcmp(TaskInfo->ThisTurnBeginTime, inittime, 7) != 0)
    {
        taskmng_task_finish(TGThreadArg.DBClientHandle, TaskInfo->TaskId);
    }

    if (dwPeriod == 0)
    {
        dwPeriod = 1; //因为数据会为0，先默认一个1
    }

    TASK_FMT_TRACE(-1, TG_LOG, "任务 %d 不在本轮 realtime %ld begin %ld end %ld \n", TaskInfo->TaskId, RealTime_t, ThisTurnBeginTime_t, ThisTurnEndTime_t);
    //任务开始时间
    DT_DateTimeShort2Time(&pTaskCfg->RunBeginTime, &TaskRunBeginTime_t);

    switch (pTaskCfg->RunCycleUnit) //执行频率单位
    {
        case TASK_CYCLE_FLAG_SEC:       //分钟
            ProThisTurnTimeMark_t  = (RealTime_t / dwPeriod) * dwPeriod; //根据周期获取当前的时标点
            ProThisTurnBeginTime_t = ProThisTurnTimeMark_t + TaskRunBeginTime_t % dwPeriod;

            if (ProThisTurnBeginTime_t > RealTime_t)
            {
                ProThisTurnBeginTime_t -= dwPeriod;
            }

            ProThisTurnEndTime_t   = (ProThisTurnBeginTime_t + dwPeriod) - 1;
            ProNextTurnBeginTime_t = ProThisTurnBeginTime_t + dwPeriod;
            ProLastTurnBeginTime_t = ProThisTurnBeginTime_t - dwPeriod;
            break;

        case TASK_CYCLE_FLAG_MIN:       //分钟
            dwPeriod *= 60L;
            ProThisTurnTimeMark_t  = (RealTime_t / dwPeriod) * dwPeriod; //根据周期获取当前的时标点
            ProThisTurnBeginTime_t = ProThisTurnTimeMark_t + TaskRunBeginTime_t % dwPeriod;

            if (ProThisTurnBeginTime_t > RealTime_t)
            {
                ProThisTurnBeginTime_t -= dwPeriod;
            }

            ProThisTurnEndTime_t   = (ProThisTurnBeginTime_t + dwPeriod) - 1;
            ProNextTurnBeginTime_t = ProThisTurnBeginTime_t + dwPeriod;
            ProLastTurnBeginTime_t = ProThisTurnBeginTime_t - dwPeriod;
            break;

        case TASK_CYCLE_FLAG_HOUR:      //小时
            dwPeriod *= 3600L;
            ProThisTurnTimeMark_t  = (RealTime_t / dwPeriod) * dwPeriod;
            ProThisTurnBeginTime_t = ProThisTurnTimeMark_t + TaskRunBeginTime_t % dwPeriod;

            if (ProThisTurnBeginTime_t > RealTime_t)
            {
                ProThisTurnBeginTime_t -= dwPeriod;
            }

            ProThisTurnEndTime_t   = (ProThisTurnBeginTime_t + dwPeriod) - 60;
            ProNextTurnBeginTime_t = ProThisTurnBeginTime_t + dwPeriod;
            ProLastTurnBeginTime_t = ProThisTurnBeginTime_t - dwPeriod;
            break;

        case TASK_CYCLE_FLAG_DAY:       //日
            dwPeriod *= 86400L;
            ProThisTurnTimeMark_t  = (RealTime_t / dwPeriod) * dwPeriod;
            ProThisTurnBeginTime_t = ProThisTurnTimeMark_t + TaskRunBeginTime_t % dwPeriod;

            if (ProThisTurnBeginTime_t > RealTime_t)
            {
                ProThisTurnBeginTime_t -= dwPeriod;
            }

            ProThisTurnEndTime_t   = (ProThisTurnBeginTime_t + dwPeriod) - 60;
            ProNextTurnBeginTime_t = ProThisTurnBeginTime_t + dwPeriod;
            ProLastTurnBeginTime_t = ProThisTurnBeginTime_t - dwPeriod;
            break;

        case TASK_CYCLE_FLAG_MON:       //月
        {
            time_t          tm_tmp = 0;
            OOP_DATETIME_S_T tmp;
            memset(&tmp, 0x00, sizeof(OOP_DATETIME_S_T));
            //将任务开始时间的日改为0
            tmp.year = pTaskCfg->RunBeginTime.year;
            tmp.month = pTaskCfg->RunBeginTime.month;
            tmp.day = 1;
            DT_DateTimeShort2Time(&tmp, &TaskRunBeginTime_t);
            Ti.unit  = pTaskCfg->RunCycleUnit;
            Ti.value = pTaskCfg->RunCycle;
            tm_tmp = TaskRunBeginTime_t;

            while (1)
            {
                DT_TimeAddInv(&tm_tmp, Ti);

                if (tm_tmp > RealTime_t)
                {
                    break;
                }
            }

            ProThisTurnBeginTime_t = tm_tmp;
            DT_TimeSubInv(&ProThisTurnBeginTime_t, Ti);
            ProThisTurnEndTime_t = ProThisTurnBeginTime_t;
            DT_TimeAddInv(&ProThisTurnEndTime_t, Ti);
            ProThisTurnEndTime_t -= 60;
            ProNextTurnBeginTime_t = ProThisTurnBeginTime_t;
            DT_TimeAddInv(&ProNextTurnBeginTime_t, Ti);
            ProLastTurnBeginTime_t = ProThisTurnBeginTime_t;
            DT_TimeSubInv(&ProLastTurnBeginTime_t, Ti);
            break;
        }

        default:
            return 0;
    }

    //把延时加上去
    Ti.unit = pTaskCfg->DelayTimeUnit;
    Ti.value = pTaskCfg->DelayTime;
    DT_TimeAddInv(&ProThisTurnBeginTime_t, Ti);
    LOCALTIME_R(&ProThisTurnBeginTime_t, &tm_time);
    TaskInfo->ThisTurnBeginTime[6] = 0x20;
    TaskInfo->ThisTurnBeginTime[5] = commfun_HexToBcd((uint8)(tm_time.tm_year - 100));  //年
    TaskInfo->ThisTurnBeginTime[4] = commfun_HexToBcd((uint8)(tm_time.tm_mon + 1));     //月
    TaskInfo->ThisTurnBeginTime[3] = commfun_HexToBcd((uint8)tm_time.tm_mday);          //日
    TaskInfo->ThisTurnBeginTime[2] = commfun_HexToBcd((uint8)tm_time.tm_hour);          //时
    TaskInfo->ThisTurnBeginTime[1] = commfun_HexToBcd((uint8)tm_time.tm_min);           //分
    TaskInfo->ThisTurnBeginTime[0] = commfun_HexToBcd((uint8)tm_time.tm_sec);           //分
    Ti.unit = pTaskCfg->DelayTimeUnit;
    Ti.value = pTaskCfg->DelayTime;
    DT_TimeAddInv(&ProThisTurnEndTime_t, Ti);
    LOCALTIME_R(&ProThisTurnEndTime_t, &tm_time);
    TaskInfo->ThisTurnEndTime[6] = 0x20;
    TaskInfo->ThisTurnEndTime[5] = commfun_HexToBcd((uint8)(tm_time.tm_year - 100));    //年
    TaskInfo->ThisTurnEndTime[4] = commfun_HexToBcd((uint8)(tm_time.tm_mon + 1));       //月
    TaskInfo->ThisTurnEndTime[3] = commfun_HexToBcd((uint8)tm_time.tm_mday);            //日
    TaskInfo->ThisTurnEndTime[2] = commfun_HexToBcd((uint8)tm_time.tm_hour);            //时
    TaskInfo->ThisTurnEndTime[1] = commfun_HexToBcd((uint8)tm_time.tm_min);         //分
    TaskInfo->ThisTurnEndTime[0] = commfun_HexToBcd((uint8)tm_time.tm_sec);         //分
    LOCALTIME_R(&ProNextTurnBeginTime_t, &tm_time);
    TaskInfo->NextTurnBeginTime[6] = 0x20;
    TaskInfo->NextTurnBeginTime[5] = commfun_HexToBcd((uint8)(tm_time.tm_year - 100));  //年
    TaskInfo->NextTurnBeginTime[4] = commfun_HexToBcd((uint8)(tm_time.tm_mon + 1));     //月
    TaskInfo->NextTurnBeginTime[3] = commfun_HexToBcd((uint8)tm_time.tm_mday);          //日
    TaskInfo->NextTurnBeginTime[2] = commfun_HexToBcd((uint8)tm_time.tm_hour);          //时
    TaskInfo->NextTurnBeginTime[1] = commfun_HexToBcd((uint8)tm_time.tm_min);           //分
    TaskInfo->NextTurnBeginTime[0] = commfun_HexToBcd((uint8)tm_time.tm_sec);           //分
    LOCALTIME_R(&ProLastTurnBeginTime_t, &tm_time);
    TaskInfo->LastTurnBeginTime[6] = 0x20;
    TaskInfo->LastTurnBeginTime[5] = commfun_HexToBcd((uint8)(tm_time.tm_year - 100));  //年
    TaskInfo->LastTurnBeginTime[4] = commfun_HexToBcd((uint8)(tm_time.tm_mon + 1));     //月
    TaskInfo->LastTurnBeginTime[3] = commfun_HexToBcd((uint8)tm_time.tm_mday);          //日
    TaskInfo->LastTurnBeginTime[2] = commfun_HexToBcd((uint8)tm_time.tm_hour);          //时
    TaskInfo->LastTurnBeginTime[1] = commfun_HexToBcd((uint8)tm_time.tm_min);           //分
    TaskInfo->LastTurnBeginTime[0] = commfun_HexToBcd((uint8)tm_time.tm_sec);           //分
    taskmng_task_monitor_update(AMR_THREAD_NUM, TaskInfo->TaskId, LOGIC_ID_YC, UDTASK_INIT, TaskInfo);
    return TRUE;
}

/*******************************************************************************
* 函数名称: taskmng_task_period_stat_update_tg
* 函数功能://交采任务更新运行时段
* 输入参数:index 任务索引
        TaskInfo 任务运行时参数
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_task_period_stat_update_tg(uint8 index, CTS_RUN_TASK_T *TaskInfo)
{
    rtc_t           rtc_tmp;
    time_t          RealTime_t;     //当前系统时间的转换的秒数
    time_t          TaskRunBeginTime_t; //方案开始执行转换后的秒
    time_t          TaskRunEndTime_t;   //方案结束执行转换后的秒
    time_t          ThisTurnBeginTime_t;
    time_t          ThisTurnEndTime_t;
    OOP_TI_T            Ti;
    //uint16          Year;
    OOP_TASK_CFG *pTaskCfg = NULL;
    uint8 i;

    for (i = 0; i < SchemeTaskCfgList.TaskNum; i++)
    {
        if (TaskInfo->TaskId == SchemeTaskCfgList.OopTaskCfg[i].TaskId)
        {
            pTaskCfg = &SchemeTaskCfgList.OopTaskCfg[i];
            break;
        }
    }

    if (NULL == pTaskCfg)
    {
        return ;
    }

    RealTime_t = time(NULL);
    memset((char *)&rtc_tmp, 0x00, sizeof(rtc_t));
    memcpy((char *)&rtc_tmp, TaskInfo->ThisTurnBeginTime, 6);
    ThisTurnBeginTime_t = commfun_rtc2time2(rtc_tmp);
    memset((char *)&rtc_tmp, 0x00, sizeof(rtc_t));
    memcpy((char *)&rtc_tmp, TaskInfo->ThisTurnEndTime, 6);
    ThisTurnEndTime_t = commfun_rtc2time2(rtc_tmp);
    Ti.unit = pTaskCfg->DelayTimeUnit;
    Ti.value = pTaskCfg->DelayTime;
    DT_TimeSubInv(&ThisTurnEndTime_t, Ti);
    DT_DateTimeShort2Time(&pTaskCfg->RunBeginTime, &TaskRunBeginTime_t);
    uint8 flag = 0;

    //time_t最多只能表示到2038年
    if (pTaskCfg->RunEndTime.year < 2038)
    {
        DT_DateTimeShort2Time(&pTaskCfg->RunEndTime, &TaskRunEndTime_t);
        flag = 1;
    }

    if ((RealTime_t < TaskRunBeginTime_t) || (flag == 1 && (RealTime_t > TaskRunEndTime_t)))
    {
        TaskInfo->ThisTurnPeriodStat = AMR_AFT_PERIOD;
        return ;
    }

    //判断当前是否是否在任务本轮次任务开始以及结束之间
    if ((RealTime_t >= ThisTurnBeginTime_t) && (RealTime_t < ThisTurnEndTime_t))
    {
        TaskInfo->ThisTurnPeriodStat = AMR_IN_PERIOD;
        //方案任务允许是否在允许时段内
        taskmng_task_run_in_period(TaskInfo, pTaskCfg);
    }

    //判断当前是否是否在任务本轮次结束之后
    if (RealTime_t < ThisTurnBeginTime_t)
    {
        TaskInfo->ThisTurnPeriodStat = AMR_PRE_PERIOD;
    }

    //判断当前是否是否在任务本轮次结束之后
    if (RealTime_t > ThisTurnEndTime_t)
    {
        TaskInfo->ThisTurnPeriodStat = AMR_AFT_PERIOD;
    }
}

/*******************************************************************************
* 函数名称: taskmng_task_run_ergodic_tg
* 函数功能://交采任务管理
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_task_run_ergodic_tg()
{
    uint8 i;
    BOOL          IsNewTurn = FALSE;

    if (TGThreadArg.TaskNeedInit == 1)
    {
        TGThreadArg.TaskModifyFlag = TASK_CFG_UNINIT;
        TGThreadArg.TaskNeedInit = 0;
    }

    switch (TGThreadArg.TaskModifyFlag)
    {
        case TASK_CFG_UNINIT:
        {
            while (1)
            {
                if (taskmng_task_record_file_del_check(AMR_THREAD_NUM) == 0)
                {
                    break;
                }
            }

            if (taskmng_task_run_init_tg() == TRUE)
            {
                TGThreadArg.TaskModifyFlag = TASK_CFG_PATCH;
            }

            break;
        }

        case TASK_CFG_PATCH:
        {
            //检查是否存在任务记录文件
            taskmng_task_record_file_check_tg();
            TGThreadArg.TaskModifyFlag = TASK_CFG_INITED;
            break;
        }

        default:
            break;
    }

    //更新每个任务的运行时参数
    pthread_rwlock_rdlock(&SchemeTaskCfgList.rwLock);

    for (i = 0; i < TGThreadArg.TaskNum; i++)
    {
        if (TGThreadArg.pTaskRunInfo[i].TaskState != 1)
        {
            //TASK_FMT_TRACE( Pipe, REC_LOG,"任务 %d 已停用 无需更新轮次 \n",pAmrThreadArg->pTaskRunInfo[i].TaskId);
            continue;
        }

        IsNewTurn = taskmng_task_run_time_update_tg(i, &TGThreadArg.pTaskRunInfo[i]);

        if (IsNewTurn)
        {
            TASK_FMT_TRACE(-1, TG_LOG, "任务 %d 新一轮 本轮开始时间 \n", TGThreadArg.pTaskRunInfo[i].TaskId);
            TASK_BUF_TRACE(-1, TG_LOG, TGThreadArg.pTaskRunInfo[i].ThisTurnBeginTime, 7);
            TASK_FMT_TRACE(-1, TG_LOG, " 本轮结束时间 上轮开始时间 \n");
            TASK_BUF_TRACE(-1, TG_LOG, TGThreadArg.pTaskRunInfo[i].ThisTurnEndTime, 7);
            TASK_BUF_TRACE(-1, TG_LOG, TGThreadArg.pTaskRunInfo[i].LastTurnBeginTime, 7);
            taskmng_task_run_info_reset(&TGThreadArg.pTaskRunInfo[i]);
            taskmng_task_monitor_update(AMR_THREAD_NUM, TGThreadArg.pTaskRunInfo[i].TaskId, LOGIC_ID_YC, UDTASK_START, &TGThreadArg.pTaskRunInfo[i]);
        }

        //更新任务执行状态
        taskmng_task_period_stat_update_tg(i, &TGThreadArg.pTaskRunInfo[i]);
    }

    pthread_rwlock_unlock(&SchemeTaskCfgList.rwLock);
}


/*******************************************************************************
* 函数名称: taskmng_task_runlist_check_tg
* 函数功能:检查是否存在要抄读的交采任务
* 输入参数:
* 输出参数: TaskInfo 要抄读的任务信息
* 返 回 值:
*******************************************************************************/
int taskmng_task_runlist_check_tg(CTS_RUN_TASK_T **TaskInfo)
{
    CTS_RUN_TASK_T *tmpTask = TGThreadArg.pTaskHead;

    while (1)
    {
        if (tmpTask != NULL)
        {
            if (tmpTask->ThisTurnPeriodStat != AMR_IN_PERIOD || tmpTask->RunResult == SCHEME_FINISHED || tmpTask->TaskState != 1)
            {
                tmpTask = tmpTask->next;
            }
            else
            {
                *TaskInfo = tmpTask;
                return 1;
            }
        }
        else
        {
            return -1;
        }
    }
}

/*******************************************************************************
* 函数名称: taskmng_task_record_file_load_tg
* 函数功能://加载交采任务记录文件
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_task_record_file_load_tg(SCHEME_TASK_RUN_INFO *TaskInfo, CTS_RUN_TASK_T* RunTask)
{
    Task_Record_T TaskRecord;
    memset(&TaskRecord, 0x00, sizeof(Task_Record_T));
    uint16 TaskId = RunTask->TaskId;
    char      pfile[100] = {0};
    memset(pfile, 0x00, sizeof(pfile));
    sprintf(pfile, "%s/%s%03d", PrivateDir, TASK_RECORD_FILE, TaskId);
    read_pdata_xram(pfile, (char*)&TaskRecord, 0, sizeof(Task_Record_T));

    //如果任务记录文件中记录的信息和任务当前轮次相同则加载记录中的抄表结果
    if (memcmp(TaskRecord.ThisTurnBeginTime, RunTask->ThisTurnBeginTime, 7) == 0)
    {
        //该表当前轮次已经抄读成功
        if (TaskRecord.MeterRecord[0].Result == 1)
        {
            TASK_FMT_TRACE(-1, TG_LOG, "PN %d 当前轮次已经抄读成功 \n", TaskRecord.MeterRecord[0].Pn);
            RunTask->SucNum++;
        }
    }
    else  //将任务记录文件中信息初始化为当前轮次
    {
        memcpy(TaskRecord.ThisTurnBeginTime, RunTask->ThisTurnBeginTime, 7);
        TaskRecord.MeterRecord[0].Result = 0;
        TASK_FMT_TRACE(-1, TG_LOG, "任务记录文件更新至当前轮次 \n");
        write_pdata_xram(pfile, (char*)&TaskRecord, 0, sizeof(Task_Record_T));
    }

    TASK_FMT_TRACE(-1, TG_LOG, "LOAD 任务记录文件后 SucNum %d \n", RunTask->SucNum);
}

/*******************************************************************************
* 函数名称: taskmng_logic_id_get
* 函数功能://根据任务的oad判断逻辑设备号
* 输入参数:
* 输出参数:
* 返 回 值: 逻辑设备号
*******************************************************************************/
uint8 taskmng_logic_id_get(TASK_INFO *CsdTask)
{
    uint8 i;
    OOP_OAD_U SubOad;

    for (i = 0; i < CsdTask->RoadTask[0].SubOadNum; i++)
    {
        SubOad = CsdTask->RoadTask[0].OadListInfo[i];

        if ((SubOad.nObjID & 0xff00) == 0x2300 || (SubOad.nObjID & 0xff00) == 0x2400)
        {
            //如果配的是交采或者脉冲计量 则逻辑设备号为0
            return 0;
        }
    }

    return 1;
}



/*******************************************************************************
* 函数名称: taskmng_data_transfer_tg
* 函数功能:交采任务数据转存
* 输入参数:
* 输出参数:
* 返 回 值: 0 正常抄到数据 -1没抄到数据
*******************************************************************************/
int taskmng_data_transfer_tg(OOP_OAD_U MainOad, OOP_OAD_U SubOad, CTS_RUN_TASK_T *RunTask, SCHEME_TASK_RUN_INFO *TaskInfo)
{
    int ret;
    uint32 i;
    time_t StoreTime;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    DateTimeBcd_t  tmpTimeBcd;
    time_t  tmpTime;
    OOP_TI_T Delay;
    static uint8 flag = 0;  //如果当前终端不支持apdu就直接从数据中心获取
    memcpy_r((uint8*)&tmpTimeBcd, (uint8*)RunTask->LastTurnBeginTime, 7);
    TASK_FMT_TRACE(-1, TG_LOG, " 上轮开始时间 \n");
    TASK_BUF_TRACE(-1, TG_LOG, &tmpTimeBcd, 7);
    DT_DateTimeBCD2Time(&tmpTimeBcd, &tmpTime);
    OOP_OAD_U tmpoad = SubOad ;
    tmpoad.nIndex = 0; //如果任务配的分 直接读块
    inReadRecord.recordOAD.logicId = taskmng_logic_id_get(&TaskInfo->CsdTask);
    TASK_FMT_TRACE(-1, TG_LOG, " logicId %d maioad %08x suboad %08x  \n", inReadRecord.recordOAD.logicId, MainOad.value, SubOad.value);

    if (inReadRecord.recordOAD.logicId == 1 && (MainOad.value == 0x0 || TaskInfo->AcqInfo.Acqtype == ACQTYP_NULL))
    {
        //如果是交采的实时数据，则存在普通数据中
        uint8 DataBuf[MAX_MSG_LEN] = {0};
        uint32 Len;
        NOMAL_OAD_T NormalOad;
        NormalOad.logicId = 1;
        NormalOad.oad = tmpoad;
        NormalOad.infoNum = 0;
        TASK_FMT_TRACE(-1, TG_LOG, " db_read_nomal oad %08x  \n", tmpoad.value);
        ret = db_read_nomal(TGThreadArg.DBClientHandle, &NormalOad, MAX_MSG_LEN, DataBuf, &Len);

        if (ret == 0)
        {
            TASK_FMT_TRACE(-1, TG_LOG, " 读取到交采数据 \n");
            tmpTimeBcd = taskmng_task_store_time_get_tg(RunTask, TaskInfo);
            DT_DateTimeBCD2Time(&tmpTimeBcd, &StoreTime);
#if IS_NEED_ADD_8HOUR
            StoreTime += 8 * 60 * 60;
#endif
            taskmng_data_save_tg(DataBuf, Len, MainOad, tmpoad, RunTask, StoreTime);
            return 0;
        }

        return -1;
    }

    //交采冻结数据组apdu从交采app获取 如果获取不到继续走原来的流程
    if (inReadRecord.recordOAD.logicId == 1 && flag == 0)
    {
        //OOP_OAD_U tmpoad =SubOad ;
        //tmpoad.nIndex = 0; //如果任务配的分 直接读块
        ret = taskmng_acmeter_frozen_get(MainOad, tmpoad, RunTask, TaskInfo);

        if (TaskInfo->SchemeType == EVENTPLAN && ret != 0)
        {
            //事件任务抄不到算成功
            return 0;
        }

        if (ret == 0)
        {
            TASK_FMT_TRACE(-1, TG_LOG, " 获取交采冻结数据成功 \n");
            return 0;
        }
        else if (ret == 1)
        {
            sleep(5);

            if (0 == taskmng_acmeter_frozen_get(MainOad, tmpoad, RunTask, TaskInfo))
            {
                TASK_FMT_TRACE(-1, TG_LOG, "二次获取交采冻结数据成功 \n");
                return 0;
            }
        }
    }

    flag = 1;
    inReadRecord.recordOAD.logicId = 0;//先这样修改
    inReadRecord.recordOAD.infoNum = 0;
    inReadRecord.cType = COL_TM_STORE;
#if IS_NEED_ADD_8HOUR
    inReadRecord.cStart = tmpTime + 1 + 8 * 60 * 60 ;
#else
    inReadRecord.cStart = tmpTime + 1 ;
#endif
    memcpy_r((uint8*)&tmpTimeBcd, (uint8*)RunTask->ThisTurnBeginTime, 7);
    DT_DateTimeBCD2Time(&tmpTimeBcd, &tmpTime);
    TASK_FMT_TRACE(-1, TG_LOG, " 本轮开始时间 \n");
    TASK_BUF_TRACE(-1, TG_LOG, &tmpTimeBcd, 7);
    Delay.unit = TaskInfo->DelayTimeUnit;
    Delay.value = TaskInfo->DelayTime;
    DT_TimeSubInv(&tmpTime, Delay);
#if IS_NEED_ADD_8HOUR
    inReadRecord.cEnd = tmpTime + 1 + 8 * 60 * 60;
#else
    inReadRecord.cEnd = tmpTime + 1 ;
#endif

    if (MainOad.nObjID == 0x5002)
    {
#if IS_NEED_ADD_8HOUR
        inReadRecord.cStart = tmpTime + 8 * 60 * 60 - 240; //因为puAmr默认冻结是5分钟
#else
        inReadRecord.cStart = tmpTime - 240 ;
#endif
    }

    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain = MainOad;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0] = tmpoad;
    TASK_FMT_TRACE(-1, TG_LOG, " inReadRecord.cStart is %d, inReadRecord.cEnd is %d\n", inReadRecord.cStart, inReadRecord.cEnd);
    stepReadRecord.MAC.nNum = 6;
    memcpy_r(stepReadRecord.MAC.value, LocalAddr, 6);
    ret = db_read_record_start(TGThreadArg.DBClientHandle, &inReadRecord, &handle, &recNum);

    if (ret != 0 || recNum == 0)
    {
        TASK_FMT_TRACE(-1, TG_LOG, " 开始读总加组数据失败 ret %d recNum %d  \n", ret, recNum);
        db_read_record_end(TGThreadArg.DBClientHandle, handle);
        return -1;
    }
    else
    {
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(TGThreadArg.DBClientHandle, handle, &stepReadRecord, &stepRecordOut);

            if (ret != 0)
            {
                TASK_FMT_TRACE(-1, TG_LOG, " 单步读交采数据失败 i %d ret %d  \n", i, ret);
                continue;
            }
            else
            {
                TASK_FMT_TRACE(-1, TG_LOG, " 单步读到的交采数据 存储时间 %ld \n", stepRecordOut.colStoreTm);

                if (TaskInfo->StoreTimeType == STRTMTYP_FRZTIME)
                {
                    //从数据中心读出来的冻结时标不需要加8小时
                    StoreTime = stepRecordOut.colStoreTm;
                }
                else
                {
                    tmpTimeBcd = taskmng_task_store_time_get_tg(RunTask, TaskInfo);
                    DT_DateTimeBCD2Time(&tmpTimeBcd, &StoreTime);
#if IS_NEED_ADD_8HOUR
                    StoreTime += 8 * 60 * 60;
#endif
                }

                taskmng_data_save_tg(stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen, MainOad, tmpoad, RunTask, StoreTime);
            }
        }
    }

    db_read_record_end(TGThreadArg.DBClientHandle, handle);
    return 0;
}
/*******************************************************************************
* 函数名称: taskmng_task_data_reading_tg
* 函数功能:交采抄表
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_task_data_reading_tg()
{
    int ret;
    uint8 i;
    OOP_OAD_U MainOad, SubOad;
    memset(&MainOad, 0x00, sizeof(OOP_OAD_U));
    memset(&SubOad, 0x00, sizeof(OOP_OAD_U));
    int dataflag = 0;       //标记是否抄到数据
    CTS_RUN_TASK_T * tmpTask = NULL;
    SCHEME_TASK_RUN_INFO SchemeTaskInfo ;
    BOOLEAN NeedBc = TRUE;

    //如果没档案 抄表不运行
    if (TGThreadArg.Pn == 0)
    {
        return 0 ;
    }

    if (TGThreadArg.TaskModifyFlag != TASK_CFG_INITED)
    {
        return 0;    //任务未初始化 结束
    }

    ret = taskmng_task_runlist_check_tg(&tmpTask);

    if (ret < 0)
    {
        TASK_FMT_TRACE(-1, TG_LOG, " 总加组当前无抄表任务 \n");
        NeedBc = GetAcNeedAgainTaskInfo(&SchemeTaskInfo, &tmpTask);

        if (NeedBc == FALSE)
        {
            return 0;
        }

        //return 0;
    }

    TASK_FMT_TRACE(-1, TG_LOG, " 总加组抄表任务 %d 开始执行 \n", tmpTask->TaskId);

    //获取抄表任务信息
    if (ret >= 0)
    {
        taskmng_task_info_get(tmpTask->TaskId, tmpTask->logicid, &SchemeTaskInfo);
    }

    taskmng_task_record_file_load_tg(&SchemeTaskInfo, tmpTask);

    if (tmpTask->SucNum >= tmpTask->NeedMtrNum)
    {
        TASK_FMT_TRACE(-1, TG_LOG, " 总加组抄表任务 %d 当前轮次已抄完 \n", tmpTask->TaskId);
        tmpTask->RunResult = SCHEME_FINISHED;
        return 1;
    }

    //延时10秒 以防数据还未冻结成功 puAmr到5秒才开始冻数据
    sleep(10);
    MainOad = SchemeTaskInfo.CsdTask.RoadTask[0].MainOad;

    for (i = 0; i < SchemeTaskInfo.CsdTask.RoadTask[0].SubOadNum; i++)
    {
        SubOad = SchemeTaskInfo.CsdTask.RoadTask[0].OadListInfo[i];
        taskmng_task_monitor_update(AMR_THREAD_NUM, tmpTask->TaskId, LOGIC_ID_YC, UDTASK_SENDFRMNUM, NULL);
        ret = taskmng_data_transfer_tg(MainOad, SubOad, tmpTask, &SchemeTaskInfo);

        if (ret == 0)
        {
            taskmng_task_monitor_update(AMR_THREAD_NUM, tmpTask->TaskId, LOGIC_ID_YC, UDTASK_RECVFRMNUM, NULL);
        }
        else
        {
            if ((dataflag == 0) && ((MainOad.nObjID & 0x3000) != 0x3000)) //事件不补冻
            {
                dataflag = 1;
            }
        }
    }

    if (dataflag == 1)
    {
        tmpTask->NeedAgain = TRUE;
    }

    if (dataflag == 0)
    {
        tmpTask->NeedAgain = FALSE;
        SetTGRecordFile(tmpTask->TaskId);
    }

    if ((tmpTask->NeedAgain == TRUE) && (tmpTask->IsAlreadyBc == TASKBC_DOING))
    {
        tmpTask->IsAlreadyBc = TASKBC_FINISHED;
    }

    taskmng_task_monitor_update(AMR_THREAD_NUM, tmpTask->TaskId, LOGIC_ID_YC, UDTASK_SUCCESSNUM, NULL);
    TASK_FMT_TRACE(-1, TG_LOG, " 交采抄表任务 %d 当前轮次全部抄完 \n", tmpTask->TaskId);
    tmpTask->RunResult = SCHEME_FINISHED;
    return 1;
}
/*******************************************************************************
* 函数名称: taskmng_time_manage_tg
* 函数功能:交采线程使用的时间管理函数
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_time_manage_tg(void)
{
    gPastTimeFlagTG.Word = 0;                           //清除时间标志
    taskmng_cur_bin_time_get(&gCurBinTimeTG);                //刷新当前HEX码时间
    taskmng_cur_bcd_time_get(&gCurBcdTimeTG);              //刷新当前BCD码时间

    //过月
    if (gCurBinTimeTG.Month != gPreBinTimeTG.Month)
    {
        //同时产生过月、过日、过时、过分、过秒标志
        gPastTimeFlagTG.BitSect.Month = TRUE;
        gPastTimeFlagTG.BitSect.Day = TRUE;
        gPastTimeFlagTG.BitSect.Hour = TRUE;
        gPastTimeFlagTG.BitSect.Minute = TRUE;
        gPastTimeFlagTG.BitSect.Second = TRUE;
    }

    //过日
    if (gCurBinTimeTG.Day != gPreBinTimeTG.Day)
    {
        //同时产生过日、过时、过分标志
        gPastTimeFlagTG.BitSect.Day = TRUE;
        gPastTimeFlagTG.BitSect.Hour = TRUE;
        gPastTimeFlagTG.BitSect.Minute = TRUE;
        gPastTimeFlagTG.BitSect.Second = TRUE;
    }
    //过时
    else if (gCurBinTimeTG.Hour != gPreBinTimeTG.Hour)
    {
        //同时产生过时、过分标志
        gPastTimeFlagTG.BitSect.Hour = TRUE;
        gPastTimeFlagTG.BitSect.Minute = TRUE;
        gPastTimeFlagTG.BitSect.Second = TRUE;
    }
    //过分
    else if (gCurBinTimeTG.Minute != gPreBinTimeTG.Minute)
    {
        //产生过分标志
        gPastTimeFlagTG.BitSect.Minute = TRUE;
        gPastTimeFlagTG.BitSect.Second = TRUE;
    }
    //过秒
    else if (gCurBinTimeTG.Second != gPreBinTimeTG.Second)
    {
        gPastTimeFlagTG.BitSect.Second = TRUE;
    }
    else
    {
        usleep(10000);
    }

    //过日处理
    if (gPastTimeFlagTG.BitSect.Day == TRUE)
    {
    }

    //过时处理
    if (gPastTimeFlagTG.BitSect.Hour == TRUE)
    {
    }

    if (gPastTimeFlagTG.BitSect.Second == TRUE)
    {
        taskmng_task_run_ergodic_tg();
    }

    //过分处理
    if (gPastTimeFlagTG.BitSect.Minute == TRUE)
    {
        while (1)
        {
            if (taskmng_task_data_reading_tg() == 0)
            {
                break;
            }

            //一个任务执行完后检查下有没有代理透传
            while (1)
            {
                if (taskmng_relay_check_tg() == -2)
                {
                    break;
                }
            }
        }
    }

    memcpy(&gPreBinTimeTG, &gCurBinTimeTG, sizeof(DateTime_T));
}

/**
*********************************************************************
* @brief:      解析代理消息，判断消息的合法性；
* @param[in]：
* @param[in]：  uint32 len  消息内容长度
* @param[in]：  uint8* buf  消息内容
* @return：TRUE表示解析成功，FALSE表示解析失败
*********************************************************************
*/
BOOL taskmng_proxy_msg_unpack_tg(uint8* buf, uint32 len, uint8* resbuf, uint16 *reslen)
{
    uint16 index = 0;
    uint8 lenoffset;
    uint16 datalen;
    uint8 piid;
    OOP_OAD_U port;
    int ret;
    uint8 recvapdu[ACMETER_BUFF_LEN] = {0};
    uint16 recvlen;
    uint8 responsebuf[MAX_MSG_LEN] = {0};
    datalen = XADR_GetEleNum(buf, &lenoffset);
    index += lenoffset;

    if (index + datalen != len)
    {
        return FALSE;
    }

    if (buf[index++] != 0x09)
    {
        return FALSE;
    }

    //只认代理透明转发
    if (buf[index++] != 0x07)
    {
        return FALSE;
    }

    piid = buf[index++];
    memcpy_r(&port, &buf[index], 4);
    index += 4;

    //只处理交采端口
    if (port.nObjID != 0xF208)
    {
        return FALSE;
    }

    index += 5; //端口控制块不需要
    index += 2; //超时时间也不用
    index += 2;
    datalen = XADR_GetEleNum(&buf[index], &lenoffset);
    index += lenoffset;
    TASK_FMT_DEBUG(-1, TG_LOG, "代理交采报文\n");
    TASK_BUF_DEBUG(-1, TG_LOG, &buf[index], datalen);
    //和交采通信
    ret = taskManager_acMeter_revsnd(&buf[index], datalen, recvapdu, &recvlen);

    if (ret != ERR_OK)
    {
        TASK_FMT_TRACE(-1, TG_LOG, "交采代理失败 ret %d \n", ret);
        recvlen = 0;
    }

    //组代理回码报文
    index = 0;
    responsebuf[index++] = 0x89;
    responsebuf[index++] = 0x07;
    responsebuf[index++] = piid;
    memcpy_r(&responsebuf[index], &port, 4);
    index += 4;

    if (recvlen == 0)
    {
        responsebuf[index++] = 0;
        responsebuf[index++] = 0x21;
    }
    else
    {
        responsebuf[index++] = 1;
        lenoffset = set_len_offset(recvlen, &responsebuf[index]);
        index += lenoffset;
        memcpy(&responsebuf[index], recvapdu, recvlen);
        index += recvlen;
    }

    responsebuf[index++] = 0;   //跟随上报域
    responsebuf[index++] = 0;   //时间标签
    lenoffset = set_len_offset(index, resbuf);
    memcpy(resbuf + lenoffset, responsebuf, index);
    *reslen = (lenoffset + index);
    return TRUE;
}

/*******************************************************************************
* 函数名称: taskmng_relay_check_tg
* 函数功能:检查是否有代理透传交采 交采透传为同步接口 直接执行
* 输入参数:
* 输出参数:
* 返 回 值:0 有透传 -1 无效代理即端口不对 -2 队列没有东西
*******************************************************************************/
int taskmng_relay_check_tg(void)
{
    int recvLen;
    uint8 recvBuf[MAX_MSG_LEN] = {0};
    MESSAGE_INFO_T * MsgInfo = NULL;
    uint16 reslen;
    uint8 resbuf[MAX_MSG_LEN] = {0};
    MESSAGE_INFO_T* MsgSend = NULL;
    recvLen = taskmng_plc_queue_read(TGThreadArg.pBufArg->SendBuf, recvBuf);

    if (recvLen > 0)
    {
        MsgInfo = (MESSAGE_INFO_T*)recvBuf;

        if (MsgInfo->IOP != 0x0011)
        {
            return -1;
        }

        if (taskmng_proxy_msg_unpack_tg(MsgInfo->playload, MsgInfo->msglen, resbuf, &reslen) == TRUE)
        {
            //组上行报文
            TASK_FMT_DEBUG(-1, TG_LOG, "代理交采回复报文\n");
            TASK_BUF_DEBUG(-1, TG_LOG, resbuf, reslen);
            MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T) + reslen);
            memset(MsgSend, 0x00, sizeof(MESSAGE_INFO_T) + reslen);
            MsgSend->rpm = 0;
            MsgSend->index = MsgInfo->index;
            MsgSend->priority = MsgInfo->priority;
            MsgSend->label = MsgInfo->label;
            MsgSend->IID = MsgInfo->IID;
            MsgSend->IOP = MsgInfo->IOP;
            MsgSend->sourlen = MsgInfo->destlen;
            memcpy(MsgSend->souraddr, MsgInfo->destaddr, MsgSend->sourlen);
            MsgSend->destlen = MsgInfo->sourlen;
            memcpy(MsgSend->destaddr, MsgInfo->souraddr, MsgSend->destlen);
            MsgSend->msglen = reslen;
            memcpy(MsgSend->playload, resbuf, reslen);
            taskmng_plc_queue_write(TGThreadArg.pBufArg->RecvBuf, (uint8*)MsgSend, sizeof(MESSAGE_INFO_T) + reslen);
            free(MsgSend);
            return 0;
        }

        return -1;
    }

    return -2;
}