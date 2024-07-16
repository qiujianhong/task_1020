/*
*********************************************************************
* @file    AutoTask.c
* @brief： 任务上报模块实现文件
* @author：贺宁
* @date：   2019.11.11
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#include "framecomm.h"
#include "baseCommon.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "appcomm.h"
#include "oopservice.h"
#include "storage.h"

#include "AutoTask.h"
#include "oopVarUpdate.h"
#include "oopSecurity.h"
#include "EventIO.h"
#include "EventFun.h"
#include "AppCommon.h"
#include "commClass.h"
#include "area.h"
#include "ssal.h"

/* 通信句柄，一个线程全局唯一*/
DB_CLIENT g_rptUdpHandle; //TODO

RPT_RES_STATUS_E ConfirmFlag = RPT_RES_INITIAL;       //事件确认标志 0 确认完毕 1 已发送等待确认(需要确认) 2 已发送等待下一帧(不需要确认)
uint8 ReportGLBpiid = 0;     //上报报文帧序列号,生成上报报文用

OOP_TASK_CFG_LIST reportTaskCfgList;//上报任务列表
RPT_PLAN_CFG_T reportList[256];//上报方案信息
uint8 outOfTime = 0;//超时时间
uint8 retryCunt = 0;//重试次数
uint8 TaskGLBpiid = 0; //任务上报帧序号，上报后判断主站确认用

//事件上报确认标记
RPT_RES_STATUS_E ConfirmFlag_event = RPT_RES_INITIAL;
uint8 outOfTime_event = 0;//超时时间
uint8 retryCunt_event = 0;//重试次数
uint8 EventGLBpiid = 0; //事件上报帧序号

RPT_RES_STATUS_E ConfirmFlag_event_met = 0;
uint8 EventGLBpiid_met = 0; //电能表事件上报帧序号

pthread_mutex_t g_evtMetmutex;

uint16 ResendBufferLenth = 0;//重新发送长度
uint8  ResendBuffer[1024] = {0};//重发缓冲区

extern ProxyLockQueue TransQueue;

RPT_RES_STATUS_E ConfirmFlag_retry = RPT_RES_INITIAL; //补报确认标记
uint8 suppGLBpiid = 0;  //补报上报帧序号

#ifdef AREA_FUJIAN

RPT_EVENT_PLAN_T g_evt_plan = 
{
    .channel = {.nNum = 1, .oad[0].value = 0x45000200},
    .priority = 5,
    .timeout = {.unit = TI_SEC, .value = 30},
    .retry = 3,
};

uint32 g_rpt_index  = 0; //全局上报索引
pthread_mutex_t g_rptIdxmutex; //全局上报索引锁

OOP_REPORT_LOG_T g_rptlog = {0};

int g_rpt_basetime = 0; //福建上报计时基准

RPT_TRANSTASK_STATE_T g_rpt_transtask_state = {0};
RPT_TRANSTASK_STATE_T g_rpt_transtask_retry_state = {0};

uint8 g_rpt_online_status = TRUE;

pthread_mutex_t g_rptStatxmutex; //全局上报索引锁
RPT_STAT_T g_rpt_stat_info = {0};
uint16 g_rpt_evtrpt_num = 0;   //当前发送的事件数
uint16 g_rpt_evtrpt_retry_num = 0;   //当前补报发送的事件数

#endif

#if DESC("上报报文组帧",1)

/**
*********************************************************************
* @brief:      上报方案中搜索指定OAD
* @param[in]： uint32   搜索目标
* @param[in]： type     类型
* @param[in]： context 方案内容

* @return：    是否找到目标，1-是 0-否
*********************************************************************
*/
BOOLEAN rpt_plan_search(uint32 oad, uint8 type, REPORT_CONTEXT *context)
{
    int i = 0;

    for (i = 0; i < context->record.tRCSD.nNum; i++)
    {
        if ((context->choice == 1) && 
            (context->record.tRCSD.cols[i].choice == 1) &&
            (context->record.tRCSD.cols[i].road.oadMain.value == oad))
        {       
            return TRUE;
        }
    }

    return FALSE;
}

/**
*********************************************************************
* @brief:   判断当前信道是否允许上报报文
* @param[in]：   chOAD  通道对象属性
* @param[out]： chID   通道ID
* @return：     通道是否允许上报
*********************************************************************
*/
BOOLEAN rpt_channel_search(uint8 *chID)
{
    uint8 id = 0;
    uint8 startID = 0;
    uint8 endID = MAX_CHANNEL_NUM;
    CHANNEL_CACHE_T *tChannel = NULL;
    BOOLEAN valid = FALSE;

    //检索上报通道,获取对应的channel信息
    for(id = startID; id < endID; id++)
    {
        tChannel = app_get_channel(id);
        
        if(NULL == tChannel || FALSE == tChannel->bOpen)
        {
            continue;
        }
        
        /* 判断通信介质 */
        if(COMM_MEDIA_GPRS != tChannel->tChannel.medium && COMM_MEDIA_ETH != tChannel->tChannel.medium)
        {
            continue;
        }

        /* 不支持服务器模式 */
        if(MODE_SERVER == tChannel->tChannel.workmode)
        {
            continue;
        }

        if(FALSE == tChannel->bConnect)
        {
            /* 混合模式先疏通通道 */
            if(MODE_MIX == tChannel->tChannel.workmode)
            {
                if(0 != send_emergency_ipcmsg(id, PROT_CONTINUE_CHANNLE, &id, sizeof(uint8)))
                {
                    RPT_FMT_DEBUG("PROT_CONTINUE_CHANNLE msg failed, id(%d)\n", id);
                }
            }
            continue;
        }

        RPT_FMT_TRACE("channel for reporting: chID(%d), medium(%d).\n", tChannel->id, tChannel->tChannel.medium);        

        valid = TRUE;
        *chID = tChannel->id;
        if(tChannel->tChannel.medium == COMM_MEDIA_GPRS)
        {
            return TRUE;
        }
    }

    return valid;
}

/**
*********************************************************************
* @brief:       获取任务周期
* @param[in]：   TaskIndex 任务索引
* @param[out]： seq        帧序列号 
* @return：      void
*********************************************************************
*/
uint32 rpt_task_period_get(OOP_TASK_CFG OopTaskCfg)
{
    uint32         dwPeriod;

    dwPeriod = OopTaskCfg.RunCycle;
    
    switch (OopTaskCfg.RunCycleUnit)    //执行频率单位
    {
        case TI_SEC: break; //秒

        case TI_MIN: dwPeriod *= 60L;break; //分

        case TI_HOUR: dwPeriod *= 3600L; break; //时

        case TI_DAY:       //日
        case TI_MON:       //月
        case TI_YEAR:      //年
        dwPeriod = 86400L;
        break;

        default:return 0;break;
    }
    
    return dwPeriod;
}

/**
*********************************************************************
* @brief:       时间自动调整，本轮上报的开始时间作为数据筛选的结束时间
                数据筛选的结束时间减任务执行周期作为数据筛选的开始时间
* @param[in]：   TaskIndex 任务索引
* @param[out]： seq        帧序列号 
* @return：      void
*********************************************************************
*/
void rpt_task_time_modify(uint8 TaskIndex)
{
    OOP_TI_T  delayTi;
    OOP_TI_T  runTi;
    DateTimeBcd_t tmpTm;
    time_t        tmpTime;
    char   beginTime[7];
    char   endTime[7];
    uint8  timeOffset;

    runTi.unit = reportTaskCfgList.OopTaskCfg[TaskIndex].RunCycleUnit;
    runTi.value = reportTaskCfgList.OopTaskCfg[TaskIndex].RunCycle;
    
    timeOffset = reportList[TaskIndex].TimeOffset;
    
    //压缩BCD码，把年份的高8位，修改成十进制20
    reportTaskCfgList.OopTaskCfg[TaskIndex].ThisTurnBeginTime[6] = 0x20;
    memcpy_r((uint8*)&tmpTm, reportTaskCfgList.OopTaskCfg[TaskIndex].ThisTurnBeginTime,7);//本轮的开始时间
    DT_DateTimeBCD2Time(&tmpTm,&tmpTime);
    
    delayTi.unit = reportTaskCfgList.OopTaskCfg[TaskIndex].DelayTimeUnit;
    delayTi.value = reportTaskCfgList.OopTaskCfg[TaskIndex].DelayTime;
    DT_TimeSubInv(&tmpTime, delayTi);                                                     //减去上报任务启动的延时
        
#ifdef AREA_ANHUI
    //安徽曲线任务10如果曲线任务按小时处理(大台区)，就上报上上小时数据
//    if(((runTi.value == 15 && runTi.unit == 1) || (runTi.value == 60 && runTi.unit == 1) || (runTi.value == 1 && runTi.unit == 2))
//    && (reportTaskCfgList.OopTaskCfg[TaskIndex].TaskId == 24 || reportTaskCfgList.OopTaskCfg[TaskIndex].TaskId == 22 || reportTaskCfgList.OopTaskCfg[TaskIndex].TaskId == 51))
//    {
//        DT_TimeSubInv(&tmpTime, runTi);
//    }
    if(((runTi.value == 15 && runTi.unit == 1) || (runTi.value == 60 && runTi.unit == 1) || (runTi.value == 1 && runTi.unit == 2))
        && rpt_plan_search(0x50020200, 0, &reportList[TaskIndex].plan.data)
        && (reportTaskCfgList.OopTaskCfg[TaskIndex].TaskId != 18))
    {
        DT_TimeSubInv(&tmpTime, runTi);  
    }
#endif
    //如果是以采集启动时间和成功时间来召 要再加一个周期
//    if(reportList[TaskIndex].apduValue[timeOffset-1]==6||reportList[TaskIndex].apduValue[timeOffset-1]==8)
//    {
//        DT_TimeAddInv(&tmpTime, runTi);
//    }

    DT_Time2DateTimeHex(tmpTime,(DateTimeHex_t*)endTime);

    DT_TimeSubInv(&tmpTime, runTi);

    DT_Time2DateTimeHex(tmpTime,(DateTimeHex_t*)beginTime);
    memcpy_s(&reportList[TaskIndex].apduValue[timeOffset], 7, beginTime, 7);
    memcpy_s(&reportList[TaskIndex].apduValue[timeOffset+7], 7, endTime, 7);
    //如果是以成功时间来招数据 则只能招到当前时间 剩下的时间留到下个周期召 这样才能召到补抄的数据
//    if(reportList[TaskIndex].apduValue[timeOffset-1]==8)
//    {
//        tmpTime = time(NULL);
//        DT_Time2DateTimeHex(tmpTime,(DateTimeHex_t*)endTime);
//        memcpy_s(&reportList[TaskIndex].apduValue[timeOffset+7], 7, endTime, 7);
//        if(reportTaskCfgList.OopTaskCfg[TaskIndex].lastEndTime!=0)
//        {
//            DT_Time2DateTimeHex(reportTaskCfgList.OopTaskCfg[TaskIndex].lastEndTime,(DateTimeHex_t*)beginTime);
//            memcpy_s(&reportList[TaskIndex].apduValue[timeOffset], 7, beginTime, 7);
//        }
//        reportTaskCfgList.OopTaskCfg[TaskIndex].lastEndTime = tmpTime;
//    }
}

/**
*********************************************************************
* @brief:       上报数据内容组帧 
* @param[in]：   pResultData 数据内容
* @param[in]：   TaskIndex   任务ID 
* @param[in]：   seq         帧序列号
* @param[in]：   isAppSlice  是否应用分帧 1-是 0-否
* @return：      void
*********************************************************************
*/
uint16 rpt_packet_make(uint8 *pApdu, uint16 apduLen, uint8 choice, uint8 cAddr, uint8 piid, uint8 *outBuf, uint16 outLen)
{
    int ret  = 0;
    uint32 len = 0;
    FRAME_HEAD_T tFrame;
    uint8 *pAddr = NULL;
    uint32 offset = 0;
    NOMAL_OAD_T nomalOAD;
    OOP_SECURITYFLAG_E safe_status;
    uint8 apdu[LOCAL_RECV_FRAM_MAXLEN] = {0};

    MEMZERO(&tFrame, sizeof(tFrame));
    MEMZERO(&nomalOAD, sizeof(nomalOAD));

    tFrame.cAddr = cAddr;
    tFrame.pApdu = apdu;

    //0-上报一系列OAD，1-上报一系列记录型数据 3-上报透明数据
    if(choice == 0)
    {
        tFrame.pApdu[offset++] = 0x88;           //[136] REPORT-Notification
        tFrame.pApdu[offset++] = 0x01;           //[1] ReportNotificationList
        tFrame.pApdu[offset++] = piid;           //PIID-ACD
        tFrame.pApdu[offset++] = 0x01;           //SEQUENCE数量
    }
    else if(choice == 1)
    {
        tFrame.pApdu[offset++] = 0x88;          //[136] REPORT-Notification
        tFrame.pApdu[offset++] = 0x02;          //[2] ReportNotificationRecordList
        tFrame.pApdu[offset++] = piid;          //PIID-ACD
        tFrame.pApdu[offset++] = 0x01;          //SEQUENCE数量
    }
    else if(choice == 6)
    {
        tFrame.pApdu[offset++] = 0x88;          //[136] REPORT-Notification
        tFrame.pApdu[offset++] = 0x06;          //[3] ReportNotificationTransData
        tFrame.pApdu[offset++] = piid;          //PIID-ACD    
    }
    else 
    {
        tFrame.pApdu[offset++] = 0x88;          //[136] REPORT-Notification
        tFrame.pApdu[offset++] = 0x03;          //[3] ReportNotificationTransData
        tFrame.pApdu[offset++] = piid;          //PIID-ACD       
    }

    memcpy_s(&tFrame.pApdu[offset], apduLen, pApdu, apduLen);
    offset += apduLen;

    //2字节是时戳与跟随上报域
    offset += 2;
    tFrame.apduLen = offset; 

    //安全模式 
    nomalOAD.oad.value = 0xf1010200;
    ret = db_read_nomal(g_rptUdpHandle, &nomalOAD, sizeof(OOP_SECURITYFLAG_E), (uint8*)&safe_status, &len);
    if((ret == 0) && safe_status == 1)
    {
        tFrame.apduLen = GenReportSecurityApdu(tFrame.pApdu,tFrame.apduLen);
    }
    else
    {
        RPT_FMT_DEBUG("security mode not be used. ret(%d), oda(0x%08x), status(%d)\n", ret, nomalOAD.oad.value, safe_status);        
    }

    //上面uNewApdu是完整的APDU，下面拼封装链路层头,设置上报的链路头信息
    tFrame.dir = 1;
    tFrame.prm = 0;  
    tFrame.funCode = 3;   
    len = app_get_localaddr(&pAddr);
    if(0 != len)
    {
        tFrame.sAddr.len = len;
        memcpy_s(tFrame.sAddr.addr, len, pAddr, len);
        tFrame.sAddr.type = 0;
    }
    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

    return app_frame_to_package(&tFrame, outBuf, outLen, TRUE);
}

/**
*********************************************************************
* @brief:       上报数据内容组帧 明文
* @param[in]：   pResultData 数据内容
* @param[in]：   TaskIndex   任务ID 
* @param[in]：   seq         帧序列号
* @param[in]：   isAppSlice  是否应用分帧 1-是 0-否
* @return：      void
*********************************************************************
*/
uint16 rpt_packet_make_plaintext(uint8 *pApdu, uint16 apduLen, uint8 choice, uint8 cAddr, uint8 piid, uint8 *outBuf, uint16 outLen)
{
    //int ret  = 0;
    uint32 len = 0;
    FRAME_HEAD_T tFrame;
    uint8 *pAddr = NULL;
    uint32 offset = 0;

    //OOP_SECURITYFLAG_E safe_status;
    uint8 apdu[LOCAL_RECV_FRAM_MAXLEN] = {0};
    MEMZERO(&tFrame, sizeof(tFrame));

    tFrame.cAddr = cAddr;
    tFrame.pApdu = apdu;

    //0-上报一系列OAD，1-上报一系列记录型数据 3-上报透明数据
    if(choice == 0)
    {
        tFrame.pApdu[offset++] = 0x88;           //[136] REPORT-Notification
        tFrame.pApdu[offset++] = 0x01;           //[1] ReportNotificationList
        tFrame.pApdu[offset++] = piid;           //PIID-ACD
        tFrame.pApdu[offset++] = 0x01;           //SEQUENCE数量
    }
    else if(choice == 1)
    {
        tFrame.pApdu[offset++] = 0x88;          //[136] REPORT-Notification
        tFrame.pApdu[offset++] = 0x02;          //[2] ReportNotificationRecordList
        tFrame.pApdu[offset++] = piid;          //PIID-ACD
        tFrame.pApdu[offset++] = 0x01;          //SEQUENCE数量
    }
    else if(choice == 6)
    {
        tFrame.pApdu[offset++] = 0x88;          //[136] REPORT-Notification
        tFrame.pApdu[offset++] = 0x06;          //[3] ReportNotificationTransData
        tFrame.pApdu[offset++] = piid;          //PIID-ACD    
    }
    else 
    {
        tFrame.pApdu[offset++] = 0x88;          //[136] REPORT-Notification
        tFrame.pApdu[offset++] = 0x03;          //[3] ReportNotificationTransData
        tFrame.pApdu[offset++] = piid;          //PIID-ACD       
    }

    memcpy_s(&tFrame.pApdu[offset], apduLen, pApdu, apduLen);
    offset += apduLen;

    //2字节是时戳与跟随上报域
    offset += 2;
    tFrame.apduLen = offset; 


    //上面uNewApdu是完整的APDU，下面拼封装链路层头,设置上报的链路头信息
    tFrame.dir = 1;
    tFrame.prm = 0;  
    tFrame.funCode = 3;   
    len = app_get_localaddr(&pAddr);
    if(0 != len)
    {
        tFrame.sAddr.len = len;
        memcpy_s(tFrame.sAddr.addr, len, pAddr, len);
        tFrame.sAddr.type = 0;
    }
    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

    return app_frame_to_package(&tFrame, outBuf, outLen, TRUE);
}


/**
*********************************************************************
* @brief:       上报数据内容组帧 
* @param[in]：   pResultData 数据内容
* @param[in]：   TaskIndex   任务ID 
* @param[in]：   seq         帧序列号
* @param[in]：   isAppSlice  是否应用分帧 1-是 0-否
* @return：      void
*********************************************************************
*/
void rpt_task_data_contruct(uint8 *pApdu, uint16 apduLen, uint16 msgmaxlen, uint8 TaskIndex, uint8 id)
{
    AUTO_DATA     tmp_data; //准备写入文件的数据
    uint32 len = 0;
    FRAME_HEAD_T tFrame;
    uint8 *pAddr = NULL;
    uint32 offset = 0;
    NOMAL_OAD_T nomalOAD;
    OOP_SECURITYFLAG_E safe_status;
    uint8 apdu[LOCAL_RECV_FRAM_MAXLEN] = {0};
    int ret = 0;

    MEMZERO(&tFrame, sizeof(tFrame));
    MEMZERO(&nomalOAD, sizeof(nomalOAD));
    
    ReportGLBpiid = RPT_ADD_VALUE_CLEAR(ReportGLBpiid, 63, 1);

    tFrame.pApdu = apdu;

    //0-上报一系列OAD，1-上报一系列记录型数据
    if(reportList[TaskIndex].plan.data.choice == 0)
    {
        tFrame.pApdu[offset++] = 0x88;           //[136] REPORT-Notification
        tFrame.pApdu[offset++] = 0x01;           //[1] ReportNotificationList
        tFrame.pApdu[offset++] = ReportGLBpiid;  //PIID-ACD
        tFrame.pApdu[offset++] = 0x01;           //SEQUENCE数量
    }
    else
    {
        tFrame.pApdu[offset++] = 0x88;          //[136] REPORT-Notification
        tFrame.pApdu[offset++] = 0x02;          //[2] ReportNotificationRecordList
        tFrame.pApdu[offset++] = ReportGLBpiid; //PIID-ACD
        tFrame.pApdu[offset++] = 0x01;          //SEQUENCE数量
    }

    memcpy_s(&tFrame.pApdu[offset], apduLen, pApdu, apduLen);
    offset += apduLen;

    //2字节是时戳与跟随上报域
    offset += 2;
    tFrame.apduLen = offset; 

    //安全模式 
    nomalOAD.oad.value = 0xf1010200;
    ret = db_read_nomal(g_rptUdpHandle, &nomalOAD, sizeof(OOP_SECURITYFLAG_E), (uint8*)&safe_status, &len);
    if((ret == 0) && safe_status == 1)
    {
        tFrame.apduLen = GenReportSecurityApdu(tFrame.pApdu,tFrame.apduLen);
    }
    else
    {
        RPT_FMT_LOGCC("security mod not enable. ret(%d), safe_status(%d), oad(0x08x).\n", ret, safe_status, nomalOAD.oad.value);
    }

    //上面uNewApdu是完整的APDU，下面拼封装链路层头,设置上报的链路头信息
    tFrame.dir = 1;
    tFrame.prm = 0;  
    tFrame.funCode = 3;   
    len = app_get_localaddr(&pAddr);
    if(0 != len)
    {
        tFrame.sAddr.len = len;
        memcpy_s(tFrame.sAddr.addr, len, pAddr, len);
        tFrame.sAddr.type = 0;
    }
    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

    memset_s((char *)&tmp_data, sizeof(AUTO_DATA), 0x0, sizeof(AUTO_DATA));

    len = app_frame_to_package(&tFrame, tmp_data.bData, 3000, TRUE);

    tmp_data.timeOut = DT_TI2Seconds(reportList[TaskIndex].plan.timeout);
    tmp_data.retryTime = reportList[TaskIndex].plan.retry;//重发次数
    
    tmp_data.bConflag = 1;//主动上报都需要主站确认
    tmp_data.piid = ReportGLBpiid;
    tmp_data.bLen = len;
    tmp_data.taskIndex = TaskIndex; //任务号

    rpt_record_write(&tmp_data);        //将数据写入上报队列
}

/**
*********************************************************************
* @brief:       获取上报数据并组成上报帧存入上报队列，即使分帧一次也只获取一个报文
* @param[in]：   TaskIndex 任务索引
* @param[in]：   isAppSlice 是否应用分帧 1-是 0-否
* @param[out]： seq         帧序列号 
* @return：      void
*********************************************************************
*/
int rpt_task_data_package(uint8 *pTaskIndex ,uint8 *seq)
{
    int ret = 0;
    uint16 msglen = 0;        
    uint8 pOutMsg[APP_SEND_MSG_MAXLEN] = {0};
    uint16 outMaxLen = APP_SEND_MSG_MAXLEN;
    BP_INFO_T* bpInfo = NULL;
    uint8 taskIndex = *pTaskIndex; //每次报1个任务
    static uint16 frameIndex = 0;

    if(reportList[taskIndex].chann.pApduBreak == NULL)
    {
    //曲线数据如果传入时间为FF需要人为修改上报时间
        if(reportList[taskIndex].isNeedUpdateTIme)
        {
                rpt_task_time_modify(taskIndex);
        }

        reportList[taskIndex].chann.clientid = g_rptUdpHandle;
        reportList[taskIndex].chann.pApduBreak = app_apdubreak_init();

    //上报长度限制
        reportList[taskIndex].chann.tContext.ApplSplitLen = 1024;

        if(reportList[taskIndex].plan.data.choice == 0)
    {
            reportList[taskIndex].chann.private[1] = 1; //[1] GetRequestNormal
    }
    else
    {
            reportList[taskIndex].chann.private[1] = 3; //[3] GetRequestRecord
    }

        reportList[taskIndex].chann.private[5] = 1; //[5] 上报标志

#if AREA_SHANDONG
        reportList[taskIndex].chann.private[6] = reportTaskCfgList.OopTaskCfg[taskIndex].RunCycle;
        reportList[taskIndex].chann.private[7] = reportTaskCfgList.OopTaskCfg[taskIndex].RunCycleUnit;
        reportList[taskIndex].chann.private[8] = 1;//表示是上报触发的规约召测
#endif

        RPT_BUF_DEBUG(reportList[taskIndex].apduValue, reportList[taskIndex].apduLength, "内部索引-%d 任务-%d 方案-%d, 伪召测报文:\n",
            taskIndex, reportTaskCfgList.OopTaskCfg[taskIndex].TaskId, reportList[taskIndex].plan.planID);
        }
        
    msglen = invoke_get_req(&reportList[taskIndex].chann, reportList[taskIndex].apduValue, reportList[taskIndex].apduLength, pOutMsg, outMaxLen);

        if(msglen > 0)
        {
        rpt_task_data_contruct(pOutMsg, msglen, outMaxLen, taskIndex, reportList[taskIndex].chann.id);
        }
        else
        {
        ret = ERR_NORMAL;
        }

    bpInfo = (BP_INFO_T*)reportList[taskIndex].chann.pApduBreak;

    //分帧则召测下一帧,但有错误不再召测下一帧
    if((bpInfo->appSlicingFlag == 1) && (msglen > 0))
    {
        reportList[taskIndex].chann.private[1] = 5; //[5] GetRequestNext
        frameIndex++;            //分帧计数

        //分帧结束，释放断点，末帧是分帧标记不会清零
        if(bpInfo->isEnd == 1)
        {
            app_apdubreak_free(&reportList[taskIndex].chann.pApduBreak);
            memset(&reportList[taskIndex].chann, 0, sizeof(CHANNEL_CACHE_T));
            RPT_FMT_DEBUG("内部索引-%d 任务-%d 方案-%d 应用分帧完成, 共分%d帧.\n", taskIndex, reportTaskCfgList.OopTaskCfg[taskIndex].TaskId, reportList[taskIndex].plan.planID, frameIndex);
            taskIndex++;
            frameIndex = 0;
        }
    }
    else
    {
        app_apdubreak_free(&reportList[taskIndex].chann.pApduBreak);
        memset(&reportList[taskIndex].chann, 0, sizeof(CHANNEL_CACHE_T));
        taskIndex++;
    }
    
    *pTaskIndex = taskIndex;
    
    return ret;
}

#endif


#if DESC("上报任务和方案处理",1)

/**
*********************************************************************
* @brief：      读取任务参数(//TODO 公共函数)
* @param[in]： TaskId    任务ID
* @param[out]：pTaskCfg 内部的任务配置
* @return：     void
*********************************************************************
*/
int rpt_task_para_read(uint8 taskId, OOP_TASK_CFG *pTaskCfg)
{
    OOP_TASK_T task = {0};
    int ret = 0;
   
    ret = app_get_task_list(taskId, &task);
    if(ret != 0)
    {
        return ret;
    }

    //应该读出的是指定taskid的任务
    if(task.id != taskId)
    {
        RPT_FMT_DEBUG("rpt_task_para_read failed. task.id(%d) != taskId(%d)\n", task.id, taskId);
        return ERR_EXECFAIL;
    }
    
    pTaskCfg->TaskId = task.id;                 //任务ID
    pTaskCfg->RunCycle = task.acqFreq.value;    //执行频率数值
    pTaskCfg->RunCycleUnit = task.acqFreq.unit; //执行频率单位
    pTaskCfg->SchemeType = task.planType;       //方案类型
    pTaskCfg->SchemeId = task.planID;           //方案编号
    
    pTaskCfg->RunBeginTime[0] = task.startTime.second;          //任务开始时间:秒
    pTaskCfg->RunBeginTime[1] = task.startTime.minute;          //任务开始时间:分    
    pTaskCfg->RunBeginTime[2] = task.startTime.hour;            //任务开始时间:时    
    pTaskCfg->RunBeginTime[3] = task.startTime.day;             //任务开始时间:日
    pTaskCfg->RunBeginTime[4] = task.startTime.month;           //任务开始时间:月    
    pTaskCfg->RunBeginTime[5] = task.startTime.year&0xff;       //任务开始时间:年低8bit
    pTaskCfg->RunBeginTime[6] = (task.startTime.year>>8)&0xff;  //任务开始时间:年高8bit
    
    pTaskCfg->RunEndTime[0] = task.endTime.second;          //任务结束时间:秒
    pTaskCfg->RunEndTime[1] = task.endTime.minute;          //任务结束时间:分    
    pTaskCfg->RunEndTime[2] = task.endTime.hour;            //任务结束时间:时    
    pTaskCfg->RunEndTime[3] = task.endTime.day;             //任务结束时间:日
    pTaskCfg->RunEndTime[4] = task.endTime.month;           //任务结束时间:月    
    pTaskCfg->RunEndTime[5] = task.endTime.year&0xff;       //任务结束时间:年低8bit
    pTaskCfg->RunEndTime[6] = (task.endTime.year>>8)&0xff;  //任务结束时间:年高8bit

    pTaskCfg->DelayTime = task.acqDelay.value;           //延时值
    pTaskCfg->DelayTimeUnit = task.acqDelay.unit;        //延时单位
    pTaskCfg->Priority = task.priority;                  //执行优先级
    pTaskCfg->TaskState = task.state;                    //任务状态
    pTaskCfg->PreRunScript = task.startScriptID;         //任务开始前脚本ID
    pTaskCfg->OverRunScript = task.endScriptID;          //任务结束后脚本ID 
    pTaskCfg->TimePeriordType = task.workPeriod.type;    //任务运行时段:时段类型   
    pTaskCfg->TimePeriordNum = task.workPeriod.nNum;     //任务运行时段:时段数
    memcpy_s(pTaskCfg->Periordhhmm, sizeof(task.workPeriod.period), task.workPeriod.period, sizeof(task.workPeriod.period)); //时段表

    return ret;
}

/**
*********************************************************************
* @brief：      上报方案的记录型上报内容编码
* @param[in]： report      原始上报方案参数
* @param[in]： offset      编码时的偏移
* @param[out]：pReportCfg 编码后的上报方案参数
* @return：     void
*********************************************************************
*/
void rpt_plan_record_read(RECORDDATA record, uint32 *offset, uint8 *apduValue)
{
    /*GetRecord∷=SEQUENCE
     {
         对象属性描述符 OAD，
         记录行选择描述符 RSD，
         记录列选择描述符 RCSD
     }*/

    //OAD类型不存储0x02-结构体 0x03-成员数 0x51-OAD
    memcpy_r(&apduValue[*offset], &record.oad, sizeof(uint32)); //OAD
    (*offset) += 4;

    //RSD读取

    apdu_prsd_tofmt698(apduValue, 500, offset, &record.tRSD, sizeof(OOP_PRSD_T));

    //RCSD读取
    apdu_pcsd_tofmt698(apduValue, 500, offset, &record.tRCSD, sizeof(OOP_PCSD_T));

    return;
}

/**
*********************************************************************
* @brief：      读取上报方案参数 //TODO 可能并不需要在这里解析成698格式
* @param[in]： PlanId      方案编号
* @param[out]：pReportCfg 上报方案参数
* @return：     void
*********************************************************************
*/
int rpt_plan_para_read(uint8 planId, RPT_PLAN_CFG_T *pReportCfg)
{
    int ret = 0;
    OOP_PLAN_REPORT_T report = {0};
    uint32 offset = 0;
    uint8 tmp[4] = {0};
    
    ret = app_get_plan_report_list(planId, &report);
    if(ret != 0)
    {
        RPT_FMT_LOGCC("rpt_plan_para_read failed. ret(%d), planId(%d)\n", ret, planId);
        return ret;
    }

    //应该读出的是指定planID的任务
    if(report.planID != planId)
    {
        RPT_FMT_DEBUG("rpt_plan_para_read failed. report.planID(%d) != planId(%d)\n", report.planID, planId);
        return ERR_EXECFAIL;
    }

#ifdef AREA_SHANDONG
        OOP_MSP_T  tmpms = {0};
        if(rpt_plan_search(0x50040200, 0, &report.data))
        {
            OOP_PRSD_T *pRSD = &report.data.record.tRSD;
            if(pRSD->choice == 10 || pRSD->choice == 6 || pRSD->choice == 7 )
            {
                if(pRSD->choice == 10)
                {
                    tmpms = pRSD->sel10.ms;
                }
                else if(pRSD->choice == 6)
                {
                    tmpms = pRSD->sel6.ms;  
                }
                else if(pRSD->choice == 7)
                {
                    tmpms = pRSD->sel7.ms;  
                }

                //联合体，目标最后改，否则影响拷贝前的数据
                pRSD->sel8.ms = tmpms; 
                pRSD->sel8.ti.unit = 2;
                pRSD->sel8.ti.value = 1;    
                memset(&pRSD->sel8.start, 0xff, sizeof(OOP_DATETIME_S_T));
                memset(&pRSD->sel8.end, 0xff, sizeof(OOP_DATETIME_S_T));
                pRSD->choice = 8;
            }
        }
#endif

    memcpy_s(&pReportCfg->plan, sizeof(report), &report, sizeof(report));

    
    //上报内容转换为698格式表示
    offset = 0;
    if(report.data.choice == 0)  //单OAD类型
    {
        memcpy_r(&pReportCfg->serviceOAD, &report.data.oad, sizeof(uint32));
        memcpy_r(&pReportCfg->apduValue[offset], &report.data.oad, sizeof(uint32));
        offset += 4;
    }
    else                         //Record类型
    {
        memcpy_r(&pReportCfg->serviceOAD, &report.data.record.oad, sizeof(uint32));
        rpt_plan_record_read(report.data.record, &offset, pReportCfg->apduValue);

        //无TimeTag
        memcpy_s(&pReportCfg->apduValue[offset], 1, tmp, 1);
        offset += 1;
    }

    pReportCfg->apduLength = offset; //转换后的698规约报文APDU部分长度
    pReportCfg->isModify = TRUE;

    return ret;
}

#endif

#if DESC("上报条件检测功能",1)

/**
*********************************************************************
* @brief：      写一条任务数据
* @param[in]： save_data  要存储的数据
* @param[out]：NA
* @return： void
*********************************************************************
*/
int rpt_record_write(AUTO_DATA *save_data)
{
    uint16 recordcount;     //记录的总条数
    int ret = 0;
    uint32 offset = 0;
    uint16 len = sizeof(AUTO_DATA);

    //读出目前记录总条数
    ret = read_pdata_xram(RPT_RECORD_FILE, (char *)&recordcount, RPT_RECORD_CNT_OFFSET, 2);
    if((ret != 0 )|| (recordcount == 0xffff))
    {
        RPT_FMT_DEBUG("%s read. ret(%d), recCnt(%d).\n", RPT_RECORD_FILE, ret, recordcount);
        recordcount = 0;
    }

    recordcount = RPT_ADD_VALUE_CLEAR(recordcount, RPT_RECORD_MAX, 1);

    offset = RPT_RECORD_DATA_OFFSET(recordcount);

    //写入任务数据
    ret = write_pdata_xram(RPT_RECORD_FILE, (char *)save_data,  offset, len);
    if(ret >= 0)
    {    //写入记录总条数
        ret = write_pdata_xram(RPT_RECORD_FILE, (char*)&recordcount, RPT_RECORD_CNT_OFFSET, 2);
        RPT_BUF_TRACE(save_data->bData, save_data->bLen, "[%d]enqueue: %s write. offset(%d), len(%d)\n", recordcount, RPT_RECORD_FILE, offset, len);
    }
    else
    {
        RPT_BUF_DEBUG(save_data->bData, save_data->bLen, "[%d]enqueue: %s write failed. offset(%d), len(%d)\n", recordcount, RPT_RECORD_FILE, offset, len);
    }

    return ret;
}

/**
*********************************************************************
* @brief：     删除上报任务参数
* @param[in]：tblid   参数表索引
* @param[in]：taskid 任务ID
* @return：     void
*********************************************************************
*/
void rpt_task_param_delete(uint8 tblid, uint8 taskid)
{
    memset(&reportTaskCfgList.OopTaskCfg[tblid], 0, sizeof(OOP_TASK_CFG));
    
    app_apdubreak_free(&reportList[tblid].chann.pApduBreak);
    memset(&reportList[tblid], 0, sizeof(RPT_PLAN_CFG_T));

    if(tblid < reportTaskCfgList.TaskNum-1)
    {
        memmove(&reportTaskCfgList.OopTaskCfg[tblid], &reportTaskCfgList.OopTaskCfg[tblid+1],
            (reportTaskCfgList.TaskNum-tblid-1)*sizeof(OOP_TASK_CFG));

        memmove(&reportList[tblid], &reportList[tblid+1],
            (reportTaskCfgList.TaskNum-tblid-1)*sizeof(RPT_PLAN_CFG_T));
    }

    reportTaskCfgList.TaskNum--;
}

/**
*********************************************************************
* @brief：     修改上报任务参数
* @param[in]：tblid   参数表索引
* @param[in]：taskid 任务ID
* @return：     void
*********************************************************************
*/
void rpt_task_param_modify(uint8 tblid, uint8 taskid)
{
    int ret = 0;

    memset(&reportTaskCfgList.OopTaskCfg[tblid], 0, sizeof(OOP_TASK_CFG));

    app_apdubreak_free(&reportList[tblid].chann.pApduBreak);
    memset(&reportList[tblid], 0, sizeof(RPT_PLAN_CFG_T));
    
    ret = rpt_task_para_read(taskid, &reportTaskCfgList.OopTaskCfg[tblid]);
    if(ret != 0)
    {
        rpt_task_param_delete(tblid, taskid);
        return;
    }

    ret = rpt_plan_para_read(reportTaskCfgList.OopTaskCfg[tblid].SchemeId, &reportList[tblid]);
    if(ret != 0)
    {
        rpt_task_param_delete(tblid, taskid);
        return;
    }

    return;
}

/**
*********************************************************************
* @brief：     增加上报任务参数
* @param[in]：tblid   参数表索引
* @param[in]：taskid 任务ID
* @return：     void
*********************************************************************
*/
int rpt_task_param_add(uint8 tblid, uint8 taskid)
{
    int ret = 0;

    memset(&reportTaskCfgList.OopTaskCfg[tblid], 0, sizeof(OOP_TASK_CFG));

    app_apdubreak_free(&reportList[tblid].chann.pApduBreak);
    memset(&reportList[tblid], 0, sizeof(RPT_PLAN_CFG_T));
    
    ret = rpt_task_para_read(taskid, &reportTaskCfgList.OopTaskCfg[tblid]);
    if(ret != 0)
    {
        return ret;
    }

    ret = rpt_plan_para_read(reportTaskCfgList.OopTaskCfg[tblid].SchemeId, &reportList[tblid]);
    if(ret != 0)
    {
        return ret;
    }

    reportTaskCfgList.TaskNum++;

    return ERR_OK;
}


/**
*********************************************************************
* @brief：     删除上报方案参数
* @param[in]：tblid   参数表索引
* @param[in]：taskid 任务ID
* @return：     void
*********************************************************************
*/
void rpt_plan_param_delete(uint8 tblid, uint8 planid)
{
    int ret = 0;

    memset(&reportTaskCfgList.OopTaskCfg[tblid], 0, sizeof(OOP_TASK_CFG));
    
    app_apdubreak_free(&reportList[tblid].chann.pApduBreak);
    memset(&reportList[tblid], 0, sizeof(RPT_PLAN_CFG_T));

    ret = rpt_plan_para_read(planid, &reportList[tblid]);
    if(ret != 0)
    {
        return;
    }

    if(tblid < reportTaskCfgList.TaskNum-1)
    {
        memmove(&reportTaskCfgList.OopTaskCfg[tblid], &reportTaskCfgList.OopTaskCfg[tblid+1],
            (reportTaskCfgList.TaskNum-tblid-1)*sizeof(OOP_TASK_CFG));

        memmove(&reportList[tblid], &reportList[tblid+1],
            (reportTaskCfgList.TaskNum-tblid-1)*sizeof(RPT_PLAN_CFG_T));
    }

    reportTaskCfgList.TaskNum--;
}

/**
*********************************************************************
* @brief：     修改上报方案参数
* @param[in]：tblid   参数表索引
* @param[in]：taskid 任务ID
* @return：     void
*********************************************************************
*/
void rpt_plan_param_modify(uint8 tblid, uint8 planid)
{
    int ret = 0;
    uint8 taskId = reportTaskCfgList.OopTaskCfg[tblid].TaskId;

    app_apdubreak_free(&reportList[tblid].chann.pApduBreak);
    memset(&reportList[tblid], 0, sizeof(RPT_PLAN_CFG_T));

    ret = rpt_plan_para_read(reportTaskCfgList.OopTaskCfg[tblid].SchemeId, &reportList[tblid]);
    if(ret != 0)
    {
        rpt_task_param_delete(tblid, reportTaskCfgList.OopTaskCfg[tblid].TaskId);
        return;
    }

    memset(&reportTaskCfgList.OopTaskCfg[tblid], 0, sizeof(OOP_TASK_CFG));
    
    ret = rpt_task_para_read(taskId, &reportTaskCfgList.OopTaskCfg[tblid]);
    if(ret != 0)
    {
        rpt_task_param_delete(tblid, reportTaskCfgList.OopTaskCfg[tblid].TaskId);
        return;
    }

    return;
}

/**
*********************************************************************
* @brief：     添加上报方案参数
* @param[in]：tblid   参数表索引
* @param[in]：taskid 任务ID
* @return：     void
*********************************************************************
*/
int rpt_plan_param_add(uint8 tblid, uint8 planid)
{
    int ret = 0;
    uint8 tmpId = 0;

    memset(&reportTaskCfgList.OopTaskCfg[tblid], 0, sizeof(OOP_TASK_CFG));

    app_apdubreak_free(&reportList[tblid].chann.pApduBreak);
    memset(&reportList[tblid], 0, sizeof(RPT_PLAN_CFG_T));

    ret = rpt_plan_para_read(planid, &reportList[tblid]);
    if(ret != 0)
    {
        return ret;
    }

    while(tmpId++ < MAX_ID_NUM - 1)
    {
        if((oopTaskList.oopTask[tmpId].planType == 4)&&(oopTaskList.oopTask[tmpId].planID == planid))
        {
            rpt_task_param_add(tblid, tmpId);
        }
    }

    if(reportTaskCfgList.TaskNum > tblid)
    {
        return ERR_OK;
    }
    else
    {
        return ERR_NORESOURCE;
    }
}

/**
*********************************************************************
* @brief：      读取上报的任务和方案
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
void rpt_task_info_read(void)
{
    uint8 tmpId = 0;
    uint8 TaskNum = 0;
    uint16 offset;
    REPORT_CONTEXT  rptContext;
    BOOLEAN isTaskChanged = 0;
    BOOLEAN isPlanChanged = 0;
    BOOLEAN isChanged = 0;
    int i = 0;
    int ret = 0;

    //检查任务变更
    while(tmpId++ < MAX_ID_NUM - 1)
    {
        if(oopTaskList.isChanged[tmpId] == TRUE)
        {
            isTaskChanged = FALSE;
            for(i = 0; i < reportTaskCfgList.TaskNum; i++)
            {
                if(reportTaskCfgList.OopTaskCfg[i].TaskId == tmpId)
                {
                    isTaskChanged = TRUE;
                    isChanged = TRUE;

                    if(oopTaskList.isVaild[tmpId] == FALSE)
                    {
                        RPT_FMT_DEBUG("删除上报任务-%d 关联方案-%d.\n", tmpId, reportTaskCfgList.OopTaskCfg[i].SchemeId);

                        //从参数列表中删除任务与对应的方案
                        rpt_task_param_delete(i, tmpId);
                    }
                    else
                    {
                        RPT_FMT_DEBUG("修改上报任务-%d.\n", tmpId);
                        //修改参数列表中的任务并更新对应方案
                        rpt_task_param_modify(i, tmpId);
                    }

                    break;
                }
            }

            if((!isTaskChanged)&&(oopTaskList.isVaild[tmpId] == TRUE)&&(oopTaskList.oopTask[tmpId].planType == 4))
            {
                RPT_FMT_DEBUG("新增上报任务-%d.\n", tmpId);
                isTaskChanged = FALSE;

                //将有对应方案的新增任务放入参数列表
                ret = rpt_task_param_add(reportTaskCfgList.TaskNum, tmpId);
                if(ret == 0)
                {
                    isChanged = TRUE;
                }
            }
        }  
    }

    tmpId = 0;

    //检查方案变更
    while(tmpId++ < MAX_ID_NUM - 1)
    {    
        //一一对应，任务数和方案数一致
        if(oopPlanReportList.isChanged[tmpId] == TRUE)
        {
            isPlanChanged = FALSE;
            for(i = 0; i < reportTaskCfgList.TaskNum; i++)
            {
                if(reportList[i].plan.planID == tmpId)
                {
                    isPlanChanged = TRUE;
                    isChanged = TRUE;
                    if(oopPlanReportList.isVaild[tmpId] == FALSE)
                    {
                        RPT_FMT_DEBUG("删除上报方案-%d 关联任务-%d.\n", tmpId, reportTaskCfgList.OopTaskCfg[i].TaskId);

                        //从参数列表中删除任务与对应的方案
                        rpt_plan_param_delete(i, tmpId);
                    }
                    else
                    {
                        RPT_FMT_DEBUG("修改上报方案-%d.\n", tmpId);

                        //修改参数列表中的方案
                        rpt_plan_param_modify(i, tmpId);
                    }
        
                    break;
                }
            }
        
            if((!isPlanChanged)&&(oopPlanReportList.isVaild[tmpId] == TRUE))
            {
                RPT_FMT_DEBUG("新增上报方案-%d.\n", tmpId);
                isPlanChanged = FALSE;

                //从任务中寻找使用该方案的任务并加载
                ret = rpt_plan_param_add(reportTaskCfgList.TaskNum, tmpId);
                if(ret == 0)
                {
                    isChanged = TRUE;
                }                
            }
        }      
    }
    
    //任务和方案都没有变化，不重新配置参数表
    if(isChanged != TRUE)
    {
        return;
    }

    MEMZERO(&rptContext, sizeof(REPORT_CONTEXT));

    //检查是否有需要调整的任务
    for(TaskNum = 0; TaskNum < reportTaskCfgList.TaskNum; TaskNum++)
    {
        if(reportList[TaskNum].isModify != TRUE)
        {
            continue;
        }
        
        reportList[TaskNum].isModify = FALSE;
        
        rptContext = reportList[TaskNum].plan.data;

#if defined(AREA_SHANDONG) || defined(AREA_ANHUI)
        //找到上报任务对应的普通采集任务
        reportTaskCfgList.OopTaskCfg[TaskNum].normalTaskId = rpt_plan_normal_taskid_get(reportTaskCfgList.OopTaskCfg[TaskNum].RunCycle,
                    reportTaskCfgList.OopTaskCfg[TaskNum].RunCycleUnit,&rptContext);
        RPT_FMT_DEBUG("上报任务%d 对应的普通采集任务 %d \n",reportTaskCfgList.OopTaskCfg[TaskNum].TaskId,reportTaskCfgList.OopTaskCfg[TaskNum].normalTaskId);
#endif 
#ifdef AREA_SHANDONG
        //将山东的日冻结上报改为一小时执行一次 延时一分钟
        if(rpt_plan_search(0x50040200, 0, &rptContext))
        {
            reportTaskCfgList.OopTaskCfg[TaskNum].RunCycleUnit = 1;
            reportTaskCfgList.OopTaskCfg[TaskNum].RunCycle = 60;
            reportTaskCfgList.OopTaskCfg[TaskNum].DelayTimeUnit=1;
            reportTaskCfgList.OopTaskCfg[TaskNum].DelayTime = 1;
        }

#endif
#ifdef AREA_HEBEI
        //将河北的一分钟冻结上报改为延时20秒
        if((rpt_plan_search(0x50020200, 0, &rptContext)) && 
           (reportTaskCfgList.OopTaskCfg[TaskNum].RunCycleUnit == 1) &&
           (reportTaskCfgList.OopTaskCfg[TaskNum].RunCycle == 1))
        {
            reportTaskCfgList.OopTaskCfg[TaskNum].DelayTimeUnit = 0;
            reportTaskCfgList.OopTaskCfg[TaskNum].DelayTime = 20;
        }

#endif
        if(rptContext.choice > 1)
        {
            continue;
        }
        else if(rptContext.choice==1)//record类型
        {

            offset = 4; //RSD的起始处

            //上报曲线数据（方法 6 7 8），如果下发时间为全FF，需要根据任务时间来自己填写时间
            if(rptContext.record.tRSD.choice > 5 && rptContext.record.tRSD.choice < 9)
            {
                //时间用FF表示无效,需要自动去填时间
                if(rptContext.record.tRSD.sel5.time.year == 0xFFFF)
                {
                    reportList[TaskNum].isNeedUpdateTIme = TRUE;
                    reportList[TaskNum].TimeOffset = offset+1;//需要在这个偏移位置开始修改时间
                }
                else
                {
                    reportList[TaskNum].isNeedUpdateTIme = FALSE;
                }
            }
        }

        RPT_BUF_DEBUG(reportList[TaskNum].apduValue, reportList[TaskNum].apduLength, "内部索引-%d 任务-%d 方案-%d 更新\n", 
            TaskNum, reportTaskCfgList.OopTaskCfg[TaskNum].TaskId, reportList[TaskNum].plan.planID);
    }
}

/**
*********************************************************************
* @brief：      当前时间落在任务执行时段内
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
BOOL IsNowInTaskPeriord(OOP_TASK_CFG *pTaskCfg)
{
    uint8       i = 0, PeriordNum = 0, PeriordType = 0;
    uint8       pBuf[192] = {0};
    struct tm   tmNow;
    time_t      RealTime_t, BeginTime_t, EndTime_t;
    rtc_t       TimePeriordBegin, TimePeriordEnd;
    OOP_TI_T        ti;

    //时段只有小时，分钟，那么年月日秒用当前时间初始化
    RealTime_t = time(NULL);
    LOCALTIME_R(&RealTime_t, &tmNow);
    memset_s(&TimePeriordBegin, sizeof(rtc_t), 0, sizeof(rtc_t));
    TimePeriordBegin.yy = commfun_HexToBcd((uint8)(tmNow.tm_year - 100));   //年
    TimePeriordBegin.mm=  commfun_HexToBcd((uint8)(tmNow.tm_mon + 1));      //月
    TimePeriordBegin.dd = commfun_HexToBcd((uint8)tmNow.tm_mday);           //日
    TimePeriordBegin.ss = 0;            //秒
    memcpy_s(&TimePeriordEnd, sizeof(rtc_t), &TimePeriordBegin, sizeof(rtc_t));

    //处理时段数据
    memcpy_s(pBuf, 192, pTaskCfg->Periordhhmm, 192);
    PeriordNum = pTaskCfg->TimePeriordNum;
    PeriordType = pTaskCfg->TimePeriordType;
    ti.unit = 1;
    ti.value = 1;
    for(i = 0; i < PeriordNum; i++)
    {
        TimePeriordBegin.hh = commfun_HexToBcd(pBuf[i*4]);
        TimePeriordBegin.nn = commfun_HexToBcd(pBuf[i*4+1]);
        BeginTime_t = commfun_rtc2time2(TimePeriordBegin);
        //前开
        if(PeriordType == 1 || PeriordType == 3)
        {
            DT_TimeAddInv(&BeginTime_t, ti);
        }

        TimePeriordEnd.hh = commfun_HexToBcd(pBuf[i*4+2]);
        TimePeriordEnd.nn = commfun_HexToBcd(pBuf[i*4+3]);       
        EndTime_t = commfun_rtc2time2(TimePeriordEnd);
        //后开
        if(PeriordType == 0 || PeriordType == 3)
        {
            DT_TimeSubInv(&EndTime_t, ti);
        }

        if((RealTime_t >= BeginTime_t) && (RealTime_t <= EndTime_t))
            return TRUE;
    }
    return FALSE;
}

/**
*********************************************************************
* @brief：      判断是否进入新的轮次
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
BOOL JudgeIfInNewTurn(OOP_TASK_CFG *pTaskCfg,time_t RealTime_t)
{
    time_t          TaskRunBeginTime_t;                     //任务起始时间
    time_t          ThisTurnBeginTime_t;
    time_t          ThisTurnEndTime_t;
    OOP_TI_T        ti;
    rtc_t           rtc_tmp;
    uint8           year_h, year_l, mon, day;

    year_h = pTaskCfg->ThisTurnBeginTime[6];
    year_l = pTaskCfg->ThisTurnBeginTime[5];
    mon = pTaskCfg->ThisTurnBeginTime[4];
    day = pTaskCfg->ThisTurnBeginTime[3];

    memset_s((char *)&rtc_tmp, sizeof(rtc_t), 0x00, sizeof(rtc_t));
    rtc_tmp.yy = commfun_HexToBcd((int)(*(uint16 *)&pTaskCfg->RunBeginTime[5])-2000);
    rtc_tmp.mm = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[4]);
    rtc_tmp.dd = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[3]);
    rtc_tmp.hh = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[2]);
    rtc_tmp.nn = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[1]);
    rtc_tmp.ss = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[0]);
    TaskRunBeginTime_t = commfun_rtc2time2(rtc_tmp);
    //把延时加上去
    ti.unit = pTaskCfg->DelayTimeUnit;
    ti.value = pTaskCfg->DelayTime;
    DT_TimeAddInv(&TaskRunBeginTime_t, ti);

    //未到达任务开始时间
    if(RealTime_t < TaskRunBeginTime_t)
    {
        return FALSE;
    }

    //当前时间大于任务开始时间,需要判断轮次
    if((year_h != 0) || (year_l != 0) || (mon != 0) || (day != 0))
    {
        memset_s(&rtc_tmp, sizeof(rtc_t), 0, sizeof(rtc_t));
        memcpy_s(&rtc_tmp, 6, pTaskCfg->ThisTurnBeginTime, 6);
        ThisTurnBeginTime_t = commfun_rtc2time2(rtc_tmp);
        
        memset_s(&rtc_tmp, sizeof(rtc_t), 0, sizeof(rtc_t));
        memcpy_s(&rtc_tmp, 6, pTaskCfg->ThisTurnEndTime, 6);
        ThisTurnEndTime_t = commfun_rtc2time2(rtc_tmp);
#ifdef AREA_SHANDONG
        time_t thisTurnBeginWithoutDelay;  //没有延时的本轮开始时间
        thisTurnBeginWithoutDelay = ThisTurnBeginTime_t;
        DT_TimeSubInv(&thisTurnBeginWithoutDelay,ti);
        if( RealTime_t>=ThisTurnEndTime_t)
        {
            pTaskCfg->checkTaskFinish =0;
        }
        if(pTaskCfg->checkTaskFinish ==2)
            return FALSE;
        if( RealTime_t>thisTurnBeginWithoutDelay &&RealTime_t<ThisTurnBeginTime_t )
        {
            if(pTaskCfg->checkTaskFinish !=2)
                pTaskCfg->checkTaskFinish =1;
        }
        //如果没有配延时，则需要查询任务是否抄完 不再走正常的流程
        //如果没有配延时，且有表抄不通会一直查任务监控状态 最后留一分钟给上报
        if(ti.value ==0 && pTaskCfg->normalTaskId!=0 &&RealTime_t > ThisTurnBeginTime_t && RealTime_t < ThisTurnEndTime_t-60)
        {
            pTaskCfg->checkTaskFinish =1;
            return FALSE;
        }

#endif
        memset_s(&rtc_tmp, sizeof(rtc_t), 0, sizeof(rtc_t));
        memcpy_s(&rtc_tmp, 6, pTaskCfg->NextTurnBeginTime, 6);

        if(!(RealTime_t > ThisTurnBeginTime_t && RealTime_t < ThisTurnEndTime_t))   //不在任务执行轮次内，直接返回
        {
            pTaskCfg->thisTurnReportFlag = FALSE;
            return FALSE;
        }

        if(IsNowInTaskPeriord(pTaskCfg))//进入新的轮次，且在有效时段内
        {
            if(pTaskCfg->thisTurnReportFlag)//本轮次已经上报过,无需上报
                return FALSE;
            else
                return TRUE;
        }
        else
        {
            return FALSE;
        }

    }
    return FALSE;
}

/**
*********************************************************************
* @brief：      当前时间已经超过本轮的区间，则更新本轮的开始和结束时间
* @param[in]： NA
* @param[out]：NA
* @return：     void
*********************************************************************
*/
BOOL UpdateTaskTime(OOP_TASK_CFG *pTaskCfg)
{
    struct tm       tm_Time;
    rtc_t           TaskRunBeginTime;                       //方案开始执行时间(RTC格式)
    //rtc_t           TaskRunEndTime;                 		//方案结束执行时间(RTC格式)
    time_t          RealTime_t;                             //当前系统时间的转换的秒数
    time_t          TaskRunBeginTime_t;                     //方案开始执行转换后的秒
    //time_t          TaskRunEndTime_t;               		//方案结束执行转换后的秒

    time_t          ThisTurnBeginTime_t;
    time_t          ThisTurnEndTime_t;
    time_t          NextTurnBeginTime_t;
    time_t          LastTurnBeginTime_t;
    OOP_TI_T        ti;
    time_t          tm_tmp = 0;
    rtc_t           rtc_tmp;
    uint32          cycle = pTaskCfg->RunCycle;
    uint8           year_h, year_l, mon, day;

    RealTime_t = time(NULL);
    LOCALTIME_R(&RealTime_t, &tm_Time);
    year_h = pTaskCfg->ThisTurnBeginTime[6];
    year_l = pTaskCfg->ThisTurnBeginTime[5];
    mon = pTaskCfg->ThisTurnBeginTime[4];
    day = pTaskCfg->ThisTurnBeginTime[3];

    //防止无效上报任务错误执行，比如pTaskCfg->RunCycle为0
    if ((pTaskCfg->RunCycle == 0) || (pTaskCfg->TaskState == 0))
    {
        return FALSE;
    }

    if((year_h != 0) || (year_l != 0) || (mon != 0) || (day != 0))
    {
        memset_s(&rtc_tmp, sizeof(rtc_t), 0, sizeof(rtc_t));
        memcpy_s(&rtc_tmp, 6, pTaskCfg->ThisTurnBeginTime, 6);
        ThisTurnBeginTime_t = commfun_rtc2time2(rtc_tmp);
        
        memset_s(&rtc_tmp, sizeof(rtc_t), 0, sizeof(rtc_t));
        memcpy_s(&rtc_tmp, 6, pTaskCfg->ThisTurnEndTime, 6);
        ThisTurnEndTime_t = commfun_rtc2time2(rtc_tmp);
        
        memset_s(&rtc_tmp, sizeof(rtc_t), 0, sizeof(rtc_t));
        memcpy_s(&rtc_tmp, 6, pTaskCfg->NextTurnBeginTime, 6);
        NextTurnBeginTime_t = commfun_rtc2time2(rtc_tmp);
        
        //还在本轮无需更新,跟老代码不同,END取了等于
        if(RealTime_t > ThisTurnBeginTime_t && RealTime_t <= ThisTurnEndTime_t) 
        {
            return FALSE;
        }
    }

    //年份使用2000年以来的时间，但是BCD码只能到99，因此时间超过2099年会出问题
    TaskRunBeginTime.yy = commfun_HexToBcd((int)(*(uint16 *)&pTaskCfg->RunBeginTime[5])-2000);
    switch(pTaskCfg->RunCycleUnit)
    {
    case TI_YEAR:
        TaskRunBeginTime.mm = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[4]);
        TaskRunBeginTime.dd = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[3]);
        TaskRunBeginTime.hh = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[2]);
        TaskRunBeginTime.nn = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[1]);
        TaskRunBeginTime.ss = 0;
        break;
    case TI_MON:
        TaskRunBeginTime.mm = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[4]);
        TaskRunBeginTime.dd = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[3]);
        TaskRunBeginTime.hh = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[2]);
        TaskRunBeginTime.nn = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[1]);
        TaskRunBeginTime.ss = 0;
        break;
    case TI_DAY:
        TaskRunBeginTime.mm = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[4]);
        TaskRunBeginTime.dd = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[3]);
        TaskRunBeginTime.hh = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[2]);
        TaskRunBeginTime.nn = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[1]);
        TaskRunBeginTime.ss = 0;
        break;
    case TI_HOUR:
        TaskRunBeginTime.mm = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[4]);
        TaskRunBeginTime.dd = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[3]);
        TaskRunBeginTime.hh = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[2]);
        TaskRunBeginTime.nn = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[1]);
        TaskRunBeginTime.ss = 0;
        break;
    case TI_MIN:
        TaskRunBeginTime.mm = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[4]);
        TaskRunBeginTime.dd = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[3]);
        TaskRunBeginTime.hh = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[2]);
        TaskRunBeginTime.nn = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[1]);
        TaskRunBeginTime.ss = 0;
        break;
    case TI_SEC:
        TaskRunBeginTime.mm = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[4]);
        TaskRunBeginTime.dd = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[3]);
        TaskRunBeginTime.hh = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[2]);
        TaskRunBeginTime.nn = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[1]);
        TaskRunBeginTime.ss = commfun_HexToBcd((int)pTaskCfg->RunBeginTime[0]);
        break;
    }
    TaskRunBeginTime_t = commfun_rtc2time2(TaskRunBeginTime);
    //任务开始时间把延时加上去
    ti.unit = pTaskCfg->DelayTimeUnit;
    ti.value = pTaskCfg->DelayTime;
    DT_TimeAddInv(&TaskRunBeginTime_t, ti);

    //方案结束时间
    //TaskRunEndTime.yy = commfun_HexToBcd((int)(*(uint16 *)&pTaskCfg->RunEndTime[5])-2000);
    //TaskRunEndTime.mm = commfun_HexToBcd((int)pTaskCfg->RunEndTime[4]);
    //TaskRunEndTime.dd = commfun_HexToBcd((int)pTaskCfg->RunEndTime[3]);
    //TaskRunEndTime.hh = commfun_HexToBcd((int)pTaskCfg->RunEndTime[2]);
    //TaskRunEndTime.nn = commfun_HexToBcd((int)pTaskCfg->RunEndTime[1]);
    //TaskRunEndTime.ss = commfun_HexToBcd((int)pTaskCfg->RunEndTime[0]);
    //TaskRunEndTime_t = commfun_rtc2time2(TaskRunEndTime);
    
    ti.unit = pTaskCfg->RunCycleUnit; //任务执行频率
    ti.value = (uint16)cycle;

    tm_tmp = TaskRunBeginTime_t;

    if(tm_tmp >= RealTime_t) //任务尚未开始
    {
        ThisTurnBeginTime_t = tm_tmp;
        DT_TimeAddInv(&tm_tmp, ti);
    }
    else  //更新本轮开始时间
    {
        if(ti.unit == TI_SEC || ti.unit == TI_MIN || ti.unit == TI_HOUR || ti.unit == TI_DAY)
        {
            int val = cycle;
            if(ti.unit == TI_MIN)
            {
                val *= 60;
            }
            if(ti.unit == TI_HOUR)
            {
                val *= 3600;
            }
            if(ti.unit == TI_DAY)
            {
                val *= 86400;
            }
            ThisTurnBeginTime_t = (RealTime_t/val)*val;
            ThisTurnBeginTime_t +=  (TaskRunBeginTime_t%val);
            if(ThisTurnBeginTime_t > RealTime_t)
            {
                ThisTurnBeginTime_t -= val;
            }
            tm_tmp = ThisTurnBeginTime_t;
            DT_TimeAddInv(&tm_tmp, ti);
        }
        else
        {
            while(1)
            {
                DT_TimeAddInv(&tm_tmp, ti);
                if(tm_tmp > RealTime_t)
                {
                    break;
                }
            }
            ThisTurnBeginTime_t = tm_tmp;
            DT_TimeSubInv(&ThisTurnBeginTime_t, ti);
        }
    }

    //若不是透明传输，更新本轮结束时间
    if(pTaskCfg->SchemeType != REPORTPLAN)
    {
        if(ti.unit == TI_SEC)
        {
            ThisTurnEndTime_t = tm_tmp - 1;
        }
        else if(ti.unit == TI_MIN)
        {
            ThisTurnEndTime_t = tm_tmp - 30;
        }
        else
        {
            ThisTurnEndTime_t = tm_tmp - 60;
        }
    }

    LOCALTIME_R(&ThisTurnBeginTime_t, &tm_Time);
    pTaskCfg->ThisTurnBeginTime[5] = commfun_HexToBcd((uint8)(tm_Time.tm_year - 100));  //年
    pTaskCfg->ThisTurnBeginTime[4] = commfun_HexToBcd((uint8)(tm_Time.tm_mon + 1));     //月
    pTaskCfg->ThisTurnBeginTime[3] = commfun_HexToBcd((uint8)tm_Time.tm_mday);          //日
    pTaskCfg->ThisTurnBeginTime[2] = commfun_HexToBcd((uint8)tm_Time.tm_hour);          //时
    pTaskCfg->ThisTurnBeginTime[1] = commfun_HexToBcd((uint8)tm_Time.tm_min);           //分
    pTaskCfg->ThisTurnBeginTime[0] = commfun_HexToBcd((uint8)tm_Time.tm_sec);           //分
    RPT_FMT_TRACE("task%d 本轮开始时间:20%02x-%02x-%02x %02x:%02x:%02x\n", pTaskCfg->TaskId, pTaskCfg->ThisTurnBeginTime[5], pTaskCfg->ThisTurnBeginTime[4],
           pTaskCfg->ThisTurnBeginTime[3], pTaskCfg->ThisTurnBeginTime[2], pTaskCfg->ThisTurnBeginTime[1], pTaskCfg->ThisTurnBeginTime[0]);

    LOCALTIME_R(&ThisTurnEndTime_t, &tm_Time);
    pTaskCfg->ThisTurnEndTime[5] = commfun_HexToBcd((uint8)(tm_Time.tm_year - 100));    //年
    pTaskCfg->ThisTurnEndTime[4] = commfun_HexToBcd((uint8)(tm_Time.tm_mon + 1));       //月
    pTaskCfg->ThisTurnEndTime[3] = commfun_HexToBcd((uint8)tm_Time.tm_mday);            //日
    pTaskCfg->ThisTurnEndTime[2] = commfun_HexToBcd((uint8)tm_Time.tm_hour);            //时
    pTaskCfg->ThisTurnEndTime[1] = commfun_HexToBcd((uint8)tm_Time.tm_min);             //分
    pTaskCfg->ThisTurnEndTime[0] = commfun_HexToBcd((uint8)tm_Time.tm_sec);             //分
    RPT_FMT_TRACE("task%d 本轮结束时间:20%02x-%02x-%02x %02x:%02x:%02x\n", pTaskCfg->TaskId, pTaskCfg->ThisTurnEndTime[5], pTaskCfg->ThisTurnEndTime[4],
           pTaskCfg->ThisTurnEndTime[3], pTaskCfg->ThisTurnEndTime[2], pTaskCfg->ThisTurnEndTime[1], pTaskCfg->ThisTurnEndTime[0]);

    LOCALTIME_R(&NextTurnBeginTime_t, &tm_Time);
    pTaskCfg->NextTurnBeginTime[5] = commfun_HexToBcd((uint8)(tm_Time.tm_year - 100));  //年
    pTaskCfg->NextTurnBeginTime[4] = commfun_HexToBcd((uint8)(tm_Time.tm_mon + 1));     //月
    pTaskCfg->NextTurnBeginTime[3] = commfun_HexToBcd((uint8)tm_Time.tm_mday);          //日
    pTaskCfg->NextTurnBeginTime[2] = commfun_HexToBcd((uint8)tm_Time.tm_hour);          //时
    pTaskCfg->NextTurnBeginTime[1] = commfun_HexToBcd((uint8)tm_Time.tm_min);           //分
    pTaskCfg->NextTurnBeginTime[0] = commfun_HexToBcd((uint8)tm_Time.tm_sec);           //分

    LastTurnBeginTime_t = ThisTurnBeginTime_t;
    OOP_TI_T tmpTi;
    tmpTi.unit = pTaskCfg->RunCycleUnit;
    tmpTi.value = pTaskCfg->RunCycle;
    DT_TimeSubInv(&LastTurnBeginTime_t,tmpTi);//这个时间点已抄读过，推迟1秒
    ti.unit = pTaskCfg->DelayTimeUnit;
    ti.value = pTaskCfg->DelayTime;
    DT_TimeSubInv(&LastTurnBeginTime_t,ti);   //任务执行的延时时间和抄表无关，要减去

    LOCALTIME_R(&LastTurnBeginTime_t, &tm_Time);
    pTaskCfg->LastTurnBeginTime[5] = commfun_HexToBcd((uint8)(tm_Time.tm_year - 100));  //年
    pTaskCfg->LastTurnBeginTime[4] = commfun_HexToBcd((uint8)(tm_Time.tm_mon + 1));     //月
    pTaskCfg->LastTurnBeginTime[3] = commfun_HexToBcd((uint8)tm_Time.tm_mday);          //日
    pTaskCfg->LastTurnBeginTime[2] = commfun_HexToBcd((uint8)tm_Time.tm_hour);          //时
    pTaskCfg->LastTurnBeginTime[1] = commfun_HexToBcd((uint8)tm_Time.tm_min);           //分
    pTaskCfg->LastTurnBeginTime[0] = commfun_HexToBcd((uint8)tm_Time.tm_sec);           //分
    // UpdateTaskMonitor(pTaskCfg->TaskId,UDTASK_INIT,NULL);//TODO
    return TRUE;
}

/**
*********************************************************************
* @brief：      任务数据产生函数,该函数每次只处理一个任务，
               但一次将该任务处理完，不管该任务分多少帧
* @param[in]：  seq   帧序列号
* @param[out]：seq    帧序列号
* @return： void
*********************************************************************
*/
int rpt_task_execute(uint8 *seq)
{
    int ret = 0;
    static uint8 nextTaskID = 0;
    uint8 tmpTaskID = nextTaskID;

    if(reportTaskCfgList.OopTaskCfg[tmpTaskID].NeedToRun == TRUE)
    {
#ifdef AREA_SHANDONG
        if(reportTaskCfgList.OopTaskCfg[tmpTaskID].checkTaskFinish ==1)
        {
            reportTaskCfgList.OopTaskCfg[tmpTaskID].checkTaskFinish =0;
        }
#endif
        ret = rpt_task_data_package(&nextTaskID, seq);
        if(ret != 0)
        {
            RPT_FMT_DEBUG("内部索引-%d 任务-%d 方案-%d 报文获取失败.\n", tmpTaskID, reportTaskCfgList.OopTaskCfg[tmpTaskID].TaskId, reportList[tmpTaskID].plan.planID);
            reportTaskCfgList.OopTaskCfg[tmpTaskID].NeedToRun = FALSE;
        }
        else
        {
            //任务序号已增加，说明当前任务执行完了
            if(nextTaskID > tmpTaskID)
            {
                RPT_FMT_DEBUG("内部索引-%d 任务-%d 方案-%d 报文获取成功, 允许上报-%d.\n", tmpTaskID, reportTaskCfgList.OopTaskCfg[tmpTaskID].TaskId, reportList[tmpTaskID].plan.planID, g_tnmEventReport);
                reportTaskCfgList.OopTaskCfg[tmpTaskID].NeedToRun = FALSE;
            }
        }
    }
    else
    {
#ifdef AREA_SHANDONG //要等采集任务的，需要检查完采集任务再切下一个
        if(reportTaskCfgList.OopTaskCfg[tmpTaskID].checkTaskFinish ==1)
        {
            if(rpt_normal_task_finish_check(g_rptUdpHandle,reportTaskCfgList.OopTaskCfg[tmpTaskID].normalTaskId,&reportTaskCfgList.OopTaskCfg[tmpTaskID].lastCheckTime)==TRUE)
            {
                //任务抄完了 可以上报了
                ret = rpt_task_data_package(&nextTaskID, seq);
                if(ret != 0)
                {
                    RPT_FMT_DEBUG("即抄即报：内部索引-%d 任务-%d 方案-%d 报文获取失败.\n", tmpTaskID, reportTaskCfgList.OopTaskCfg[tmpTaskID].TaskId, reportList[tmpTaskID].plan.planID);
                }
                else
                {
                    //任务序号已增加，说明当前任务执行完了
                    if(nextTaskID > tmpTaskID)
                    {            
                        RPT_FMT_DEBUG("即抄即报：内部索引-%d 任务-%d 方案-%d 报文获取成功, 允许上报-%d.\n", tmpTaskID, reportTaskCfgList.OopTaskCfg[tmpTaskID].TaskId, reportList[tmpTaskID].plan.planID, g_tnmEventReport);
                        reportTaskCfgList.OopTaskCfg[tmpTaskID].NeedToRun = FALSE;
                        reportTaskCfgList.OopTaskCfg[tmpTaskID].checkTaskFinish = 2;
                    }
                }
            }
            else  //该任务不需要执行且未检测到采集完成
            {
                nextTaskID++;
            }
        }
        else  //该任务不需要执行且未检测到采集完成
        {
            nextTaskID++;
        }
#else
        nextTaskID++;
#endif
    }

    if(nextTaskID >= reportTaskCfgList.TaskNum)
    {
        nextTaskID = 0;
    }

    return ret;    
}

/**
*********************************************************************
* @brief：      任务数据产生函数,该函数每次只处理一个任务，
               但一次将该任务处理完，不管该任务分多少帧
* @param[in]：  seq   帧序列号
* @param[out]：seq    帧序列号
* @return： void
*********************************************************************
*/
void rpt_task_detect(void)
{
    int i = 0;
    BOOL isInNew;
    time_t RealTime_t = time(NULL);;
    static BOOL firstRunFlags = TRUE;

    //判断上报方案集任务是否达到执行条件,扫描出若干个需要执行的任务
    for(i = 0; i < reportTaskCfgList.TaskNum; i++)
    {
        //更新上报方案集任务执行时间
        UpdateTaskTime(&reportTaskCfgList.OopTaskCfg[i]);

        //判断本轮上报方案是否进入新一轮次的执行时间
        isInNew = JudgeIfInNewTurn(&reportTaskCfgList.OopTaskCfg[i],RealTime_t);
        if(isInNew)
        {
            reportTaskCfgList.OopTaskCfg[i].thisTurnReportFlag = TRUE;//本轮已经上报
            if(firstRunFlags == TRUE)//首次运行不执行上报方案
            {
                reportTaskCfgList.OopTaskCfg[i].NeedToRun = FALSE;
                reportTaskCfgList.OopTaskCfg[i].thisTurnReportFlag = TRUE;//本轮已经上报(第一轮不上报)
                continue;
            }
            reportTaskCfgList.OopTaskCfg[i].NeedToRun = TRUE;
            
            RPT_FMT_DEBUG("内部索引-%d 任务-%d 方案-%d 需要执行上报.\n", i, reportTaskCfgList.OopTaskCfg[i].TaskId, reportList[i].plan.planID);
        }
    }
    
    firstRunFlags = FALSE;
}

#endif

#if DESC("任务上报函数",1)

/**
*********************************************************************
* @brief： 检查主站是否响应上报消息
* @param[in]： piid   服务序号与优先级
* @param[out]：NA
* @return： void
*********************************************************************
*/
void rpt_task_res_check(uint8 piid)
{
    if(ConfirmFlag != RPT_RES_ALREADY)
    {
        if(!RPT_TASK_IS_MY_RES(piid)) //帧序列号不一致，不是对此帧的确认
        {
            return;
        }
        
        ConfirmFlag = RPT_RES_ALREADY;        //已收到确认消息
        RPT_FMT_TRACE("rpt task already response.\n");
    }  

    return;
}

/**
*********************************************************************
* @brief：读取文件中上报记录并加工成可以上报的报文
* @param[in]： piid   服务序号与优先级
* @param[out]：NA
* @return： void
*********************************************************************
*/
int rpt_task_record_package(RPT_CFG_T *rptCfg, uint16 rptCnt, RPT_RESULT_T *rptResult)
{
    int ret = 0;
    AUTO_DATA read_data;               //读取到的任务数据
    uint32 offset = RPT_RECORD_DATA_OFFSET(rptCnt);
    uint16 len = sizeof(AUTO_DATA);

    memset_s(&read_data, len, 0x0, len);

    //读取任务数据失败的话，将计数器加一，下次读取下一条记录
    ret = read_pdata_xram(rptCfg->filename, (char*)&read_data, offset, len);
    if((ret != 0) || (read_data.bLen > rptResult->bufLen) || (read_data.bLen == 0))
    {
        RPT_FMT_DEBUG("rptTask->[%d]dequeue: %s read failed. ret(%d), offset(%d), needlen(%d), readlen(%d), buflen(%d)\n", rptCnt, rptCfg->filename, ret, offset, len, read_data.bLen, rptResult->bufLen);
        write_pdata_xram(rptCfg->filename, (char *)&rptCnt, RPT_REPORT_CNT_OFFSET, 2);
        return ret;
    }

    RPT_BUF_TRACE(read_data.bData, read_data.bLen, "[%d]dequeue: %s read. offset(%d), len(%d)\n", rptCnt, rptCfg->filename, offset, len);

    *rptCfg->piid = read_data.piid;
    if(read_data.bLen > 0)
    {
        *rptCfg->resStatus = RPT_RES_WAITING;   //确认标志
        outOfTime = read_data.timeOut;   //超时时间
        retryCunt  = read_data.retryTime;//重试次数
        rptResult->taskIndex = read_data.taskIndex;
        memcpy_s(rptResult->pOutBuf, read_data.bLen, read_data.bData, read_data.bLen);       //传递报文
    }

    rptResult->msgLen = read_data.bLen;

    return ret;
}

#endif


#if DESC("事件上报函数",1)

/**
*********************************************************************
* @brief：      写一条电能表事件记录
* @param[in]： save_data  要存储的数据
* @param[out]：NA
* @return： void
*********************************************************************
*/
int rpt_mtr_evt_record_write(RPT_MTREVT_INFO *save_data)
{
    uint16 recordcount;     //记录的总条数
    int ret = 0;
    uint32 offset = 0;
    uint16 len = sizeof(RPT_MTREVT_INFO);

    //读出目前记录总条数
    ret = read_pdata_xram(RPT_MTREVT_REC_FILE, (char *)&recordcount, RPT_RECORD_CNT_OFFSET, 2);
    if( (ret != 0) || (recordcount == 0xffff))
    {
        RPT_FMT_DEBUG("%s read. ret(%d), recCnt(%d).\n", RPT_MTREVT_REC_FILE, ret, recordcount);
        recordcount = 0;
    }

    recordcount = RPT_ADD_VALUE_CLEAR(recordcount, RPT_MTREVT_REC_MAX, 1);

    offset = RPT_MTREVT_REC_OFFSET(recordcount);

    //写入任务数据
    ret = write_pdata_xram(RPT_MTREVT_REC_FILE, (char *)save_data,  offset, len);
    if(ret >= 0)
    {    //写入记录总条数
        ret = write_pdata_xram(RPT_MTREVT_REC_FILE, (char *)&recordcount, RPT_RECORD_CNT_OFFSET, 2);
        RPT_BUF_DEBUG(save_data->pdataBuf, save_data->dataLen, "[%d]enqueue: %s write. offset(%d), len(%d)\n", recordcount, RPT_MTREVT_REC_FILE,  offset, len);
    }
    else
    {
        RPT_BUF_DEBUG(save_data->pdataBuf, save_data->dataLen, "[%d]enqueue: %s write failed. offset(%d), len(%d)\n", recordcount, RPT_MTREVT_REC_FILE, offset, len);
    }

    return ret;
}

/**
*********************************************************************
* @brief： 检查主站是否响应上报消息
* @param[in]： piid   服务序号与优先级
* @param[out]：NA
* @return： void
*********************************************************************
*/
void rpt_event_res_check(uint8 piid)
{
    if(ConfirmFlag_event != RPT_RES_ALREADY)
    {
        if(!RPT_EVENT_IS_MY_RES(piid)) //帧序列号不一致，不是对此帧的确认
        {
            return;
        }
        
        ConfirmFlag_event = RPT_RES_ALREADY;        //已收到确认消息
        RPT_FMT_TRACE("rpt event already response.\n");
    
        //收到确认帧，回写事件记录中的通道上报状态
    }

    return;
}

/**
*********************************************************************
* @brief： 检查主站是否响应上报消息
* @param[in]： piid   服务序号与优先级
* @param[out]：NA
* @return： void
*********************************************************************
*/
void rpt_mtr_evt_res_check(uint8 piid)
{
    if(ConfirmFlag_event_met != RPT_RES_ALREADY)
    {
        if(!RPT_MTREVT_IS_MY_RES(piid)) //帧序列号不一致，不是对此帧的确认
        {
            return;
        }
        
        ConfirmFlag_event_met = RPT_RES_ALREADY;        //已收到确认消息
        RPT_FMT_TRACE("rpt mtrevt already response.\n");
    
        //收到确认帧，回写事件记录中的通道上报状态
    }

    return;
}

/**
*********************************************************************
* @brief： 检查主站是否响应上报消息
* @param[in]： piid   服务序号与优先级
* @param[out]：NA
* @return： void
*********************************************************************
*/
void rpt_retry_res_check(uint8 piid)
{
    if(ConfirmFlag_retry != RPT_RES_ALREADY)
    {
        if(!RPT_RETRY_IS_MY_RES(piid)) //帧序列号不一致，不是对此帧的确认
        {
            return;
        }
        
        ConfirmFlag_retry = RPT_RES_ALREADY;        //已收到确认消息
        RPT_FMT_TRACE("rpt retry already response.\n");
    
        //收到确认帧，回写事件记录中的通道上报状态
    }

    return;
}

/**
*********************************************************************
* @brief：读取文件中上报记录并加工成可以上报的报文
* @param[in]： piid   服务序号与优先级
* @param[out]：NA
* @return： void
*********************************************************************
*/
int rpt_event_record_package(RPT_CFG_T *rptCfg, uint16 rptCnt, RPT_RESULT_T *rptResult)
{
    int ret = 0;
    EVENT_REPORT_INFO read_data;               //读取到的任务数据
    uint32 offset = RPT_EVENT_RECORD_OFFSET(rptCnt);
    uint16 len = sizeof(EVENT_REPORT_INFO);

    memset_s(&read_data, len, 0x0, len);

    //读取任务数据失败的话，将计数器加一，下次读取下一条记录
    ret = read_pdata_xram(rptCfg->filename, (char*)&read_data, offset, len);
    if((ret != 0) || (read_data.dataLen > rptResult->bufLen) || (read_data.dataLen == 0)) 
    {
        RPT_FMT_DEBUG("rptEvent->[%d]dequeue: %s read failed. ret(%d), offset(%d), needlen(%d), readlen(%d), buflen(%d)\n", rptCnt, rptCfg->filename, ret, offset, len, read_data.dataLen, rptResult->bufLen);
        write_pdata_xram(rptCfg->filename, (char *)&rptCnt, RPT_REPORT_CNT_OFFSET, 2);
        return ret;
    }

    RPT_BUF_TRACE(read_data.pdataBuf, read_data.dataLen, "[%d]dequeue: %s read. offset(%d), len(%d)\n", rptCnt, rptCfg->filename, offset, len);

    if(read_data.dataLen > 0)
    {
        
        *rptCfg->resStatus = RPT_RES_WAITING;   //确认标志

        //非重发，增加piid
        if(rptCfg->retryTag != TRUE)
        {
            *rptCfg->piid = RPT_ADD_VALUE_CLEAR(*rptCfg->piid, 63, 1);
        }

        rptResult->msgLen = rpt_packet_make(read_data.pdataBuf, read_data.dataLen, 1, read_data.cAddr, *rptCfg->piid, rptResult->pOutBuf, rptResult->bufLen);

        //将报文拷贝到重发缓冲区，超时重发时使用
        /*ResendBufferLenth = rptResult->msgLen;
        memset_s(ResendBuffer, sizeof(ResendBuffer), 0, sizeof(ResendBuffer));
        memcpy_s(ResendBuffer, ResendBufferLenth, rptResult->pOutBuf, ResendBufferLenth);*/
    }

    return ret;
}

/**
*********************************************************************
* @brief：读取文件中上报记录并加工成可以上报的报文
* @param[in]： piid   服务序号与优先级
* @param[out]：NA
* @return： void
*********************************************************************
*/
int rpt_mtr_evt_record_package(RPT_CFG_T *rptCfg, uint16 rptCnt, RPT_RESULT_T *rptResult)
{
    int ret = 0;
    RPT_MTREVT_INFO read_data;               //读取到的任务数据
    uint32 offset = RPT_MTREVT_REC_OFFSET(rptCnt);
    uint16 len = sizeof(RPT_MTREVT_INFO);

    memset_s(&read_data, len, 0x0, len);

    //读取任务数据失败的话，将计数器加一，下次读取下一条记录
    ret = read_pdata_xram(rptCfg->filename, (char*)&read_data, offset, len);
    if((ret != 0) || (read_data.dataLen > rptResult->bufLen) || (read_data.dataLen == 0)) 
    {
        RPT_FMT_DEBUG("rptMtre->[%d]dequeue: %s read failed. ret(%d), offset(%d), needlen(%d), readlen(%d), buflen(%d)\n", rptCnt, rptCfg->filename, ret, offset, len, read_data.dataLen, rptResult->bufLen);
        write_pdata_xram(rptCfg->filename, (char *)&rptCnt, RPT_REPORT_CNT_OFFSET, 2);
        return ret;
    }

    RPT_BUF_TRACE(read_data.pdataBuf, read_data.dataLen, "[%d]dequeue: %s read. offset(%d), len(%d)\n", rptCnt, rptCfg->filename, offset, len);

    if(read_data.dataLen > 0)
    {
        *rptCfg->resStatus = RPT_RES_WAITING;   //确认标志

        //非重发，增加piid
        if(rptCfg->retryTag != TRUE)
        {
            *rptCfg->piid = RPT_ADD_VALUE_CLEAR(*rptCfg->piid, 63, 1);
        }

        //rptResult->msgLen = rpt_packet_make(read_data.pdataBuf, read_data.dataLen, read_data.rptType, 0, *rptCfg->piid, rptResult->pOutBuf, rptResult->bufLen);
        //电表事件改为明文上报
        rptResult->msgLen = rpt_packet_make_plaintext(read_data.pdataBuf, read_data.dataLen, read_data.rptType, 0, *rptCfg->piid, rptResult->pOutBuf, rptResult->bufLen);
        //将报文拷贝到重发缓冲区，超时重发时使用
        /*ResendBufferLenth = rptResult->msgLen;
        memset_s(ResendBuffer, sizeof(ResendBuffer), 0, sizeof(ResendBuffer));
        memcpy_s(ResendBuffer, ResendBufferLenth, rptResult->pOutBuf, ResendBufferLenth);*/
        
    }

    return ret;
}

/**
*********************************************************************
* @brief：     全事件数据变更初始化
* @param[in]： NA
* @param[out]：NA
* @return：     -1 执行错误
*********************************************************************
*/
void rpt_mtr_evt_init()
{
    pthread_mutex_init(&g_evtMetmutex, NULL);
}

/**
*********************************************************************
* @brief:       电能表事件上报入口(通知方式上报)
* @detail:      两种方法：1-通知写队列，然后复用现有事件上报 2-更新通知中直接上报
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int rpt_mtr_evt_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int ret = 0;
    RPT_MTREVT_INFO readData;
    NOMAL_OAD_T normal;
    CLEAR_DATA_T clearData;

    MEMZERO(&readData, sizeof(readData));
    MEMZERO(&normal, sizeof(normal));
    MEMZERO(&clearData, sizeof(clearData));

    RPT_FMT_DEBUG("rpt_met_event_update logicId(%d) infoNum(%d) value(0x%08x) running.....\n", logicId, infoNum, value);

    if(!GetTnmEventReport())
    {
        return -1;
    }
    //读出数据中心更新的事件记录
    switch(value)
    {
        case 0xFF100A00: readData.rptType = 1; break;
        case 0xFF100B00: readData.rptType = 2; break;
        case 0xFF100C00: readData.rptType = 1; break;
        case 0xFF100D00: readData.rptType = 6; break;
        default: return ret;
    }

    normal.infoNum = infoNum;
    normal.oad.value = value;
     
    ret = db_read_nomal(clientid, &normal, RPT_MTREVT_RECBUF_MAX, readData.pdataBuf, &readData.dataLen);
    if(ret != 0)
    {
        RPT_FMT_DEBUG("read mtrevt failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ret;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_evtMetmutex);

    //将读出的记录存入上报队列
    ret = rpt_mtr_evt_record_write(&readData);
    if(ret < 0)
    {
         RPT_BUF_DEBUG(readData.pdataBuf, readData.dataLen, "rpt_mtrevt_record_write failed. ret(%d), rptType(%d), oad(0x%08x)\n",
            ret, readData.rptType, value);
        pthread_mutex_unlock(&g_evtMetmutex);   
        return ret;       
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_evtMetmutex);   

    //读出后清掉数据中心的记录
    clearData.recordOAD.infoNum = infoNum;
    clearData.recordOAD.road.oadCols.nNum = 1;
    clearData.recordOAD.road.oadCols.oad[0].value = value;
    
    ret = db_clear_oad_data(clientid, &clearData);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("clear mtrevt failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, clearData.recordOAD.logicId, clearData.recordOAD.infoNum, clearData.recordOAD.road.oadCols.oad[0].value);
        return ret;
    }
    
    return ret;
} 

/**
*********************************************************************
* @brief:       把最近的事件写入数据上报队列
* @detail:      获取到事件更新则检查上报标记，上报标记使能则写入上报队列
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int rpt_curr_event_write(int clientid, uint8 logicId, uint16 infoNum, uint32 oadEvent, uint8 rptFlag)
{
    int                 ret         = 0;
    READ_RECORD_T       startReadRecord;
    RECORD_UNIT_T       stepRecordOut;
    EVENT_COMM_INFO     eventCommInfo;

    memset_s(&startReadRecord, sizeof(READ_RECORD_T), 0, sizeof(READ_RECORD_T));
    memset_s(&stepRecordOut, sizeof(RECORD_UNIT_T), 0, sizeof(RECORD_UNIT_T));
    memset_s(&eventCommInfo, sizeof(eventCommInfo), 0, sizeof(EVENT_COMM_INFO));
    
    startReadRecord.recordOAD.road.oadMain.value = 0;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.road.oadCols.oad[0].value= oadEvent;
    startReadRecord.recordOAD.logicId = logicId;
    startReadRecord.recordOAD.infoNum = infoNum;
    startReadRecord.MAC.nNum = 0;
    startReadRecord.cType = 0x02;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = 0xffffffff;
    
    ret = db_read_record(clientid,&startReadRecord,DATA_LEN_MAX,&stepRecordOut);
    if(ret != 0)
    {
        RPT_FMT_DEBUG("db_read_record faild, oad(0x%08x).ret %d \n", oadEvent,ret);
        return ret;
    }

    //TODO 判断发生还是恢复事件
    
    eventCommInfo.eventOAD.value = oadEvent;
    ret = evt_default_rcsd_get(&eventCommInfo.eventOAD, &eventCommInfo.eventRCSD);
    if(ret != 0)
    {
        RPT_FMT_DEBUG("evt_default_rcsd_get faild, oad(0x%08x).\n", oadEvent);
        return ret;
    }

   return WriteEventReportData(EVENT_RECORD_FILE, &eventCommInfo, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
}

/**
*********************************************************************
* @brief:       终端事件上报入口(通知方式上报)
* @detail:      获取到事件更新则检查上报标记，上报标记使能则写入上报队列
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int rpt_term_evt_update(int clientid, uint8 logicId, uint16 infoNum, uint32 oadMain, uint8 oadColsNum, uint32* oadCols)
{
    int ret = 0;
    RPT_MTREVT_INFO readData;
    NOMAL_OAD_T normal;
    CLEAR_DATA_T clearData;
    uint8 reportFlag = 0;
    uint32 len = 0;
    uint32 evtOAD = oadCols[0];

    MEMZERO(&readData, sizeof(readData));
    MEMZERO(&normal, sizeof(normal));
    MEMZERO(&clearData, sizeof(clearData));
        
    RPT_FMT_TRACE("rpt_term_evt_update logicId(%d) infoNum(%d) value(0x%08x) evtoad %08x running.....\n", logicId, infoNum, oadMain,evtOAD);

    if(!GetTnmEventReport())
    {
    	RPT_FMT_DEBUG("rpt_term_evt_update logicId(%d) infoNum(%d) evtoad %08x GetTnmEventReport(0) return.....\n", logicId, infoNum, evtOAD);
        return -1;
    }

    normal.logicId = logicId;
    normal.infoNum = infoNum;
    normal.oad.value = evtOAD;

    normal.oad.attID = 8; 

    //读取事件上报标记
    ret = db_read_nomal(clientid, &normal, sizeof(uint8), &reportFlag, &len);
    if((ret != 0) || (len != sizeof(uint8)))
    {
        RPT_FMT_DEBUG("read term failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ret;
    }

	RPT_FMT_DEBUG("rpt_term_evt_update logicId(%d) infoNum(%d) evtoad %08x reportFlag(%d)running.....\n", logicId, infoNum, evtOAD, reportFlag);
	
    //需要上报
    if(reportFlag != 0)
    {
        //读出事件，判断是发生还是恢复
        rpt_curr_event_write(clientid, logicId, infoNum, evtOAD, reportFlag);
    }
    
    return ret;
} 

//int creat_refuseslave_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
//{
//    uint8 i=0;
//    uint8 index=0;
//    OOP_REFUSESLAVE_T refuseslave;
//    OOP_OCTETSTR6_T sa;
//    WRITE_RECORD_T      recInData;
//    READ_RECORD_T       startReadRecord;
//    RECORD_UNIT_T       stepRecordOut;
//    UPDATE_RECORD_T     updateRecordData;
//    time_t              timep   = 0;
//    uint32 storeNo=0;
//
//    if(value == 0xF2090C00)
//    {
//        int ret = 0;
//        NOMAL_OAD_T nomal = {0};
//        uint32 len = 0;
//
//        MEMZERO(&sa, sizeof(sa));
//        MEMZERO(&recInData, sizeof(recInData));
//        MEMZERO(&startReadRecord, sizeof(startReadRecord));
//        MEMZERO(&stepRecordOut, sizeof(stepRecordOut));
//        MEMZERO(&updateRecordData, sizeof(updateRecordData));
//        MEMZERO(&refuseslave, sizeof(refuseslave));
//        
//        nomal.oad.value = 0xF2090C00;
//
//        ret = db_read_nomal(clientid, &nomal, sizeof(sa), (uint8*)&sa, &len);
//        if(ret != 0 )
//        {
//            STAT_FMT_TRACE("tmn_port_authorize read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
//                ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(acrossarer));
//            return ret;
//        }
//
//        startReadRecord.recordOAD.optional = 0;
//        startReadRecord.recordOAD.road.oadCols.nNum = 1;
//        startReadRecord.recordOAD.road.oadCols.oad[0].value= 0x31110200;
//        startReadRecord.recordOAD.logicId = 0;
//        startReadRecord.recordOAD.infoNum = 0;
//        startReadRecord.cType = COL_TM_STORE;
//        startReadRecord.cStart = 0;
//        startReadRecord.cEnd = time(NULL);
//        startReadRecord.sortType = 1;
//        startReadRecord.MAC = MAC;
//
//        ret=db_read_record(clientid,&startReadRecord,DATA_LEN_MAX,&stepRecordOut);
//        if(ret !=0)
//        {
//            time(&timep); /*获取time_t类型的当前时间*/
//            recInData.recordOAD.classtag = CLASS_DATA_EVENT;
//            recInData.recordOAD.road.oadCols.nNum = 1;
//            recInData.recordOAD.road.oadCols.oad[0] = 0x31110200;
//            recInData.MAC = MAC;
//            recInData.colStartTm = timep;
//            recInData.colEndTm = timep;
//            recInData.colStoreTm = 1;
//
//            refuseslave.nIndex=1;
//            refuseslave.tmStart= searchmet.serchTime;
//            refuseslave.tmEnd = searchmet.serchTime;
//            smlog.evtState.nNum = g_tnmPort.nNum;
//            for(i=0; i<g_tnmPort.nNum; i++)
//            {
//                smlog.evtState.state[i].oad = g_tnmPort.port[i];
//                smlog.evtState.state[i].state = 0;
//            }
//            smlog.source.choice=DT_NULL;
//            smlog.nResNum=1;
//            smlog.Res[0]=searchmet;
//
//            memcpy_s(recInData.inDataBuf, sizeof(OOP_UNMETERLOG_T), (uint8*)&smlog, sizeof(OOP_UNMETERLOG_T));
//            
//            ret = db_write_record(clientid, &recInData, &storeNo);
//            if (ret != ERR_OK)
//            {
//                return ret;
//            }
//        }
//        else
//        {
//            memcpy_s((uint8*)&smlog, stepRecordOut.record[0].outDatalen, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen)
//            if(smlog.nResNum>=OOP_MAX_SMREPORT)
//            {
//                index=smlog.nIndex;
//                MEMZERO(&smlog, sizeof(smlog));
//                time(&timep); /*获取time_t类型的当前时间*/
//                recInData.recordOAD.classtag = CLASS_DATA_EVENT;
//                recInData.recordOAD.road.oadCols.nNum = 1;
//                recInData.recordOAD.road.oadCols.oad[0] = 0x31120200;
//                recInData.MAC = MAC;
//                recInData.colStartTm = timep;
//                recInData.colEndTm = timep;
//                recInData.colStoreTm = index+1;
//
//                smlog.nIndex=index+1;
//                smlog.tmStart= searchmet.serchTime;
//                smlog.tmEnd = searchmet.serchTime;
//                smlog.evtState.nNum = g_tnmPort.nNum;
//                for(i=0; i<g_tnmPort.nNum; i++)
//                {
//                    smlog.evtState.state[i].oad = g_tnmPort.port[i];
//                    smlog.evtState.state[i].state = 0;
//                }
//                smlog.source.choice=DT_NULL;
//                smlog.nResNum=1;
//                smlog.Res[0]=searchmet;
//
//                memcpy_s(recInData.inDataBuf, sizeof(OOP_UNMETERLOG_T), (uint8*)&smlog, sizeof(OOP_UNMETERLOG_T));
//                
//                ret = db_write_record(clientid, &recInData, &storeNo);
//                if (ret != ERR_OK)
//                {
//                    return ret;
//                }
//
//            }
//            else
//            {
//                updateRecordData.recordOAD.optional = 0;
//                updateRecordData.recordOAD.road.oadCols.nNum = 1;
//                updateRecordData.recordOAD.road.oadCols.oad[0] = 0x31110200;
//                updateRecordData.MAC = MAC;
//                updateRecordData.filter.choice = 1;
//                updateRecordData.filter.storeTm = smlog.nIndex;
//
//
//                smlog.tmEnd = searchmet.serchTime;
//                smlog.nResNum++;
//                smlog.Res[smlog.nResNum-1]=searchmet;
//                ret = db_update_record(clientid, &updateRecordData);
//                if (ret != ERR_OK)
//                {
//                    return ret;
//                }
//                
//            }
//
//        }
//
//    }
//
//    return 0;
//}

/**
*********************************************************************
* @brief:       电能表事件上报入口(通知方式上报)
* @detail:      两种方法：1-通知写队列，然后复用现有事件上报 2-更新通知中直接上报
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int rpt_trans_result_update(int clientid, uint8 logicId, uint16 infoNum, uint32 oadMain, uint8 oadColsNum, uint32* oadCols)
{
    int                 ret         = 0;
    int                 i           = 0;
 //   int                 j           = 0;
//    int                 offset      = 0;
//    time_t              timep       = 0;
    READ_RECORD_T       startReadRecord;
    RECORD_UNIT_T       stepRecordOut;
    OOP_METER_T         oopMeter;
//    uint32 stepHandle = 0;  //单步读句柄，最多4轮  
//    uint32 recNum     = 0;  //记录条数 
//    uint32 outRecNumAll = 0;  //总的要输出的记录条数
//    uint32 ctrlRecNum = 0xffffffff;
//    STEP_READ_RECORD_T stepRead;
//    RECORD_UNIT_T      stepOut;
    OOP_TRANSRES_T TransResult;
    uint32 colStartTm;
    uint32 colEndTm;


    uint8 planid=0;
    OOP_PLAN_TRANS_T PLAN_TRANS_T;

    if((oadMain & 0xFFFFFF00) !=0x601A0200)
    {
        return -1;
    }
    planid=oadMain&0xFF;
    PRTL_FMT_LOGCC("rpt_trans_result_update888888,planid is %d,oadMain is %d\n",planid,oadMain);
    
    ret=app_get_plan_transparent_list(planid,&PLAN_TRANS_T);
    if(ret !=0)
    {
        PRTL_FMT_LOGCC("rpt_trans_result_update777777,planid is %d\n",planid);
        return -1;
    }
    if(PLAN_TRANS_T.value.info[0].flag.bReport==FALSE)
    {
        PRTL_FMT_LOGCC("rpt_trans_result_update666666,planid is %d\n",planid);
        return -1;
    }
    memset_s(&oopMeter, sizeof(OOP_METER_T), 0, sizeof(OOP_METER_T));
    memset_s(&startReadRecord, sizeof(READ_RECORD_T), 0, sizeof(READ_RECORD_T));
    memset_s(&stepRecordOut, sizeof(RECORD_UNIT_T), 0, sizeof(RECORD_UNIT_T));

    //time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep
    startReadRecord.recordOAD.road.oadMain.value = oadMain;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.road.oadCols.oad[0].value= *oadCols;
    startReadRecord.recordOAD.logicId = logicId;
    startReadRecord.recordOAD.infoNum = infoNum;
    startReadRecord.cType = COL_NO_INFO;
    startReadRecord.cStart = infoNum;
    startReadRecord.cEnd = infoNum;
    startReadRecord.sortType = 0;
    startReadRecord.MAC.nNum = 6;

    PRTL_FMT_LOGCC("rpt_trans_result_update123456,infoNum is %d,oadMain is %x,logicId is %d\n",infoNum,oadMain,logicId);

      /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&oopMeterList.mutex);

     for (i = 0; i < oopMeterList.oopMeterNum; i++)
    {
        if (oopMeterList.oopMeter[i].nIndex == infoNum)
        {
            oopMeter = oopMeterList.oopMeter[i];
            ret = 1;
            break;
        }
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&oopMeterList.mutex);

    PRTL_BUF_LOGCC(oopMeter.basic.tsa.add,6,"MAC地址11");

 
    
    memcpy_s(startReadRecord.MAC.value, 6, oopMeter.basic.tsa.add, 6);
    if(ret)
    {
           ret=db_read_record(clientid,&startReadRecord,DATA_LEN_MAX,&stepRecordOut);
           if(ret !=0)
           {
                return -1;
           }

           PRTL_BUF_LOGCC((uint8*)stepRecordOut.record[0].outDataBuf,stepRecordOut.record[0].outDatalen,"透明方案上报报文");

           TransResult.planID = stepRecordOut.record[0].road.oadMain.nIndex;
            TransResult.res.result[0].data.nNum = stepRecordOut.record[0].outDatalen;
            memcpy(TransResult.res.result[0].data.value,stepRecordOut.record[0].outDataBuf,stepRecordOut.record[0].outDatalen);
            TransResult.res.result[0].nIndex = (stepRecordOut.record[0].road.oadCols.oad[0].value);
            TransResult.res.nNum = 1;
            app_get_meter_list(stepRecordOut.infoNum,&oopMeter);
            TransResult.addr = oopMeter.basic.tsa;
            colStartTm = stepRecordOut.colStartTm - 8*60*60;
            DT_Time2DateTimeShort(colStartTm,&TransResult.eTime);
            colEndTm = stepRecordOut.colEndTm - 8*60*60;
            DT_Time2DateTimeShort(colEndTm,&TransResult.res.result[0].aTime);
           
           ret=ProxyWriteQueue(&TransQueue,(uint8*)&TransResult,sizeof(TransResult));
           if (ret != 0x01)
           {
                        
               return -1;
           } 
    }

    return 0;
} 
#endif


#if DESC("上报记录发送功能",1)
/**
*********************************************************************
* @brief:       获取重试的参数
* @param[in]：   rptCfg   上报信息
* @param[out]： timeOut  超时时间
* @param[out]： retryCnt 重试次数
* @return：
*********************************************************************
*/
int rpt_retry_info_read(RPT_CFG_T *rptCfg, RPT_RETRY_T *retry)
{
    //任务上报的超时重试参数来源于任务，事件上报的参数来源于以太网的参数451x
    if(rptCfg->type == RPT_TYPE_TASK)
    {
        retry->timeOut = outOfTime;
        retry->retryCnt = retryCunt;
    }
    else if((rptCfg->type == RPT_TYPE_EVENT)||(rptCfg->type == RPT_TYPE_MTREVT))
    {
        //*timeOut = outOfTime_event;
        //*retryCnt = retryCunt;
        retry->timeOut = 11;//不要改 适配跑分台体
        retry->retryCnt = 6;
    }

    return 0;
}

/**
*********************************************************************
* @brief：读取文件中上报记录并加工成可以上报的报文
* @param[in]： piid   服务序号与优先级
* @param[out]：NA
* @return： void
*********************************************************************
*/
int rpt_record_package(RPT_CFG_T *rptCfg, uint16 rptCnt, RPT_RESULT_T *rptResult)
{
    if(rptCfg->type == RPT_TYPE_TASK)
    {
        return rpt_task_record_package(rptCfg, rptCnt, rptResult);
    }
    else if(rptCfg->type == RPT_TYPE_EVENT)
    {
        return rpt_event_record_package(rptCfg, rptCnt, rptResult);        
    }
    else if(rptCfg->type == RPT_TYPE_MTREVT)
    {
        return rpt_mtr_evt_record_package(rptCfg, rptCnt, rptResult);        
    }

    return ERR_O_RANGE;
}

/**
*********************************************************************
* @brief： 读取任务记录总条数和上报条数
* @param[out]：DataCount    记录条数
* @param[out]：ReportCount 已上报条数
* @return： void
*********************************************************************
*/
int rpt_count_read(RPT_CFG_T *rptCfg, uint16 *DataCount, uint16 *ReportCount)
{
    int ret = 0;
    uint8 tmpBuf[4];
    uint16 tmpDataCount = 0;        //任务记录总条数
    uint16 tmpReportCount = 0;      //已经上报的条数

    memset_s(tmpBuf, sizeof(tmpBuf), 0x0, sizeof(tmpBuf));
    
    //读出目前记录总条数和已上报的数量
    ret = read_pdata_xram(rptCfg->filename, (char *)tmpBuf, RPT_RECORD_CNT_OFFSET, 4);
    if(ret < 0)
    {
        tmpDataCount = 0xffff;
        tmpReportCount = 0xffff;
    }
    else
    {
        tmpDataCount = *((uint16 *)tmpBuf);
        tmpReportCount = *((uint16 *)(tmpBuf + 2));
    }
    
    if(tmpDataCount > rptCfg->maxRecord)       //判断任务记录总条数是否越限
    {
        RPT_FMT_LOGCC("type[%d]: queue is full. recCnt(%d) more than mamRecord(%d), will be set to zero.\n", rptCfg->type, tmpDataCount, rptCfg->maxRecord);
        tmpDataCount = 0;
        write_pdata_xram(rptCfg->filename, (char *)&tmpDataCount, RPT_RECORD_CNT_OFFSET, 2);
    }

    if(tmpReportCount > rptCfg->maxRecord)     //判断已经上报的条数是否越限
    {  
        RPT_FMT_LOGCC("type[%d]: queue is full. rptCnt(%d) more than mamRecord(%d), will be set to zero.\n", rptCfg->type, tmpReportCount, rptCfg->maxRecord);
        tmpReportCount = 0;
        write_pdata_xram(rptCfg->filename, (char *)&tmpReportCount, RPT_REPORT_CNT_OFFSET, 2);
    }

    *DataCount = tmpDataCount;
    *ReportCount = tmpReportCount;
       
    return 1;
}

/**
*********************************************************************
* @brief： 检查主站是否响应上报消息
* @param[in]： piid   服务序号与优先级
* @param[out]：NA
* @return： void
*********************************************************************
*/
void rpt_response_check(RPT_TYPE_E type, uint8 piid)
{
    if(type == RPT_TYPE_TASK)
    {
        rpt_task_res_check(piid);
    }
    else if(type == RPT_TYPE_EVENT)
    {
        rpt_event_res_check(piid);
    }
    else if(type == RPT_TYPE_MTREVT)
    {
        rpt_mtr_evt_res_check(piid);
    }
    else if(type == RPT_TYPE_RETRY)
    {
        rpt_retry_res_check(piid);
    }

    return;
}

/**
*********************************************************************
* @brief： 分析上报响应
* @param[in]： NA
* @param[out]：result 处理结果
* @return： void
*********************************************************************
*/
void rpt_response_analyse(RPT_CFG_T *rptCfg, RPT_RETRY_T *baseRetry, RPT_RETRY_T *curRetry, RPT_RES_PROC_E *pResult)
{
    char *typeStr[3] = {"任务上报", "终端事件上报", "电表事件上报"};

    if(*rptCfg->resStatus == RPT_RES_ALREADY)
    {
        curRetry->timeOut = 0;
        curRetry->retryCnt = 0;
        *pResult = RPT_RES_PORC_NEXT;  

        return;
    }

    //curRetry->timeOut += (rptCfg->curTime - rptCfg->lstTime);

    if(rptCfg->curTime != rptCfg->lstTime)
    {
        curRetry->timeOut++;
    }

    /* 超时重发,直到超过最大重发次数，过秒执行,计数1次即为1秒 */
    if(curRetry->timeOut > baseRetry->timeOut)  
    {
        RPT_FMT_DEBUG("%s 服务序号%d 超时: 已等待%d秒, 最多等待%d秒.\n", typeStr[rptCfg->type], *rptCfg->piid, curRetry->timeOut, baseRetry->timeOut);

        curRetry->timeOut = 0;
      
        if(++curRetry->retryCnt >= baseRetry->retryCnt)
        {
            RPT_FMT_DEBUG("%s 服务序号%d 重试完成.\n", typeStr[rptCfg->type], *rptCfg->piid);
            *rptCfg->resStatus = RPT_RES_ALREADY; 
            curRetry->timeOut = 0; 
            curRetry->retryCnt = 0;
            *pResult = RPT_RES_PORC_NEXT;
        }
        else
        {
            RPT_FMT_DEBUG("%s 服务序号%d 正在重试: 第%d次, 应小于%d次.\n", typeStr[rptCfg->type], *rptCfg->piid, curRetry->retryCnt, baseRetry->retryCnt);
            *pResult = RPT_RES_PORC_RETRY;  
        }
    }
    else
    {
        *pResult = RPT_RES_PORC_WAITING;
    }
    
    return;
}

/**
*********************************************************************
* @brief： 对上报响应消息的处理
* @param[in]： NA
* @param[out]：result 处理结果
* @return： void
*********************************************************************
*/
void rpt_response_proc(RPT_CFG_T *rptCfg, RPT_RES_PROC_E *pResult)
{
    RPT_RETRY_T baseRetry = {0};
    static RPT_RETRY_T taskCurRetry = {0};
    static RPT_RETRY_T eventCurRetry = {0};
    static RPT_RETRY_T mtrEvtCurRetry = {0};

    if(*rptCfg->resStatus == RPT_RES_WAITING)
    {
        //获取超时时间和重试次数
        rpt_retry_info_read(rptCfg, &baseRetry);
    }
    else if (*rptCfg->resStatus == RPT_RES_NONEED)  //不等待主站应答
    {
        baseRetry.retryCnt = 0;
        baseRetry.timeOut = RPT_TASK_MIN_TIMEOUT;
    }
    else if (*rptCfg->resStatus == RPT_RES_INITIAL) //尚未上报第一条
    {
        *pResult = RPT_RES_PORC_RETRY;
        return;
    }

    if(rptCfg->type == RPT_TYPE_TASK)
    {
        return rpt_response_analyse(rptCfg, &baseRetry, &taskCurRetry, pResult);
    }
    else if(rptCfg->type == RPT_TYPE_EVENT)
    {
        return rpt_response_analyse(rptCfg, &baseRetry, &eventCurRetry, pResult);
    }     
    else if(rptCfg->type == RPT_TYPE_MTREVT)
    {
        return rpt_response_analyse(rptCfg, &baseRetry, &mtrEvtCurRetry, pResult);
    }  

    return;
}

/**
*********************************************************************
* @brief： 存储上次上报后的上报计数
* @param[in]： NA
* @param[out]：NA
* @return： void
*********************************************************************
*/
void rpt_reportcnt_store(RPT_CFG_T *rptCfg)
{
    uint16 DataCount = 0;               //任务记录总条数
    uint16 ReportCount = 0;             //已经上报的条数

    rpt_count_read(rptCfg, &DataCount, &ReportCount);
    
    if(ReportCount != DataCount)
    {
        ReportCount = RPT_ADD_VALUE_CLEAR(ReportCount, rptCfg->maxRecord, 1);
        write_pdata_xram(rptCfg->filename, (char *)&ReportCount, RPT_REPORT_CNT_OFFSET, 2);

        RPT_FMT_LOGCC("type[%d] reported success. update rptCnt(%d), recCnt(%d)\n", rptCfg->type, ReportCount, DataCount);
    }
}

/**
*********************************************************************
* @brief： 通道是否可用
* @param[in]： NA
* @param[out]：result 处理结果
* @return： -1  信道异常，-2  信道正常，但不允许主动上报，1   信道正常
*********************************************************************
*/
int rpt_channel_used(void)
{
    int ret = 0;
    NOMAL_OAD_T nomalOAD;
    BOOLEAN bReportFlag = 0;
    uint32 len = 0;
    static uint32 readFailedCnt = 0; 

    MEMZERO(&nomalOAD, sizeof(nomalOAD));

    nomalOAD.oad.value = 0x43000800;

    ret = db_read_nomal(g_rptUdpHandle, &nomalOAD, sizeof(BOOLEAN), (uint8*)&bReportFlag, &len);   
    if((ret != 0) || (bReportFlag == 0))
    {
        readFailedCnt++;
        if(readFailedCnt%1000 == 0)
        {
            RPT_FMT_DEBUG("db_read_nomal failed. readFailedCnt(%d), ret(%d), oda(0x%08x), reportflag(%d)\n", readFailedCnt, ret, nomalOAD.oad.value, bReportFlag);
        }
        return -2;
    }
    
    return ret;
}


/**
*********************************************************************
* @brief：      主动上报报文获取
* @param[in]： CommUse      使用的通道
* @param[in]： nOutBufLen  上行缓冲区大小
* @param[out]：pOutBuf      上行缓冲区
* @return：     报文长度
*********************************************************************
*/
int rpt_record_report(RPT_CFG_T *rptCfg, RPT_RESULT_T *rptResult)
{
    int ret = 0;
    uint16 DataCount = 0;               //任务记录总条数
    uint16 ReportCount = 0;             //已经上报的条数
    RPT_RES_PROC_E Result;
    //static uint32 notAllowedCnt = 0; //打印计数，防止频繁打印
    static uint32 rptAllCnt = 0;

    //处理上报后的主站响应
    rpt_response_proc(rptCfg, &Result);
    
    /* 尚在等待超时，不做后续处理，发送下一条前，先存储上一条的计数 */
    if(Result == RPT_RES_PORC_WAITING)
    {
        return ret;
    }
    else if (Result == RPT_RES_PORC_NEXT)
    {
        rpt_reportcnt_store(rptCfg);
    }
    else if(Result == RPT_RES_PORC_RETRY)
    {
        /*if(rptCfg->type == RPT_TYPE_EVENT) //调试好后任务上报也用这种方法,通道信息暂时没解决
        {
            memcpy_s(rptResult->pOutBuf, ResendBufferLenth, ResendBuffer, ResendBufferLenth);
            rptResult->msgLen = ResendBufferLenth;

            return 0;
        }*/
        rptCfg->retryTag = TRUE;
    }

    //未设置是否允许上报的标志或不允许上报时，将上报计数器置为任务总数
    //等打开上报标志后只上报新任务数据 
//    if(!GetTnmEventReport())
//    {
//        notAllowedCnt++;
//        if(notAllowedCnt%1000 == 0)
//        {
//            RPT_FMT_LOGCC("type[%d] report not allowed. notAllowedCnt(%d), rptCnt(%d) will be equel to recCnt(%d)\n", rptCfg->type, notAllowedCnt, ReportCount, DataCount);
//        }        
//        write_pdata_xram(rptCfg->filename, (char *)&DataCount, RPT_REPORT_CNT_OFFSET, 2);
//        return ret;
//    }
//
//    if(notAllowedCnt)
//    {
//        RPT_FMT_LOGCC("type[%d] last report not allowed. notAllowedCnt(%d), rptCnt(%d) had been equel to recCnt(%d)\n", rptCfg->type, notAllowedCnt, ReportCount, DataCount);
//        notAllowedCnt = 0;
//    }

    //读取任务记录总条数和上报条数
    rpt_count_read(rptCfg, &DataCount, &ReportCount);  

    if (DataCount == ReportCount)
    {
        rptAllCnt++;
        if(rptAllCnt%1000 == 0)
        {
            RPT_FMT_LOGCC("type[%d] reported all records. rptAllCnt(%d), rptCnt(%d) equel to recCnt(%d)\n", rptCfg->type, rptAllCnt, ReportCount, DataCount);
        }     
        
        *rptCfg->resStatus = RPT_RES_INITIAL;
        return ret;
    }

    if(rptAllCnt)
    {
        RPT_FMT_LOGCC("type[%d] last reported all records. rptAllCnt(%d)\n", rptCfg->type, rptAllCnt);
        rptAllCnt = 0;
    }
    
    RPT_FMT_LOGCC("type[%d]: report begin, rptCnt(%d), recCnt(%d), maxRecord(%d)\n", rptCfg->type, ReportCount, DataCount, rptCfg->maxRecord);
    
    ReportCount = RPT_ADD_VALUE_CLEAR(ReportCount, rptCfg->maxRecord, 1);        //读取下一条记录

    //从文件中读出一条记录
    return rpt_record_package(rptCfg, ReportCount, rptResult);
}


/**
*********************************************************************
* @brief： 报文是否都上报
* @param[in]：filename   FIFO文件名
* @return： void
*********************************************************************
*/
BOOL rpt_all_data_is_dequeue(const char *filename)
{
    int ret = 0;
    uint8 tmpBuf[4];
    uint16 tmpDataCount = 0;        //任务记录总条数
    uint16 tmpReportCount = 0;      //已经上报的条数

    memset_s(tmpBuf, sizeof(tmpBuf), 0x0, sizeof(tmpBuf));
    
    //读出目前记录总条数和已上报的数量
    ret = read_pdata_xram(filename, (char *)tmpBuf, RPT_RECORD_CNT_OFFSET, 4);
    if(ret != 0)
    {
        tmpDataCount = 0xffff;
        tmpReportCount = 0xffff;
    }
    else
    {
        tmpDataCount = *((uint16 *)tmpBuf);
        tmpReportCount = *((uint16 *)(tmpBuf + 2));
    }

    if(tmpDataCount == tmpReportCount)
    {
        RPT_FMT_TRACE("all %d records is reported.\n", tmpDataCount);
        return TRUE;
    }

    return FALSE;
}

/**
*********************************************************************
* @name      ：rpt_data_init
* @brief     ：上报模块数据初始化
* @return    ：
* @author    : 贺宁
* @Date      ：2019-12-6
*********************************************************************
*/
void rpt_data_init(uint8 type, DB_CLIENT fd)
{
    int ret = ERR_OK;
    char datapath[256] = {0};
    
    //初始化数据，将原来的上报队列文件删除
    memset(datapath, 0, sizeof(datapath));
    sprintf(datapath, "/data/app/%s/data/rpt_record_file", ADV_APP_NAME);
    
    ret = remove(datapath);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
    }
    else
    {
        PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
    }

    memset(datapath, 0, sizeof(datapath));
    sprintf(datapath, "/data/app/%s/data/event_record_file", ADV_APP_NAME);

    ret = remove(datapath);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
    }
    else
    {
        PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
    }

    memset(datapath, 0, sizeof(datapath));
    sprintf(datapath, "/data/app/%s/data/rpt_mtrevt_rec_file", ADV_APP_NAME);

    ret = remove(datapath);
    if(ret != 0)
    {
        PRTL_FMT_DEBUG("remove \"%s\". ret(%d), %s\n", datapath, ret, strerror(errno));
    }
    else
    {
        PRTL_FMT_DEBUG("remove \"%s\" success.\n", datapath);
    }
    
#ifdef AREA_FUJIAN
    rpt_commlog_qclr();
    rpt_evtstat_clear();
#endif
}

#endif 


#if DESC("主动上报入口",1)

/**
*********************************************************************
* @brief:       上报信息初始化
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_task_init(void)
{
    uint16 DataCount = 0;                   //任务记录总条数
    uint16 ReportCount = 0;                 //已经上报的条数
    RPT_CFG_T cfg;

    MEMZERO(&cfg, sizeof(cfg));

    g_rptUdpHandle = db_client_create(DB_YC_AGENT, DB_CLIENT_REPORT);
    if(g_rptUdpHandle <= 0)
    {
        RPT_FMT_DEBUG("rpt db_handle_create failed!\n");
        return;
    }

    //上报私有FIFO文件
    if(reg_pdata_xram(ADV_APP_NAME, RPT_RECORD_FILE) != 0)
    {
        RPT_FMT_DEBUG("register \"%s\" file failed.\n", RPT_RECORD_FILE);
    }

    if(reg_pdata_xram(ADV_APP_NAME, EVENT_RECORD_FILE) != 0)
    {
        RPT_FMT_DEBUG("register \"%s\" file failed.\n", EVENT_RECORD_FILE);
    }

    if(reg_pdata_xram(ADV_APP_NAME, RPT_MTREVT_REC_FILE) != 0)
    {
        RPT_FMT_DEBUG("register \"%s\" file failed.\n", RPT_MTREVT_REC_FILE);
    }

    rpt_task_info_read();

    //首次上电的时候，先读一下记录条数
    //真正目的是防止程序首次运行（初始化后、第一次装程序）的时候，能够将记录总数、上报条数的值初始化为0
    cfg.type = RPT_TYPE_TASK;
    cfg.resStatus = &ConfirmFlag;
    cfg.filename = RPT_RECORD_FILE;
    cfg.maxRecord = RPT_RECORD_MAX;

    rpt_count_read(&cfg, &DataCount, &ReportCount);

    cfg.type = RPT_TYPE_EVENT;
    cfg.resStatus = &ConfirmFlag_event;
    cfg.filename = EVENT_RECORD_FILE;
    cfg.maxRecord = EVENT_RECORD_MAX_NUM;

    rpt_count_read(&cfg, &DataCount, &ReportCount);    

    cfg.type = RPT_TYPE_MTREVT;
    cfg.resStatus = &ConfirmFlag_event_met;
    cfg.filename = RPT_MTREVT_REC_FILE;
    cfg.maxRecord = RPT_MTREVT_REC_MAX;

    rpt_count_read(&cfg, &DataCount, &ReportCount);      
    
/*
    GetAppConectInfo(0, frameHead, SecMod_NULL, &appConectInfo);
    appConectInfo.SecurityServer = FALSE;
    appConectInfo.clientRecvMaxLen = 1024;
    appConectInfo.clientSendMaxLen = 1024;*/

    return;
}

/**
*********************************************************************
* @brief:       任务上报入口
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_task_entry(time_t curTime, time_t lstTime)
{
    int ret = 0;
    uint8 globle_seq = 0; //TODO 为什么要全局的帧序号
    uint8 rptmsg[APP_SEND_MSG_MAXLEN] = {0};
    RPT_CFG_T cfg;
    RPT_RESULT_T result;
    uint8 chID = 0; //上报通道ID

    MEMZERO(&cfg, sizeof(cfg));
    MEMZERO(&result, sizeof(result));

    if(!GetTnmEventReport())
    {
        return;
    }
    
    //上报任务检测,过秒执行
    //if(curTime-lstTime)
    {
        rpt_task_info_read();

        rpt_task_detect();
            
        //组织上报报文，1秒内可有多个任务执行
        rpt_task_execute(&globle_seq);

        //上报队列都报完，且无需重试，则不再往后执行
        if((ConfirmFlag == RPT_RES_INITIAL) && (rpt_all_data_is_dequeue(RPT_RECORD_FILE)))
        {
            return;
        }

        //取上报报文,处理超时重发
        cfg.type = RPT_TYPE_TASK;
        cfg.resStatus = &ConfirmFlag;
        cfg.filename = RPT_RECORD_FILE;
        cfg.maxRecord = RPT_RECORD_MAX;
        cfg.piid = &TaskGLBpiid;
        cfg.lstTime = lstTime;
        cfg.curTime = curTime;
        
        result.pOutBuf = rptmsg;
        result.bufLen = APP_SEND_MSG_MAXLEN;

        //此处查通道是规避安徽芜湖长期断网导致报文丢失问题，混合模式需再讨论 //TODO
        if(!rpt_channel_search(&chID))
        {
            RPT_FMT_TRACE("任务-%d 无可用的上报通道.\n", reportTaskCfgList.OopTaskCfg[result.taskIndex].TaskId);
            return;
        }
        
        ret = rpt_record_report(&cfg, &result); 
        if(ret != 0)
        {
            RPT_FMT_DEBUG("rpt_task_record_report failed.\n");
            return;
        }

        //上行报文发送
        if(result.msgLen > 0)
        {
            if(!rpt_channel_search(&chID))//混合模式需先有报文再打开通道
            {
                RPT_FMT_TRACE("任务-%d 无可用的上报通道.\n", reportTaskCfgList.OopTaskCfg[result.taskIndex].TaskId);
                return;
            }
            if(1 == ssal_get_status() && chID >= CHL_GPRS_1)
            {
                CHANNEL_CACHE_T *ptChannel = NULL;
                ptChannel = app_get_channel(chID);
                result.msgLen = ssal_commdata_package(ptChannel->tChannel.ip, rptmsg, result.msgLen, rptmsg, APP_SEND_MSG_MAXLEN);
            }
            ret = send_emergency_ipcmsg(chID, 0, rptmsg, result.msgLen);
            if(ret != 0)
            {
                RPT_FMT_DEBUG("send_emergency_ipcmsg failed. id(%d)\n", chID);
                return;
            } 
        }
    }
    
    return;
} 

/**
*********************************************************************
* @brief:       事件上报入口
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_event_entry(time_t curTime, time_t lstTime)
{
    int ret = 0;
    uint8 rptmsg[APP_SEND_MSG_MAXLEN];
    RPT_CFG_T cfg;
    RPT_RESULT_T result;
    uint8 chID = 0; //上报通道ID

    if(!GetTnmEventReport())
    {
        return;
    }
    //事件上报,过秒执行
//    if(0 == curTime - lstTime)
//    {
//        return;
//    }
    MEMZERO(&cfg, sizeof(cfg));
    MEMZERO(&result, sizeof(result));
    memset(rptmsg, 0, APP_SEND_MSG_MAXLEN);
    
    //取上报报文,处理超时重发
    cfg.type = RPT_TYPE_EVENT;
    cfg.resStatus = &ConfirmFlag_event;
    cfg.filename = EVENT_RECORD_FILE;
    cfg.maxRecord = EVENT_RECORD_MAX_NUM;
    cfg.piid = &EventGLBpiid;
    cfg.lstTime = lstTime;
    cfg.curTime = curTime;
    
    result.pOutBuf = rptmsg;
    result.bufLen = APP_SEND_MSG_MAXLEN;

    //此处查通道是解决上电事件上报因无线主站来不及连而丢失问题，及长期断网导致上报数据丢失，混合模式需再讨论 //TODO
    if(!rpt_channel_search(&chID))
    {
        RPT_FMT_TRACE("no channel for task(%d) reporting.\n", reportTaskCfgList.OopTaskCfg[result.taskIndex].TaskId);
        return;
    }
        
    ret = rpt_record_report(&cfg, &result); 
    if(ret != 0)
    {
        RPT_FMT_DEBUG("rpt_event_record_report failed.\n");
        return;
    }
    
    //上行报文发送
    if(result.msgLen > 0)
    {
        if(!rpt_channel_search(&chID))
        {
            return;
        }
        if(1 == ssal_get_status() && chID >= CHL_GPRS_1)
        {
            CHANNEL_CACHE_T *ptChannel = NULL;
            ptChannel = app_get_channel(chID);
            result.msgLen = ssal_commdata_package(ptChannel->tChannel.ip, rptmsg, result.msgLen, rptmsg, APP_SEND_MSG_MAXLEN);
        }
        ret = send_emergency_ipcmsg(chID, 0, rptmsg, result.msgLen);
        if(ret != 0)
        {
            RPT_FMT_DEBUG("send_emergency_ipcmsg failed. id(%d)\n", chID);
            return;
        } 
    }
    
    return;
} 

/**
*********************************************************************
* @brief:       电能表事件上报入口（后期与事件上报合并）
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_mtr_evt_entry(time_t curTime, time_t lstTime)
{
    int ret = 0;
    uint8 rptmsg[APP_SEND_MSG_MAXLEN];
    RPT_CFG_T cfg;
    RPT_RESULT_T result;
    uint8 chID = 0; //上报通道ID

    //事件上报,过秒执行
//    if(0 == curTime-lstTime)
//    {
//        return;
//    }

    MEMZERO(&cfg, sizeof(cfg));
    MEMZERO(&result, sizeof(result));
    memset(rptmsg, 0, APP_SEND_MSG_MAXLEN);
    
    //取上报报文,处理超时重发
    cfg.type = RPT_TYPE_MTREVT;
    cfg.resStatus = &ConfirmFlag_event_met;
    cfg.filename = RPT_MTREVT_REC_FILE;
    cfg.maxRecord = RPT_MTREVT_REC_MAX;
    cfg.piid = &EventGLBpiid_met;
    cfg.lstTime = lstTime;
    cfg.curTime = curTime;
    
    result.pOutBuf = rptmsg;
    result.bufLen = APP_SEND_MSG_MAXLEN;

    //此处查通道是解决上电事件上报因无线主站来不及连而丢失问题，及长期断网导致上报数据丢失，混合模式需再讨论 //TODO
    if(!rpt_channel_search(&chID))
    {
        RPT_FMT_TRACE("no channel for task(%d) reporting.\n", reportTaskCfgList.OopTaskCfg[result.taskIndex].TaskId);
        return;
    }
    
    ret = rpt_record_report(&cfg, &result); 
    if(ret != 0)
    {
        RPT_FMT_DEBUG("rpt_event_record_report failed.\n");
        return;
    }
    
    //上行报文发送
    if(result.msgLen > 0)
    {
        if(!rpt_channel_search(&chID))
        {
            return;
        }
        if(1 == ssal_get_status() && chID >= CHL_GPRS_1)
        {
            CHANNEL_CACHE_T *ptChannel = NULL;
            ptChannel = app_get_channel(chID);
            result.msgLen = ssal_commdata_package(ptChannel->tChannel.ip, rptmsg, result.msgLen, rptmsg, APP_SEND_MSG_MAXLEN);
        }
        ret = send_emergency_ipcmsg(chID, 0, rptmsg, result.msgLen);
        if(ret != 0)
        {
            RPT_FMT_DEBUG("send_emergency_ipcmsg failed. id(%d)\n", chID);
            return;
        } 
    }
    
    return;
} 

/**
*********************************************************************
* @brief:       透传上报报文
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_transresult_data()
{
    int32 datalen = 0;
	uint8 buffer[512];
    uint8 id;
	uint8 rptmsg[APP_SEND_MSG_MAXLEN];
	uint8 tembuf[512];
    uint16 outLen=0;
    uint32 msgLen;
    OOP_DAR_E result;
    int ret;
    static uint8 piid=0;

    /* 先判断是否有数据 */
    if(FALSE == ProxyCheckQueue(&TransQueue))
    {
        return;
    }

    /* 再获取通道 因为混合模式需要先主动打开, 所以放在流程前面 */
    if(!rpt_channel_search(&id))
    {
        return;
    }
    
    datalen = ProxyReadQueue(&TransQueue, buffer);  
    if(datalen <= 0)
    {
        return;
    }
    
    result = transresult_to_frame(buffer, datalen, tembuf, &outLen,512);
    if(result != DATA_SUCCESS)
    {
        return;
    }

    piid = RPT_ADD_VALUE_CLEAR(piid, 63, 1);
    memset(rptmsg, 0, APP_SEND_MSG_MAXLEN);
    msgLen = rpt_packet_make(tembuf, outLen, 1, 0, piid, rptmsg, APP_SEND_MSG_MAXLEN);
    if(msgLen < 0)
    {
        return;
    }
    PRTL_BUF_LOGCC(rptmsg, msgLen, "发送的消息");
    if(1 == ssal_get_status() && id >= CHL_GPRS_1)
    {
        CHANNEL_CACHE_T *ptChannel = NULL;
        ptChannel = app_get_channel(id);
        msgLen = ssal_commdata_package(ptChannel->tChannel.ip, rptmsg, msgLen, rptmsg, APP_SEND_MSG_MAXLEN);
    }
    ret = send_emergency_ipcmsg(id, 0, rptmsg, msgLen);
    if(ret < 0)
    {
        return;
    }
} 

#endif


#if DESC("精准对时任务",1)


/**
*********************************************************************
* @brief:       对时任务检查
* @param[in]：   NA
* @param[out]： TimeTask 对时任务结构体 
* @return：      void
*********************************************************************
*/
void rpt_timetask_check(TIMETASK_STRUCT *TimeTask)
{
    int ret;
    time_t  timenow;
    DateTimeHex_t dateTimeHex;    
    int timingmode ;
    OOP_SYNCCLOCK_T  syncclock;
    uint32 start,end;       //精准对时任务起始时间和结束时间
    MEMZERO(&syncclock, sizeof(OOP_SYNCCLOCK_T));
    if(TimeTask->TimeTaskType !=NULL_TASK)
    {
        return;
    }
    //if(TimeTask->TaskSucBit ==1)
    //{
        //RPT_FMT_DEBUG(" 当日心跳校时已成功 \n");
     //   return ;
   // }
    timenow = time(NULL);
    DT_Time2DateTimeHex(timenow,&dateTimeHex);
    timingmode = app_get_timingmode();
    if(timingmode !=1)
    {
        //RPT_FMT_DEBUG(" 主站对终端对时模式0x40000300未设置为心跳校时 timingmode %d \n",timingmode);
        return ;
    }
    ret = app_get_syncparam(&syncclock);
    if(ret<0)
    {
        //RPT_FMT_DEBUG(" 主站对终端对时模式0x40000400未设置为心跳校时参数 \n");
        return;
    }
    start = TMN_TIME_TICK_BEGIN *3600 ;  //终端精确对时从每天14点开始
    end = TMN_TIME_TICK_END*3600 ;  //终端精确对时从每天16点结束
    if((dateTimeHex.hour*3600+dateTimeHex.min*60)>=start && (dateTimeHex.hour*3600+dateTimeHex.min*60)<=end)
    {
        if(TimeTask->TaskSucBit ==0)
        {
            TimeTask->TimeTaskType = MASTER_TO_TMN_PRECESE_TASK;
            rpt_taskstate(TMN_TO_METER_IDLE,NULL);
            RPT_FMT_DEBUG("产生终端精准校时任务 \n");
        }
    }else
    {
        TimeTask->TaskSucBit = 0;
    }
}

/**
*********************************************************************
* @brief:       主站对终端对时状态处理
* @param[in]：   RunStat     //读取或者设置状态处理
              ClockLimt   //误差计算
* @param[out]： 
* @return：      主站对终端对时状态 
*********************************************************************
*/
uint8 rpt_taskstate(int8 RunStat,float  *ClockLimt)
{
    static float               sClockLimit;
    static MASTER_TO_TMN_STAT  MasterToMetertRunStat;

    //获取当前状态
    if(RunStat == -1)
    {
        //如果当前状态为主站应答完,那么同时回复
        if(MasterToMetertRunStat == MASTER_TO_TMN_PRECESE_SUCC
                || MasterToMetertRunStat == MASTER_TO_TMN_PRECESE_SUCC_SEC)
        {
            if(ClockLimt != NULL)
                *ClockLimt = sClockLimit;
        }
        return MasterToMetertRunStat;
    }

    MasterToMetertRunStat = RunStat;
    if(MasterToMetertRunStat == MASTER_TO_TMN_PRECESE_SUCC
            || MasterToMetertRunStat == MASTER_TO_TMN_PRECESE_SUCC_SEC)
    {
        if(ClockLimt != NULL)
            sClockLimit = *ClockLimt;
    }

    return MasterToMetertRunStat;
}
/**
*********************************************************************
* @brief:       获取发送心跳使用的通道，优先使用以太网
* @param[in]：   
* @param[out]： NA 
* @return：      通道
*********************************************************************
*/
CHANNEL_CACHE_T* rpt_hearbeatchannel_get()
{
    CHANNEL_CACHE_T *ptChannel = NULL;
    CHANNEL_CACHE_T *ret = NULL;
    uint8 id;
    uint8 flag = 0;
    for(id = 0; id < MAX_CHANNEL_NUM; id++)
    {
        ptChannel = app_get_channel(id);
        if(NULL == ptChannel || FALSE == ptChannel->bOpen)
        {
            continue;
        }
        /* 判断通信介质 */
        if(COMM_MEDIA_GPRS != ptChannel->tChannel.medium && COMM_MEDIA_ETH != ptChannel->tChannel.medium)
        {
            continue;
        }
        if(COMM_MEDIA_GPRS == ptChannel->tChannel.medium && flag ==0)
        {
            ret = ptChannel;
        }
        if(COMM_MEDIA_ETH == ptChannel->tChannel.medium)
        {
            flag =1;
            ret = ptChannel;
        }
    }
    return ret;
}
void rpt_heartbeat_send(CHANNEL_CACHE_T   *channel,uint32 *SendTime)
{
    uint8 buf[100]={0};
    uint16 buflen=0;
    int ret;
    buflen = app_linkrequest_package(LINK_HEART - 1, CHANNEL_GET_HEART(channel), buf, 100);
    RPT_FMT_DEBUG("app_linkrequest_package buflen %d  \n",buflen);
    if(buflen>0)
    {
        if(1 == ssal_get_status())
        {
            buflen = ssal_linkrequest_package(channel->tChannel.ip, buf, buflen, buf, 256);
        }
        ret = send_emergency_ipcmsg(channel->id, PRO_HEART_FRAME, buf, buflen);
        if(ret==0)
        {
            RPT_FMT_DEBUG("发送心跳请求  \n");
            RPT_BUF_DEBUG(buf,buflen,"心跳报文\n ");
            //暂停屏蔽正常的心跳 使其不发送
            channel->heartbeatflag = 1;
            *SendTime = time(NULL);
        }
    }

}
/**
*********************************************************************
* @brief:       冒泡排序
* @param[in]：   
* @param[out]： NA 
* @return：      返回剔除最大最小数据之后的新数据个数(有效个数)
*********************************************************************
*/
int BubbleSortFun(float *ClockLimtLink,uint8 RollCnt,uint8 MaxlimitNum,uint8 MinlimitNum)
{
    uint8       i,j;
    float       temp;

    for(i = 0; i < (RollCnt-1); i++)
    {
        //内循环选择要进行比较的数
        for(j = 0; j < (RollCnt-1-i); j++)
        {
            if(ClockLimtLink[j]>ClockLimtLink[j+1])
            {
                temp=ClockLimtLink[j];
                ClockLimtLink[j]=ClockLimtLink[j+1];
                ClockLimtLink[j+1]=temp;
            }
        }
    }
    RPT_FMT_DEBUG("排序结果 : \n");
    for(i = 0; i < RollCnt; i++)
        RPT_FMT_DEBUG("%f \n",ClockLimtLink[i]);

    //剔除最大
    if(RollCnt >= (MaxlimitNum + MinlimitNum))
    {
        ClockLimtLink = &ClockLimtLink[MinlimitNum];
        RollCnt = RollCnt - MaxlimitNum - MinlimitNum;
        return RollCnt;
    }
    else
        return -1;
}

/**
*********************************************************************
* @brief:       给终端对时
* @param[in]：   float   *ClockLimt         //误差内存
            uint8   ClockCnt           //带计算误差内存个数
            uint16  MasterToTmn_Limit  //阈值大小
* @param[out]： NA 
* @return：      
*********************************************************************
*/
int rpt_ticktime(float *ClockLimt,uint8 ClockCnt,uint16  MasterToTmn_Limit)
{
    time_t timenow = time(NULL);
    uint8 i;
    float limit = 0;
    OOP_DATETIME_S_T preTimeOOP;    
    OOP_DATETIME_S_T TimeOOP;
    DateTimeHex_t   Timehex;
    MEMZERO(&preTimeOOP, sizeof(OOP_DATETIME_S_T));
    MEMZERO(&TimeOOP, sizeof(OOP_DATETIME_S_T));
    MEMZERO(&Timehex, sizeof(DateTimeHex_t));
    DT_Time2DateTimeShort(timenow,&preTimeOOP);
    RPT_FMT_DEBUG("对时前时间 %d %d %d %d %d %d  \n",preTimeOOP.year,preTimeOOP.month,preTimeOOP.day,
        preTimeOOP.hour,preTimeOOP.minute,preTimeOOP.second);

    if(ClockCnt !=1)
    {
        for(i=0;i<ClockCnt;i++)
        {
            limit += ClockLimt[i];
        }
        limit = (limit/ClockCnt);
    }else
    {
        limit = ClockLimt[0];
    }
    RPT_FMT_DEBUG("%d 组时间偏差计算平均值为 %f \n",ClockCnt,limit);

    timenow += (int)limit;
    DT_Time2DateTimeHex(timenow,&Timehex);
    DT_Time2DateTimeShort(timenow,&TimeOOP);

    if(abs(limit) >MasterToTmn_Limit )
    {
        if(systime_set(&Timehex))
        {
            RPT_FMT_DEBUG("对时成功 当前时间 %d %d %d %d %d %d  \n",TimeOOP.year,TimeOOP.month,TimeOOP.day,
                TimeOOP.hour,TimeOOP.minute,TimeOOP.second);
            CheckERC_3114(g_rptUdpHandle,preTimeOOP,TimeOOP);
        }else
        {
            RPT_FMT_DEBUG("设置时钟失败！ \n");
        }
        return 1;
    }else
    {
        RPT_FMT_DEBUG("时钟误差小于阈值范围:无需进行对时 \n");
        return 0;
    }
}

/**
*********************************************************************
* @brief:       精准对时任务
* @param[in]：   timetask
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int rpt_timetask_pro(TIMETASK_STRUCT *timetask)
{
    static OOP_SYNCCLOCK_T  syncclock;
    static CHANNEL_CACHE_T   *channel;
    static uint32 SendTime;         //发送心跳的时间
    static uint8 TryCnt;    //心跳超时次数
    static float Clocklimt[100];
    static uint8 Clockindex;
    uint8 HeartBeatValidNum;
    int ret;
    uint16 heartInterval;
#if AREA_ZHEJIANG ==1
    static uint32 LastTaskTime;     //记录下上次任务结束时间 以防一次校时结束后立刻发送心跳
#endif
    switch(rpt_taskstate(-1,NULL))
    {
    case MASTER_TO_TMN_IDLE:
    {
        RPT_FMT_DEBUG("终端精准校时任务初始化 \n");
        SendTime = 0;
        TryCnt = 0;
        Clockindex = 0;
        MEMZERO(Clocklimt,sizeof(Clocklimt));
        MEMZERO(&syncclock,sizeof(OOP_SYNCCLOCK_T));
        ret = app_get_syncparam(&syncclock);
        if(ret <0)
        {
            RPT_FMT_DEBUG(" 主站对终端对时模式0x40000400未设置为心跳校时参数 \n");
            return 1;
        }
        channel = rpt_hearbeatchannel_get();
        if(NULL == channel)
        {
            RPT_FMT_DEBUG(" 当前通道为空 \n");
            return 1;
        }
        RPT_FMT_DEBUG("最新心跳总个数 %d 延时阈值 %d \n",syncclock.nSum,syncclock.nDelay);
        rpt_taskstate(MASTER_TO_TMN_NEW,NULL);
    }
        break;
    case MASTER_TO_TMN_NEW:         //向主站请求心跳
    case MASTER_TO_TMN_NEW_SEC:
    {
        heartInterval = CHANNEL_GET_HEART(channel);
        uint32 timenow = time(NULL);
#if AREA_ZHEJIANG ==1
        if(timenow > LastTaskTime && timenow-LastTaskTime<heartInterval)
            break;
#endif
        uint32 timepast = timenow-SendTime;
        if( timepast>= heartInterval)
        {
            rpt_heartbeat_send(channel,&SendTime);
        }else
        {
            break;
        }
        
        if(rpt_taskstate(-1,NULL) ==MASTER_TO_TMN_NEW )
        {
            rpt_taskstate(MASTER_TO_TMN_WAIT,NULL);
        }else
        {
            rpt_taskstate(MASTER_TO_TMN_WAIT_SEC,NULL);
        }
    }
        break;
    case MASTER_TO_TMN_WAIT://终端对主站的对时操作等待接收
    case MASTER_TO_TMN_WAIT_SEC://终端对主站的对时操作等待接收
    {
        heartInterval = CHANNEL_GET_HEART(channel);
        if(time(NULL) - SendTime >heartInterval+5)
        {
            RPT_FMT_DEBUG(" 心跳等待超时 \n");
            if(rpt_taskstate(-1,NULL) ==MASTER_TO_TMN_WAIT )
            {
                rpt_taskstate(MASTER_TO_TMN_OVERTIME,NULL);
            }else
            {
                rpt_taskstate(MASTER_TO_TMN_OVERTIME_SEC,NULL);
            }
        }
    }
        break;
    case MASTER_TO_TMN_OVERTIME://终端对主站的对时操作等待接收
    case MASTER_TO_TMN_OVERTIME_SEC:
    {
        TryCnt++;
        if(TryCnt>=6)
        {
            TryCnt = 0;
            RPT_FMT_DEBUG(" 连续6次未收到心跳回复，任务失败！ \n");
            timetask->TaskSucBit =0;
            rpt_taskstate(MASTER_TO_TMN_IDLE,NULL);
            return 1;
        }
        if(rpt_taskstate(-1,NULL) ==MASTER_TO_TMN_OVERTIME )
        {
            rpt_taskstate(MASTER_TO_TMN_NEW,NULL);
        }else
        {
            rpt_taskstate(MASTER_TO_TMN_NEW_SEC,NULL);
        }
    }
        break;
    case MASTER_TO_TMN_PRECESE_SUCC:    //终端请求时钟任务(成功接收)
    {
        rpt_taskstate(-1,&Clocklimt[Clockindex]);
        //校时参数有可能有修改 重新加载下
        app_get_syncparam(&syncclock);
        //有可能在校时过程中有对时操作 所以对比下误差值 如果和前面的差的太多 就以当前的为准
        if(Clockindex > 0)
        {
            if(abs(Clocklimt[Clockindex] - Clocklimt[Clockindex-1]) > syncclock.nDelay)
            {
                RPT_FMT_DEBUG(" 精确对时过程中有对时 重新开始 \n");
                Clocklimt[0] = Clocklimt[Clockindex];
                Clockindex = 0;
            }
        }
        TryCnt = 0;
        Clockindex++;
        if(Clockindex >=syncclock.nSum )
        {
            HeartBeatValidNum = BubbleSortFun(Clocklimt,Clockindex,syncclock.nMaxDelNum,syncclock.nMinDelNum);
            if(HeartBeatValidNum > syncclock.nValidNum)
            {
                Clockindex = 0;
                if(rpt_ticktime(Clocklimt,HeartBeatValidNum,syncclock.nDelay)==0)
                {
                    RPT_FMT_DEBUG(" 精确对时任务完成，对时后时差小于阈值！ \n");
                    //浙江功能 任务不结束 一直运行
                    #if AREA_ZHEJIANG==0   
                    timetask->TaskSucBit=1;
                    #endif
                    rpt_taskstate(MASTER_TO_TMN_IDLE,NULL);
                    #if AREA_ZHEJIANG==1
                    LastTaskTime = time(NULL);
                    #endif  
                    channel->heartbeatflag = 0;
                    return 1;
                }
                else
                {
                    RPT_FMT_DEBUG(" 进入二次时钟查询！ \n");
                    SendTime = time(NULL);
                    rpt_taskstate(MASTER_TO_TMN_NEW_SEC,NULL);
                }
            }
        }
        else
        {
            rpt_taskstate(MASTER_TO_TMN_NEW,NULL);
        }
    }
        break;
    case MASTER_TO_TMN_PRECESE_SUCC_SEC:
    {
        rpt_taskstate(-1,&Clocklimt[0]);
        RPT_FMT_DEBUG(" 二次心跳时钟查询验证 误差 %d  \n",abs(Clocklimt[0]));
        if(abs(Clocklimt[0])<=syncclock.nDelay)
        {
            RPT_FMT_DEBUG(" 精确对时任务完成，对时后时差小于阈值！ \n");
            #if AREA_ZHEJIANG==0   
            timetask->TaskSucBit =1;
            #else
            LastTaskTime = time(NULL);
            #endif  
        }else
        {
            RPT_FMT_DEBUG(" 精确对时任务失败，对时后时差大于阈值！ \n");
            timetask->TaskSucBit =0;
        }
        channel->heartbeatflag = 0;
        rpt_taskstate(MASTER_TO_TMN_IDLE,NULL);
        return 1;
    }
        break;
    default:
        break;
    }
    return -1;
}
/**
*********************************************************************
* @brief:       对时任务入口（精准对时）
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_timetask_entry(time_t lasttime)
{
    static TIMETASK_STRUCT TimeTask;
    time_t timenow = time(NULL);
    if(lasttime == timenow)
        return;
#if AREA_ZHEJIANG==0   
    OOP_DATETIME_S_T Last,Now;
    MEMZERO(&Last, sizeof(OOP_DATETIME_S_T));
    MEMZERO(&Now, sizeof(OOP_DATETIME_S_T));
    DT_Time2DateTimeShort(lasttime,&Last);
    DT_Time2DateTimeShort(timenow,&Now);
    if(Last.day !=Now.day)
    {
        RPT_FMT_DEBUG(" 过日清空精准校时成功标识 \n");
        TimeTask.TaskSucBit =0;
    }
#endif
    rpt_timetask_check(&TimeTask);

    switch(TimeTask.TimeTaskType)
    {
    case MASTER_TO_TMN_PRECESE_TASK:
    {
        if(rpt_timetask_pro(&TimeTask)>0)
        {
            TimeTask.TimeTaskType = NULL_TASK;
        }
    }
        break;
    default:
        break;
    }
}

time_t DateTimeToSec(uint8 yy,uint8 mm,uint8 dd,uint8 hh,uint8 nn,uint8 ss)
{
    struct tm p;
    time_t timep;

    time(&timep);
    LOCALTIME_R(&timep,&p);

    p.tm_year = yy + 100;
    p.tm_mon  = mm - 1;
    p.tm_mday = dd;
    p.tm_hour = hh;
    p.tm_min  = nn;
    p.tm_sec  = ss;
    p.tm_wday = 0;

    time_t sec_t = FastDateToSecond(&p);

    return sec_t;
}
void rpt_ansertime_deal(uint8 *Inbuf,uint16 buflen)
{
    struct      timeval   T1;
    struct      timeval   T2;
    struct      timeval   T3;
    struct      timeval   T4;
    float       U;
    float       V;
    float       K;
    float       ClockLimt;

    
    RPT_BUF_DEBUG(Inbuf,buflen,"收到心跳回码 \n");
    
    //T1终端发送时间
    T1.tv_usec = Inbuf[11]*255+Inbuf[12];
    T1.tv_sec  = DateTimeToSec(Inbuf[3]*256+Inbuf[4]-2000,Inbuf[5],Inbuf[6],Inbuf[8],Inbuf[9],Inbuf[10]);

    //T2主站接收时间
    T2.tv_usec = Inbuf[21]*255+Inbuf[22];
    T2.tv_sec  = DateTimeToSec(Inbuf[13]*256+Inbuf[14]-2000,Inbuf[15],Inbuf[16],Inbuf[18],Inbuf[19],Inbuf[20]);

    //T3主站响应时间
    T3.tv_usec = Inbuf[31]*255+Inbuf[32];
    T3.tv_sec  = DateTimeToSec(Inbuf[23]*256+Inbuf[24]-2000,Inbuf[25],Inbuf[26],Inbuf[28],Inbuf[29],Inbuf[30]);

    //T4终端接收的时间
    gettimeofday(&T4, NULL);//获取毫秒级时间

    U = (T2.tv_sec + (T2.tv_usec*0.001)) - (T1.tv_sec + (T1.tv_usec*0.001));


    T4.tv_usec /= 1000;
    V = (T4.tv_sec + (T4.tv_usec*0.001)) - (T3.tv_sec + (T3.tv_usec*0.001));
    RPT_FMT_DEBUG(" T1 %d T2 %d t3 %d t4 %d u %f v %f   \n",T1.tv_sec,T2.tv_sec,T3.tv_sec,T4.tv_sec,U,V);


    K = (U-V)/2;
    ClockLimt = K;
    RPT_FMT_DEBUG(" 误差为 %f   \n",ClockLimt);
    if(rpt_taskstate(-1,NULL) == MASTER_TO_TMN_WAIT)
        rpt_taskstate(MASTER_TO_TMN_PRECESE_SUCC,&ClockLimt);
    else if(rpt_taskstate(-1,NULL) == MASTER_TO_TMN_WAIT_SEC)
        rpt_taskstate(MASTER_TO_TMN_PRECESE_SUCC_SEC,&ClockLimt);
}
#endif

#if DESC("透明任务上报处理",AREA_FUJIAN)

/**********************************************************************
* @name      : rpt_transtask_state_msg_send
* @brief     ：通信代理的应用连接状态通知
* @param[in] ：ack   确认消息
* @return    ：返回消息长度
* @Create    : 贺宁
* @Date      ：2020-04-20
* @Update    :
**********************************************************************/
void rpt_transtask_state_msg_send(RPT_TRANSTASK_STATE_T *pTranstaskState, uint8 type)
{
    int ret = ERR_OK;
    uint8 msgdata[1400] = {0};
    uint16 msglen=0;

    //透明任务方案号
    memcpy_r(&msgdata[msglen], &pTranstaskState->taskid, sizeof(uint16));
    msglen += sizeof(uint16);

    //采集存储时标
    memcpy_r(&msgdata[msglen], &pTranstaskState->storeTime.year, sizeof(uint16));
    msglen += sizeof(uint16);

    msgdata[msglen++] = pTranstaskState->storeTime.month;
    msgdata[msglen++] = pTranstaskState->storeTime.day;
    msgdata[msglen++] = pTranstaskState->storeTime.hour;
    msgdata[msglen++] = pTranstaskState->storeTime.minute;
    msgdata[msglen++] = pTranstaskState->storeTime.second;
    
    msgdata[msglen++] = type;

    if(type == 0)
    {
        memcpy_r(&msgdata[msglen], &pTranstaskState->reportNum, sizeof(uint16));
        msglen += sizeof(uint16);
    }
    else
    {
        memcpy_r(&msgdata[msglen], &pTranstaskState->confirmNum, sizeof(uint16));
        msglen += sizeof(uint16);
    }
       
    ret = appmain_send_msg(MSG_APPAMIN_CTS_RPTSTATE, 0, msgdata, msglen);
    if(ret != ERR_OK)
    {
        RPT_FMT_DEBUG("appmain_send_msg failed. ret(%d)\n",ret);
        return;
    }

    return;
}

/**
*********************************************************************
* @brief：     全事件数据变更初始化
* @param[in]： NA
* @param[out]：NA
* @return：     -1 执行错误
*********************************************************************
*/
void rpt_fj_index_clock_init()
{
    pthread_mutex_init(&g_rptIdxmutex, NULL);
}

/**
*********************************************************************
* @brief：     全事件数据变更初始化
* @param[in]： NA
* @param[out]：NA
* @return：     -1 执行错误
*********************************************************************
*/
void rpt_fj_rptlog_clock_init()
{
    pthread_mutex_init(&g_rptlog.mutex, NULL);
}

void rpt_task_init_fj(void)
{
    int ret         = 0;
    READ_RECORD_T readRecord = {0};
    RECORD_UNIT_T recordUnit = {0};

    rpt_fj_index_clock_init();
    rpt_fj_rptlog_clock_init();

    //上报私有FIFO文件
    if(reg_pdata_xram(ADV_APP_NAME, RPT_LOGREC_FILE) != 0)
    {
        RPT_FMT_DEBUG("register \"%s\" file failed.\n", RPT_LOGREC_FILE);
    }

    ret = read_pdata_xram(RPT_LOGREC_FILE, (char*)&g_rptlog.fifo, 0, sizeof(RPT_LOGFIFO_T));
    if((ret != 0) || (g_rptlog.fifo.rdptr == 0xffff))
    {
        RPT_FMT_DEBUG("读队列文件%s失败,返回%d\n", RPT_LOGREC_FILE, ret);
        memset(&g_rptlog.fifo, 0, sizeof(RPT_LOGFIFO_T));
    }

    if(reg_pdata_xram(ADV_APP_NAME, RPT_STAT_FILE) != 0)
    {
        RPT_FMT_DEBUG("register \"%s\" file failed.\n", RPT_STAT_FILE);
    }

    ret = read_pdata_xram(RPT_STAT_FILE, (char*)&g_rpt_stat_info, 0, sizeof(RPT_STAT_T));
    if((ret != 0) || (g_rpt_stat_info.valid == 0xffff))
    {
        RPT_FMT_DEBUG("读文件%s失败,返回%d\n", RPT_STAT_FILE, ret);
        memset(&g_rpt_stat_info, 0, sizeof(RPT_STAT_T));
    }

    /* 再次初始化促使解锁 */
    g_rptUdpHandle = db_client_create(DB_YC_AGENT, DB_CLIENT_REPORT);
    if(g_rptUdpHandle <= 0)
    {
        APP698_FMT_DEBUG("rpt db_handle_create failed!\n");
    }

    /*获取基础时间*/
    readRecord.recordOAD.infoNum = 0xffff;
    readRecord.recordOAD.road.oadMain.value = RPT_REPORT_LISTID;
    readRecord.recordOAD.road.oadCols.nNum = 0;
    readRecord.cType = COL_TM_STORE;
    readRecord.cStart = 0;
    readRecord.cEnd = 0xffffffff;
    readRecord.bTable = TRUE;
    
    ret = db_read_record(g_rptUdpHandle, &readRecord, DATA_LEN_MAX, &recordUnit);
    if(ret == 0)
    {
        g_rpt_basetime = recordUnit.colStartTm;
        g_rpt_index = recordUnit.colStoreTm;
    }
    else
    {
        readRecord.recordOAD.road.oadMain.value = RPT_RETRYP_LISTID;
        ret = db_read_record(g_rptUdpHandle, &readRecord, DATA_LEN_MAX, &recordUnit);
        if(ret == 0)
        {
            g_rpt_basetime = recordUnit.colStartTm;
            g_rpt_index = recordUnit.colStoreTm;
        }
        else
        {
            RPT_FMT_DEBUG("基础时间获取失败，返回(%d).\n", ret);
        }
    }

    RPT_FMT_DEBUG("福建上报初始化完成,基础时间-%d,内容编号-%d\n", g_rpt_basetime, g_rpt_index);
}

/**
*********************************************************************
* @brief:       获取全局索引，每个记录都不同
* @detail:      
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int rpt_fj_index_get(void)
{
    uint16 curindex = 0;

    /* 使用互斥量进行加锁 */
    PTHREAD_LOCK_SAFE(&g_rptIdxmutex);

    curindex = g_rpt_index++;

    /* 使用互斥量进行解锁 */
    PTHREAD_UNLOCK_SAFE(&g_rptIdxmutex);   

    return curindex;
}

/**
*********************************************************************
* @brief:       福建任务上报执行
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
OOP_DAR_E rpt_trans_result_apdu(OOP_OAD_U oad, OOP_RCSD_T rcsd, DATA_CONVERT_T *convert)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    IC_OAD_INFO_T icInfo;   
    uint8 *dstBuf = (uint8*)convert->dstBuf;
    uint32 dstLen = convert->dstLen;
    uint32  dOffset = *convert->dOffset;    

    //OAD
    memcpy_r(&dstBuf[dOffset], &oad, sizeof(uint32));
    dOffset += 4;

    //RCSD
    dar = apdu_rcsd_tofmt698(dstBuf, dstLen, &dOffset, &rcsd, sizeof(OOP_RCSD_T));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("apdu_rcsd_tofmt698 failed. rcsd.nNum(%d)\n", rcsd.nNum);
        return dar;        
    }

    //响应数据
    dstBuf[dOffset++] = 1; //[1] SEQUENCE OF A-RecordRow

    //记录条数
    dstBuf[dOffset++] = 1; //M条记录

    //记录数据
    icInfo.choice = 0;
    icInfo.oad.value = rcsd.cols[0].oad.value;

    *convert->dOffset = dOffset;
    
    dar = normal_data_to_fmt698(&icInfo, convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("normal_data_to_fmt698 failed. oad(0x%08x)\n", icInfo.oad.value);
        return dar;        
    }
    
    return dar;
}

/**
*********************************************************************
* @brief:       把透明任务方案需上报的数据写入上报队列
* @detail:      
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int rpt_task_write(int clientid, uint16 taskid, void* data, uint16 datalen, uint8 check)
{
    int ret         = 0;
    DATA_CONVERT_T convert;
    EVENT_REPORT_INFO eventReport;
    uint32 offsetTemp = 0;
    OOP_RCSD_T  tRCSD = {0};
    OOP_DAR_E dar = DATA_SUCCESS;
    WRITE_RECORD_T  writeRecord = {0};
    uint32 storeNo = 0;
    RPT_FIFO_T *rptFifo = (RPT_FIFO_T *)writeRecord.inDataBuf;
    OOP_TRANS_TASK_T tmpTask = {0};
    OOP_TRANS_REPORT_T tmpPlan = {0};
    OOP_TRANS_TASK_RESULT_T *pData = (OOP_TRANS_TASK_RESULT_T *)data;

    READ_RECORD_T readRecord = {0};
    RECORD_UNIT_T recordUnit = {0};
    
    int timenow = get_time(TIME_MONOTONIC)+g_rpt_basetime;

    //读取上报参数
    ret = app_get_task_list_fj(taskid, &tmpTask);
    if(ret != 0)
    {
        RPT_FMT_DEBUG("不上报 透明任务方案-%d 不存在.\n", taskid);
        return ret;
    }
    
    ret = app_get_plan_report_list_fj(tmpTask.reportid, &tmpPlan);
    if(ret != 0)
    {
        RPT_FMT_DEBUG("不上报 透明任务方案-%d 的 上报方案-%d 不存在.\n", taskid, tmpTask.reportid);
        return ret;
    }

    if(tmpPlan.retry == 0)
    {
        RPT_FMT_DEBUG("不上报 透明任务方案-%d 的 上报方案-%d 最大上报次数为0.\n", taskid, tmpTask.reportid);
        return 0;
    }

    if((tmpPlan.resultFlag == 1)&&((check == RPT_CHECK_NO) || (pData->result == 0)))
    {
        RPT_FMT_DEBUG("不上报 透明任务方案-%d 上报方案-%d 验证通过上报 但验证不通过或失败[验证-%d 结果-%d].\n", taskid, tmpTask.reportid, check, pData->result);
        return 0;        
    }

    if((tmpPlan.resultFlag == 2) && ((check == RPT_CHECK_OK) || (pData->result == 0)))
    {
        RPT_FMT_DEBUG("不上报 透明任务方案-%d 上报方案-%d 验证不通过上报 但验证通过或失败[验证-%d 结果-%d].\n", taskid, tmpTask.reportid, check, pData->result);
        return 0;        
    }

    if((tmpPlan.reportTime < 255) && (tmpPlan.reportTime > 0))
    {
        //先读取本透明方案的最近一条记录，获得结束时间
        readRecord.recordOAD.infoNum = 0xffff;
        readRecord.recordOAD.road.oadMain.value = RPT_REPORT_LISTID;
        readRecord.recordOAD.road.oadCols.nNum = 1;
        readRecord.recordOAD.road.oadCols.oad[0].value = taskid;
        readRecord.cType = COL_TM_START;
        readRecord.cStart = 0;
        readRecord.cEnd = timenow;
        readRecord.bTable = TRUE;
        
        ret = db_read_record(clientid, &readRecord, DATA_LEN_MAX, &recordUnit);
        if(ret != 0)
        {
            writeRecord.colEndTm = tmpPlan.reportTime*(timenow/tmpPlan.reportTime+1);
            RPT_FMT_DEBUG("首次 更新上报时间-%ds\n", writeRecord.colEndTm);
        }
        else
        {
            if(timenow >= recordUnit.colEndTm)
            {
                writeRecord.colEndTm = tmpPlan.reportTime*(timenow/tmpPlan.reportTime+1);
                RPT_FMT_DEBUG("更新上报时间-%ds\n", writeRecord.colEndTm);
            }
            else
            {
                writeRecord.colEndTm = recordUnit.colEndTm;
                RPT_FMT_DEBUG("保持上报时间-%ds\n", writeRecord.colEndTm);
            }
        }
    }
    else
    {
        writeRecord.colEndTm = timenow;
    }
    
    //转化为报文
    memset(&eventReport, 0, sizeof(EVENT_REPORT_INFO));
    
    convert.srcBuf = data;
    convert.srcLen = datalen;
    convert.sOffset = &offsetTemp;
    
    convert.dOffset = &eventReport.dataLen;
    convert.dstBuf = eventReport.pdataBuf;
    convert.dstLen = EVENT_RECORD_MAX_LEN;

    //根据上报方案组织报文
    tRCSD.nNum = tmpPlan.data.record.tRCSD.nNum;
    tRCSD.cols[0].oad.value = tmpPlan.data.record.tRCSD.cols[0].oad.value;

    //构造透明方案报文
    dar = rpt_trans_result_apdu(tmpPlan.data.record.oad, tRCSD, &convert);
    if(dar != DATA_SUCCESS)
    {
        RPT_FMT_DEBUG("构造任务上报报文失败,返回-%d\n", dar);
        return dar;
    }

    writeRecord.recordOAD.road.oadCols.nNum = 1;
    writeRecord.recordOAD.road.oadCols.oad[0].value = taskid;
    writeRecord.recordOAD.infoNum = tmpPlan.priority; //上报优先级
    writeRecord.colStartTm = timenow;
    writeRecord.colStoreTm = rpt_fj_index_get(); //全局唯一索引

    //存入上报队列
    rptFifo->taskID = taskid;
    rptFifo->planID = tmpTask.reportid;
    rptFifo->timeOut = DT_TI2Seconds(tmpPlan.timeout);
    rptFifo->retryCnt = tmpPlan.retry;
    rptFifo->priority = tmpPlan.priority;
    rptFifo->storeTime = pData->storeTime;
    rptFifo->mergeNum = 1;
    rptFifo->dataLen = eventReport.dataLen;
    memcpy(rptFifo->databuf, eventReport.pdataBuf, eventReport.dataLen);
        
    writeRecord.inDatalen = sizeof(RPT_FIFO_T)+rptFifo->dataLen;

    //将报文写入上报队列
    if(tmpPlan.reportTime < 255)
    {
        writeRecord.recordOAD.road.oadMain.value = RPT_REPORT_LISTID; //上报OAD
        
        RPT_BUF_DEBUG(rptFifo->databuf, rptFifo->dataLen, "[入队列]任务上报 内容编号-%d,任务-%d,方案-%d,超时时间-%d,重试次数-%d,优先级-%d:",  
            writeRecord.colStoreTm, rptFifo->taskID, rptFifo->planID, rptFifo->timeOut, rptFifo->retryCnt, rptFifo->priority);
    }
    else
    {
        writeRecord.recordOAD.road.oadMain.value = RPT_RETRYP_LISTID; //补报OAD
        
        RPT_BUF_DEBUG(rptFifo->databuf, rptFifo->dataLen, "[入队列]等待补报 内容编号-%d,任务-%d,方案-%d,超时时间-%d,重试次数-%d,优先级-%d:",  
            writeRecord.colStoreTm, rptFifo->taskID, rptFifo->planID, rptFifo->timeOut, rptFifo->retryCnt, rptFifo->priority);
    }
    
    ret = db_write_record(clientid, &writeRecord, &storeNo);
    if(ret != 0)
    {
        RPT_FMT_DEBUG("写入任务上报报文失败,返回-%d\n", ret);
    }

    return ret;
}

/**
*********************************************************************
* @brief：     写事件统计状态
* @param[in]：rptInf   补报信息
* @return：    none
*********************************************************************
*/
void rpt_evtstat_write(uint16 confirmNum)
{
    int ret = 0;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_rptStatxmutex);

    g_rpt_stat_info.valid = TRUE;
    g_rpt_stat_info.evtconfirm += confirmNum; 

    ret = write_pdata_xram(RPT_STAT_FILE, (char*)&g_rpt_stat_info, 0, sizeof(RPT_STAT_T));
    if(ret != 0)
    {
        RPT_FMT_DEBUG("写文件%s失败,返回%d\n", RPT_STAT_FILE, ret);
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_rptStatxmutex);

    return;
}

/**
*********************************************************************
* @brief：     读事件统计状态
* @param[in]：rptInf   补报信息
* @return：    none
*********************************************************************
*/
int rpt_evtstat_read(uint16 *confirmNum)
{
    int ret = 0;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_rptStatxmutex);

    if(g_rpt_stat_info.valid == TRUE)
    {
        *confirmNum = g_rpt_stat_info.evtconfirm; 
    }
    else
    {
        ret = ERR_INVALID;
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_rptStatxmutex);

    return ret;
}

/**
*********************************************************************
* @brief：     清除事件统计状态
* @param[in]：rptInf   补报信息
* @return：    none
*********************************************************************
*/
void rpt_evtstat_clear(void)
{
    int ret = 0;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_rptStatxmutex);

    memset(&g_rpt_stat_info, 0, sizeof(RPT_STAT_T));

    ret = write_pdata_xram(RPT_STAT_FILE, (char*)&g_rpt_stat_info, 0, sizeof(RPT_STAT_T));
    if(ret != 0)
    {
        RPT_FMT_DEBUG("写文件%s失败,返回%d\n", RPT_STAT_FILE, ret);
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_rptStatxmutex);

    return;
}

/**
*********************************************************************
* @brief：     通信日志写队列
* @param[in]：rptInf   补报信息
* @return：    none
*********************************************************************
*/
void rpt_commlog_qset(OOP_RPT_LOG_T *plogfile)
{
    int ret = 0;
    
    if (plogfile == NULL)
    {
        PRTL_FMT_DEBUG("parm null plogfile[%p]\n", plogfile);
        return;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_rptlog.mutex);

    if((g_rptlog.fifo.wtptr+1)%256 == g_rptlog.fifo.rdptr)
    {
        g_rptlog.fifo.rdptr = (g_rptlog.fifo.rdptr+1)%256;
    }

    memcpy(&g_rptlog.fifo.fileinfo[g_rptlog.fifo.wtptr], plogfile, sizeof(OOP_RPT_LOG_T));
    
    g_rptlog.fifo.wtptr = (g_rptlog.fifo.wtptr+1)%256;

    ret = write_pdata_xram(RPT_LOGREC_FILE, (char*)&g_rptlog.fifo, 0, sizeof(RPT_LOGFIFO_T));
    if(ret != 0)
    {
        RPT_FMT_DEBUG("写队列文件%s失败,返回%d\n", RPT_LOGREC_FILE, ret);
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_rptlog.mutex);
    
    return;
}

/**
*********************************************************************
* @brief：     通信日志写队列
* @param[in]：rptInf   补报信息
* @return：    none
*********************************************************************
*/
void rpt_commlog_qget(OOP_RPT_LOGS_T *plogfile)
{
    int ret = 0;
    int i = 0;

    if (plogfile == NULL)
    {
        PRTL_FMT_DEBUG("parm null plogfile[%p]\n", plogfile);
        return;
    }
    
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_rptlog.mutex);

    for(i = 0; i < 256; i++)
    {
        if(g_rptlog.fifo.wtptr == g_rptlog.fifo.rdptr)
        {
            break;
        }

        memcpy(&plogfile->fileinfo[plogfile->num++], &g_rptlog.fifo.fileinfo[g_rptlog.fifo.rdptr], sizeof(OOP_RPT_LOG_T));
        
        g_rptlog.fifo.rdptr = (g_rptlog.fifo.rdptr+1)%256;
    }

    ret = write_pdata_xram(RPT_LOGREC_FILE, (char*)&g_rptlog.fifo, 0, sizeof(RPT_LOGFIFO_T));
    if(ret != 0)
    {
        RPT_FMT_DEBUG("写队列文件%s失败,返回%d\n", RPT_LOGREC_FILE, ret);
    }
    
    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_rptlog.mutex);
    
    return;
}

/**
*********************************************************************
* @brief：     通信日志写队列
* @param[in]：rptInf   补报信息
* @return：    none
*********************************************************************
*/
void rpt_commlog_qclr(void)
{
    int ret = 0;
    
    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_rptlog.mutex);

    memset(&g_rptlog.fifo, 0, sizeof(RPT_LOGFIFO_T));

    ret = write_pdata_xram(RPT_LOGREC_FILE, (char*)&g_rptlog.fifo, 0, sizeof(RPT_LOGFIFO_T));
    if(ret != 0)
    {
        RPT_FMT_DEBUG("写队列文件%s失败,返回%d\n", RPT_LOGREC_FILE, ret);
    }
    
    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_rptlog.mutex);
    
    return;
}

extern void current_tm_to_ooptm(OOP_DATETIME_S_T *poop_time);

/**********************************************************************
* @name      : rpt_task_msg
* @brief     ：透明任务上报通知
* @param[in] ：ack   确认消息
* @return    ：返回消息长度
* @Create    : 贺宁
* @Date      ：2020-04-20
* @Update    :
**********************************************************************/
void rpt_trans_task_msg(int clientid, TASK_MSG_t *pMsg)
{
    uint16 msglen = pMsg->msglen;
    uint8 *revcbuf = pMsg->msg;
    uint8  offset = 0;
    OOP_TRANS_TASK_RESULT_T data = {0};
    uint8 offlen = 0;
    OOP_RPT_LOG_T rptlog = {0};
    uint8 check = 0;

    /*通知内容        CHOICE
    {
          透明方案结果 [0] TRANS_RESULT
          日志文件路径 [1] visible-string
    }*/ 
    
    if(msglen != 0)
    {
        if(revcbuf[offset++] == 1)
        {
           //透明任务方案号
            memcpy_r(&rptlog.transTaskid, &revcbuf[offset], sizeof(uint16));
            offset += sizeof(uint16);

            //采集存储时标
            memcpy_r(&rptlog.storeTime.year, &revcbuf[offset], sizeof(uint16));
            offset += sizeof(uint16);

            rptlog.storeTime.month  = revcbuf[offset++];  
            rptlog.storeTime.day    = revcbuf[offset++]; 
            rptlog.storeTime.hour   = revcbuf[offset++]; 
            rptlog.storeTime.minute = revcbuf[offset++]; 
            rptlog.storeTime.second = revcbuf[offset++]; 

            //终端抄表时标
            current_tm_to_ooptm(&rptlog.readTime);
            
            //日志文件名
            offlen = revcbuf[offset++];

            memcpy(rptlog.filename, &revcbuf[offset], offlen);
            rpt_commlog_qset(&rptlog);
            
            RPT_FMT_DEBUG("通信日志文件:%s 任务-%d 存储时间-%4d-%02d-%02d %02d:%02d:%02d.\n", 
                rptlog.filename, rptlog.transTaskid, rptlog.storeTime.year, rptlog.storeTime.month,
                rptlog.storeTime.day, rptlog.storeTime.hour, rptlog.storeTime.minute, rptlog.storeTime.second);
            
        }
        else
        {
            //透明任务方案号
            memcpy_r(&data.transTaskid, &revcbuf[offset], sizeof(uint16));
            offset += sizeof(uint16);

            //具体任务编号
            memcpy_r(&data.nodeid, &revcbuf[offset], sizeof(uint16));
            offset += sizeof(uint16);

            //通信地址
            data.groupAdd.len = 5;
            memcpy(data.groupAdd.add, &revcbuf[offset], data.groupAdd.len+1); 
            offset += data.groupAdd.len + 1;

            //通信对象类型
            data.commType = revcbuf[offset++];

            //采集存储时标
            memcpy_r(&data.storeTime.year, &revcbuf[offset], sizeof(uint16));
            offset += sizeof(uint16);

            data.storeTime.month  = revcbuf[offset++];  
            data.storeTime.day    = revcbuf[offset++]; 
            data.storeTime.hour   = revcbuf[offset++]; 
            data.storeTime.minute = revcbuf[offset++]; 
            data.storeTime.second = revcbuf[offset++]; 

            //终端抄表时间
            memcpy_r(&data.readTime.year, &revcbuf[offset], sizeof(uint16));
            offset += sizeof(uint16);

            data.readTime.month  = revcbuf[offset++];  
            data.readTime.day    = revcbuf[offset++]; 
            data.readTime.hour   = revcbuf[offset++]; 
            data.readTime.minute = revcbuf[offset++]; 
            data.readTime.second = revcbuf[offset++]; 

            //存储序号
            memcpy_r(&data.storeNO, &revcbuf[offset], sizeof(uint16)); 
            offset += sizeof(uint16);

            //重试轮次
            data.retryTurn = revcbuf[offset++];

            //任务结果
            data.result = revcbuf[offset++];

            //响应报文格式字
            data.resultMessType = revcbuf[offset++];

            //响应报文内容-错误编码
            data.transMsg.errCode = revcbuf[offset++];

            //响应报文内容-返回数据
            data.transMsg.resultData.nNum = get_len_offset(&revcbuf[offset], &offlen);
            offset += offlen;

            memcpy(&data.transMsg.resultData.value[0], &revcbuf[offset], data.transMsg.resultData.nNum); 
            offset += data.transMsg.resultData.nNum;

            check = revcbuf[offset++];

            //内容写入上报或补报队列
            rpt_task_write(clientid, data.transTaskid, &data, sizeof(data), check);
        }      

        if(offset > msglen)
        {
            RPT_FMT_DEBUG("透明任务上报消息: offset(%d) > msglen(%d).\n", offset, msglen);
            return;
        }          
    }
    
    return;
}

/**
*********************************************************************
* @brief:       终端事件上报入口(通知方式上报)
* @detail:      获取到事件更新则检查上报标记，上报标记使能则写入上报队列
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int rpt_fj_evt_plan_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                ret = 0;
    uint32             readlen = 0;
    NOMAL_OAD_T        tOAD;
    OOP_TRANS_EVENT_REPORT_T event_rpt;

    if(0x6E330200 != value)
    {
        return -1;
    }
    
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
    
    ret = db_read_nomal(clientid, &tOAD, sizeof(OOP_TRANS_EVENT_REPORT_T), (uint8 *)&event_rpt, &readlen);
    if(0 != ret || readlen != sizeof(OOP_TRANS_EVENT_REPORT_T))
    {
        RPT_FMT_DEBUG("主动上报事件策略读取失败\n");
    }

    memcpy(&g_evt_plan, &event_rpt, sizeof(OOP_TRANS_EVENT_REPORT_T));
    
    return 0;    
}


/**
*********************************************************************
* @brief:       把最近的事件写入数据上报队列
* @detail:      上报标记使能则写入上报队列
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int rpt_event_write(int clientid, void* data, uint16 datalen)
{
    int ret         = 0;
    DATA_CONVERT_T convert;
    EVENT_REPORT_INFO eventReport;
    uint32 offsetTemp = 0;
    OOP_RCSD_T  evtRCSD = {0};
    OOP_DAR_E dar = DATA_SUCCESS;
    WRITE_RECORD_T  writeRecord = {0};
    uint32 storeNo = 0;
    RPT_FIFO_T *rptFifo = (RPT_FIFO_T *)writeRecord.inDataBuf;
    RPT_EVENT_PLAN_T tmpPlan = g_evt_plan;
    int timenow = get_time(TIME_MONOTONIC)+g_rpt_basetime;
    OOP_OAD_U mainOAD = {RPT_EVT_SUBLISTID};

    if(tmpPlan.retry == 0)
    {
        RPT_FMT_DEBUG("最大上报次数为0，不上报.\n");
        return 0;
    }
    
    //转化为报文
    memset(&eventReport, 0, sizeof(EVENT_REPORT_INFO));
    
    convert.srcBuf = data;
    convert.srcLen = datalen;
    convert.sOffset = &offsetTemp;
    
    convert.dOffset = &eventReport.dataLen;
    convert.dstBuf = eventReport.pdataBuf;
    convert.dstLen = EVENT_RECORD_MAX_LEN;

    evtRCSD.nNum = 1;
    evtRCSD.cols[0].choice = 0;
    evtRCSD.cols[0].oad.value = 0x6e320200;

    //构造事件上报报文
    dar = rpt_trans_result_apdu(mainOAD, evtRCSD, &convert);
    if(dar != DATA_SUCCESS)
    {
        RPT_FMT_DEBUG("构造事件上报报文失败,返回%d\n", dar);
        return dar;
    }

    //将报文写入上报队列
    writeRecord.recordOAD.road.oadMain.value = RPT_REPORT_LISTID; //上报OAD
    writeRecord.recordOAD.road.oadCols.nNum = 1;
    writeRecord.recordOAD.road.oadCols.oad[0].value = RPT_EVT_SUBLISTID;
    writeRecord.recordOAD.infoNum = tmpPlan.priority; //事件上报优先级
    writeRecord.colStartTm = timenow;
    writeRecord.colEndTm = writeRecord.colStartTm;
    writeRecord.colStoreTm = rpt_fj_index_get(); //全局唯一索引

    //存入上报队列
    
    rptFifo->taskID = RPT_EVT_SUBLISTID;
    rptFifo->timeOut = DT_TI2Seconds(tmpPlan.timeout);
    rptFifo->retryCnt = tmpPlan.retry;
    rptFifo->priority = tmpPlan.priority;
    
    rptFifo->dataLen = eventReport.dataLen;
    memcpy(rptFifo->databuf, eventReport.pdataBuf, eventReport.dataLen);
        
    writeRecord.inDatalen = sizeof(RPT_FIFO_T)+rptFifo->dataLen;

    RPT_BUF_DEBUG(rptFifo->databuf, rptFifo->dataLen, "[入队列]事件上报 内容编号-%d,超时时间-%d,重试次数-%d,优先级-%d:",  
        writeRecord.colStoreTm, rptFifo->timeOut, rptFifo->retryCnt, rptFifo->priority);
    
    ret = db_write_record(clientid, &writeRecord, &storeNo);
    if(ret != 0)
    {
        RPT_FMT_DEBUG("写入事件上报报文失败,返回%d\n", ret);
    }

    return ret;
}

/**
*********************************************************************
* @brief:       把最近的事件写入数据上报队列
* @detail:      上报标记使能则写入上报队列
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int rpt_retry_write(RPT_ENTRY_T *pEntry)
{
    int ret         = 0;
    WRITE_RECORD_T  writeRecord = {0};
    uint32 storeNo = 0;
    RPT_FIFO_T *rptFifo = (RPT_FIFO_T *)writeRecord.inDataBuf;
    int timenow = get_time(TIME_MONOTONIC)+g_rpt_basetime;

    //将报文写入上报队列
    writeRecord.recordOAD.road.oadMain.value = RPT_RETRYP_LISTID; //补报OAD
    writeRecord.recordOAD.road.oadCols.nNum = 1;
    writeRecord.recordOAD.road.oadCols.oad[0].value = pEntry->rptData.taskID;
    writeRecord.recordOAD.infoNum = pEntry->rptData.priority; //上报优先级
    writeRecord.colStartTm = timenow;
    writeRecord.colEndTm = writeRecord.colStartTm;
    writeRecord.colStoreTm = rpt_fj_index_get(); //全局唯一索引

    //存入补报队列
    rptFifo->taskID = pEntry->rptData.taskID;
    rptFifo->planID = pEntry->rptData.planID;
    rptFifo->timeOut = pEntry->rptData.timeOut;
    rptFifo->retryCnt = pEntry->rptData.retryCnt;
    rptFifo->priority = pEntry->rptData.priority;
    rptFifo->storeTime = pEntry->rptData.storeTime;
    rptFifo->mergeNum = pEntry->rptData.mergeNum;

    rptFifo->dataLen =  pEntry->rptData.rtrlen;
    memcpy(rptFifo->databuf, pEntry->rptData.rtrbuf, pEntry->rptData.rtrlen);

    writeRecord.inDatalen = sizeof(RPT_FIFO_T)+rptFifo->dataLen;

    if(rptFifo->taskID == RPT_EVT_SUBLISTID)
    {
        RPT_BUF_DEBUG(rptFifo->databuf, rptFifo->dataLen, "[入队列]事件补报 内容编号-%d,超时时间-%d,重试次数-%d,上报优先级-%d:",
            writeRecord.colStoreTm, rptFifo->timeOut, rptFifo->retryCnt, rptFifo->priority);
    }
    else
    {
        RPT_BUF_DEBUG(rptFifo->databuf, rptFifo->dataLen, "[入队列]任务补报 内容编号-%d,任务-%d,方案-%d,超时时间-%d,重试次数-%d,优先级-%d:",
            writeRecord.colStoreTm, rptFifo->taskID, rptFifo->planID, rptFifo->timeOut, rptFifo->retryCnt, rptFifo->priority);
    }
        
    ret = db_write_record(pEntry->dbClient, &writeRecord, &storeNo);
    if(ret != 0)
    {
        RPT_FMT_DEBUG("写入补报报报文失败,返回%d.\n", ret);
    }

    return ret;
}

/**
*********************************************************************
* @brief:       从上报队列中删除任务
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_task_fj_del(RPT_ENTRY_T *rpt_entry)
{
    int i = 0;
    int ret = 0;
    CLEAR_DATA_T clearData = {0};

    clearData.bFilter = TRUE;
    clearData.recordOAD.infoNum = 0xffff;
    clearData.recordOAD.road.oadMain.value = rpt_entry->listID;
    clearData.recordOAD.road.oadCols.oad[0].value = rpt_entry->rptData.taskID;

    for(i = 0; i< rpt_entry->rptData.idxNum; i++)
    {
        clearData.startFilter = rpt_entry->rptData.index[i];
        clearData.endFilter = clearData.startFilter;
        ret = db_clear_oad_data(rpt_entry->dbClient, &clearData);
        if(ret != ERR_OK)
        {
            RPT_FMT_DEBUG("删除队列0x%08x失败, 内容编号-%d ,返回-%d\n", rpt_entry->listID, clearData.startFilter, ret);
        }
    }
}

/**
*********************************************************************
* @brief:       福建任务上报执行
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_task_fj_wait(RPT_ENTRY_T *pEntry)
{
    char *typeStr[4] = {"任务上报", "终端事件上报", "电表事件上报", "补报"};
    RPT_RETRY_T baseRetry = {0};

    if(*pEntry->resStatus == RPT_RES_INITIAL)
    {
        pEntry->status = RPT_NEXT; 
        pEntry->realType = RPT_TYPE_UNDEF;
        return;
    }
    
    //判断主站是否回了确认帧
    if(*pEntry->resStatus == RPT_RES_ALREADY)
    {
        pEntry->timeOut = 0;
        pEntry->retryCnt = 0;
        pEntry->status = RPT_NEXT;  

        //非补报
        if(pEntry->type != RPT_TYPE_RETRY) 
        {
            if(pEntry->realType == RPT_TYPE_TASK)
            {
                //通知已经确认的条数
                g_rpt_transtask_state.confirmNum = g_rpt_transtask_state.reportNum;
                rpt_transtask_state_msg_send(&g_rpt_transtask_state, 1);
            }
            else if(pEntry->realType == RPT_TYPE_EVENT)
            {
                rpt_evtstat_write(g_rpt_evtrpt_num);
            }
        }
        else //补报
        {
            if(pEntry->realType == RPT_TYPE_TASK)
            {
                //通知已经确认的条数
                g_rpt_transtask_retry_state.confirmNum = g_rpt_transtask_retry_state.reportNum;
                rpt_transtask_state_msg_send(&g_rpt_transtask_retry_state, 1);
            }
            else if(pEntry->realType == RPT_TYPE_EVENT)
            {
                rpt_evtstat_write(g_rpt_evtrpt_retry_num);
            }
        }
        
        //从上报队列中删除本次上报的数据
        rpt_task_fj_del(pEntry);

        pEntry->realType = RPT_TYPE_UNDEF;
        pEntry->retryTag = FALSE;
        return;
    }

    //读取超时时间和重试次数
    baseRetry.timeOut = pEntry->rptData.timeOut;
    baseRetry.retryCnt = pEntry->rptData.retryCnt;

    //超时时间累积
    if(pEntry->curTime != pEntry->lstTime)
    {
       pEntry->timeOut++;
    }
    
    /* 超时重发,直到超过最大重发次数，过秒执行,计数1次即为1秒 */
    if(pEntry->timeOut > baseRetry.timeOut)  
    {
        RPT_FMT_DEBUG("%s 服务序号%d 超时: 已等待%d秒, 最多等待%d秒.\n", typeStr[pEntry->type], *pEntry->piid, pEntry->timeOut, baseRetry.timeOut);

        pEntry->timeOut = 0;
      
        if(++pEntry->retryCnt >= baseRetry.retryCnt)
        {
            RPT_FMT_DEBUG("%s 服务序号%d 重试完成.\n", typeStr[pEntry->type], *pEntry->piid);
            *pEntry->resStatus = RPT_RES_ALREADY; 
            pEntry->timeOut = 0; 
            pEntry->retryCnt = 0;
            pEntry->status = RPT_NEXT;

            if(pEntry->type != RPT_TYPE_RETRY)
            {
                //将上报数据保存到补报队列
                rpt_retry_write(pEntry);
            }

           //从上报队列中删除本次上报的数据
            rpt_task_fj_del(pEntry);
            
            pEntry->realType = RPT_TYPE_UNDEF;
            pEntry->retryTag = FALSE;
        }
        else
        {
            RPT_FMT_DEBUG("%s 服务序号%d 正在重试: 第%d次, 应小于%d次.\n", typeStr[pEntry->type], *pEntry->piid, pEntry->retryCnt, baseRetry.retryCnt);
            pEntry->status = RPT_RETRY;  
        }
    }
    else
    {
        pEntry->status = RPT_WAIT;
    }
    
    return;    
}

/**
*********************************************************************
* @brief:       福建任务上报执行
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_task_fj_next(RPT_ENTRY_T *rpt_entry)
{
    int ret = 0;
    STEP_READ_RECORD_T stepRead = {0};
    RECORD_UNIT_T      stepOut = {0};
    READ_RECORD_T readRecord = {0};
    uint32 stepHandle = 0;  //单步读句柄，最多4轮  
    uint32 recNum     = 0;  //记录条数   
    uint32 outRecNumAll = 0;  //总的要输出的记录条数
    uint32 ctrlRecNum = 0xffff; //控制每次读取的记录条数
    int i = 0;
    RECORD_UNIT_T   firststepOut; //本轮条用于后续记录合并的参考
    RPT_FIFO_T *rptFifo = (RPT_FIFO_T *)stepOut.record[0].outDataBuf;

    memset(&rpt_entry->rptData, 0, sizeof(RPT_DATA_T));
   
    readRecord.recordOAD.infoNum = 0xffff;
    readRecord.recordOAD.road.oadMain.value = rpt_entry->listID;
    readRecord.recordOAD.road.oadCols.nNum = 0;
    readRecord.cType = COL_TM_END;
    readRecord.sortType = UP_SORT;
    readRecord.cStart = 0;
    readRecord.cEnd = rpt_entry->curTime;
    readRecord.bTable = TRUE;

    //开始读
    ret = db_read_record_start(g_rptUdpHandle, &readRecord, &stepHandle, &recNum);
    if (ret != ERR_OK)
    {
        RPT_FMT_DEBUG("db_read_record_start failed. ret(%d), reNum(%d)\n", ret, recNum);
        return;
    }

    outRecNumAll = MIN(recNum, ctrlRecNum); 

    //单步读
    for(i = 0; i < outRecNumAll; i++)
    {
        ret = db_read_record_step(g_rptUdpHandle, stepHandle, &stepRead, &stepOut);
        if ((ret != ERR_OK) || (stepOut.recordnum == 0))
        {
            RPT_BUF_DEBUG(stepRead.MAC.value, 6, "db_read_record_step failed. ret(%d), recordnum(%d), mainOAD(0x%08x)\n", 
                ret, stepOut.recordnum, readRecord.recordOAD.road.oadMain.value);
            continue;
        }

        //本次长度已达到最大，下次再报，注意698报文上报格式头尾等固定部分还有23字节，这里仅拼装APDU的数据部分
        if(rpt_entry->rptData.rtrlen+rptFifo->dataLen > RPT_BUF_MAX-23)
        {
            break;
        }

        //首条报文，后面与之相同的透明任务上报的合并
        if(i == 0)
        {
            memcpy(&firststepOut, &stepOut, sizeof(RECORD_UNIT_T));
            rpt_entry->rptData.index[rpt_entry->rptData.idxNum++] = stepOut.colStoreTm;
            memcpy(&rpt_entry->rptData.rtrbuf[rpt_entry->rptData.rtrlen], rptFifo->databuf, rptFifo->dataLen);
            rpt_entry->rptData.rtrlen += rptFifo->dataLen;

            //次OAD保存的透明任务方案号或是透明事件集OAD
            rpt_entry->rptData.taskID = stepOut.record[0].road.oadCols.oad[0].value;
            rpt_entry->rptData.planID = rptFifo->planID;
            rpt_entry->rptData.timeOut = rptFifo->timeOut;
            rpt_entry->rptData.retryCnt = rptFifo->retryCnt;
            rpt_entry->rptData.priority = rptFifo->priority;
            rpt_entry->rptData.storeTime = rptFifo->storeTime;
                
            RPT_FMT_DEBUG("上报内容编号-%d\n", stepOut.colStoreTm);
        }
        else
        {
            //每次单步只包含一个透明任务,相同的透明任务且相同存储时间的报文合并上报
            if((stepOut.record[0].road.oadCols.oad[0].value == firststepOut.record[0].road.oadCols.oad[0].value)
                &&(memcmp(&rptFifo->storeTime,&rpt_entry->rptData.storeTime,sizeof(OOP_DATETIME_S_T)) == 0))
            {
                //去掉公共部分OAD+RCSD长度
                rpt_entry->rptData.index[rpt_entry->rptData.idxNum++] = stepOut.colStoreTm;
                memcpy(&rpt_entry->rptData.rtrbuf[rpt_entry->rptData.rtrlen], &rptFifo->databuf[12], rptFifo->dataLen-12);
                rpt_entry->rptData.rtrlen += rptFifo->dataLen-12;

                //修改合并后的记录条数,不会超过127条
                rpt_entry->rptData.rtrbuf[11] += rptFifo->databuf[11];

                RPT_FMT_DEBUG("上报内容编号-%d\n", stepOut.colStoreTm);
            }
        }

    }

    /*结束读*/
    db_read_record_end(g_rptUdpHandle, stepHandle); 

    //有数据需要上报则上报
    if(rpt_entry->rptData.idxNum > 0)
    {
        *rpt_entry->piid = RPT_ADD_VALUE_CLEAR(*rpt_entry->piid, 63, 1);

        //封装完整698报文
        rpt_entry->rptData.rptlen = rpt_packet_make(rpt_entry->rptData.rtrbuf, rpt_entry->rptData.rtrlen, 1, 0, *rpt_entry->piid, rpt_entry->rptData.rptbuf, RPT_BUF_MAX);

        if(rptFifo->taskID == RPT_EVT_SUBLISTID)
        {
            rpt_entry->realType = RPT_TYPE_EVENT;
            RPT_BUF_DEBUG(rpt_entry->rptData.rptbuf, rpt_entry->rptData.rptlen, "[出队列]事件上报 记录数-%d,超时时间-%d,重试次数-%d,优先级-%d:",
                rpt_entry->rptData.idxNum, rptFifo->timeOut, rptFifo->retryCnt, rptFifo->priority);
        }
        else
        {
            rpt_entry->realType = RPT_TYPE_TASK;
            RPT_BUF_DEBUG(rpt_entry->rptData.rptbuf, rpt_entry->rptData.rptlen, "[出队列]任务上报 记录数-%d,任务-%d,方案-%d,超时时间-%d,重试次数-%d,优先级-%d:",
                rpt_entry->rptData.idxNum, rptFifo->taskID, rptFifo->planID, rptFifo->timeOut, rptFifo->retryCnt, rptFifo->priority);
        }

        rpt_entry->rptData.mergeNum = rpt_entry->rptData.idxNum;
        
        rpt_entry->status = RPT_WAIT;
        *rpt_entry->resStatus = RPT_RES_WAITING; 
    }
}

/**
*********************************************************************
* @brief:       福建上报任务调度
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_task_fj_sched(RPT_ENTRY_T *pEntry)
{
    rpt_task_fj_wait(pEntry);

    switch(pEntry->status)
    {
        case RPT_NEXT:   //取得下一帧
        {
            rpt_task_fj_next(pEntry);
            pEntry->sendTag = TRUE;
        }break;

        case RPT_WAIT:  //等待主站回确认
        {
            pEntry->sendTag = FALSE;
        }break;
        
        case RPT_RETRY:
        {
            pEntry->status = RPT_WAIT; 
            pEntry->sendTag = TRUE;
            pEntry->retryTag = TRUE;
        }break;

        default:break;
    }
}

/**
*********************************************************************
* @brief:       福建任务上报入口
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_task_fj_entry(time_t curTime, time_t lstTime)
{
    int ret = 0;
    uint8 chID = 0; //上报通道ID
    static RPT_ENTRY_T rpt_entry = {0}; 

    if(!GetTnmEventReport())
    {
        return;
    }
    
    //上报任务检测,过秒执行
    if(curTime-lstTime)
    {
        rpt_entry.type = RPT_TYPE_TASK;
        rpt_entry.resStatus = &ConfirmFlag;
        rpt_entry.piid = &TaskGLBpiid;
        rpt_entry.lstTime = lstTime;
        rpt_entry.curTime = curTime;
        rpt_entry.listID = RPT_REPORT_LISTID;
        rpt_entry.dbClient = g_rptUdpHandle;
            
        //上报流程处理
        rpt_task_fj_sched(&rpt_entry);

        //上报报文发送
        if((rpt_entry.rptData.rptlen > 0) && (rpt_entry.sendTag == TRUE))
        {
            //查询可用的上报通道
            if(!rpt_channel_search(&chID))
            {
                RPT_FMT_DEBUG("通道断网 任务-%d 待补报.\n", rpt_entry.rptData.taskID);

                *rpt_entry.resStatus = RPT_RES_ALREADY; 
                rpt_entry.realType = RPT_TYPE_UNDEF;
                rpt_retry_write(&rpt_entry);
                return;
            }

            ret = send_emergency_ipcmsg(chID, 0, rpt_entry.rptData.rptbuf, rpt_entry.rptData.rptlen);
            if(ret != 0)
            {
                RPT_FMT_DEBUG("上报发送失败 任务-%d 通道号-%d\n", rpt_entry.rptData.taskID, chID);
                return;
            } 

            //首次上报通知上报的条数
            if(rpt_entry.retryTag == FALSE)
            {
                if(rpt_entry.realType == RPT_TYPE_TASK)
                {
                    g_rpt_transtask_state.taskid = rpt_entry.rptData.taskID;
                    g_rpt_transtask_state.storeTime = rpt_entry.rptData.storeTime;
                    g_rpt_transtask_state.reportNum = rpt_entry.rptData.mergeNum;
                    rpt_transtask_state_msg_send(&g_rpt_transtask_state, 0);
                }
                else if(rpt_entry.realType == RPT_TYPE_EVENT)
                {
                    g_rpt_evtrpt_num = rpt_entry.rptData.mergeNum;
                }                
            }
        }
    }
    
    return;
}

/**
*********************************************************************
* @brief:       福建任务上报执行
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int rpt_retry_recordnum(RPT_ENTRY_T *rpt_entry, uint32 *stepHandle)
{
    int ret = 0;
    READ_RECORD_T readRecord = {0};
    uint32 recNum     = 0;  //记录条数   

    readRecord.recordOAD.infoNum = 0xffff;
    readRecord.recordOAD.road.oadMain.value = rpt_entry->listID;

    if(rpt_entry->sublistID == 0xffff)
    {
        readRecord.recordOAD.road.oadCols.nNum = 0;
    }
    else
    {
        readRecord.recordOAD.road.oadCols.nNum = 1;
        readRecord.recordOAD.road.oadCols.oad[0].value = rpt_entry->sublistID; //指定任务筛选
    }
    
    readRecord.cType = COL_NO_INFO;
    readRecord.sortType = UP_SORT;
    readRecord.cStart = 0;
    readRecord.cEnd = 0xffff;
    readRecord.bTable = TRUE;

    //开始读
    ret = db_read_record_start(g_rptUdpHandle, &readRecord, stepHandle, &recNum);
    if (ret != ERR_OK)
    {
        RPT_FMT_DEBUG("db_read_record_start failed. ret(%d), reNum(%d)\n", ret, recNum);
        return recNum;
    }

    return recNum; 
}

/**
*********************************************************************
* @brief:       福建任务上报执行
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_retry_fj_next(RPT_ENTRY_T *rpt_entry, uint32 stepHandle)
{
    int ret = 0;
    STEP_READ_RECORD_T stepRead = {0};
    RECORD_UNIT_T      stepOut = {0};
    RPT_FIFO_T *rptFifo = (RPT_FIFO_T *)stepOut.record[0].outDataBuf;
    
    ret = db_read_record_step(g_rptUdpHandle, stepHandle, &stepRead, &stepOut);
    if ((ret != ERR_OK) || (stepOut.recordnum == 0))
    {
        RPT_BUF_DEBUG(stepRead.MAC.value, 6, "db_read_record_step failed. ret(%d), recordnum(%d)\n", 
            ret, stepOut.recordnum);
        return;
    }

    //由于事件和任务上报报文存在一起，所以要过滤掉事件
    if((rpt_entry->sublistID == 0xffff) && (stepOut.record[0].road.oadCols.oad[0].value == RPT_EVT_SUBLISTID))
    {
        return;
    }
        
    //本次长度已达到最大，下次再报，注意698报文上报格式头尾等固定部分还有23字节，这里仅拼装APDU的数据部分
    if(rpt_entry->rptData.rtrlen+rptFifo->dataLen > RPT_BUF_MAX-23)
    {
        return;
    }

    rpt_entry->rptData.index[rpt_entry->rptData.idxNum++] = stepOut.colStoreTm;
    memcpy(&rpt_entry->rptData.rtrbuf[rpt_entry->rptData.rtrlen], rptFifo->databuf, rptFifo->dataLen);
    rpt_entry->rptData.rtrlen += rptFifo->dataLen;
    
    //次OAD保存的透明任务方案号或是透明事件集OAD
    rpt_entry->rptData.taskID = stepOut.record[0].road.oadCols.oad[0].value;
    rpt_entry->rptData.planID = rptFifo->planID;
    rpt_entry->rptData.timeOut = rptFifo->timeOut;
    rpt_entry->rptData.retryCnt = rptFifo->retryCnt;
    rpt_entry->rptData.priority = rptFifo->priority;
    rpt_entry->rptData.storeTime = rptFifo->storeTime;
    rpt_entry->rptData.mergeNum = rptFifo->mergeNum;

    RPT_FMT_DEBUG("补报内容编号-%d\n", stepOut.colStoreTm);

    /*结束读*/
    db_read_record_end(g_rptUdpHandle, stepHandle); 

    *rpt_entry->piid = RPT_ADD_VALUE_CLEAR(*rpt_entry->piid, 63, 1);

    //封装完整698报文
    rpt_entry->rptData.rptlen = rpt_packet_make(rpt_entry->rptData.rtrbuf, rpt_entry->rptData.rtrlen, 1, 0, *rpt_entry->piid, rpt_entry->rptData.rptbuf, RPT_BUF_MAX);

    if(rptFifo->taskID == RPT_EVT_SUBLISTID)
    {
        rpt_entry->realType = RPT_TYPE_EVENT;
        RPT_BUF_DEBUG(rpt_entry->rptData.rptbuf, rpt_entry->rptData.rptlen, "[出队列]事件补报 记录数-%d,超时时间-%d,重试次数-%d,优先级-%d:",
            rpt_entry->rptData.idxNum, rptFifo->timeOut, rptFifo->retryCnt, rptFifo->priority);
    }
    else
    {
        rpt_entry->realType = RPT_TYPE_TASK;
        RPT_BUF_DEBUG(rpt_entry->rptData.rptbuf, rpt_entry->rptData.rptlen, "[出队列]任务补报 记录数-%d,任务-%d,方案-%d,超时时间-%d,重试次数-%d,优先级-%d:",
            rpt_entry->rptData.idxNum, rptFifo->taskID, rptFifo->planID, rptFifo->timeOut, rptFifo->retryCnt, rptFifo->priority);
    }

    rpt_entry->status = RPT_WAIT;
    *rpt_entry->resStatus = RPT_RES_WAITING; 
}

/*******************************************************************************
* 函数名称: comm_log_read 
* 函数功能:给通信日志上报方案用 每次从文件中读1k数据
* 输入参数:file 文件名字 commlog_22033119.tar.gz 文件由taskManager生成 puamr从文件读数据上报
        
* 输出参数: outbuf 读出的数据
            buflen 读出的数据长度
* 返 回 值: 1 文件读完了
            0 正常读到数据
            -1 失败
*******************************************************************************/

int comm_log_read(char *file,char *outbuf,uint32 *buflen)
{
    FILE *fp;
    static int index = 0;
    static char lastfile[100]={0};
    static int endflag = 0;
    char bufread[2000]={0};
    int readlen;
    if(NULL == file || NULL == outbuf || NULL == buflen)
        return -1;
    if(strcmp(lastfile,file)!=0)
     {
        index = 0;
        strcpy(lastfile,file);
    }
    else
    {
        if(endflag == 1)
        {
            endflag = 0;
            return 1;
        }
    }
        
    fp = fopen(file,"rb");
    if(NULL == fp)
    {
        return -2;
    }
    fseek(fp,index*1024,SEEK_SET);
    readlen = fread(bufread,1,1024,fp);
    index++;
    memcpy(outbuf,bufread,readlen);
    *buflen = readlen;

    if(feof(fp) !=0)
    {
        index = 0;
        endflag = 1;
    }
    
    fclose(fp);
    return 0;
}

/**
*********************************************************************
* @brief:       福建任务上报执行
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_retry_log_fj(RPT_ENTRY_T *rpt_entry, int *innerTaskID)
{
    int ret = 0;
    static int i = 0;
    OOP_TRANS_TASK_RESULT_T data = {0};
    OOP_OAD_U oad = {0x6E150200};
    OOP_RCSD_T rcsd = {.nNum=1, .cols[0].choice=0, .cols[0].oad={0x6E160200}};
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    static int segNum = 0;
    DATA_CONVERT_T convert;
    static OOP_RPT_LOGS_T rptlog = {0};
    uint8 *pAddr = NULL;

    //获取终端地址
    app_get_localaddr(&pAddr);
    
    data.transTaskid = rpt_entry->sublistID;
    data.storeNO = 1;
    data.retryTurn = 1;
    data.result = 1;
    data.groupAdd.len = 5;
    memcpy(data.groupAdd.add, pAddr, 6);
    
    data.transMsg.errCode = OOP_FJ_ERR_SUCCESS;

    rpt_commlog_qget(&rptlog);

    while(i < rptlog.num)
    {
        //读取抄表通信日志
        ret = comm_log_read(rptlog.fileinfo[i].filename, (char*)data.transMsg.resultData.value, &data.transMsg.resultData.nNum);
        if(ret != 0)
        {
            RPT_FMT_DEBUG("通信日志[%d]%s分%d次上报完成 返回%d.\n", i, rptlog.fileinfo[i].filename, segNum, ret);
            i++;
            segNum = 0;
            continue;
        }
        
        sOffset = 0;
        dOffset = 0;
        segNum++;
        memset(&rpt_entry->rptData, 0, sizeof(RPT_DATA_T));

        data.nodeid = segNum;
        data.storeTime = rptlog.fileinfo[i].storeTime;
        data.readTime = rptlog.fileinfo[i].readTime;

        //转化为报文        
        convert.srcBuf = &data;
        convert.srcLen = sizeof(OOP_TRANS_TASK_RESULT_T);
        convert.sOffset = &sOffset;
        
        convert.dOffset = &dOffset;
        convert.dstBuf = rpt_entry->rptData.rtrbuf;
        convert.dstLen = RPT_BUF_MAX;

        dar = rpt_trans_result_apdu(oad, rcsd, &convert);
        if(dar != DATA_SUCCESS)
        {
            RPT_FMT_DEBUG("构造通信日志上报报文失败,返回-%d\n", dar);
            break;
        }

        rpt_entry->rptData.rtrlen = dOffset;

        //封装完整698报文
        rpt_entry->rptData.rptlen = rpt_packet_make(rpt_entry->rptData.rtrbuf, rpt_entry->rptData.rtrlen, 1, 0, *rpt_entry->piid, rpt_entry->rptData.rptbuf, RPT_BUF_MAX);

        RPT_BUF_DEBUG(rpt_entry->rptData.rptbuf, rpt_entry->rptData.rptlen, "通信日志[%d]%s上报 透明任务-%d 具体任务-%d, 上报报文:",
            i, rptlog.fileinfo[i].filename, data.transTaskid, data.nodeid);
        
        break;
    }

    if(i == rptlog.num)
    {
        i = 0;
        (*innerTaskID)++;
        memset(&rptlog, 0, sizeof(OOP_RPT_LOGS_T));
    }
    
    return;
}

/**
*********************************************************************
* @brief:       福建上报任务调度
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_retry_fj_sched(RPT_ENTRY_T *pEntry)
{
    int ret = 0;
    static OOP_RPT_INF_T rpt_info = {0};
    static int innerTaskID = 0;
    OOP_TRANS_TASK_T tmpTask = {0};
    static uint8 evtTriNum = 0;
    uint32 stepHandle = 0;

    rpt_task_fj_wait(pEntry);

    switch(pEntry->status)
    {
        case RPT_NEXT:   //取得下一帧
        {
            //透明任务补报
            if(rpt_info.num == 0)
            {
                ret = get_report_trigger(&rpt_info);
            }

            if(rpt_info.num > 0)
            {
                while(innerTaskID < rpt_info.num)
                {
                    memset(&pEntry->rptData, 0, sizeof(RPT_DATA_T));
                    
                    ret = app_get_task_list_fj(rpt_info.taskId[innerTaskID], &tmpTask);
                    if((ret != 0) && (rpt_info.taskId[innerTaskID] != 0xffff))
                    {
                        innerTaskID++;
                        continue;
                    }
                
                    pEntry->sublistID = rpt_info.taskId[innerTaskID];
                    
                    if(tmpTask.taskTypeData.taskType != 4)
                    {
                        if(rpt_retry_recordnum(pEntry, &stepHandle) > 0)
                        {
                            rpt_retry_fj_next(pEntry, stepHandle);
                        }
                        else
                        {
                            innerTaskID++;
                            continue;
                        }
                    }
                    else //通信日志补报
                    {
                        rpt_retry_log_fj(pEntry, &innerTaskID);
                    }
                
                    break;
                }

                //本条补报指令完成
                if(innerTaskID == rpt_info.num)
                {
                    innerTaskID = 0;
                    memset(&rpt_info, 0, sizeof(OOP_RPT_INF_T));
                }
            }
            else
            {
                //透明事件补报
                if(evtTriNum == 0)
                {
                   evtTriNum = get_event_rpt_trigger();
                }

                if(evtTriNum > 0)
                {
                    while(innerTaskID < evtTriNum)
                    {
                        memset(&pEntry->rptData, 0, sizeof(RPT_DATA_T));
                       
                        pEntry->sublistID = RPT_EVT_SUBLISTID;
       
                        if(rpt_retry_recordnum(pEntry, &stepHandle) > 0)
                        {
                            rpt_retry_fj_next(pEntry, stepHandle);
                        }
                        else
                        {
                            innerTaskID++;
                            continue;
                        }
                        
                        break;
                    }

                    //本条补报指令完成
                    if(innerTaskID == evtTriNum)
                    {
                        innerTaskID = 0;
                        evtTriNum = 0;
                    }
                }                
            }
            
            pEntry->sendTag = TRUE;
        }break;

        case RPT_WAIT:  //等待主站回确认
        {
            pEntry->sendTag = FALSE;
        }break;
        
        case RPT_RETRY:
        {
            pEntry->status = RPT_WAIT; 
            pEntry->sendTag = TRUE;
        }break;

        default:break;
    }
}

/**
*********************************************************************
* @brief:       福建补报入口
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
void rpt_retry_fj_entry(time_t curTime, time_t lstTime)
{
    int ret = 0;
    uint8 chID = 0; //上报通道ID
    static RPT_ENTRY_T rpt_entry = {0}; 

    if(!GetTnmEventReport())
    {
        return;
    }
    
    //上报任务检测,过秒执行
    if(curTime-lstTime)
    {
        //查询可用的上报通道
        if(!rpt_channel_search(&chID))
        {
            RPT_FMT_TRACE("任务-%d 无可用的上报通道.\n", rpt_entry.rptData.taskID);
            g_rpt_online_status = FALSE;
            return;
        }

        //从离线到在线触发一次事件补报
        if(g_rpt_online_status == FALSE)
        {
            g_rpt_online_status = TRUE;
            set_event_rpt_trigger();
        }
        
        rpt_entry.type = RPT_TYPE_RETRY;
        rpt_entry.resStatus = &ConfirmFlag_retry;
        rpt_entry.piid = &suppGLBpiid;
        rpt_entry.lstTime = lstTime;
        rpt_entry.curTime = curTime;
        rpt_entry.listID = RPT_RETRYP_LISTID;
        rpt_entry.chID = chID;
        rpt_entry.dbClient = g_rptUdpHandle;
        
        //补报流程处理
        rpt_retry_fj_sched(&rpt_entry);

        //上报报文发送
        if((rpt_entry.rptData.rptlen > 0) && (rpt_entry.sendTag == TRUE))
        {
            ret = send_emergency_ipcmsg(chID, 0, rpt_entry.rptData.rptbuf, rpt_entry.rptData.rptlen);
            if(ret != 0)
            {
                RPT_FMT_DEBUG("send_emergency_ipcmsg failed. id(%d)\n", chID);
                return;
            } 

            //首次上报通知上报的条数
            if(rpt_entry.retryTag == FALSE)
            {
                if(rpt_entry.realType == RPT_TYPE_TASK)
                {
                    g_rpt_transtask_retry_state.taskid = rpt_entry.rptData.taskID;
                    g_rpt_transtask_retry_state.storeTime = rpt_entry.rptData.storeTime;
                    g_rpt_transtask_retry_state.reportNum = rpt_entry.rptData.mergeNum;
                    rpt_transtask_state_msg_send(&g_rpt_transtask_retry_state, 0);
                }
                else if(rpt_entry.realType == RPT_TYPE_EVENT)
                {
                    g_rpt_evtrpt_retry_num = rpt_entry.rptData.mergeNum;
                }
            }
        }

    }
    
    return;
} 

#endif

#if DESC("批量下发档案缓存",1)
CACHE_METER_LIST_T g_meterListCache;    //批量下发的档案缓存
extern OOP_DAR_E wait_oop_meter_data_update(uint32 infoNum);
extern int meter_data_clear(DB_CLIENT fd, uint16 infoNum);

/**
*********************************************************************
* @brief:       初始化
* @param[in]：   
* @param[out]： 
* @return：      void
*********************************************************************
*/
void cache_meter_init()
{
    memset(&g_meterListCache,0x00,sizeof(CACHE_METER_LIST_T));
    pthread_mutex_init(&g_meterListCache.lock, NULL);
}

/**
*********************************************************************
* @brief:       添加档案
* @param[in]：   meterFile
* @param[out]： 
* @return：      
*********************************************************************
*/
int cache_meter_add(OOP_METER_T *meterFile,uint16 num)
{
    uint16 i;
    pthread_mutex_lock(&g_meterListCache.lock);
    //第一帧的时候延时一秒回 看下效果
    //if(g_meterListCache.meterNum==0)
    //    sleep(2);
    for(i=0;i<num;i++)
    {
        if(g_meterListCache.meterNum >= NO_VIR_MTR_MAX_NUM)
        {
            PRTL_FMT_DEBUG("缓存的档案个数过多\n");
            continue;
        }
        //PRTL_FMT_DEBUG("add pn %d meterNum %d\n",meterFile[i].nIndex,g_meterListCache.meterNum);
        memcpy(&g_meterListCache.meterList[g_meterListCache.meterNum],&meterFile[i],sizeof(OOP_METER_T));
        g_meterListCache.meterNum++;
    }
    //重新设置延时时间
    g_meterListCache.delay = 3;
    pthread_mutex_unlock(&g_meterListCache.lock);
    return 0;
}

/**
*********************************************************************
* @brief：      有条件清除指定电能表的数据
* @param[in]： pApduInfo  apdu信息
* @param[in]： oldMeter   老电表档案数据
* @param[in]： curMeter   新电表档案数据
* @return：     =0, 成功 <0 失败
*********************************************************************
*/
int cache_meter_data_if_clear(DB_CLIENT client, OOP_METER_T *oldMeter, OOP_METER_T *curMeter)
{
    int ret = ERR_OK;
    CLEAR_DATA_T clear;

    MEMZERO(&clear, sizeof(clear));

    if((curMeter->nIndex != oldMeter->nIndex)||(!oop_tsa_equal(&curMeter->basic.tsa, &oldMeter->basic.tsa))||
        (curMeter->basic.port.value != oldMeter->basic.port.value)||
        (curMeter->basic.protocol != oldMeter->basic.protocol)||(curMeter->basic.userType != oldMeter->basic.userType))
    {
        PRTL_FMT_TRACE("meter changed, and collect data clear.\n");

        PRTL_BUF_TRACE(&oldMeter->basic.tsa, sizeof(OOP_TSA_T), "old->[index:(%d), port:(0x%08x), protocol:(%d), userType:(%d), tsa:]\n",
            oldMeter->nIndex, oldMeter->basic.port.value, oldMeter->basic.protocol,  oldMeter->basic.userType);

        PRTL_BUF_TRACE(&curMeter->basic.tsa, sizeof(OOP_TSA_T), "cur->[index:(%d), port:(0x%08x), protocol:(%d), userType:(%d), tsa:]\n",
            curMeter->nIndex, curMeter->basic.port.value, curMeter->basic.protocol,  curMeter->basic.userType);
        
        return meter_data_clear(client, oldMeter->nIndex);
    }

    return ret;
}


/**
*********************************************************************
* @brief:       过秒检查是否需要将档案写入数据中心
* @param[in]：   
* @param[out]： 
* @return：      
*********************************************************************
*/

void cache_meter_entry(DB_CLIENT client,time_t curtime,time_t lasttime)
{
    if(curtime == lasttime)
        return;
    uint16 i;
    NOMAL_OAD_T nomalOad;
    OOP_METER_T *meterFile = NULL;
    int meterret = ERR_OK;
    OOP_METER_T oldOopMeter;
    int ret;
    
    pthread_mutex_lock(&g_meterListCache.lock);
    do
    {
        if(g_meterListCache.meterNum == 0 )
        {
            break;
        }
        if(g_meterListCache.delay > 0)
        {
            g_meterListCache.delay--;
            break;
        }
        PRTL_FMT_DEBUG("准备写入档案 总个数%d \n",g_meterListCache.meterNum);
        //延时结束后把档案写数据中心
        for(i=0;i<g_meterListCache.meterNum;i++)
        {
            meterFile = &g_meterListCache.meterList[i];
            //PRTL_FMT_DEBUG("%d pn %d \n",i,meterFile->nIndex);
            memset(&nomalOad,0,sizeof(NOMAL_OAD_T));
            nomalOad.logicId = 0x00;
            nomalOad.infoNum = meterFile->nIndex;
            nomalOad.oad.value = 0x60000200;
            nomalOad.classtag = CLASS_DATA_INIT;
            //先读出原电表信息
            meterret = app_get_meter_list(nomalOad.infoNum, &oldOopMeter);
            if(meterret != ERR_OK)
            {
                PRTL_FMT_TRACE("get old meter list. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d\n",
                    meterret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value, sizeof(OOP_METER_T));
            }
            
            //置档案数据更新标志
            app_set_meter_update_start(nomalOad.infoNum);
            
            ret = db_write_nomal(client, &nomalOad, (uint8*)meterFile, sizeof(OOP_METER_T));
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("class11_invoke_act_oop_meter db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                    ret, nomalOad.logicId, nomalOad.infoNum, nomalOad.oad.value);
                continue;
            }
            
            //等待数据更新完成
            if (wait_oop_meter_data_update(nomalOad.infoNum) != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("app_set_meter_invalid err\n");
                continue;
            }
            
            //删档案，或通信地址，端口，规约类型，用户类型发生变化，则清电表数据
            if(meterret == ERR_OK)
            {
                ret = cache_meter_data_if_clear(client, &oldOopMeter, meterFile);
                if(ret != ERR_OK)
                {
                    PRTL_FMT_DEBUG("meter data clear failed. ret(%d)\n", ret);
                }
            }
        }
        PRTL_FMT_DEBUG("写入档案个数 %d\n",g_meterListCache.meterNum);
        g_meterListCache.meterNum = 0;
        memset(g_meterListCache.meterList,0,sizeof(g_meterListCache.meterList));
    }while(0);
    
    
    pthread_mutex_unlock(&g_meterListCache.lock);
}

#endif

