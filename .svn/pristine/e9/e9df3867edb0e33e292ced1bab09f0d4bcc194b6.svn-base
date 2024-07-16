/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：EventFun.c
* 版本号：1.0
* 功能：实现事件的检测与主动上报功能
* 原始开发人及时间：董宁 2006年12月25日
* 修改人员、时间及修改内容:无
* 其他：
******************************************************************-*/
#include "OopAll.h"
#include "task_id.h"
#include "framecomm.h"
#include "AppCommon.h"
#include "commClass.h"
#include "EventComm.h"
#include "Event.h"
#include "EventFun.h"
#include "oopVarUpdate.h"
#include "appoutmsg.h"
#include "msg_id.h"
#include "label_id.h"
#include "storage.h"
#include "area.h"
#include "hal.h"
#include "power.h"


EVENT_GLOBAL_T  event_global;       //-共享测量点数据结构-
uint8 a_ptrl[5],b_ptrl[5],c_ptrl[5];//-规约数据格式-
int16 vc_angle[3] = {0};	        //-三相电压电流夹角-

extern uint8 bCheckParaTemp[128];   //-检测终端参数时，临时存放读取的参数-

METER_HALT_STAT_STRUCT gMeterHalt_Info;//需要读取停电事件的结构信息


/**********************************************************************
* @name      : evt_read_nomal_data
* @brief     ：读取普通型数据
* @param[in] ：int    clientid     数据中心句柄
               uint8 logicId   逻辑设备号
               uint16 mtr_id   信息点号
               uint32 value_oad OAD
               uint16 datalen   数据长度
* @param[out]：uint8* pout_buf   数据
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2021-1-26
* @Update    :
**********************************************************************/
int evt_read_nomal_data(int clientid, uint8 logicid,uint16 infonum, uint32 value_oad, uint32 data_len, uint8 *pout_buf)
{
    int         ret     = -1;
    uint32      len     = 0;
    NOMAL_OAD_T nomalOAD;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    nomalOAD.logicId = logicid;
    nomalOAD.infoNum = infonum;
    nomalOAD.oad.value = value_oad;
    ret = db_read_nomal(clientid, &nomalOAD, data_len, pout_buf, &len);
    if(ret != ERR_OK &&  ret != ERR_NOTEXIST)
    {
        EVENT_FMT_DEBUG("读取普通数据[0x%08x]失败，返回%d\n", value_oad, ret);
        return ERR_NORMAL;
    }
	
	if(ret == ERR_OK &&  len != data_len)
	{
		EVENT_FMT_DEBUG("读取普通数据[0x%08x]失败，长度[%d]和入参[%d]不一致\n", value_oad, len, data_len);
		return ERR_NORMAL;
	}

    return ret;
}

/**********************************************************************
* @brief     ：事件写普通型数据
* @param[in] ：int    clientid     数据中心句柄
               uint8 logicId   逻辑设备号
               uint16 mtr_id   信息点号
               uint32 value_oad OAD
               uint16 datalen   数据长度
* @param[out]：uint8* pbuf   数据
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2021-1-26
* @Update    :
**********************************************************************/
int evt_write_nomal_data(int clientid, uint8 logicid,uint16 infonum, uint32 value_oad, uint8 *pbuf, uint32 data_len)
{
    int         ret     = -1;
    NOMAL_OAD_T nomalOAD;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    nomalOAD.logicId = logicid;
    nomalOAD.infoNum = infonum;
    nomalOAD.oad.value = value_oad;
#ifdef AREA_FUJIAN
	if(nomalOAD.oad.nObjID == 0x4E01 || nomalOAD.oad.nObjID == 0x4E02)
    {
        nomalOAD.classtag = CLASS_DATA_INIT;
    }
#endif
    ret = db_write_nomal(clientid, &nomalOAD, pbuf, data_len);
    if(ret != ERR_OK )
    {
        EVENT_FMT_DEBUG("写普通数据[0x%08x]失败，返回%d\n", value_oad, ret);
    }

    return ret;
}


/**********************************************************************
* @name      : GetTaskOopTI
* @brief     ：获取指定ID的任务的执行时间间隔
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-20
* @Update    :
**********************************************************************/
int GetTaskOopTI(uint8 taskID, OOP_TI_T *oopTI)
{
    OOP_TASK_T oopTaskTemp;
    
    if(oopTI == NULL)
    {
        return ERR_NORMAL;
    }
    
    memset(&oopTaskTemp, 0, sizeof(OOP_TASK_T));

    if(app_get_task_list(taskID, &oopTaskTemp) == ERR_OK)
    {
        *oopTI = oopTaskTemp.acqFreq;     
        EVENT_FMT_TRACE("Get GetTaskOopTI.unit = %hu OK!\n", oopTI->unit);
        EVENT_FMT_TRACE("Get GetTaskOopTI.value = %hu OK!\n", oopTI->value);
        return ERR_OK;
    }

    return ERR_NORMAL;
}

/**********************************************************************
* @brief     ：获取指定ID的任务的执行延时时间
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2021-06-18
* @Update    :
**********************************************************************/
int GetTaskOopDelay(uint8 taskID, OOP_TI_T *oopTI)
{
    OOP_TASK_T oopTaskTemp;
    
    if(oopTI == NULL)
    {
        return ERR_NORMAL;
    }
    
    memset(&oopTaskTemp, 0, sizeof(OOP_TASK_T));

    if(app_get_task_list(taskID, &oopTaskTemp) == ERR_OK)
    {
        *oopTI = oopTaskTemp.acqDelay;     
        EVENT_FMT_TRACE("Get GetTaskOopDelay.unit = %hu OK!\n", oopTI->unit);
        EVENT_FMT_TRACE("Get GetTaskOopDelay.value = %hu OK!\n", oopTI->value);
        return ERR_OK;
    }

    return ERR_NORMAL;
}


/**********************************************************************
* @name      : OopTIToSec
* @brief     ：执行时间间隔转换成秒
* @param[in] ：OOP_TI_T oopTI          执行时间间隔
* @param[out]：
* @return    ：秒数
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
uint32 OopTIToSec(OOP_TI_T oopTI)
{
    switch(oopTI.unit)
    {
    case 0:
        return oopTI.value;
    case 1:
        return oopTI.value*60;
    case 2:
        return oopTI.value*60*60;
    case 3:
        return oopTI.value*60*60*24;
    case 4:
 //       return oopTI.value*60*60*24*30;
    case 5:
 //       return oopTI.value*60*60*24*30*365;
    default:
        return CB_INTERVAL*60;
    }
}

/**********************************************************************
* @name      : getMsgLenOffset
* @brief     ：获取octet-string数据元素长度,偏移量
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：uint8 *bytes             偏移量
* @return    ：长度
* @Create    : 梅安宁
* @Date      ：2020-1-19
* @Update    :
**********************************************************************/
uint16 getMsgLenOffset(uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;
    
    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
    {
        return 0;
    }
    
    if(bytes != NULL)
        *bytes = nbytes;
        
    return EleNum;
}

#if DESC("事件关联数据写入数据中心",1)

/**********************************************************************
* @name      : WriteEventReportData
* @brief     ：写终端事件主动上报私有文件
* @param[in] ：const char *filename   事件上报私有文件名称
               OOP_OAD_U eventOAD     事件OAD
               uint8 *bDatabuf        事件内容
               uint32 bufLen          事件内容长度
* @param[out]：写入事件上报私有文件
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-01-11
* @Update    :
**********************************************************************/
int WriteEventReportData(const char *filename, EVENT_COMM_INFO *eventCommInfo, uint8 *bDatabuf, uint32 bufLen)
{
    uint32 evtOffset = 0;       //事件写入位置偏移
    uint16 tmpDataCount = 0;        //记录总条数
    DATA_CONVERT_T convert;
    EVENT_REPORT_INFO eventReport;
    uint32 offsetTemp = 0;

    if(bDatabuf == NULL)
        return -1;

    memset(&eventReport, 0, sizeof(EVENT_REPORT_INFO));
    
    convert.srcBuf = bDatabuf;
    convert.srcLen = bufLen;
    convert.sOffset = &offsetTemp;
    
    convert.dOffset = &eventReport.dataLen;
    convert.dstBuf = eventReport.pdataBuf;
    convert.dstLen = EVENT_RECORD_MAX_LEN;

    //构造事件上报报文
    record_response_make(eventCommInfo->eventOAD, eventCommInfo->eventRCSD, &convert);

    if(eventReport.dataLen > EVENT_RECORD_MAX_LEN)
        eventReport.dataLen = EVENT_RECORD_MAX_LEN;
    
    //读出目前记录总条数
    if(read_pdata_xram(filename, (char *)&tmpDataCount, EVENT_RECORD_CNT_OFFSET, 2) < 0)
    {
        tmpDataCount = 0;
    }

    tmpDataCount ++;

    if(tmpDataCount >= EVENT_RECORD_MAX_NUM)       //判断任务记录总条数是否越限
    {
        tmpDataCount = 0;
    }
    //计算文件写入位置偏移
    evtOffset = tmpDataCount*sizeof(EVENT_REPORT_INFO) + EVENT_REPORT_FILE_HEAD_LEN;
    EVENT_BUF_TRACE(bDatabuf,bufLen,"事件上报数据源:\n");
    EVENT_BUF_TRACE(eventReport.pdataBuf,eventReport.dataLen, "evtOffset = %d, recCnt = %d, 事件上报数据:\n", evtOffset, tmpDataCount);

    //上报到在线安全监测系统
    if(eventCommInfo->eventOAD.value == 0x31400200)
    {
        eventReport.cAddr = 1;
    }

    write_pdata_xram(filename, (const char *)&eventReport, evtOffset, sizeof(EVENT_REPORT_INFO));
    write_pdata_xram(filename, (const char *)&tmpDataCount, EVENT_RECORD_CNT_OFFSET, 2);
    return 0;
}

/**********************************************************************
* @name      : evt_report_data_deal
* @brief     ：事件上报数据处理
* @param[in] ：const char *filename   事件上报私有文件名称
               OOP_OAD_U eventOAD     事件OAD
               uint8   bType          事件状态 1发生，2恢复 
               uint8 *bDatabuf        事件内容
               uint32 bufLen          事件内容长度
* @param[out]：写入事件上报私有文件
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-01-11
* @Update    :
**********************************************************************//*
int evt_report_data_deal(const char *filename, EVENT_COMM_INFO *eventCommInfo, uint8 bType, uint8 *bDatabuf, uint32 bufLen)
{
    uint32 evtOffset = 0;       //事件写入位置偏移
    uint16 tmpDataCount = 0;        //记录总条数
    EVENT_REPORT_INFO tevtReport;

    if(bDatabuf == NULL)
        return -1;

    memset(&tevtReport, 0, sizeof(EVENT_REPORT_INFO));
    
    //构造事件上报报文
//    record_response_make(eventCommInfo->eventOAD, eventCommInfo->eventRCSD, &convert);

    tevtReport.evtType = bType;
    tevtReport.eventOAD = eventCommInfo->eventOAD;
    tevtReport.storagenum = eventCommInfo->storagenum;
    tevtReport.eventRCSD = eventCommInfo->eventRCSD;
    tevtReport.dataLen = bufLen;
    if(tevtReport.dataLen > EVENT_RECORD_MAX_LEN)
    {
        tevtReport.dataLen = EVENT_RECORD_MAX_LEN;
    }
    memcpy(tevtReport.pdataBuf, bDatabuf, bufLen);
    
    
    //读出目前记录总条数
    if(read_pdata_xram(filename, (char *)&tmpDataCount, EVENT_RECORD_CNT_OFFSET, 2) < 0)
    {
        tmpDataCount = 0;
    }

    tmpDataCount ++;

    if(tmpDataCount >= EVENT_RECORD_MAX_NUM)       //判断任务记录总条数是否越限
    {
        tmpDataCount = 0;
    }
    //计算文件写入位置偏移
    evtOffset = tmpDataCount*sizeof(EVENT_REPORT_INFO) + EVENT_REPORT_FILE_HEAD_LEN;
    EVENT_BUF_TRACE(bDatabuf,bufLen,"事件上报数据源:\n");
    EVENT_BUF_TRACE(tevtReport.pdataBuf,tevtReport.dataLen, "evtOffset = %d, recCnt = %d, 事件上报数据:\n", evtOffset, tmpDataCount);

    //上报到在线安全监测系统
    if(eventCommInfo->eventOAD.value == 0x31400200)
    {
        tevtReport.cAddr = 1;
    }

    write_pdata_xram(filename, (const char *)&tevtReport, evtOffset, sizeof(EVENT_REPORT_INFO));
    write_pdata_xram(filename, (const char *)&tmpDataCount, EVENT_RECORD_CNT_OFFSET, 2);
    return 0;
}
*/
/**********************************************************************
* @name      : SetEventNum
* @brief     ：终端事件记录条数记录
* @param[in] ：EVENT_COMM_INFO *eventCommInfo     事件基础信息       
* @param[out]：   
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-03-03
* @Update    :
**********************************************************************/
int SetEventNum(DB_CLIENT fd, EVENT_COMM_INFO *eventCommInfo)
{
    int             ret     = -1;
    NOMAL_OAD_T     nomalOAD;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    //事件记录条数
    eventCommInfo->eventNum++;
    nomalOAD.oad.value = Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0400);
    nomalOAD.classtag = CLASS_DATA_EVENT;
    ret = db_write_nomal(fd, &nomalOAD, (uint8 *)&eventCommInfo->eventNum, 2);
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_write_nomal oad[%08X]记录事件条数=%d失败返回%d\n", nomalOAD.oad.value, eventCommInfo->eventNum, ret);
    }
    EVENT_FMT_LOGCC("db_write_nomal oad[%08X]记录事件条数=%d成功\n", nomalOAD.oad.value, eventCommInfo->eventNum);
    return ret;
}


/**********************************************************************
* @name      : SetEventStat
* @brief     ：终端事件当前值记录
* @param[in] ：mtrnum_t          mtr_id            测量点号
               EVENT_COMM_INFO *eventCommInfo     事件基础信息
               OOP_EVTSOURCE_T  eventSource       事件发生源
               uint8            bType             事件状态 0(只记录事件，无发生，恢复区分)，1发生，2恢复        
* @param[out]：   
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-03-03
* @Update    :
**********************************************************************/
int SetEventStat(DB_CLIENT fd, mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, uint8 bType)
{
    int             ret     = -1;
    uint32          time    = 0;
    NOMAL_OAD_T     nomalOAD;
    
    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    eventCommInfo->eventState.source = eventSource;
    eventCommInfo->eventState.dwCount = eventCommInfo->eventNum;
    eventCommInfo->eventState.dbIdx = eventCommInfo->storagenum;

    //恢复事件
    if(bType == 2)
    {   
        ooptm_to_systm(eventCommInfo->endtime, (time_t *)&time);
        eventCommInfo->eventState.dwTimes += time - eventCommInfo->eventState.tmStart;
    }
    else
    {
        ooptm_to_systm(eventCommInfo->starttime, (time_t *)&time);
        eventCommInfo->eventState.tmStart = time;
    }

    //事件当前值记录
    nomalOAD.oad.value = Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0700);
    nomalOAD.classtag = CLASS_DATA_EVENT;
    ret = db_write_nomal(fd, &nomalOAD, (uint8 *)&eventCommInfo->eventState, sizeof(EVTSTATE));
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_write_nomal oad[%08X]记录事件当前值失败 返回%d\n", nomalOAD.oad.value, ret);
    }
    return ret;
}


/**********************************************************************
* @name      : SetEventTime
* @brief     ：终端事件时间状态记录
* @param[in] ：mtrnum_t          mtr_id            测量点号
               EVENT_COMM_INFO *eventCommInfo     事件基础信息
               OOP_EVTSOURCE_T  eventSource       事件发生源
               OOP_EVTBUF_T     eventData         事件关联对象数据
               uint8            bType             事件状态 0(只记录事件，无发生，恢复区分)，1发生，2恢复        
* @param[out]：   
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2019-12-22
* @Update    :
**********************************************************************/
int SetEventTime(DB_CLIENT fd, mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, uint8 bType)
{
    int             ret     = -1;
    NOMAL_OAD_T     nomalOAD;
    
    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));

    eventCommInfo->eventTime.source = eventSource;
    eventCommInfo->eventTime.dbIdx = eventCommInfo->storagenum;

    //恢复事件
    if(bType == 2)
    {   
        eventCommInfo->eventTime.tmEnd = eventCommInfo->endtime;
    }
    else
    {
        eventCommInfo->eventTime.tmStart = eventCommInfo->starttime;
    }

    //事件时间状态记录
    nomalOAD.oad.value = Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0A00);
    nomalOAD.classtag = CLASS_DATA_EVENT;
    ret = db_write_nomal(fd, &nomalOAD, (uint8 *)&eventCommInfo->eventTime, sizeof(EVTTIME));
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_write_nomal oad[%08X]记录事件时间状态失败 返回%d\n", nomalOAD.oad.value, ret);
    }
    return ret;
}

/**
*********************************************************************
* @brief:   判断当前信道是否允许上报报文
* @param[in]：   chOAD  通道对象属性
* @param[out]： chID   通道ID
* @return：     通道是否允许上报
*********************************************************************
*/
int rpt_channel_check(OOP_OAD_U chOAD, CHANNEL_CACHE_T *ptChannel)
{
    uint8 id = 0;
    uint8 startID = 0;
    uint8 endID = 0;
    CHANNEL_CACHE_T *tChannel = NULL;
    BOOLEAN valid = FALSE;

    switch(chOAD.nObjID)
    {
        case 0x4500: //GPRS1
        {
            startID = CHL_GPRS_1;
            endID = CHL_GPRS_1+0x19;
        }break;

        case 0x4501: //GPRS2
        {
            startID = CHL_GPRS_2;
            endID = CHL_GPRS_2+0x19;
        }break; 

        case 0x4510: //ETH1
        {
            startID = CHL_ETH_1;
            endID = CHL_ETH_1+0x19;
        }break;

        case 0x4511: //ETH2
        {
            startID = CHL_ETH_2;
            endID = CHL_ETH_2+0x19;
        }break;  

        default:
        {
            RPT_FMT_DEBUG("fault channel for task report. oda(0x%08x)\n", chOAD.value);
            return ERR_OPTION;
        }break;
    }

    //检索上报通道,获取对应的channel信息
    for(id = startID; id < endID; id++)
    {
        tChannel = app_get_channel(id);
        
        if((NULL == tChannel) || (FALSE == tChannel->bOpen) || (FALSE == tChannel->bConnect))
        {
            continue;
        }
        
        /* 判断通信介质 */
        if(COMM_MEDIA_GPRS != tChannel->tChannel.medium && COMM_MEDIA_ETH != tChannel->tChannel.medium)
        {
            continue;
        }

        /* 判断工作模式 */
        if((tChannel->tChannel.workmode != MODE_MIX) && (tChannel->tChannel.workmode != MODE_CLIENT))
        {
            continue;
        }

        valid = TRUE;
        //拷贝通道，需要修改其中参数
        memcpy_s(ptChannel, sizeof(CHANNEL_CACHE_T), tChannel, sizeof(CHANNEL_CACHE_T));
    }

    if(!valid)
    {
        RPT_FMT_DEBUG("not find channel for event report. oda(0x%08x)\n", chOAD.value);        
        return ERR_OPTION;
    }
    
    return ERR_OK;
}

/**
*********************************************************************
* @brief:       获取上报通道
* @param[in]： 
* @param[out]： pChOAD         通道oad 
* @return：      void
*********************************************************************
*/
int rpt_task_package(OOP_OAD_U *pChOAD)
{
    int ret = 0;
    CHANNEL_CACHE_T  tChannel;
    OOP_OADS_T tChOAD = {.nNum = 4, .oad = {{0x45100200},{0x45110200},{0x45000200},{0x45010200}}};
    uint8 i = 0;

	memset(&tChannel, 0, sizeof(CHANNEL_CACHE_T));

    //上报通道要么以太网, 要么GPRS, 优先使用以太网通道
    for(i = 0; i < tChOAD.nNum; i++)
    {
        //只要不失败，返回的不是GPRS就是以太网
        ret = rpt_channel_check(tChOAD.oad[i], &tChannel);
        if(ret != 0)
        {
            continue;
        }
        
        *pChOAD = tChOAD.oad[i];    

        //以GPRS通道可用则优先使用以GPRS通道
        if(tChannel.tChannel.medium == COMM_MEDIA_GPRS)
        {
            break;
        }
    }

    //ETH和GPRS通道都异常，则无法上报
    if((i == tChOAD.nNum) && (tChannel.tChannel.medium != COMM_MEDIA_GPRS) && (tChannel.tChannel.medium != COMM_MEDIA_ETH))
    {
        return 0;
    }

    return 1;
}


/**********************************************************************
* @name      : evt_get_oop_struct_data
* @brief     ：获取终端事件的698规约结构体数据
* @param[in] ：mtrnum_t          mtr_id            测量点号
               EVENT_COMM_INFO  *eventCommInfo    事件基础信息
               OOP_EVTSOURCE_T  eventSource       事件发生源
               OOP_EVTBUF_T     eventData         事件关联对象数据
               uint32           max_len           buf的最大长度 
               uint8            *pout_buf         数据内容
               uint8            bType             事件状态 0(只记录事件，无发生，恢复区分)，1发生，2恢复  
* @param[out]：   
* @return    ：0 失败， >0 数据长度
* @Create    : 梅安宁
* @Date      ：2019-12-22
* @Update    :
**********************************************************************/
int evt_get_oop_struct_data(mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint32 max_len, uint8 *pout_buf, uint8 bType)
{
    int                 out_len = 0;
    time_t              timep   = 0;
	uint8 				index	= 0;
    OOP_EVENT_T         oopEvent;        //标准事件单元

    memset(&oopEvent,0,sizeof(OOP_EVENT_T));

    oopEvent.source = eventSource;
    oopEvent.data = eventData;
    
    if(bType <= 1)
    {
        //事件发生时间
        current_tm_to_ooptm(&oopEvent.tmStart);
        //事件记录序号
        oopEvent.nIndex = eventCommInfo->recordnum+1;
    }
    else if(bType == 2)
    {
    	if (eventCommInfo->eventOAD.value == 0x36000200 || eventCommInfo->eventOAD.value == 0x36010200 || eventCommInfo->eventOAD.value == 0x36020200)
        {
        	switch(eventCommInfo->eventOAD.value)
			{
				case 0x36000200:
					index = event_3600.index;		//主板温度，CPU，内存，磁盘使用率超阈值，有发送和恢复
					break;
				case 0x36010200:
					index = event_3601.index;
					break;
				case 0x36020200:
					index = event_3602.index;
					break;
			}
			//恢复事件记录内容
	        read_pdata_xram("^Event", (char *)&eventCommInfo->storagenum, (eventCommInfo->EventStoragNumRecAddr+(index)*(4)), 4);
	        read_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(index)*(4)), 4);
			read_pdata_xram("^Event", (char *)&oopEvent.nIndex, (eventCommInfo->EventStoragIndexAddr+(index)*(4)), 4);
    	}
		else
		{
			//恢复事件记录内容
	        read_pdata_xram("^Event", (char *)&eventCommInfo->storagenum, (eventCommInfo->EventStoragNumRecAddr+(mtr_id)*(4)), 4);
	        read_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(mtr_id)*(4)), 4);
			//事件记录序号
			if(eventCommInfo->EventStoragIndexAddr != 0)		//电能表相关事件，同一时期，可以有多个事件发生和恢复，记录序号需要存储
			{
				read_pdata_xram("^Event", (char *)&oopEvent.nIndex, (eventCommInfo->EventStoragIndexAddr+(mtr_id)*(4)), 4);
			}
			else
			{
				oopEvent.nIndex = eventCommInfo->recordnum;
			}
		}
			

        //如果没有开始时间，不进行事件更新
        if(timep == 0)
        {
            return 0;
        }
        
        systm_to_ooptm(timep, &oopEvent.tmStart);
        current_tm_to_ooptm(&oopEvent.tmEnd);
        
    }

    //bType=1事件发生时，不记录结束时间，等事件恢复时更新
    if(bType == 0)
    {
        oopEvent.tmEnd = oopEvent.tmStart;
    }
#ifdef AREA_HUNAN
    //湖南温度告警事件特殊处理
    uint16 i;
    EVENT_3361_METER *meterinfo=NULL;
    if(eventCommInfo->eventOAD.value==0X33610200)
    {
        for(i=0;i<EVENT_3361_RECORD_COUNT;i++)
        {
            if(event_3361.info[i].isvalid==1&&mtr_id==event_3361.info[i].pn)
            {
                meterinfo = &event_3361.info[i];
                break;
            }
        }
        //事件发生 结束时间置0
        if(NULL==meterinfo)
        {
            memset(&oopEvent.tmEnd,0x00,sizeof(OOP_DATETIME_S_T));
            for(i=0;i<EVENT_3361_RECORD_COUNT;i++)
            {
                if(event_3361.info[i].isvalid==0)
                {
                    event_3361.info[i].isvalid = 1;
                    event_3361.info[i].pn = mtr_id;
                    event_3361.info[i].begintime = oopEvent.tmStart;
                    break;
                }
            }
        }else
        {
            oopEvent.tmStart = meterinfo->begintime;
            meterinfo->isvalid = 0;
            meterinfo->pn = 0;
        }
        write_pdata_xram("^event_3361",(char*)event_3361.info,0,sizeof(EVENT_3361_METER)*EVENT_3361_RECORD_COUNT);
    }
#endif

    switch(eventCommInfo->eventOAD.value)
    {
#ifdef AREA_SHAANXI
	//陕西ICCID变更事件
	case 0X31990200:			//ICCID变更事 
	{
		OOP_ICCID_CHANGE_EVENT_T   pEvent;
		memset(&pEvent,0,sizeof(OOP_ICCID_CHANGE_EVENT_T));
		
		memcpy(&pEvent, &oopEvent, sizeof(OOP_EVENT_T));
		pEvent.iccidBef = event_3199.iccidBef;
		pEvent.iccidCur = event_3199.iccidCur;
		
		out_len = sizeof(OOP_ICCID_CHANGE_EVENT_T);
		if(out_len <= max_len)
		{
			memcpy(pout_buf, &pEvent, out_len);
		}		 
		break;
	}
#endif

    case 0X31140200:            //时钟超差事件 
    {
        memcpy(&oopEvent.tmStart, eventData.buf, sizeof(OOP_DATETIME_S_T));
        memcpy(&oopEvent.tmEnd, eventData.buf+sizeof(OOP_DATETIME_S_T), sizeof(OOP_DATETIME_S_T));
        out_len = sizeof(OOP_EVENT_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &oopEvent, out_len);
        } 
        break;
    }
    case 0X31050200:            //时钟超差事件 
    {
        OOP_CLOCKFLYLOG_T   clockEvent;
        memset(&clockEvent,0,sizeof(OOP_CLOCKFLYLOG_T));
        
        memcpy(&clockEvent, &oopEvent, sizeof(OOP_EVENT_T));
        clockEvent.clkMeter = event_3105.mettime;
        clockEvent.clkTerm = event_3105.tmntime;
        
        out_len = sizeof(OOP_CLOCKFLYLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &clockEvent, out_len);
        }        
        break;
    }
	case 0X31110200:            //发现未知电能表事件
    {
        OOP_SMLOG_T         smEvent;
        memset(&smEvent,0,sizeof(OOP_SMLOG_T));
        
        smEvent.nIndex = oopEvent.nIndex;
        smEvent.tmStart = oopEvent.tmStart;
        smEvent.tmEnd = oopEvent.tmEnd;
        smEvent.evtState = oopEvent.state;
        smEvent.source = oopEvent.source;
        smEvent.nResNum = event_3111.MetNum;
        memcpy(smEvent.Res, event_3111.Res, sizeof(OOP_SEARCHMET_RES_T)*OOP_MAX_SMREPORT);
        
        out_len = sizeof(OOP_SMLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &smEvent, out_len);
        }
	    break;
    }
    case 0X311B0200:            //终端对电表校时事件
    {
        OOP_P2PCLOCKLOG_T p2pclock;
        memset(&p2pclock,0x00,sizeof(OOP_P2PCLOCKLOG_T));
        p2pclock.nIndex = oopEvent.nIndex;
        p2pclock.tmStart = oopEvent.tmStart;
        p2pclock.tmEnd = oopEvent.tmEnd;
        p2pclock.evtState = oopEvent.state;
        p2pclock.source = eventSource;
        p2pclock.preTime = event_311B.preTime;
        p2pclock.offset = event_311B.offset;
        out_len = sizeof(OOP_P2PCLOCKLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &p2pclock, out_len);
        }
        break;
    }
    case 0X311F0200:            //拒绝从节点入网记录事件事件
    {
        OOP_REFUSENODEACCESSLOG_T refusenode;
        memset(&refusenode,0x00,sizeof(OOP_REFUSENODEACCESSLOG_T));
        refusenode.nIndex = oopEvent.nIndex;
        refusenode.tmStart = oopEvent.tmStart;
        refusenode.tmEnd = oopEvent.tmEnd;
        refusenode.evtState = oopEvent.state;
        refusenode.source = eventSource;
        refusenode.info = event_311F.refuseslave;

        out_len = sizeof(OOP_REFUSENODEACCESSLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &refusenode, out_len);
        }
        break;
    }
    case 0X3E030200:            //拓扑识别事件
    {
        OOP_IdentifyResult_T identifyresult;
        memset(&identifyresult,0x00,sizeof(OOP_IdentifyResult_T));
        identifyresult.nIndex = oopEvent.nIndex;
        identifyresult.tmStart = oopEvent.tmStart;
        identifyresult.tmEnd = oopEvent.tmEnd;
        identifyresult.evtState = oopEvent.state;
        identifyresult.source = eventSource;
        identifyresult.IdentifyInformations = event_3E03.IdentifyInformations;

        EVENT_FMT_DEBUG("Identifynum=%d, signalphase=%d \n", 
            identifyresult.IdentifyInformations.Identifynum, identifyresult.IdentifyInformations.IdentifyInformation[0].signalphase);

        out_len = sizeof(OOP_IdentifyResult_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &identifyresult, out_len);
        }
        break;
    }
    case 0X31120200:            //跨台区电能表事件
    {
        OOP_UNMETERLOG_T unmeterlog;
        memset(&unmeterlog,0x00,sizeof(OOP_UNMETERLOG_T));
        unmeterlog.nIndex = oopEvent.nIndex;
        unmeterlog.tmStart = oopEvent.tmStart;
        unmeterlog.tmEnd = oopEvent.tmEnd;
        unmeterlog.evtState = oopEvent.state;
        unmeterlog.source = eventSource;
        unmeterlog.nResNum = 1;
        unmeterlog.Res[0] = event_3112.acrossarea;

        out_len = sizeof(OOP_UNMETERLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &unmeterlog, out_len);
        }
        break;
    }
    case 0X30300200:            //模块变更事件
    {
        OOP_MODULELOG_T modulelog;
        memset(&modulelog,0x00,sizeof(OOP_MODULELOG_T));
        modulelog.nIndex = oopEvent.nIndex;
        modulelog.tmStart = oopEvent.tmStart;
        modulelog.tmEnd = oopEvent.tmEnd;
        modulelog.evtState = oopEvent.state;
        modulelog.source = eventSource;
        modulelog.add.nNum = event_3030.nownode.add.len;
        memcpy(modulelog.add.value,event_3030.nownode.add.add ,event_3030.nownode.add.len);
        modulelog.preID = event_3030.prenode.id;
        modulelog.curID = event_3030.nownode.id;
        out_len = sizeof(OOP_MODULELOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &modulelog, out_len);
        }
        break;
    }
    case 0X31060200:            //停上电事件
    {
        OOP_POWERLOG_T      powerEvent;
        memset(&powerEvent,0,sizeof(OOP_POWERLOG_T));
        
	    memcpy(&powerEvent, &oopEvent, sizeof(OOP_EVENT_T));
        
        powerEvent.tmStart = event_3106.tmnStarttime;
        EVENT_FMT_TRACE("终端停电时间%u-%u-%u %u:%u:%u ishappen=%02x\n", event_3106.tmnStarttime.year, event_3106.tmnStarttime.month, event_3106.tmnStarttime.day, event_3106.tmnStarttime.hour, event_3106.tmnStarttime.minute, event_3106.tmnStarttime.second, event_3106.ishappen);
        if(event_3106.ishappen == 0xaa && eventSource.nValue == 1 )
        {   
            powerEvent.tmEnd = event_3106.tmnEndtime;
            EVENT_FMT_TRACE("终端上电时间%u-%u-%u %u:%u:%u \n", event_3106.tmnEndtime.year, event_3106.tmnEndtime.month, event_3106.tmnEndtime.day, event_3106.tmnEndtime.hour, event_3106.tmnEndtime.minute, event_3106.tmnEndtime.second);
        }
        else
        {
#ifdef AREA_SHANDONG
		
			powerEvent.tmEnd = event_3106.tmnEndtime;
#else
			memset(&powerEvent.tmEnd, 0, sizeof(OOP_DATETIME_S_T));
#endif

        }
        
//        powerEvent.flag.nValue = event_3106.eventFlag;

        powerEvent.flag.nValue = 0xc0;      //24规范都是正常，有效
		//字节反转
		membitrev(&powerEvent.flag.nValue, 8);

        out_len = sizeof(OOP_POWERLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &powerEvent, out_len);
        }
	    break;
    }
    case 0X31180200:            //终端编程事件
    {
        OOP_PROGRAMLOG_T    programEvent;
        memset(&programEvent,0,sizeof(OOP_PROGRAMLOG_T));
        
	    memcpy(&programEvent, &oopEvent, sizeof(OOP_EVENT_T));
        programEvent.nOADNum = event_3118.nOADNum;
        memcpy(programEvent.oad, event_3118.oad, sizeof(OOP_OAD_U)*OOP_MAX_OADS);

        out_len = sizeof(OOP_PROGRAMLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &programEvent, out_len);
        }
	    break;
    }
    case 0X311C0200:            //电能表数据变更监控
    {
        OOP_METERDIRTYLOG_T meterDirtyEvent;
        memset(&meterDirtyEvent,0,sizeof(OOP_METERDIRTYLOG_T));
        
	    meterDirtyEvent.nIndex = oopEvent.nIndex;
        meterDirtyEvent.tmStart = oopEvent.tmStart;
        if(bType == 1)
        {
            memset(&meterDirtyEvent.tmEnd,0xff,sizeof(OOP_DATETIME_S_T));
        }
        else
        {
            meterDirtyEvent.tmEnd = oopEvent.tmEnd;
        }
        
        meterDirtyEvent.evtState = oopEvent.state;
        meterDirtyEvent.source = oopEvent.source;
        meterDirtyEvent.di = event_311C.di;
        meterDirtyEvent.aftLen = event_311C.aftLen;
        meterDirtyEvent.preLen = event_311C.preLen;
        memcpy(meterDirtyEvent.aft, event_311C.aft, event_311C.aftLen);
        memcpy(meterDirtyEvent.pre, event_311C.pre, event_311C.preLen);

        out_len = sizeof(OOP_METERDIRTYLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &meterDirtyEvent, out_len);
        }
	    break;
    }
    case 0X31400200:            //安全事件变更记录   
    {
        OOP_SECURITYLOG_T   securityEvent;   //安全事件变更记录   
        memset(&securityEvent,0,sizeof(OOP_SECURITYLOG_T));
        
        securityEvent.nIndex = oopEvent.nIndex;
        securityEvent.tmStart = oopEvent.tmStart;
        securityEvent.tmEnd = oopEvent.tmEnd;
        securityEvent.evtState = oopEvent.state;
        securityEvent.source = oopEvent.source;
        memcpy(&securityEvent.flag, &event_3140.flag, sizeof(OOP_BITSTR16_T));
        securityEvent.nEthPort = event_3140.nEthPort;
        memcpy(securityEvent.ethPort, event_3140.ethPort, sizeof(event_3140.ethPort));
        securityEvent.nEthConnects = event_3140.nEthConnects;
        memcpy(securityEvent.ethConnects, event_3140.ethConnects, sizeof(event_3140.ethConnects));
        securityEvent.nLtePort = event_3140.nLtePort;
        memcpy(securityEvent.ltePort, event_3140.ltePort, sizeof(event_3140.ltePort));
        securityEvent.nLteConnects = event_3140.nLteConnects;
        memcpy(&securityEvent.lteConnects, &event_3140.lteConnects, sizeof(event_3140.lteConnects));
        memcpy(&securityEvent.usbInfo, &event_3140.usbInfo, sizeof(event_3140.usbInfo));
        memcpy(&securityEvent.rs232Info, &event_3140.rs232Info, sizeof(event_3140.rs232Info));
        securityEvent.nChgPath = event_3140.nChgPath;
        memcpy(securityEvent.chgPath, event_3140.chgPath, sizeof(event_3140.chgPath));
        memcpy(&securityEvent.ethFlag, &event_3140.ethFlag, sizeof(event_3140.ethFlag));
        memcpy(&securityEvent.rs232Flag, &event_3140.rs232Flag, sizeof(event_3140.rs232Flag));
        memcpy(&securityEvent.errFlag, &event_3140.errFlag, sizeof(event_3140.errFlag));

        //22版本新增功能
        #if SEC_MONITOR_NEW_22 == 1
        memcpy(&securityEvent.errSSH, &event_3140.errSSH, sizeof(event_3140.errSSH));
        memcpy(&securityEvent.errSession, &event_3140.errSession, sizeof(event_3140.errSession));
        memcpy(&securityEvent.errStream, &event_3140.errStream, sizeof(event_3140.errStream));
        memcpy(&securityEvent.errProg, &event_3140.errProg, sizeof(event_3140.errProg));
        memcpy(&securityEvent.errLogin, &event_3140.errLogin, sizeof(event_3140.errLogin));
        #endif
        
        out_len = sizeof(OOP_SECURITYLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &securityEvent, out_len);
        }
        break;
    }
    case 0X31150200:            //遥控跳闸
    {
        OOP_RCTRLLOG_T rctrllogEvent;
        memset(&rctrllogEvent, 0, sizeof(OOP_RCTRLLOG_T));

	    rctrllogEvent.nIndex = oopEvent.nIndex;
        rctrllogEvent.tmStart = oopEvent.tmStart;
//        rctrllogEvent.tmEnd = oopEvent.tmEnd;
        rctrllogEvent.evtState = oopEvent.state;
        rctrllogEvent.source = oopEvent.source;
		rctrllogEvent.cmd = event_3115.rCtrlType;
        if(bType == 1)
        {
            memset(rctrllogEvent.prePower, 0xee, sizeof(int64)*8);
        }
        else
        {
            memcpy(rctrllogEvent.prePower, event_3115.prePower, sizeof(int64)*8);
        }
        memcpy(rctrllogEvent.llPower, event_3115.llPower, sizeof(int64)*8);

        out_len = sizeof(OOP_RCTRLLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &rctrllogEvent, out_len);
        }
	    break;
    }
    case 0X32000200:            //功控跳闸
    {
        OOP_PCTRLLOG_T pctrllogEvent;
        memset(&pctrllogEvent, 0, sizeof(OOP_PCTRLLOG_T));

	    pctrllogEvent.nIndex = oopEvent.nIndex;
        pctrllogEvent.tmStart = oopEvent.tmStart;
//        pctrllogEvent.tmEnd = oopEvent.tmEnd;
        pctrllogEvent.evtState = oopEvent.state;
        pctrllogEvent.source = oopEvent.source;
        pctrllogEvent.objCtrl = event_3200.objCtrl;
        pctrllogEvent.turnState = event_3200.turnState;
		//字节反转
//		membitrev(&pctrllogEvent.turnState.nValue, 8);
        if(bType == 1)
        {
            memset(&pctrllogEvent.power2Min, 0xee, sizeof(int64));
        }
        else
        {
            pctrllogEvent.power2Min = event_3200.power2Min;
        }
        pctrllogEvent.llSetting = event_3200.llSetting;
        pctrllogEvent.power = event_3200.power;
        
        out_len = sizeof(OOP_PCTRLLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &pctrllogEvent, out_len);
        }
	    break;
    }
    case 0X32010200:            //电控跳闸
    {
        OOP_ECTRLLOG_T ectrllogEvent;
        memset(&ectrllogEvent, 0, sizeof(OOP_ECTRLLOG_T));

	    ectrllogEvent.nIndex = oopEvent.nIndex;
        ectrllogEvent.tmStart = oopEvent.tmStart;
//        ectrllogEvent.tmEnd = oopEvent.tmEnd;
        ectrllogEvent.evtState = oopEvent.state;
        ectrllogEvent.data = oopEvent.data;
        ectrllogEvent.source = oopEvent.source;
        ectrllogEvent.objCtrl = event_3201.objCtrl;
        ectrllogEvent.turnState = event_3201.turnState;
		//字节反转
//		membitrev(&ectrllogEvent.turnState.nValue, 8);
        ectrllogEvent.llSetting = event_3201.llSetting;
        ectrllogEvent.energy = event_3201.energy;
        
        out_len = sizeof(OOP_ECTRLLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &ectrllogEvent, out_len);
        }
	    break;
    }
    case 0X32030200:            //电控告警
    {
        OOP_EWARNLOG_T ewarnEvent;
        memset(&ewarnEvent, 0, sizeof(OOP_EWARNLOG_T));

	    ewarnEvent.nIndex = oopEvent.nIndex;
        ewarnEvent.tmStart = oopEvent.tmStart;
//        ewarnEvent.tmEnd = oopEvent.tmEnd;
        ewarnEvent.evtState = oopEvent.state;
        ewarnEvent.data = oopEvent.data;
        ewarnEvent.source = oopEvent.source;
        ewarnEvent.objCtrl = event_3203.objCtrl;
        ewarnEvent.llSetting = event_3203.llSetting;
        
        out_len = sizeof(OOP_EWARNLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &ewarnEvent, out_len);
        }
	    break;
    }
	case 0X36000200:         
    {
        OOP_MONITORLOG_T systemmonitor;
    	memset(&systemmonitor, 0, sizeof(OOP_MONITORLOG_T));

	    systemmonitor.nIndex = oopEvent.nIndex;
        systemmonitor.tmStart = oopEvent.tmStart;
        systemmonitor.tmEnd = oopEvent.tmEnd;
        systemmonitor.evtState = oopEvent.state;
        systemmonitor.source = oopEvent.source;
        systemmonitor.warn = event_3600.warnType;
        systemmonitor.containerName = event_3600.containerName;
		systemmonitor.appName = event_3600.appName;
        
        out_len = sizeof(OOP_MONITORLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &systemmonitor, out_len);
        }
	    break;
    }
	case 0X36010200:         
    {
        OOP_MONITORLOG_T systemmonitor;
    	memset(&systemmonitor, 0, sizeof(OOP_MONITORLOG_T));

	    systemmonitor.nIndex = oopEvent.nIndex;
        systemmonitor.tmStart = oopEvent.tmStart;
        systemmonitor.tmEnd = oopEvent.tmEnd;
        systemmonitor.evtState = oopEvent.state;
        systemmonitor.source = oopEvent.source;
        systemmonitor.warn = event_3601.warnType;
        systemmonitor.containerName = event_3601.containerName;
		systemmonitor.appName = event_3601.appName;
        
        out_len = sizeof(OOP_MONITORLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &systemmonitor, out_len);
        }
	    break;
    }
	case 0X36020200:         
    {
        OOP_MONITORLOG_T systemmonitor;
    	memset(&systemmonitor, 0, sizeof(OOP_MONITORLOG_T));

	    systemmonitor.nIndex = oopEvent.nIndex;
        systemmonitor.tmStart = oopEvent.tmStart;
        systemmonitor.tmEnd = oopEvent.tmEnd;
        systemmonitor.evtState = oopEvent.state;
        systemmonitor.source = oopEvent.source;
        systemmonitor.warn = event_3602.warnType;
        systemmonitor.containerName = event_3602.containerName;
		systemmonitor.appName = event_3602.appName;
        
        out_len = sizeof(OOP_MONITORLOG_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &systemmonitor, out_len);
        }
	    break;
    }
    default:                    //标准事件单元
        out_len = sizeof(OOP_EVENT_T);
        if(out_len <= max_len)
        {
            memcpy(pout_buf, &oopEvent, out_len);
        }  
    }

    if(out_len > max_len)
    {
        EVENT_FMT_DEBUG("evt_get_writedb_record_data oda[%08X],out_len[%d]>max_len[%u]\n", eventCommInfo->eventOAD.value, out_len, max_len);
        return 0;
    }

    return out_len;
}

/**********************************************************************
* @name      : evt_report_state_update
* @brief     ：更新事件的通道上报状态
* @param[in] ：uint8 *pevt_data       事件数据
               OOP_REPORTS_T state    事件通道上报状态
* @param[out]：   
* @return    ：
* @Create    : 梅安宁
* @Date      ：2021-2-4
* @Update    :
**********************************************************************/
void evt_report_state_update(uint8 *pevt_data, OOP_REPORTS_T state)
{
    OOP_EVENT_T         *pevtTmp = (OOP_EVENT_T*)pevt_data;
    pevtTmp->state = state;
    return ;
}


/**********************************************************************
* @name      : evt_db_write
* @brief     ：终端事件写入数据库
* @param[in] ：DB_CLIENT fd            数据中心句柄
               EVENT_COMM_INFO *eventCommInfo     事件基础信息
               uint8 *pevt_data       事件数据
               uint32 data_len        事件数据长度
               OOP_REPORTS_T state    事件通道上报状态
* @param[out]：   
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2021-2-4
* @Update    :
**********************************************************************/
int evt_db_write(DB_CLIENT fd, EVENT_COMM_INFO *eventCommInfo, uint8 *pevt_data, uint32 data_len, OOP_REPORTS_T state)
{  
    time_t              timep = 0;
    WRITE_RECORD_T      recInData;   
    
    memset(&recInData,0,sizeof(WRITE_RECORD_T));

    evt_report_state_update(pevt_data, state);
    
    time(&timep); /*获取time_t类型的当前时间*/
    recInData.recordOAD.classtag = CLASS_DATA_EVENT;
    recInData.recordOAD.road.oadCols.nNum = 1;
    recInData.recordOAD.road.oadCols.oad[0] = eventCommInfo->eventOAD;
//    recInData.MAC = MAC;
    recInData.colStartTm = timep + COMPENSATION_TIME;
    recInData.colEndTm = timep + COMPENSATION_TIME;
    recInData.colStoreTm = eventCommInfo->recordnum+1;

    recInData.inDatalen = data_len;
    memcpy(recInData.inDataBuf, pevt_data, data_len);

    return db_write_record(fd, &recInData, &eventCommInfo->storagenum);
    
}


/**********************************************************************
* @name      : evt_db_update
* @brief     ：更新数据中心中的事件
* @param[in] ：DB_CLIENT fd            数据中心句柄
               OOP_OAD_U evt_oad      事件oad
               uint32 storagenum      事件存储序号
               uint8 *pevt_data       事件数据
               uint32 data_len        事件数据长度
               OOP_REPORTS_T state    事件通道上报状态
* @param[out]：   
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2021-2-4
* @Update    :
**********************************************************************/
int evt_db_update(DB_CLIENT fd, OOP_OAD_U evt_oad, uint32 storagenum, uint8 *pevt_data, uint32 data_len, OOP_REPORTS_T state)
{  
    UPDATE_RECORD_T     updateRecordData;
    
    memset(&updateRecordData,0,sizeof(UPDATE_RECORD_T));

    evt_report_state_update(pevt_data, state);

    updateRecordData.recordOAD.optional = 0;
    updateRecordData.recordOAD.road.oadCols.nNum = 1;
    updateRecordData.recordOAD.road.oadCols.oad[0] = evt_oad;
//    updateRecordData.MAC = MAC;
    updateRecordData.filter.choice = 2;

    updateRecordData.inDatalen = data_len;
    updateRecordData.filter.storeNo = storagenum;
    memcpy(updateRecordData.inDataBuf, pevt_data, data_len);

    return db_update_record(fd, &updateRecordData);
}

/**********************************************************************
* @name      : WriteEventToDB
* @brief     ：终端事件写入数据库
* @param[in] ：mtrnum_t          mtr_id            测量点号
               EVENT_COMM_INFO *eventCommInfo     事件基础信息
               OOP_EVTSOURCE_T  eventSource       事件发生源
               OOP_EVTBUF_T     eventData         事件关联对象数据
               uint8            bType             事件状态 0(只记录事件，无发生，恢复区分)，1发生，2恢复        
* @param[out]：   
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2019-12-22
* @Update    :
**********************************************************************/
int WriteEventToDB(DB_CLIENT fd, mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint8 bType)
{ 
    int                 ret                    = 0;
    time_t              timep                  = 0;
    uint8               bDatabuf[DATA_LEN_MAX] = {0};
    uint32              bufLen                 = 0;   
    OOP_REPORTS_T       state;
	uint8 				index				   = 0;

    memset(&state,0,sizeof(OOP_REPORTS_T));

    //停上电事件特殊处理，有效性在此判断，其他逻辑，不管有效性都一样处理
    if(eventCommInfo->eventOAD.value == 0x31060200  && eventCommInfo->checkflag == 0 )
    {
        return 0;
    }

    //上报状态
    state.nNum = rpt_task_package(&state.state[0].oad);

    //获取事件数量和记录序号
    if(eventCommInfo->recordnum == 0)
    {
        GetEventNumAndIndex(fd, eventCommInfo->eventOAD.value, &eventCommInfo->eventNum, &eventCommInfo->recordnum);
    }
    
    EVENT_FMT_DEBUG("eventCommInfo->eventNum is %d,eventCommInfo->recordnum is %d\n", eventCommInfo->eventNum, eventCommInfo->recordnum);
    bufLen = evt_get_oop_struct_data(mtr_id, eventCommInfo, eventSource, eventData, DATA_LEN_MAX, bDatabuf, bType);
    

#ifdef AREA_FUJIAN
	if(gGather == OOP_GATHER_FJ)		//福建增补
	{
		evt_write_data_FJ(fd, mtr_id, eventCommInfo, eventSource, eventData, bType);
	}
#endif

    if(bufLen == 0)
    {
        return ERR_O_RANGE;
    }

    if(bType <= 1)//发生
    {
        ret = evt_db_write(fd, eventCommInfo, bDatabuf, bufLen, state);
        if (ret != ERR_OK)
        {
            return ret;
        }
        
        //事件记录条数
        SetEventNum(fd,eventCommInfo);

		eventCommInfo->recordnum++;  
        
		if (eventCommInfo->eventOAD.value == 0x31150200)	//遥控只有跳闸会有两分钟后功率
        {
            if(event_3115.rCtrlType == 0 && bType == 1) 	//遥控只有跳闸会有两分钟后功率
	        {
	            //记录存储序号和事件开始时间
	            write_pdata_xram("^Event", (char *)&eventCommInfo->storagenum, (eventCommInfo->EventStoragNumRecAddr+(event_3115.turn-1)*(4)), 4);
	            time(&timep); /*获取time_t类型的当前时间*/
	            write_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(event_3115.turn-1)*(4)), 4);
	            write_pdata_xram("^Event", (char *)&eventCommInfo->recordnum, (eventCommInfo->EventStoragIndexAddr+(event_3115.turn-1)*(4)), 4);
	        }
        }
		else if (eventCommInfo->eventOAD.value == 0x32000200)	//两分钟后功率
        {
            if(bType == 1) 
	        {
	            //记录存储序号和事件开始时间
	            write_pdata_xram("^Event", (char *)&eventCommInfo->storagenum, (eventCommInfo->EventStoragNumRecAddr+mtr_id*EventStoragCtrlRecLen+(event_3200.turn-1)*(4)), 4);
	            time(&timep); /*获取time_t类型的当前时间*/
	            write_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+mtr_id*EventStoragCtrlRecLen+(event_3200.turn-1)*(4)), 4);
		        write_pdata_xram("^Event", (char *)&eventCommInfo->recordnum, (eventCommInfo->EventStoragIndexAddr+mtr_id*EventStoragCtrlRecLen+(event_3200.turn-1)*(4)), 4);
	        }
        }
		else if (eventCommInfo->eventOAD.value == 0x36000200 || eventCommInfo->eventOAD.value == 0x36010200 || eventCommInfo->eventOAD.value == 0x36020200)
        {
        	switch(eventCommInfo->eventOAD.value)
			{
				case 0x36000200:
					index = event_3600.index;		//主板温度，CPU，内存，磁盘使用率超阈值，有发送和恢复
					break;
				case 0x36010200:
					index = event_3601.index;
					break;
				case 0x36020200:
					index = event_3602.index;
					break;
			}
            if(bType == 1) 
	        {
	            //记录存储序号和事件开始时间
	            write_pdata_xram("^Event", (char *)&eventCommInfo->storagenum, (eventCommInfo->EventStoragNumRecAddr+(index)*(4)), 4);
	            time(&timep); /*获取time_t类型的当前时间*/
	            write_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(index)*(4)), 4);
				write_pdata_xram("^Event", (char *)&eventCommInfo->recordnum, (eventCommInfo->EventStoragIndexAddr+(index)*(4)), 4);
	        }
        }
        else if(bType == 1) //电能表相关
        {
            //记录存储序号和事件开始时间
            write_pdata_xram("^Event", (char *)&eventCommInfo->storagenum, (eventCommInfo->EventStoragNumRecAddr+(mtr_id)*(4)), 4);
            time(&timep); /*获取time_t类型的当前时间*/
            write_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(mtr_id)*(4)), 4);

			//电能表相关事件，同一时期，可以有多个事件发生和恢复，记录序号需要存储
			if(eventCommInfo->EventStoragIndexAddr != 0)
			{
				write_pdata_xram("^Event", (char *)&eventCommInfo->recordnum, (eventCommInfo->EventStoragIndexAddr+(mtr_id)*(4)), 4);
			}
        }

    }
    else if(bType == 2)
    {
        ret = evt_db_update(fd, eventCommInfo->eventOAD, eventCommInfo->storagenum, bDatabuf, bufLen, state);
        if (ret != ERR_OK)
        {
            return ret;
        }
    }

    //事件当前值记录
    SetEventStat(fd,mtr_id, eventCommInfo, eventSource, bType);
    //事件时间状态记录
    SetEventTime(fd,mtr_id, eventCommInfo, eventSource, bType);

    EVENT_FMT_DEBUG("事件上报 OAD[%08X] reportflag[%d] g_tnmEventReport[%d] state.nNum[%d] state.oad[%08X]\n", eventCommInfo->eventOAD.value, 
        eventCommInfo->reportflag, g_tnmEventReport, state.nNum, state.state[0].oad.value);

#ifdef AREA_FUJIAN		
	if(gGather == OOP_GATHER_FJ)		//福建模式不上报698规约
	{
		return ERR_OK;
	}
#endif

    //终端主动上报标志, 事件主动上报
    if(g_tnmEventReport == 1 && eventCommInfo->reportflag > EVENT_NO_REPORT)
    {
        //只上报恢复事件
        if(bType <= 1 && eventCommInfo->reportflag == EVENT_RESTORE_REPORT)
            return ERR_OK;
            
        //只上报发生事件
        if(bType == 2 && eventCommInfo->reportflag == EVENT_HAPPENED_REPORT )
            return ERR_OK;

        //2分钟之后更新事件，不需要上报了
        if(bType == 2 && (eventCommInfo->eventOAD.value == 0x32000200 || eventCommInfo->eventOAD.value == 0x31150200))
            return ERR_OK;


#ifdef AREA_ZHEJIANG		
        //浙江第二次上电事件判断有效性，无效不上报
        if(bType == 0 && eventCommInfo->eventOAD.value == 0x31060200)
        {
            if(event_3106.ishappen == 0xaa && event_3106.eventFlag != 0xc0)
            {
                return ERR_OK;
            }
        }
#endif
//        evt_report_data_deal(EVENT_RECORD_FILE, eventCommInfo, (bType == 2)?2:1, bDatabuf, bufLen);
        WriteEventReportData(EVENT_RECORD_FILE, eventCommInfo, bDatabuf, bufLen);
        EVENT_BUF_TRACE(bDatabuf, bufLen,"写入上报文件 ");

    }
    return ERR_OK;
}


/**********************************************************************
* @name      : WriteEventToDB
* @brief     ：终端事件写入数据库,福建特殊要求，事件开始时间由入参带入
* @param[in] ：mtrnum_t          mtr_id            测量点号
               EVENT_COMM_INFO *eventCommInfo     事件基础信息
               OOP_EVTSOURCE_T  eventSource       事件发生源
               OOP_EVTBUF_T     eventData         事件关联对象数据
               uint8            bType             事件状态 0(只记录事件，无发生，恢复区分)，1发生，2恢复        
* @param[out]：   
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2019-12-22
* @Update    :
**********************************************************************/
int WriteEventToDB_fj(DB_CLIENT fd, mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint8 bType)
{ 
    int                 ret                    = 0;
    time_t              timep                  = 0;
    uint8               bDatabuf[DATA_LEN_MAX] = {0};
    uint32              bufLen                 = 0;   
    OOP_REPORTS_T       state;
	uint8 				index				   = 0;

    memset(&state,0,sizeof(OOP_REPORTS_T));

    //上报状态
    state.nNum = rpt_task_package(&state.state[0].oad);

    //获取事件数量和记录序号
    if(eventCommInfo->recordnum == 0)
    {
        GetEventNumAndIndex(fd, eventCommInfo->eventOAD.value, &eventCommInfo->eventNum, &eventCommInfo->recordnum);
    }
    
    EVENT_FMT_DEBUG("eventCommInfo->eventNum is %d,eventCommInfo->recordnum is %d\n", eventCommInfo->eventNum, eventCommInfo->recordnum);
    bufLen = evt_get_oop_struct_data(mtr_id, eventCommInfo, eventSource, eventData, DATA_LEN_MAX, bDatabuf, bType);

	if(eventCommInfo->eventOAD.value == 0x31040200)
    {
        OOP_EVENT_T* pEvt = (OOP_EVENT_T*)bDatabuf;
		memcpy(&pEvt->tmStart, &eventCommInfo->starttime, sizeof(eventCommInfo->starttime));
    }
	

#ifdef AREA_FUJIAN
	if(gGather == OOP_GATHER_FJ)		//福建增补
	{
		evt_write_data_FJ(fd, mtr_id, eventCommInfo, eventSource, eventData, bType);
	}
#endif

    if(bufLen == 0)
    {
        return ERR_O_RANGE;
    }

    if(bType <= 1)//发生
    {
        ret = evt_db_write(fd, eventCommInfo, bDatabuf, bufLen, state);
        if (ret != ERR_OK)
        {
            return ret;
        }
        
        //事件记录条数
        SetEventNum(fd,eventCommInfo);

		eventCommInfo->recordnum++;  
        
		if (eventCommInfo->eventOAD.value == 0x31150200)	//遥控只有跳闸会有两分钟后功率
        {
            if(event_3115.rCtrlType == 0 && bType == 1) 	//遥控只有跳闸会有两分钟后功率
	        {
	            //记录存储序号和事件开始时间
	            write_pdata_xram("^Event", (char *)&eventCommInfo->storagenum, (eventCommInfo->EventStoragNumRecAddr+(event_3115.turn-1)*(4)), 4);
	            time(&timep); /*获取time_t类型的当前时间*/
	            write_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(event_3115.turn-1)*(4)), 4);
	            write_pdata_xram("^Event", (char *)&eventCommInfo->recordnum, (eventCommInfo->EventStoragIndexAddr+(event_3115.turn-1)*(4)), 4);
	        }
        }
		else if (eventCommInfo->eventOAD.value == 0x32000200)	//两分钟后功率
        {
            if(bType == 1) 
	        {
	            //记录存储序号和事件开始时间
	            write_pdata_xram("^Event", (char *)&eventCommInfo->storagenum, (eventCommInfo->EventStoragNumRecAddr+mtr_id*EventStoragCtrlRecLen+(event_3200.turn-1)*(4)), 4);
	            time(&timep); /*获取time_t类型的当前时间*/
	            write_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+mtr_id*EventStoragCtrlRecLen+(event_3200.turn-1)*(4)), 4);
		        write_pdata_xram("^Event", (char *)&eventCommInfo->recordnum, (eventCommInfo->EventStoragIndexAddr+mtr_id*EventStoragCtrlRecLen+(event_3200.turn-1)*(4)), 4);
	        }
        }
		else if (eventCommInfo->eventOAD.value == 0x36000200 || eventCommInfo->eventOAD.value == 0x36010200 || eventCommInfo->eventOAD.value == 0x36020200)
        {
        	switch(eventCommInfo->eventOAD.value)
			{
				case 0x36000200:
					index = event_3600.index;		//主板温度，CPU，内存，磁盘使用率超阈值，有发送和恢复
					break;
				case 0x36010200:
					index = event_3601.index;
					break;
				case 0x36020200:
					index = event_3602.index;
					break;
			}
            if(bType == 1) 
	        {
	            //记录存储序号和事件开始时间
	            write_pdata_xram("^Event", (char *)&eventCommInfo->storagenum, (eventCommInfo->EventStoragNumRecAddr+(index)*(4)), 4);
	            time(&timep); /*获取time_t类型的当前时间*/
	            write_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(index)*(4)), 4);
				write_pdata_xram("^Event", (char *)&eventCommInfo->recordnum, (eventCommInfo->EventStoragIndexAddr+(index)*(4)), 4);
	        }
        }
        else if(bType == 1) //电能表相关
        {
            //记录存储序号和事件开始时间
            write_pdata_xram("^Event", (char *)&eventCommInfo->storagenum, (eventCommInfo->EventStoragNumRecAddr+(mtr_id)*(4)), 4);
            time(&timep); /*获取time_t类型的当前时间*/
            write_pdata_xram("^Event", (char *)&timep, (eventCommInfo->EventStoragTmRecAddr+(mtr_id)*(4)), 4);

			//电能表相关事件，同一时期，可以有多个事件发生和恢复，记录序号需要存储
			if(eventCommInfo->EventStoragIndexAddr != 0)
			{
				write_pdata_xram("^Event", (char *)&eventCommInfo->recordnum, (eventCommInfo->EventStoragIndexAddr+(mtr_id)*(4)), 4);
			}
        }

    }
    else if(bType == 2)
    {
        ret = evt_db_update(fd, eventCommInfo->eventOAD, eventCommInfo->storagenum, bDatabuf, bufLen, state);
        if (ret != ERR_OK)
        {
            return ret;
        }
    }

    //事件当前值记录
    SetEventStat(fd,mtr_id, eventCommInfo, eventSource, bType);
    //事件时间状态记录
    SetEventTime(fd,mtr_id, eventCommInfo, eventSource, bType);

    EVENT_FMT_DEBUG("事件上报 OAD[%08X] reportflag[%d] g_tnmEventReport[%d] state.nNum[%d] state.oad[%08X]\n", eventCommInfo->eventOAD.value, 
        eventCommInfo->reportflag, g_tnmEventReport, state.nNum, state.state[0].oad.value);

#ifdef AREA_FUJIAN		
	if(gGather == OOP_GATHER_FJ)		//福建模式不上报698规约
	{
		return ERR_OK;
	}
#endif

    //终端主动上报标志, 事件主动上报
    if(g_tnmEventReport == 1 && eventCommInfo->reportflag > EVENT_NO_REPORT)
    {
        //只上报恢复事件
        if(bType <= 1 && eventCommInfo->reportflag == EVENT_RESTORE_REPORT)
            return ERR_OK;
            
        //只上报发生事件
        if(bType == 2 && eventCommInfo->reportflag == EVENT_HAPPENED_REPORT )
            return ERR_OK;

        //2分钟之后更新事件，不需要上报了
        if(bType == 2 && (eventCommInfo->eventOAD.value == 0x32000200 || eventCommInfo->eventOAD.value == 0x31150200))
            return ERR_OK;

//        evt_report_data_deal(EVENT_RECORD_FILE, eventCommInfo, (bType == 2)?2:1, bDatabuf, bufLen);
        WriteEventReportData(EVENT_RECORD_FILE, eventCommInfo, bDatabuf, bufLen);
        EVENT_BUF_TRACE(bDatabuf, bufLen,"写入上报文件 ");

    }
    return ERR_OK;
}

#endif

#if DESC("停上电事件",1)

#if DESC("停上电事件,透传报文解析",1)
/*******************************************************************************
* 函数名称:	PrtlUnPackMeterData
* 函数功能:	电表规约解帧
* 输入参数:	FrameLen            帧长度
*           FrameBuff           帧数据
* 输出参数:	MtrAddr             电表地址
*           DataLen             数据域长度
*           DataBuff            数据域内容
*           CtrlCode            控制码
* 返 回 值: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
int PrtlUnPackMeterData(uint16 FrameLen, uint8 *FrameBuff, uint8 *MtrAddr, uint8 *CtrlCode, uint8 *DataLen, uint8 *DataBuff)
{
	uint16  DataIndex = 0xff;                   //指针偏移
	uint8   CS = 0;
    uint16   i;

    //滑动偏移，定位帧头
    for (i = 0; i < FrameLen; i++)
    {
        if ((FrameBuff[i] == 0x68) && (FrameBuff[i+7] == 0x68))
        {
            DataIndex = i;
            break;
        }
    }

    //未定位到帧头，判为失败
    if (DataIndex == 0xff)
    {
        return -1;
    }

	//刷新地址域
    memcpy(MtrAddr, &FrameBuff[DataIndex+1], 6);

    //控制码
    *CtrlCode = FrameBuff[DataIndex+8];

    //刷新数据长度
    *DataLen = FrameBuff[DataIndex+9];

    //判断帧尾
    if (FrameBuff[(DataIndex + *DataLen + 11)] != 0x16)
    {
        return -1;
    }

    //判断校验和
    for (i=0; i < (*DataLen + 10); i++)
    {
        CS += FrameBuff[DataIndex+i];
    }
    if (CS != FrameBuff[(DataIndex + *DataLen + 10)])
    {
        return -1;
    }

    //刷新数据域内容，同时做余3码处理
    for (i = 0; i < *DataLen; i++)
    {
        DataBuff[i] = FrameBuff[DataIndex+10+i] - 0x33;
    }

    //判断是否是否认帧
    if (*CtrlCode & 0x40)
    {
        return 2;
    }

    //判断是否是确认帧
    if (*DataLen == 0)
    {
        return 1;
    }

    return 0;
}
#ifdef AREA_HUNAN

extern uint16 g_val_a[OOP_MAX_POWERCUT_MET];
#endif

/*******************************************************************************
* 函数名称: Bcd2Hex
* 函数功能: 将单字节BCD数据转Hex格式数据
* 输入参数: BcdNum  需要转换的BCD码
* 输出参数: 无   
* 返 回 值: 转换后HEX码
*******************************************************************************/
uint8 Bcd2Hex(uint8 BcdNum)
{
    uint8 HexNum = 0;

    HexNum = ((BcdNum >> 4) & 0x0F) * 10 + (BcdNum & 0x0F);

    return (HexNum);
}

/*******************************************************************************
* 函数名称: Bcd2Hex2
* 函数功能: 将双字节BCD码转为HEX格式
* 输入参数: BcdNum  需要转换的BCD码
* 输出参数: 无   
* 返 回 值: 转换后HEX码
*******************************************************************************/
uint16 Bcd2Hex2(uint16 BcdNum)
{
    uint16 HexNum = 0;

    HexNum = Bcd2Hex(BcdNum >> 8) * 100 + Bcd2Hex(BcdNum & 0xFF);

    return (HexNum);
}

/*******************************************************************************
* 函数名称: SaveMeterHaltData
* 函数功能: 掉电记录刷新
* 输入参数: *Databuf
            DataLen
* 输出参数:
* 内部处理:
* 返 回 值: 无
*******************************************************************************/
int SaveMeterHaltData(uint8 *Databuf,uint8 DataLen)
{
    uint8              MeterIndex = gMeterHalt_Info.MeterHaltIndex;
    rtc_t              rtcTm;

    if(*(uint32 *)Databuf == 0x03110000)
    {
        if(DataLen != 5)
        {
    		gMeterHalt_Info.Relaydstat = RELAY_FAIL;
    		return -1;
    	}
        
        gMeterHalt_Info.Relaydstat = RELAY_RECVD;
        return 0;
    }

	if(*(uint32 *)Databuf == 0x03110001)
	{
	    if(DataLen != 16)
            return -1;
        
    	ooptm_to_rcttm(event_3106.metEndtime[MeterIndex],&rtcTm);

        //printf("上次电表停电纪录: %02x %02x %02x %02x %02x %02x \n", HaltInfo[0], HaltInfo[1], HaltInfo[2], HaltInfo[3], HaltInfo[4], HaltInfo[5]);
        //printf("本次电表停电纪录: %02x %02x %02x %02x %02x %02x \n", Databuf[4], Databuf[5], Databuf[6], Databuf[7], Databuf[8], Databuf[9]);
        // 比较上电的记录如果相等不存储
        // 或者上电记录全0也不存储,上电记录全0可能是电表还在停电中
        if ((memcmp(&rtcTm, &Databuf[10], 6) == 0) || (Databuf[13] == 0x00 && Databuf[14] == 0x00))
        {
            return -1;
        }

    	//如果产生一条新记录
    	memcpy(&rtcTm, &Databuf[4], 6);
        rtctm_to_ooptm(rtcTm, &event_3106.metStarttime[MeterIndex]);
        
		memcpy(&rtcTm, &Databuf[10], 6);
        rtctm_to_ooptm(rtcTm, &event_3106.metEndtime[MeterIndex]);
        gMeterHalt_Info.Relaydstat = RELAY_RECVD;
		return 0;

	}
#ifdef AREA_HUNAN
    if(*(uint32 *)Databuf == 0x02010100)  //a项电压
	{
	    if(DataLen != 6)
            return -1;
        g_val_a[MeterIndex] = Bcd2Hex2(*(uint16*)&Databuf[4]);
        EVENT_FMT_TRACE("电能表[%u]停电时a相电压%u \n", MeterIndex,        g_val_a[MeterIndex]);
        gMeterHalt_Info.Relaydstat = RELAY_RECVD;
        return 0;
	}
#endif
    return -1;

}

/**********************************************************************
* @name      : DealFram07
* @brief     : 解析07报文
* @param[in] ：
    -			msgBuf	    报文
    -			msgLen		报文长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-01-19
* @Update    :
**********************************************************************/
int DealFram07(uint8 *msgBuf, uint16 msgLen)
{
    uint8              RecvBuf[128] = {0};
    uint8              CtrlCode;
    uint8              DataLen;
    int                Result;
    uint8              MetAddr[6] = {0};
    
    Result = PrtlUnPackMeterData(msgLen,msgBuf,MetAddr,&CtrlCode,&DataLen,RecvBuf);

    if(Result != 0 || (CtrlCode & 0xdf) != 0x91 || DataLen < 5)
    {
    	return -1;
    }

    return SaveMeterHaltData(RecvBuf,DataLen);
}

/**********************************************************************
* @name      : DealFramOop
* @brief     : 解析面向对象报文
* @param[in] ：
    -			msgBuf	    报文
    -			msgLen		报文长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-01-19
* @Update    :
**********************************************************************/
int deal_msg(uint8 *msgBuf, uint16 msgLen)
{
    
    uint32  offSet = 0;
    uint8   addLen = 0;
    int     dar = 0;
    
    uint8   MeterIndex = gMeterHalt_Info.MeterHaltIndex;

    if(msgBuf[offSet] != 0x68)
    {
        return -1;
    }
    offSet += 1;
    offSet += 2;
    offSet += 1;
    
    addLen = msgBuf[offSet++] &0x0f;
    offSet += addLen+1;

    offSet += 1;

    offSet += 2;
#ifdef AREA_HUNAN
    if (event_3106.ishappen == 0x55)
    {
        offSet += 3;

        if(msgBuf[offSet++] != 0x20|| msgBuf[offSet++] != 0x00 ||msgBuf[offSet++] != 0x02 || msgBuf[offSet++] != 0x01)
        {
            EVENT_BUF_DEBUG(msgBuf, msgLen, "check OAD!=0x20000201  error. offset(%d)", offSet);
            return -1;    
        }

        if(msgBuf[offSet++] != 0x01)
        {
            return -1;
        }

        if(msgBuf[offSet++] != DT_LONG_UNSIGNED)
        {
            EVENT_BUF_DEBUG(msgBuf, msgLen, "check val type failed. offset(%d)", offSet);
            return -1;
        }  
        
        memcpy_r(&g_val_a[MeterIndex], &msgBuf[offSet], 2);
        EVENT_FMT_TRACE("电能表[%u]停电时a相电压%u \n", MeterIndex,        g_val_a[MeterIndex]);
    }        
    else
#endif
    {
        offSet += 26;
        
        if(msgBuf[offSet++] != 0x01)
        {
            return -1;
        }
        
        if(msgBuf[offSet++] != 0x01)
        {
            return -1;
        }

		if(msgBuf[offSet++] != DT_DOUBLE_LONG_UNSIGNED)			//事件记录序号
        {
            return -1;
        }
		offSet += 4;
        
        if(msgBuf[offSet++] != DT_DATETIME_S)
        {
            EVENT_BUF_DEBUG(msgBuf, msgLen, "check datetime_s type failed. offset(%d)", offSet);
            return -1;
        }  
        
        dar = apdu_datetime_s_fromfmt698(msgBuf, msgLen, &offSet, &event_3106.metStarttime[MeterIndex], sizeof(OOP_DATETIME_S_T));
        if(dar != 0)
        {
            EVENT_BUF_DEBUG(msgBuf, msgLen, "apdu_datetime_s_fromfmt698 failed. dar(%d), offset(%d)", dar, offSet);
            return -1;           
        }
        
        if(msgBuf[offSet++] != DT_DATETIME_S)
        {
            EVENT_BUF_DEBUG(msgBuf, msgLen, "check datetime_s type failed. offset(%d)", offSet);
            return -1;
        }  
        
        dar = apdu_datetime_s_fromfmt698(msgBuf, msgLen, &offSet, &event_3106.metEndtime[MeterIndex], sizeof(OOP_DATETIME_S_T));
        if(dar != 0)
        {
            EVENT_BUF_DEBUG(msgBuf, msgLen, "apdu_datetime_s_fromfmt698 failed. dar(%d), offset(%d)", dar, offSet);
            return -1;           
        }
        
        EVENT_FMT_TRACE("电能表[%u]停电时间%u-%u-%u %u:%u:%u \n", MeterIndex,        event_3106.metStarttime[MeterIndex].year, event_3106.metStarttime[MeterIndex].month, event_3106.metStarttime[MeterIndex].day, event_3106.metStarttime[MeterIndex].hour, event_3106.metStarttime[MeterIndex].minute, event_3106.metStarttime[MeterIndex].second);
        EVENT_FMT_TRACE("电能表[%u]上电时间%u-%u-%u %u:%u:%u \n", MeterIndex,        event_3106.metEndtime[MeterIndex].year, event_3106.metEndtime[MeterIndex].month, event_3106.metEndtime[MeterIndex].day, event_3106.metEndtime[MeterIndex].hour, event_3106.metEndtime[MeterIndex].minute, event_3106.metEndtime[MeterIndex].second);
    }


    gMeterHalt_Info.Relaydstat = RELAY_RECVD;
    return 0;
}

/**********************************************************************
* @name      : DealFramOop
* @brief     : 解析面向对象报文
* @param[in] ：
    -			msgBuf	    报文
    -			msgLen		报文长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-01-19
* @Update    :
**********************************************************************/
int DealFramOop(uint8 *msgBuf, uint16 msgLen)
{
    CHANNEL_INFO_T tChannelInfo;
    uint8          msgrecv[APP_RECV_MSG_MAXLEN] = {0};     //接收IPC缓存
    uint16         newlen = 0;

    memset(&tChannelInfo, 0, sizeof(CHANNEL_INFO_T));
    
    app_open_channel(CHANNEL_DEFAULT, &tChannelInfo);
    app_insert_recvmsg(CHANNEL_DEFAULT, msgBuf, msgLen, APP698_RIPC_LOCK);
    newlen = (uint32)app_get_recvmsg_s(CHANNEL_DEFAULT, msgrecv, APP_RECV_MSG_MAXLEN, APP698_PROC_LOCK);

    int ret = deal_msg(msgrecv, newlen);
    
    app_close_channel(CHANNEL_DEFAULT);
    
    return ret;
}

/**********************************************************************
* @name      : dealMsgBuf
* @brief     : 处理接收的透传报文
* @param[in] ：
    -			msgBuf	    报文
    -			msgLen		报文长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-01-19
* @Update    :
**********************************************************************/
int dealMsgBuf(uint8 *msgBuf, uint16 msgLen)
{
    uint8	MeterIndex = gMeterHalt_Info.MeterHaltIndex;
    uint8   IdMtTyp = gMeterHalt_Info.MeterHaltInfo[MeterIndex].protocol;
    
    if (IdMtTyp == (uint8)PROTO_698)
	{
		return DealFramOop(msgBuf, msgLen);   //解帧
	}

	if (IdMtTyp == (uint8)PROTO_07)
	{
		return DealFram07(msgBuf, msgLen); //解帧
	}

    EVENT_FMT_DEBUG("IdMtTyp=%u error!\n", IdMtTyp);
    gMeterHalt_Info.Relaydstat = RELAY_FAIL;
    return -1;
}

#endif
/*******************************************************************************
* 函数名称: 	crc_calc
* 函数功能: 	电表校验未计算
* 输入参数：	databuf
				datalen
* 输出参数：
* 内部处理：
* 返 回 值: 	NULL
*******************************************************************************/
uint8 crc_calc(char *databuf, uint8 datalen)
{
	uint8	start, i;
	uint8	crc = 0;

	start = 0;
	while (start < datalen)
	{
		if (databuf[start] == 0x68)
		{
			break;
		}
		start++;
	}

	for (i = start; i < datalen; i++)
	{
		crc += databuf[i];
	}

	return crc;
}

/*******************************************************************************
* 函数名称: OopSpeedSwitchToMode
* 函数功能: 速度转换成模式字
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: mode
*******************************************************************************/
uint8 OopSpeedSwitchToMode(uint8 speed)
{
	uint8 mode = 0;

	switch (speed)
	{
	case 0x0B:
		mode = 0;
		break;

	case 0x2B:
		mode = 1;
		break;

	case 0x4B:
		mode = 2;
		break;

	case 0x6B:
		mode = 3;
		break;

	case 0x8B:
		mode = 4;
		break;

	case 0xAB:
		mode = 5;
		break;

	case 0xCB:
		mode = 6;
		break;

	case 0xEB:
		mode = 7;
		break;

	default:
		mode = 3;
		break;
	}

	return mode;
}

/**********************************************************************
* @name      : GetPortCOMDCB
* @brief     ：获取端口参数
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int GetPortCOMDCB(OOP_OAD_U port, OOP_COMDCB_T* pCOMDCB)
{
    NOMAL_OAD_T nomalOAD;
    OOP_RS485_T Rs485;
    int32 ret = -1;
    uint32 len = 0;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
    memset(&Rs485,0,sizeof(OOP_RS485_T));

    nomalOAD.logicId = 0;
    nomalOAD.infoNum = 0;
    nomalOAD.oad = port;
    nomalOAD.oad.nIndex = 0;

    ret = db_read_nomal(gDbCenterClient, &nomalOAD, sizeof(OOP_RS485_T), (uint8 *)&Rs485, &len);
    if (ret != ERR_OK || len == 0)
    {
        EVENT_FMT_DEBUG("db_read_nomal OAD=%8X  error ret%d, len=%u\n", nomalOAD.oad.value, ret, len);
    }
    else if(Rs485.nNum < port.nIndex)
    {
        EVENT_FMT_DEBUG("端口号OAD=%8X 数量nNum[%u]<nIndex[%u] error!\n", nomalOAD.oad.value, Rs485.nNum, port.nIndex);
        ret = -1;
    }
    else
    {
        *pCOMDCB = Rs485.rs485[port.nIndex - 1].param;
        EVENT_FMT_TRACE("GetPortCOMDCB baud=%u, parity=%u, flowctrl=%u, databits=%u, stopbits=%u \n", pCOMDCB->baud, pCOMDCB->parity, pCOMDCB->flowctrl, pCOMDCB->databits, pCOMDCB->stopbits);
    }

    return ret;
}
/*******************************************************************************
* 函数名称: 	MakeFram_07_volt
* 函数功能: 	07电表组帧
* 输入参数：	DataBuf   //发送的帧内容
				DataLen   //发送的帧长度
* 输出参数：
* 内部处理：
* 返 回 值:
*******************************************************************************/
uint8 MakeFram_07_volt(char *DataBuf,uint16 *DataLen)
{
    uint8    FrameLen = 0;
    uint8    MeterIndex = gMeterHalt_Info.MeterHaltIndex;//待中继的抄读电表停电记录的索引

    //4字节前引导符
    memset(DataBuf,0xFE,4);
    FrameLen = 4;

    //首字节0x68
    DataBuf[FrameLen++] = 0x68;

    //6字节地址
    memcpy_r(&DataBuf[FrameLen],
           gMeterHalt_Info.MeterHaltInfo[MeterIndex].Addr.add,6);//读取电表地址
    FrameLen += 6;

    //次字节0x68
    DataBuf[FrameLen++] = 0x68;

    //功能码
    DataBuf[FrameLen++] = 0x11;

    //长度
    DataBuf[FrameLen++] = 0x04;

    //数据标识,电表电压(A相电压)
    DataBuf[FrameLen++] = 0x00+0x33;
    DataBuf[FrameLen++] = 0x01+0x33;
    DataBuf[FrameLen++] = 0x01+0x33;
    DataBuf[FrameLen++] = 0x02+0x33;

    //CRC
    DataBuf[FrameLen] = crc_calc(DataBuf,FrameLen);
    FrameLen++;

    //结束符
    DataBuf[FrameLen++] = 0x16;

    *DataLen = FrameLen;

    return 1;
}


/*******************************************************************************
* 函数名称: 	MakeFram_07
* 函数功能: 	07电表组帧
* 输入参数：	DataBuf   //发送的帧内容
				DataLen   //发送的帧长度
* 输出参数：
* 内部处理：
* 返 回 值:
*******************************************************************************/
uint8 MakeFram_07(char *DataBuf, uint16 *DataLen)
{
	uint8    FrameLen = 0;
	uint8    MeterIndex = gMeterHalt_Info.MeterHaltIndex;//待中继的抄读电表停电记录的索引

	//4字节前引导符
	memset(DataBuf, 0xFE, 4);
	FrameLen = 4;

	//首字节0x68
	DataBuf[FrameLen++] = 0x68;

	//6字节地址
	memcpy_r(&DataBuf[FrameLen],
		gMeterHalt_Info.MeterHaltInfo[MeterIndex].Addr.add, 6);//读取电表地址
	FrameLen += 6;

	//次字节0x68
	DataBuf[FrameLen++] = 0x68;

	//功能码
	DataBuf[FrameLen++] = 0x11;

	//长度
	DataBuf[FrameLen++] = 0x04;

#ifdef AREA_HUNAN
    if (event_3106.ishappen == 0x55)
    {
        //数据标识,电表电压(A相电压)
        DataBuf[FrameLen++] = 0x00+0x33;
        DataBuf[FrameLen++] = 0x01+0x33;
        DataBuf[FrameLen++] = 0x01+0x33;
        DataBuf[FrameLen++] = 0x02+0x33;
    }        
    else
#endif
    {
        //先抄掉电次数，再抄上一次掉电记录
    	if (gMeterHalt_Info.MeterIdNum == 0)
    	{
    		//数据标识
    		DataBuf[FrameLen++] = 0x00 + 0x33;
    		DataBuf[FrameLen++] = 0x00 + 0x33;
    		DataBuf[FrameLen++] = 0x11 + 0x33;
    		DataBuf[FrameLen++] = 0x03 + 0x33;
    	}
    	else if (gMeterHalt_Info.MeterIdNum == 1)
    	{
    		//数据标识
    		DataBuf[FrameLen++] = 0x01 + 0x33;
    		DataBuf[FrameLen++] = 0x00 + 0x33;
    		DataBuf[FrameLen++] = 0x11 + 0x33;
    		DataBuf[FrameLen++] = 0x03 + 0x33;
    	}
    }
    	
	//CRC
	DataBuf[FrameLen] = crc_calc(DataBuf, FrameLen);
	FrameLen++;

	//结束符
	DataBuf[FrameLen++] = 0x16;

	*DataLen = FrameLen;

	return 1;
}

/*******************************************************************************
* 函数名称: 	MakeFramOop
* 函数功能: 	面向对象组帧
* 输入参数：	DataBuf   //发送的帧内容
				DataLen   //发送的帧长度
* 输出参数：
* 内部处理：
* 返 回 值:
*******************************************************************************/
uint8 MakeFramOop(char *DataBuf, uint16 *DataLen)
{
	uint8       MeterIndex  = gMeterHalt_Info.MeterHaltIndex;//待中继的抄读电表停电记录的索引
	uint8       FrameLen    = 0;
    uint16      headLen     = 0;
    uint16      offSet      = 0;
    uint8       apdu[1024]   = {0};
    uint16      apduLen     = 0;
    uint16      tmpCrc      = 0;
    OOP_TSA_T   TAS;

    TAS = gMeterHalt_Info.MeterHaltInfo[MeterIndex].Addr;
    
	if (gMeterHalt_Info.MeterIdNum == 0)
	{
	    EVENT_FMT_LOGCC("抄上一次掉电记录\n");
        
#ifdef AREA_HUNAN
        if (event_3106.ishappen == 0x55)
        {
            //Client-APDU
    		//读取请求
    		apdu[apduLen ++] = 0x05;
            
            //GetRequestNormal 数据类型
            apdu[apduLen ++] = 0x01;
            
            //PIID
            apdu[apduLen ++] = 0x01;

            //oad
            apdu[apduLen ++] = 0x20;
            apdu[apduLen ++] = 0x00;
            apdu[apduLen ++] = 0x02;
            apdu[apduLen ++] = 0x01;

			//时间标签域 TimeTag OPTIONAL
        	apdu[apduLen ++] = 0x00;
        }        
        else
#endif
        {
        	//安全请求，明文+随机数
        	apdu[apduLen ++] = 0x10;
            apdu[apduLen ++] = 0x00;
            
        	
            //Client-APDU
    		//读取请求
    		apdu[apduLen ++] = 0x1a;	//长度
    		apdu[apduLen ++] = 0x05;
            
            //GetRequestRecord 数据类型
            apdu[apduLen ++] = 0x03;
            
            //PIID
            apdu[apduLen ++] = 0x01;

            //oad
            apdu[apduLen ++] = 0x30;
            apdu[apduLen ++] = 0x11;
            apdu[apduLen ++] = 0x02;
            apdu[apduLen ++] = 0x00;

            //RSD
            apdu[apduLen ++] = 0x09;
            apdu[apduLen ++] = 0x01;

            //RCSD
            apdu[apduLen ++] = 0x03;
			apdu[apduLen ++] = 0x00;
            apdu[apduLen ++] = 0x20;
            apdu[apduLen ++] = 0x22;
            apdu[apduLen ++] = 0x02;
            apdu[apduLen ++] = 0x00;
            apdu[apduLen ++] = 0x00;
            apdu[apduLen ++] = 0x20;
            apdu[apduLen ++] = 0x1E;
            apdu[apduLen ++] = 0x02;
            apdu[apduLen ++] = 0x00;
            apdu[apduLen ++] = 0x00;
            apdu[apduLen ++] = 0x20;
            apdu[apduLen ++] = 0x20;
            apdu[apduLen ++] = 0x02;
            apdu[apduLen ++] = 0x00;

			//时间标签域 TimeTag OPTIONAL
        	apdu[apduLen ++] = 0x00;

			//随机数
			apdu[apduLen ++] = 0x01;
	        apdu[apduLen ++] = 0x10;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
	        apdu[apduLen ++] = 0x00;
        }

        EVENT_BUF_TRACE(apdu, apduLen, "adpu:");

//报文组帧
        //4字节前引导符
    	memset(DataBuf, 0xFE, 4);
    	offSet = 4;

    	//首字节0x68
    	DataBuf[offSet++] = 0x68;

    	//长度域 L
    	headLen = 2+1+(1+(TAS.len+1)+1);
	    FrameLen = headLen+2+ apduLen+2;
        DataBuf[offSet++] = FrameLen & 0x00FF;
	    DataBuf[offSet++] = (FrameLen>>8)& 0x003F;

    	//控制域C
    	DataBuf[offSet++] = 0x43;

    	//地址域A
    	DataBuf[offSet++] = TAS.af;
        memcpy_r(DataBuf+offSet,TAS.add,TAS.len+1);
	    offSet += (TAS.len+1);

        DataBuf[offSet++] = 0x10;

    	//帧头校验HCS
    	tmpCrc = CRC16_Get((uint8*)&DataBuf[5] ,headLen);
        DataBuf[offSet++] =  tmpCrc & 0x00FF;
	    DataBuf[offSet++] =  (tmpCrc & 0xFF00)>>8;
	    
        //apdu
        memcpy(DataBuf+offSet,apdu,apduLen);
		offSet += apduLen;
        
        //帧校验 FCS
        tmpCrc = CRC16_Get((uint8*)&DataBuf[5] ,FrameLen-2);
    	DataBuf[offSet++] =  tmpCrc & 0x00FF;
	    DataBuf[offSet++] =  (tmpCrc & 0xFF00)>>8;

    	//结束符
    	DataBuf[offSet++] = 0x16;

        EVENT_BUF_TRACE(DataBuf, offSet, "DataBuf:");

    	*DataLen = offSet;     
        
    }
	return 1;
}
/*******************************************************************************
* 函数名称: MeterHalt_Relayd
* 函数功能: 电表停电记录中继命令
* 输入参数: void
* 输出参数:
* 内部处理:
* 返 回 值: 无
*******************************************************************************/
int MeterHalt_Relayd(char *SendFrameBuf, uint16 SendFrameLen)
{
	uint8		   MeterIndex = gMeterHalt_Info.MeterHaltIndex;
	static  int16 RelayOverTime;

    OOP_OAD_U port;
    int ret = 0;
    
    uint8 msgdata[1400] = {0};
	uint16 msglen=0;

	port = gMeterHalt_Info.MeterHaltInfo[MeterIndex].commnum;
    
	if (gMeterHalt_Info.Relaydstat == RELAY_SEND)
	{
	    memcpy_r(msgdata, &port.value, 4);
        msglen += 4;
        
        msgdata[msglen++] = gMeterHalt_Info.MeterHaltInfo[MeterIndex].baud;//波特率

		msgdata[msglen++] = gMeterHalt_Info.MeterHaltInfo[MeterIndex].ctrtype;//控制位

/*
		if(port.nObjID == 0XF201)  //485
        {
            //接收等待报文超时时间50s
            msgdata[msglen++] = 0x00;
            msgdata[msglen++] = 0x32;
        }
		else  //载波
		{
            //接收等待报文超时时间 20s
            msgdata[msglen++] = 0x00;
            msgdata[msglen++] = 0x14;
		}
*/      if(port.nObjID==0xF209)
        {
            //接收等待报文超时时间20s
            msgdata[msglen++] = 0x00;
            msgdata[msglen++] = 0x5A;
        }
        else
        {
            //接收等待报文超时时间20s
            msgdata[msglen++] = 0x00;
            msgdata[msglen++] = 0x14;
        }
        
        //接收等待字节超时时间 10ms
        msgdata[msglen++] = 0x00;
        msgdata[msglen++] = 0x0a;
        
        ret = set_len_offset(SendFrameLen, msgdata + msglen);
        
        msglen += ret;
        memcpy(msgdata + msglen, SendFrameBuf, SendFrameLen);
        msglen += SendFrameLen;

		//透传   
        
        ret = appmain_send_msg(MSG_APPAMIN_CTS_READ_LIST,LABEL_HALT_EVENT,msgdata,msglen);
        
        EVENT_BUF_LOGCC(msgdata, msglen, "透传ret=%d ,msgdata:", ret);
        
		gMeterHalt_Info.Relaydstat = RELAY_WATT;

   
	    if(port.nObjID==0xF209)
	    {
	        RelayOverTime = 90;
	        EVENT_FMT_DEBUG("载波端口 电表的RelayOverTime=%u\n", RelayOverTime);
	    }
	    else
	    {
	        RelayOverTime = 5;
	        EVENT_FMT_DEBUG("非载波端口 电表的RelayOverTime=%u\n", RelayOverTime);
	    }
		//RelayOverTime = 5;//穿透延时时间

		return -1;		
	}

	//如果已经接收完成,那么返回成功
	if (gMeterHalt_Info.Relaydstat == RELAY_RECVD
		|| gMeterHalt_Info.Relaydstat == RELAY_FAIL)
	{
		RelayOverTime = 0;
		return 1;
	}

	//判断超时
	RelayOverTime--;

	if (RelayOverTime <= 0)
	{
		gMeterHalt_Info.Relaydstat = RELAY_FAIL;
	}

	return -1;
}

/*
*********************************************************************
功能:
	终端停、上电时间处理函数；
返回值:
	1:		终端上电；
	0:		终端未上电；
*********************************************************************
*/
int PowerDownCheck(void)
{
	int len = -1;
	OOP_DATETIME_S_T last_time = {0};	//-停电时间-
	time_t nowSecond, lastSecond = 0;
    

	len = read_pdata_xram("^Event", (char *)&last_time, ProgDownAddr, ProgDownRecLen);
	if (len < 0)	//-读停电时间-
		return 0;
	if ((last_time.month == 0) || (last_time.month > 0x12) ||
		(last_time.day == 0) || (last_time.day > 0x31))
		return 0;
    
    ooptm_to_systm(last_time, &lastSecond);

	//-比较60S,超过算上电-
	time(&nowSecond);

    EVENT_FMT_TRACE("停电时间%u-%u-%u %u:%u:%u 秒数%u, nowSecond=%u\n", last_time.year, last_time.month, last_time.day, last_time.hour, last_time.minute, last_time.second, lastSecond, nowSecond);
	return nowSecond - lastSecond;
}

/**********************************************************************
* @name      : get_power_state
* @brief     ：获取电源状态
* @param[in] ：  
* @param[out]：state: 电源状态，1有电，0 掉电
* @return    ：1 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-07-27
* @Update    :
**********************************************************************/
int get_power_state(int *state)
{
	POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
	if(NULL == dev)
	{
        EVENT_FMT_LOGCC("dev is null!\n");
        return -1;
	}
	
	if(dev->power_power_state_get(dev,POWER_MAIN,state) < 0)
	{
		EVENT_FMT_LOGCC("get power state failed!\n");
		return -1;
	}

    EVENT_FMT_TRACE("get power state%d! \n", *state);
    
	hal_device_release((HW_DEVICE *)dev);
	return 1;
}

#define 		COUNT_NUM 		15		// 持续判断的次数               24规范防抖时间为15秒


/*******************************************************************************
* 函数名称: CheckTmn_Halt
* 函数功能: 终端的停上电检测
* 输入参数: void
* 输出参数: < 0  //停电
			= 0  //新的上电记录
			> 0  //上电
* 内部处理:
* 返 回 值: 无
*******************************************************************************/
int CheckTmn_Halt(void)
{
	int   bTemp = 0;				// 上电; 0:未上电
	OOP_DATETIME_S_T TmnTime = { 0 };           //终端当前时间
	static OOP_DATETIME_S_T lastPowerOffTime = { 0 };//终端时间
	static int bfgPower = 1;	            //-电池状态标志-
	uint16 SetVol = 1320;              //停电电压限值
	uint16 ResetVol = 1760;            //上电电压限值
//	OOP_METER_PWRTYPE_E powerType = PWRTYPE_UNKNOWN;
	int fixVolt = 0;				// 额定电压
	static uint8 PowerOffCount = 0;
	static uint8 PowerOnCount = 0;
	static uint8 FirstRun = 0x55;
    static uint8 first_power_on = 1;    //第一次上电,终端启动时，耗时太常，适配台体测试，

    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据 
    
//    static OOP_DWORD3V_T lastTmnVoltage = { 0 };    //上次终端电压

    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));   
    
	current_tm_to_ooptm(&TmnTime);    //-取实时时间-

	// 接线方式及额定电压，
//	powerType = PWRTYPE_3PHASE4;
    fixVolt = 2200;
	
	//取停电电压限值
	SetVol = event_3106.eventParam.lParam.occVolt;
	if (SetVol == 0xFFFF || SetVol == 0)
	{
		SetVol = fixVolt * 60 / 100;
	}

	//取上电电压限值
	ResetVol = event_3106.eventParam.lParam.recVolt;
	if (ResetVol == 0xFFFF || ResetVol == 0)
	{
		ResetVol = fixVolt * 80 / 100;
	}

	//读取停电告警发生标志
    if(read_pdata_xram("^Event", (char *)&event_3106.ishappen, TmnPowerOffAddr, TmnPowerOffLen) == 0 && event_3106.ishappen == 0x55)
    {
        event_3106.ishappen = 0x55;		//停电已经发生
        bfgPower = 0;
    }
    else
    {
        event_3106.ishappen = 0xaa;		//停电未发生
        bfgPower = 1;
        write_pdata_xram("^Event", (const char *)&event_3106.ishappen, TmnPowerOffAddr, TmnPowerOffLen);
    }

	if (first_power_on == 1 && event_3106.ishappen == 0xaa)
	{
		if (FirstRun != 0x55)
		{
			first_power_on = 0;		//第一次启动时，没有停电事件时，不用处理
		}
	}
	

	//读取电池供电标志
#if (NEW_TNM == 1)
    if (get_power_state(&bfgPower) && bfgPower == 0)
    {
        bfgPower = -1;
    }
    else
    {
        bfgPower = 1;
    }

#ifdef AREA_FUJIAN
	check_power_status(gDbCenterClient, bfgPower);
	check_battery_status_run(gDbCenterClient);

#endif
	
#endif

    //读取三相电压实时值
    event_3106.tmnVoltage = tmnVoltage;
//    event_3106.tmnVoltage.nValue[0] = TermSharedData.PhaAVolt;
//    event_3106.tmnVoltage.nValue[1] = TermSharedData.PhaBVolt;
//    event_3106.tmnVoltage.nValue[2] = TermSharedData.PhaCVolt;

	EVENT_FMT_TRACE("CheckERC_3106 检测终端停电事件ishappen%02X bfgPower = %d Volt[0] = %d Volt[1] = %d Volt[2] = %d SetVol = %d ResetVol = %d\n", 
         event_3106.ishappen, bfgPower, event_3106.tmnVoltage.nValue[0], event_3106.tmnVoltage.nValue[1], event_3106.tmnVoltage.nValue[2], SetVol, ResetVol);

	// 防错// 不会出现电池供电的情况下还有大于工作门限的电压
	if ((bfgPower < 0) && (event_3106.tmnVoltage.nValue[0] >= 900 || event_3106.tmnVoltage.nValue[1] >= 900 || event_3106.tmnVoltage.nValue[2] >= 900))
	{
		return 1;
	}


    if (bfgPower < 0 ||
        (event_3106.tmnVoltage.nValue[0] <= SetVol && event_3106.tmnVoltage.nValue[1] <= SetVol && event_3106.tmnVoltage.nValue[2] <= SetVol
			&& (event_3106.tmnVoltage.nValue[0] >= 40 || event_3106.tmnVoltage.nValue[1] >= 40 || event_3106.tmnVoltage.nValue[2] >= 40)))
	{
        
		//多次，防止电压抖动
		if (PowerOffCount < COUNT_NUM)
		{
			PowerOffCount++;
		}
		PowerOnCount = 0;
        
	}
	//不用电池供电，且当前电压大于上电限值，算上电
	else if (bfgPower >= 0 &&
                ((event_3106.tmnVoltage.nValue[0] >= ResetVol || event_3106.tmnVoltage.nValue[1] >= ResetVol || event_3106.tmnVoltage.nValue[2] >= ResetVol)
			    || (event_3106.tmnVoltage.nValue[0] < 40 && event_3106.tmnVoltage.nValue[1] < 40 && event_3106.tmnVoltage.nValue[2] < 40)))
			
	{
		//多次，防止电压抖动
		if (PowerOnCount < COUNT_NUM)
		{
			PowerOnCount++;
		}
		PowerOffCount = 0;
        
	}

	//检测终端停电时间是否超过180S
	bTemp = PowerDownCheck();

 	EVENT_FMT_TRACE("CheckERC_3106 检测终端停电时间bTemp=[%d]  PowerOffCount = %d PowerOnCount = %d flag = %02x\n", bTemp, PowerOffCount, PowerOnCount, event_3106.ishappen);

	//当前停电且未记录过停电
	if (PowerOffCount >= COUNT_NUM && event_3106.ishappen == 0xaa)
	{
		PowerOnCount = 0;
        EVENT_FMT_DEBUG("CheckERC_3106 检测终端停电事件ishappen%02X bfgPower = %d Volt[0] = %d Volt[1] = %d Volt[2] = %d SetVol = %d ResetVol = %d\n", 
         event_3106.ishappen, bfgPower, event_3106.tmnVoltage.nValue[0], event_3106.tmnVoltage.nValue[1], event_3106.tmnVoltage.nValue[2], SetVol, ResetVol);

        //电池供电
        bfgPower = 0;

        //停电状态保存
		event_3106.ishappen = 0x55;
        write_pdata_xram("^Event", (const char *)&event_3106.ishappen, TmnPowerOffAddr, TmnPowerOffLen);

//		EVENT_FMT_TRACE("CheckERC_3106 检测终端产生停电事件\n");
        
		write_pdata_xram("^Event", (char *)&TmnTime, ProgDownAddr, ProgDownRecLen);
        event_3106.tmnStarttime = TmnTime;
		
#ifdef AREA_FUJIAN
		//福建停电事件，一分钟后上报，不到一分钟的，上电立即上报
		//停电上报标记       0x3c 等待上报， 0x55 立即上报，0xaa 已经上报,
		event_3106.rptFlag = 0x3c;
		write_pdata_xram("^Event", (char *)&event_3106.rptFlag, EventStopRpt, 1);

		event_3106.tmFlag = 0xaa;
		write_pdata_xram("^Event", (char *)&event_3106.tmFlag, EventStopTm, 1);
		return -1;
#endif

#ifdef AREA_SHANDONG
		read_pdata_xram("^Event", (char *)&event_3106.tmnEndtime,ProgUpAddr, ProgDownRecLen);
#endif

#ifndef AREA_HUNAN
        int  ret = 0;
        //事件发生源
        eventSource.choice = DT_ENUM;
        eventSource.nValue = 0;             //停电

        //事件关联数据
        
		//当前判断为停电且未写过停电记录
		event_3106.eventFlag = 0x80;
		//写数据库
        ret = WriteEventToDB(gDbCenterClient, 0, &event_3106.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3106 终端停电事件记录失败返回%d\n", ret);
            return ret;
        }
        EVENT_FMT_TRACE("CheckERC_3106 终端停电事件记录成功, storagenum=%u !\n", event_3106.eventCommInfo.storagenum);
#endif
        return -1;
	}
	//当前上电且未记录过上电，或第一次上电且停电时间超过限值
	else if ((PowerOnCount >= COUNT_NUM && event_3106.ishappen == 0x55)
#ifdef AREA_FUJIAN
		|| (bTemp > 120 && FirstRun == 0x55))			//福建要求，无电池，停电1分钟以上，上报停上电事件
#else
		|| (bTemp > POWER_STOP_TIME_LMT && FirstRun == 0x55))
#endif

	{

		PowerOffCount = 0;
        EVENT_FMT_DEBUG("CheckERC_3106 检测终端上电事件ishappen%02X bfgPower = %d Volt[0] = %d Volt[1] = %d Volt[2] = %d SetVol = %d ResetVol = %d\n", 
         event_3106.ishappen, bfgPower, event_3106.tmnVoltage.nValue[0], event_3106.tmnVoltage.nValue[1], event_3106.tmnVoltage.nValue[2], SetVol, ResetVol);
		
#ifdef AREA_FUJIAN

		appmain_send_powerup_evnet();		//发消息给taskManager

		//福建停电事件，一分钟后上报，不到一分钟的，上电立即上报
		if (event_3106.rptFlag < 0x55)
        {
            event_3106.rptFlag = 0x55;	//0x55 立即上报
        }

		//没有电池
		if (bTemp > 120 && FirstRun == 0x55 && event_3106.ishappen == 0xaa)
        {
            event_3106.rptFlag = 0x55;	//0x55 立即上报
        }
#endif

        //正常供电
        bfgPower = 1;
		
		read_pdata_xram("^Event", (char *)&event_3106.tmnStarttime,ProgDownAddr, ProgDownRecLen);
#if DELAY

        if (first_power_on == 1)
        {
            time_t timep;
            time(&timep);
            EVENT_FMT_TRACE("终端上电时间%u-%u-%u %u:%u:%u \n", TmnTime.year, TmnTime.month, TmnTime.day, TmnTime.hour, TmnTime.minute, TmnTime.second);

            timep -= DELAY;
            systm_to_ooptm(timep, &TmnTime);
            EVENT_FMT_TRACE("终端上电时间，第一次启动处理后结果：%u-%u-%u %u:%u:%u \n", TmnTime.year, TmnTime.month, TmnTime.day, TmnTime.hour, TmnTime.minute, TmnTime.second);
            first_power_on = 0;
        }
            
#endif
        
        event_3106.tmnEndtime = TmnTime;
        write_pdata_xram("^Event", (char *)&TmnTime,ProgDownAddr, ProgDownRecLen);
		
#ifdef AREA_SHANDONG
		write_pdata_xram("^Event", (char *)&TmnTime,ProgUpAddr, ProgDownRecLen);
#endif


		event_3106.ishappen = 0xaa;
        write_pdata_xram("^Event", (const char *)&event_3106.ishappen, TmnPowerOffAddr, TmnPowerOffLen);

		return 0;
	}
	else if (bfgPower > 0 && event_3106.ishappen == 0xaa)
	{
		//停电事件只要精确到秒，所以私有外村只记录到分钟即可，否则每秒都要写入，很浪费Nand写入次数
		if (lastPowerOffTime.minute != TmnTime.minute || lastPowerOffTime.hour != TmnTime.hour || lastPowerOffTime.day != TmnTime.day ||
			lastPowerOffTime.month != TmnTime.month || lastPowerOffTime.year != TmnTime.year)
		{
			lastPowerOffTime = TmnTime;
            EVENT_FMT_TRACE("终端停电时间%u-%u-%u %u:%u:%u \n", TmnTime.year, TmnTime.month, TmnTime.day, TmnTime.hour, TmnTime.minute, TmnTime.second);
			write_pdata_xram("^Event", (char *)&TmnTime, ProgDownAddr, ProgDownRecLen);
		}
	}

	FirstRun = 0xAA;
	return 1;
}

/*******************************************************************************
* 函数名称: SpeedSwitchToMode
* 函数功能: 协议波特率转换成通讯字
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: mode
*******************************************************************************/
uint8 SpeedSwitchToMode(uint8 speed)
{
	uint8 mode = 0;

	switch (speed)
	{
	case 0:
		mode = 0x0B;
		break;

	case 1:
		mode = 0x2B;
		break;

	case 2:
		mode = 0x4B;
		break;

	case 3:
		mode = 0x6B;
		break;

	case 4:
		mode = 0x8B;
		break;

	case 5:
		mode = 0xAB;
		break;

	case 6:
		mode = 0xCB;
		break;

	case 7:
		mode = 0xEB;
		break;

	default:
		mode = 0x6B;
		break;
	}

	return mode;
}

/*******************************************************************************
* 函数名称: MeterValidCheck
* 函数功能: 电表参数的符合性检查
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: > 0: 符合要求   <=0: 不符合要求
*******************************************************************************/
int GetParaAndValidCheck(mtrnum_t Pn, int16 realMeterHaltNum)
{
    OOP_METER_T oopMeter;
      
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
	// 有效性
	if (app_get_meter_list(Pn, &oopMeter) != 0)
	{
		EVENT_FMT_TRACE(" // 有效性无效\n");
		return -1;
	}

	// 测量点性质
	if ((oopMeter.basic.port.nObjID != OI_485) && (oopMeter.basic.port.nObjID  != OI_PLC) 
//        && (oopMeterInfo[Pn].oopMeter.basic.userType != MTR_PLCMUI)
        )
	{
		EVENT_FMT_TRACE(" // 测量点性质不符\n");
		return -1;
	}

	// 规约类型
	if ((oopMeter.basic.protocol != (uint8)PROTO_07 && oopMeter.basic.protocol != (uint8)PROTO_698))
	{
		EVENT_FMT_TRACE(" // 规约类型不符\n");
		return -1;
	}
/*
	// 端口号
	if ((ReadXPara(MTR_PARA_TYPE, Mt_Para_Com_ID, (char *)&metPort, Pn) < 0) || (metPort == 0xff))
	{
		DEPRINT(ERC_NO_14, " // 端口号不符\n");
		return -1;
	}

	// 电表地址
	if (ReadXPara(MTR_PARA_TYPE, Mt_Para_Addr_ID, metAdd, Pn) < 0)
	{
		DEPRINT(ERC_NO_14, " // 电表地址无效\n");
		return -1;
	}

	// 波特率
	if ((ReadXPara(MTR_PARA_TYPE, Mt_Para_Speed_ID, (char *)&speed, Pn) < 0) || (speed == 0x00ff))
	{
		DEPRINT(ERC_NO_14, " // 电表波特率无效\n");
		return -1;
	}
	else
	{
		speed *= 300;
	}
*/

	// 把符合要求的参数放到停电判断的队列中
	gMeterHalt_Info.MeterHaltInfo[realMeterHaltNum].Addr = oopMeter.basic.tsa;
	gMeterHalt_Info.MeterHaltInfo[realMeterHaltNum].Pn = Pn;
	gMeterHalt_Info.MeterHaltInfo[realMeterHaltNum].commnum = oopMeter.basic.port;
	gMeterHalt_Info.MeterHaltInfo[realMeterHaltNum].baud = oopMeter.basic.baud;
	gMeterHalt_Info.MeterHaltInfo[realMeterHaltNum].ctrtype =  3 + (2<<2);//默认偶校验
    gMeterHalt_Info.MeterHaltInfo[realMeterHaltNum].protocol = oopMeter.basic.protocol;

	return 1;
}

/*******************************************************************************
* 函数名称: ReadMeterHaltPara
* 函数功能: 电表的停上电参数初始化
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: 无
*******************************************************************************/
int ReadMeterHaltPara(void)
{
	uint8       i;
	uint32      Pn;
	uint16      meterHaltNum = 0;
	uint8       realMeterHaltNum = 0;
	uint8       sampleFlag = 0;
    OOP_METER_T oopMeter;
        
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
   
    meterHaltNum = event_3106.eventParam.cParam.meterNum;
    if(meterHaltNum > 6)
	{
		EVENT_FMT_LOGCC("CheckERC_3106 停上电事件 电能表个数[%d]>6异常\n", meterHaltNum);
		meterHaltNum = 1;
	}

    sampleFlag = event_3106.eventParam.cParam.flag;
	if (sampleFlag == 0 || sampleFlag == 0xFF)
	{
		EVENT_FMT_LOGCC("CheckERC_3106 停上电事件 判断停电数据采集标志[%X]异常\n", sampleFlag);
		sampleFlag = 0XC0;
	}

	EVENT_FMT_TRACE("CheckERC_3106 停上电事件 配置电能表个数[%d]\n", meterHaltNum);

	for (i = 0; i < meterHaltNum; i++)
	{
		for (Pn=0; Pn<NO_VIR_MTR_MAX_NUM; Pn++)
		{
			//根据TSA地址判断是否存在此测量点信息
			if (app_get_meter_list(Pn+1, &oopMeter) == 0 && CompareTSA(oopMeter.basic.tsa, event_3106.eventParam.cParam.meter[i]) == 0)
			{
                // 找到可以参与停上电判断的电表并将参数赋值
                if (GetParaAndValidCheck(Pn+1, realMeterHaltNum) > 0)
                {
                	realMeterHaltNum++;
                }
			}
		}
	}
	
	//没有配置电表，随机找寻2-3个电表
	if ((meterHaltNum == 0) || (sampleFlag & 0x40))
	{
		realMeterHaltNum = 0;
		for (Pn = 0; Pn < NO_VIR_MTR_MAX_NUM; Pn++)
		{
			// 找到可以参与停上电判断的电表并将参数赋值
			if (GetParaAndValidCheck(Pn+1, realMeterHaltNum) > 0)
			{
				realMeterHaltNum++;
			}

			// 未配置个数时最大选择1快表
			if ((meterHaltNum == 0) && (realMeterHaltNum >= 1))
			{
				break;
			}

			// 设置了个数按设置的来
			if ((meterHaltNum != 0) && (realMeterHaltNum >= meterHaltNum))
			{
				break;
			}
		}
	}
	EVENT_FMT_TRACE("CheckERC_3106 指定测量点的实际个数 realMeterHaltNum = %d\n", realMeterHaltNum);
	// 未找到电表
	if (realMeterHaltNum == 0)
	{
		return -1;
	}

	gMeterHalt_Info.MeterHaltNum = realMeterHaltNum;
	if (gMeterHalt_Info.CheckMeterHalt == 1)//如果是上电检测那么清除穿透结果
	{
		gMeterHalt_Info.MeterHaltIndex = 0;
		gMeterHalt_Info.MeterIdNum = 0;
	}

	gMeterHalt_Info.HaltTaskStartTime = time(NULL);//赋值任务起始时间

	return 1;
}

/*******************************************************************************
* 函数名称: MergePowerOffTime
* 函数功能: 终端与电表停电记录对比
* 输入参数: checkFlag  0:不判断电表的记录,   1:判断电表记录
* 输出参数:
* 内部处理:
* 返 回 值: 属性标志    D0 0: 属性异常, 1:属性正常;
						D1 0: 事件无效, 1:事件有效
*******************************************************************************/
uint8 PowerOffTimeCheck(int checkFlag)
{
	uint16      minTime = 0;
	uint16      maxTime = 0;
	uint16      startTime = 0;
	uint16      secTime = 0;
	uint8       flag = 1;
	uint8       allUser = 0;
	int         tmnTimeDiff = 0;
	int         mtrTimeDiff = 0;
	uint32      tmnStartTime = 0;
	uint32      tmnEndTime = 0;
	uint32      mtrStartTime = 0;
	uint32      mtrEndTime = 0;
	int       i = 0;

	//读取事件限值参数
	minTime = event_3106.eventParam.lParam.minInterval;
	if (minTime == 0xFFFF || minTime == 0)
	{
		minTime = 1;
	}

    maxTime = event_3106.eventParam.lParam.maxInterval;
	if ( maxTime == 0xFFFF || maxTime == 0)
	{
		maxTime = 3 * 1440;
	}

    startTime = event_3106.eventParam.lParam.limit1;
	if (startTime == 0xFFFF || startTime == 0)
	{
		startTime = 5;
	}

    secTime = event_3106.eventParam.lParam.limit2;
	if (secTime == 0xFFFF || secTime == 0)
	{
		secTime = 120;
	}
	else
	{
		secTime *= 60;
	}
	// 防止主台设得很小导致现场不能正常产生
	if (startTime < 5)
	{
		startTime = 5;
	}

	//由于上电耗时较长，将区间差值增加
	EVENT_FMT_TRACE("CheckERC_3106 minTime=%hu, maxTime=%hu起止偏差StartTime=%hu 区段偏差SecTime=%hu -----------\n", minTime, maxTime, startTime, secTime);
    
	//对比停电时间
    ooptm_to_systm(event_3106.tmnStarttime, (time_t *)&tmnStartTime);
    ooptm_to_systm(event_3106.tmnEndtime, (time_t *)&tmnEndTime);
	tmnTimeDiff = tmnEndTime - tmnStartTime;


	//判断是否为属性正常事件
	if (tmnTimeDiff < minTime * 60 || tmnTimeDiff > maxTime * 60)
	{
		flag = 0;
	}

	//判断是否为有效事件
	for (i = 0; (i < gMeterHalt_Info.MeterHaltNum) && (checkFlag == 1); i++)
	{
		ooptm_to_systm(event_3106.metStarttime[i], (time_t *)&mtrStartTime);
		ooptm_to_systm(event_3106.metEndtime[i], (time_t *)&mtrEndTime);
        EVENT_FMT_TRACE("电能表[%u]停电时间%u-%u-%u %u:%u:%u \n", i,        event_3106.metStarttime[i].year, event_3106.metStarttime[i].month, event_3106.metStarttime[i].day, event_3106.metStarttime[i].hour, event_3106.metStarttime[i].minute, event_3106.metStarttime[i].second);
        EVENT_FMT_TRACE("电能表[%u]上电时间%u-%u-%u %u:%u:%u \n", i,        event_3106.metEndtime[i].year, event_3106.metEndtime[i].month, event_3106.metEndtime[i].day, event_3106.metEndtime[i].hour, event_3106.metEndtime[i].minute, event_3106.metEndtime[i].second);
		mtrTimeDiff = mtrEndTime - mtrStartTime;

		EVENT_FMT_LOGCC("TmnStartTime=%d MtrStartTime=%d TmnTimeDiff=%d MtrTimeDiff=%d ------\n", tmnStartTime, mtrStartTime, tmnTimeDiff, mtrTimeDiff);
        
		//有一块电表符合时，算台区停电
		if ((COMMFUN_ABS(mtrStartTime, tmnStartTime) <= (startTime * 60)) && (COMMFUN_ABS(tmnTimeDiff, mtrTimeDiff) <= secTime))
		{
			allUser = 1;
		}
		//上电时间早于停电时间，电表事件有误，记录
		if (mtrTimeDiff < 0)
		{
			allUser = 0;
		}

		// 有一块有效就跳出来判断
		if (allUser == 1)
		{
			break;
		}
	}

	EVENT_FMT_TRACE("CheckERC_3106 flag = %u   allUser = %u\n", flag, allUser);
	// 组合成D0D1的属性标志
	flag |= (allUser << 1);
	return flag;
}
/*******************************************************************************
* 函数名称: MergePowerOffTime
* 函数功能: 终端与电表停电记录对比
* 输入参数: checkFlag   0:不判断电表的记录,
						1:判断电表记录,
						2:不判断电表记录，根据属性正常、异常判断要不要继续抄读
						  异常时直接记录事件不进行抄读
* 输出参数:
* 内部处理:
* 返 回 值:
*******************************************************************************/
int MergePowerOffTime(int checkFlag)
{
	uint8       powerOffFlag = 0;

    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据     
    int  ret = 0;

    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T)); 
    
    if (event_3106.ishappen == 0x55 )
    {
#ifdef AREA_HUNAN
        event_3106.eventFlag = 0xc0;        //默认正常有效
        int i;
        for (i = 0; (i < gMeterHalt_Info.MeterHaltNum) && (checkFlag == 1); i++)
        {    
            EVENT_FMT_TRACE("g_val_a[%d]=%d\n", i, g_val_a[i]);
            
            //有一块电表不符合时，无效
            if (g_val_a[i] > 2200*0.6)
            {
                event_3106.eventFlag = 0;
                break;
            } 
        }
#endif
    }
    else
    {
    	powerOffFlag = PowerOffTimeCheck(checkFlag);
        if (checkFlag == 1)
        {
        	event_3106.eventFlag = (((powerOffFlag << 7) & 0x80) | ((powerOffFlag << 5) & 0x40));
        }
        else
        {
        	event_3106.eventFlag = ((powerOffFlag << 7) & 0x80);
        }
    }

	EVENT_FMT_TRACE("CheckERC_3106 eventFlag = %02x！\n", event_3106.eventFlag);

    // 上电时如果所有电表都和终端停电时间不一致则返回0继续抄读
	if ((checkFlag == 1) && ((event_3106.eventFlag & 0x40) == 0) && event_3106.ishappen == 0xaa && gMeterHalt_Info.Relaydstat == RELAY_FAIL)
	{
		return 0;
	}

	// 上电时先判断属性是否正常，先不记录事件等抄完表后判断
	// 如果不正常，没必要再去抄读事件判断
	if ((checkFlag == 2) && ((event_3106.eventFlag & 0x80) != 0))
	{
		return 0;
	}

#ifdef AREA_HUNAN
    if(event_3106.ishappen == 0x55)
    {
        //事件发生源
        eventSource.choice = DT_ENUM;
        eventSource.nValue = 0;             //停电

        //事件关联数据

        //写数据库
        ret = WriteEventToDB(gDbCenterClient, 0, &event_3106.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3106 终端上电事件记录失败返回%d\n", ret);
            return 0;
        }
        EVENT_FMT_LOGCC("CheckERC_3106 终端上电事件记录成功, storagenum=%u !\n", event_3106.eventCommInfo.storagenum);
        return 1;
    }
        
#endif

    //事件发生源
    eventSource.choice = DT_ENUM;
    eventSource.nValue = 1;             //上电

    //事件关联数据

    //写数据库
    ret = WriteEventToDB(gDbCenterClient, 0, &event_3106.eventCommInfo, eventSource, eventData, 0);
    if (ret < ERR_OK)
    {
        EVENT_FMT_DEBUG("CheckERC_3106 终端上电事件记录失败返回%d\n", ret);
        return 0;
    }
    EVENT_FMT_LOGCC("CheckERC_3106 终端上电事件记录成功, storagenum=%u !\n", event_3106.eventCommInfo.storagenum);



	return 1;
}
/*******************************************************************************
* 函数名称: CheckMeter_Halt
* 函数功能: 电表的停上电检测
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: 无
*******************************************************************************/
int CheckMeter_Halt(void)
{
	char                 SendFrameBuf[128];
	uint16               SendFrameLen = 0;
	uint8                Period;           //抄表间隔
	uint16               MaxOverTime = 0;      //抄表限值
	OOP_DATETIME_S_T     TmnTime;          //终端时间
	static int           countTime = 0;
    int      realMeterHaltNum = 0;

	current_tm_to_ooptm(&TmnTime);//-取实时时间-
	//判断停电数据采集标志
	EVENT_FMT_TRACE("CheckERC_3106 采集标志%02X \n", event_3106.eventParam.cParam.flag);
    
	if ((event_3106.eventParam.cParam.flag & 0X80) == 0)
	{
		//只判断终端事件
		if (gMeterHalt_Info.CheckMeterHalt == 1)
		{
			MergePowerOffTime(0);
			gMeterHalt_Info.CheckMeterHalt = 0;
			gMeterHalt_Info.MeterHalt_Stat = HALT_IDLE;
		}
		EVENT_FMT_LOGCC("CheckERC_3106 停电数据采集标志无效，不采集电能表数据\n");
	}
	//抄表间隔配置信息
	Period = event_3106.eventParam.cParam.interval;
	if (Period == 0 || Period == 0xFF)
	{
		Period = 24;//默认24小时
		EVENT_FMT_LOGCC("CheckERC_3106 判断停电事件抄读时间间隔[%02X]无效，设置默认24小时\n", Period);
	}

	//抄读限值时间
	MaxOverTime = event_3106.eventParam.cParam.limit;
	if (MaxOverTime == 0x00FF || MaxOverTime == 0)
	{
		MaxOverTime = 70;
		EVENT_FMT_LOGCC("CheckERC_3106 判断停电事件抄读时间限值[%04X]无效，设置默认70秒\n", MaxOverTime);
	}
	else
	{
		// 转换成秒
		MaxOverTime *= 60;
	}

	// 默认成70秒防止表台测试不过
	if (MaxOverTime > 70)
	{
		MaxOverTime = 70;
	}
    realMeterHaltNum = gMeterHalt_Info.MeterHaltIndex;
    if(gMeterHalt_Info.MeterHaltInfo[realMeterHaltNum].commnum.nObjID==0xF209)
    {
        MaxOverTime = event_3106.eventParam.cParam.limit*60;
        EVENT_FMT_TRACE("载波端口 电表的MaxOverTime=%u\n", MaxOverTime);
    }
    else
    {
        MaxOverTime = 70;
        EVENT_FMT_TRACE("非载波端口 电表的MaxOverTime=%u\n", MaxOverTime);
    }
	switch (gMeterHalt_Info.MeterHalt_Stat)
	{
	case HALT_IDLE://空闲时空转
		break;

	case HALT_INIT: //电表停电抄读初始化
		// 完全断电的上电初始化要停留5秒钟防止抄表进程未启动好
		if (countTime++ < 5)
		{
			break;
		}

		EVENT_FMT_TRACE("CheckERC_3106 电表的停上电检测初始化\n");

		if (ReadMeterHaltPara() > 0)
		{
			gMeterHalt_Info.Relaydstat = RELAY_SEND;
			gMeterHalt_Info.MeterHalt_Stat = HALT_RELAYD;
		}
		else
		{
			gMeterHalt_Info.MeterHalt_Stat = HALT_IDLE;

			if (gMeterHalt_Info.CheckMeterHalt == 1)
			{
				MergePowerOffTime(0);
				gMeterHalt_Info.CheckMeterHalt = 0;
				gMeterHalt_Info.MeterHalt_Stat = HALT_IDLE;
			}
		}
		break;
	case HALT_RELAYD://电表停电抄读中继
		EVENT_FMT_TRACE("CheckERC_3106 电表的停上电中继\n");

		uint8    IdMtTyp = 0;
		

		realMeterHaltNum = gMeterHalt_Info.MeterHaltIndex;
		IdMtTyp = gMeterHalt_Info.MeterHaltInfo[realMeterHaltNum].protocol;
        EVENT_FMT_TRACE("CheckERC_3106 电表的IdMtTyp=%u\n", IdMtTyp);

        if (IdMtTyp == (uint8)PROTO_698)
    	{
    		MakeFramOop(SendFrameBuf, &SendFrameLen);   //组帧
    	}
    	else if (IdMtTyp == (uint8)PROTO_07)
    	{
    		MakeFram_07(SendFrameBuf, &SendFrameLen);//组帧
    	}
        
		if (MeterHalt_Relayd(SendFrameBuf, SendFrameLen) > 0)
		{
			gMeterHalt_Info.MeterIdNum++;
			gMeterHalt_Info.Relaydstat = RELAY_SEND;

			//数据索引如果大于最大抄读下移个电表
			if (((IdMtTyp == (uint8)PROTO_07) && (gMeterHalt_Info.MeterIdNum >= 2)) || ((IdMtTyp == (uint8)PROTO_698) && (gMeterHalt_Info.MeterIdNum >= 1)))
			{
				gMeterHalt_Info.MeterHaltIndex++;
				gMeterHalt_Info.MeterIdNum = 0;
			}

			//如果电表都抄读完成
			if (gMeterHalt_Info.MeterHaltIndex == gMeterHalt_Info.MeterHaltNum)
			{
				gMeterHalt_Info.MeterHalt_Stat = HALT_DEAL;//电表停电抄读处理
			}
		}
		break;

	case HALT_RETRY: //电表停电抄读中继重试
		EVENT_FMT_TRACE("CheckERC_3106 电表停电抄读中继重试 :%u \n", time(NULL) - gMeterHalt_Info.HaltTaskStartTime);
		if (COMMFUN_ABS(time(NULL), gMeterHalt_Info.HaltTaskStartTime) > MaxOverTime)//如果超时
		{
		    EVENT_FMT_LOGCC("CheckERC_3106 电表停电抄读中继重试超时, %u,%u \n", time(NULL), gMeterHalt_Info.HaltTaskStartTime);
			if (gMeterHalt_Info.CheckMeterHalt == 1)
			{
				MergePowerOffTime(0);
				gMeterHalt_Info.CheckMeterHalt = 0;
			}
			gMeterHalt_Info.MeterHalt_Stat = HALT_IDLE;
			break;
		}

		EVENT_FMT_TRACE("CheckERC_3106 电表停电继续抄读处理进入重试机制\n");
		gMeterHalt_Info.Relaydstat = RELAY_SEND;
		gMeterHalt_Info.MeterHalt_Stat = HALT_RELAYD;//电表停电抄读处理
		break;

	case HALT_DEAL: //电表停电抄读处理
		EVENT_FMT_TRACE("CheckERC_3106 电表停电抄读处理\n");

		//将电表停电事件与终端对比
		EVENT_FMT_TRACE("CheckERC_3106  gMeterHalt_Info.CheckMeterHalt = %d gMeterHalt_Info.MeterHaltIndex = %d\n",
			gMeterHalt_Info.CheckMeterHalt, gMeterHalt_Info.MeterHaltIndex);

		if (gMeterHalt_Info.CheckMeterHalt == 1)
		{
			// 上电检测不符合停电判断重试
			if (MergePowerOffTime(1) == 0)
			{
				gMeterHalt_Info.MeterHalt_Stat = HALT_RETRY;
			}
			else
			{
				gMeterHalt_Info.CheckMeterHalt = 0;
				gMeterHalt_Info.MeterHalt_Stat = HALT_IDLE;
			}
		}
		else
		{
			gMeterHalt_Info.MeterHalt_Stat = HALT_IDLE;
		}
		gMeterHalt_Info.MeterHaltIndex = 0;
		gMeterHalt_Info.MeterIdNum = 0;
		break;
	}

	return 1;
}

#endif

#if DESC("电能表数据变更监控",1)

/***********************************************************************
* @name：       evt_oadinfo_get
* @brief：      查询ODA列表，得到OAD描述信息
* @param[in] ：OOP_OAD_U oad        查询OAD
* @param[out]：OAD_INFO_T *pstOdaInfo     ODA描述信息
* @return    ：0-成功/错误码 
* @author    : 梅安宁
* @Date      ：2020-03-26
**********************************************************************/
int evt_oadinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo)
{
    uint8       OIA  = 0;
    uint8       OIA1 = 0;
    uint8       OIA2 = 0;  
    uint32      size = 0;
    const OAD_TAB_T   *pstOdaTab = NULL;

    OIA = (uint8)(oad.nObjID>>8);
    OIA1 = (uint8)((OIA>>4)&0x0f);
    OIA2 = (uint8)((OIA)&0x0f);

    //总分格式的数据在数据中心 如果oad是分 则存到总里面 获取总OAD信息
    if(oad.nIndex != 0)
    {   
        pstOdaInfo->nIndex = oad.nIndex;
        oad.nIndex = 0;
    }

    switch(OIA1)
    {
        case 0x00: pstOdaTab = oadEnergyInfoList; size = oadEnergyInfoListLen; break;
        case 0x01: pstOdaTab = oadDemandInfoList; size = oadDemandInfoListLen; break; 
        case 0x02: pstOdaTab = oadVariateInfoList; size = oadVariateInfoListLen; break;
        case 0x03: pstOdaTab = oadEvnetInfoList; size = oadEvnetInfoListLen; break;    
        case 0x04: pstOdaTab = oadParaInfoList; size = oadParaInfoListLen; break;
        case 0x05: pstOdaTab = oadFreezeInfoList; size = oadFreezeInfoListLen; break; 
        case 0x06: pstOdaTab = oadCollectInfoList; size = oadCollectInfoListLen; break;
        case 0x07: pstOdaTab = oadGatherInfoList; size = oadGatherInfoListLen; break; 
        case 0x08: pstOdaTab = oadControlInfoList; size = oadControlInfoListLen; break;  
        case 0x0f: 
        {
            switch(OIA2)
            {
                case 0x00: pstOdaTab = oadFileInfoList; size = oadFileInfoListLen; break;
                case 0x01: pstOdaTab = oadEsamInfoList; size = oadEsamInfoListLen; break;   
                case 0x02: pstOdaTab = oadIoInfoList;   size = oadIoInfoListLen; break;  
                default:break; 
            }
        }break;
        default:break;
    }

    if(pstOdaTab != NULL)
    {
        while(size > 0)
        {
            if((pstOdaTab->oad.value == oad.value) ||
                 ( (pstOdaTab->oad.nObjID == oad.nObjID) &&
                   (pstOdaTab->oad.attID == oad.attID) &&
                   (pstOdaTab->oad.nIndex == oad.nIndex)
                 )
               )
            {
                pstOdaInfo->pTab = pstOdaTab;
                return 0;
            }
               
            size--;
            pstOdaTab++;
        }
    }

    EVENT_FMT_DEBUG("search oadInfoList miss. OAD(0x%08x), OIA1(%d), OIA2(%d)\n", oad.value, OIA1, OIA2);
    
    return -1;
}



/***********************************************************************
* @name：       evt_subdata_get_byoad
* @brief：      通过ODA描述信息从总的数据中获取分项数据
* @param[in] ：OAD_INFO_T stOdaInfo     ODA描述信息
                void *srcBuf            总项数据
                uint32 srcLen
* @param[out]： void *dstData           分项数据
                uint32 *dstLen

* @return    ：0-成功/错误码 
* @author    : 梅安宁
* @Date      ：2020-03-26
**********************************************************************/
int evt_subdata_get_byoad(OOP_OAD_U oad, void *srcBuf, uint32 srcLen, void *dstData, uint32 *dstLen)
{
    int ret         = 0;
    OAD_INFO_T  stOdaInfo;    //总分格式的数据信息

    memset(&stOdaInfo,0,sizeof(OAD_INFO_T));

    ret = evt_oadinfo_get(oad, &stOdaInfo);
    if(ret < 0 || stOdaInfo.pTab->dataLen != srcLen)
    {   
        return -1;
    }

    //如果是array格式 则认为是总分格式的 
    if(stOdaInfo.pTab->dataType == DT_ARRAY)
    {
        switch(stOdaInfo.pTab->eData)
        {
        case E_OOP_ENERGY_T:
        {
            OOP_ENERGY_T *precord = (OOP_ENERGY_T *)srcBuf;
            if(stOdaInfo.nIndex > precord->nNum)
            {
                return -2;
            }
            *dstLen = sizeof(uint32);
            memcpy(dstData,&precord->nValue[stOdaInfo.nIndex-1],*dstLen);
        }
            break;
        case E_OOP_ENERGYS_T:
        {
            OOP_ENERGYS_T *precord = (OOP_ENERGYS_T *)srcBuf;
            if(stOdaInfo.nIndex > precord->nNum)
            {
                return -2;
            }
            *dstLen = sizeof(int);
            memcpy(dstData,&precord->nValue[stOdaInfo.nIndex-1],*dstLen);
        }
            break;
        case E_OOP_HENERGY_T:
        {
            OOP_HENERGY_T *precord = (OOP_HENERGY_T *)srcBuf;
            if(stOdaInfo.nIndex > precord->nNum)
            {
                return -2;
            }
            *dstLen = sizeof(uint32);
            memcpy(dstData,&precord->nValue[stOdaInfo.nIndex-1],*dstLen);
        }
            break;
        case E_OOP_HENERGYS_T:
        {
            OOP_HENERGYS_T *precord = (OOP_HENERGYS_T *)srcBuf;
            if(stOdaInfo.nIndex > precord->nNum)
            {
                return -2;
            }
            *dstLen = sizeof(int64);
            memcpy(dstData,&precord->nValue[stOdaInfo.nIndex-1],*dstLen);
        }
            break;
        case E_OOP_DEMAND_T:  //需量
        case E_OOP_DEMANDS_T:
        {
            OOP_DEMAND_T *precord = (OOP_DEMAND_T *)srcBuf;
            if(stOdaInfo.nIndex > precord->nNum)
            {
                return -2;
            }
            *dstLen = sizeof(DEMAND);
            memcpy(dstData,&precord->demand[stOdaInfo.nIndex-1],*dstLen);
        }
            break;
        case E_OOP_WORD3V_T://分相变量
        {
            OOP_WORD3V_T *precord = (OOP_WORD3V_T *)srcBuf;
            if(stOdaInfo.nIndex > precord->nNum)
            {
                return -2;
            }
            *dstLen = sizeof(uint32);
            memcpy(dstData,&precord->nValue[stOdaInfo.nIndex-1],*dstLen);
        }
            break;
        case E_OOP_INT4V_T://功率变量
        {
            OOP_INT4V_T *precord = (OOP_INT4V_T *)srcBuf;
            if(stOdaInfo.nIndex > precord->nNum)
            {
                return -2;
            }
            *dstLen = sizeof(int);
            memcpy(dstData,&precord->nValue[stOdaInfo.nIndex-1],*dstLen);
        }
            break;
        case E_OOP_METWORD_T:
        {
            OOP_METWORD_T *precord = (OOP_METWORD_T *)srcBuf;
            if(stOdaInfo.nIndex > precord->nNum)
            {
                return -2;
            }
            *dstLen = sizeof(uint16);
            memcpy(dstData,&precord->metWord[stOdaInfo.nIndex-1],*dstLen);
        }
            break;
        case E_OOP_LONG4V_T:
        {
            OOP_LONG4V_T *precord = (OOP_LONG4V_T *)srcBuf;
            if(stOdaInfo.nIndex > precord->nNum)
            {
                return -2;
            }
            *dstLen = sizeof(int16);
            memcpy(dstData,&precord->nValue[stOdaInfo.nIndex-1],*dstLen);
        }
            break;
        default:
            EVENT_FMT_DEBUG("oad&08x, OdaInfo.pTab->eData[%u] err\n", stOdaInfo.pTab->eData);
            return -1;

        }

        return 0;
    }

    
    EVENT_FMT_DEBUG("oad&08x,stOdaInfo.pTab->dataType[%u]!=%u\n",stOdaInfo.pTab->oad.value, stOdaInfo.pTab->dataType, DT_ARRAY);    
    return -1;
}


/**********************************************************************
* @name      : CheckCsdData
* @brief     : 检查CSD对应的数据是否发生变更
* @param[in] ：OOP_OAD_U	 oad        数据OAD
               mtrnum_t mtr_id      测量点号
* @param[out]：uint8* last2Data      变更前数据
               uint8 *lastData      变更后数据
* @return    ：<0 错误码，>0 数据长度
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int evt_check_csd_data(mtrnum_t mtr_id, OOP_OAD_U	oad, uint8* last2Data, uint8 *lastData)
{
    time_t      timep           = 0;
    int         ret             = 0;
    uint32      taskInterval    = 0;            //任务执行间隔 单位秒
    uint32      handle          = 0; 
    uint32      recNum          = 0;
    uint32      lastLen         = 0;
    uint32      last2Len        = 0;
    static uint32       aftTime[MTR485_MAX_NUM] = {0};    //上第2次记录时间，防止多次判断
    READ_RECORD_T       startReadRecord;
    RECORD_UNIT_T       stepRecordOut;
    STEP_READ_RECORD_T  tRecord;
    OOP_METER_T         oopMeter;
    OOP_TI_T            oopTI;
    
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&oopTI, 0, sizeof(OOP_TI_T));
    memset(&startReadRecord,0,sizeof(READ_RECORD_T));
    memset(&stepRecordOut,0,sizeof(RECORD_UNIT_T));
    memset(&tRecord,0,sizeof(STEP_READ_RECORD_T));
    
    if (app_get_meter_list(mtr_id, &oopMeter) != ERR_OK)
    {
        //如果没有读取到电表抄表标志，直接返回
        EVENT_FMT_DEBUG("event_311C[%hu] 档案无效\n", mtr_id);
        return ERR_NORMAL;
    }
    app_get_meter_list(mtr_id, &oopMeter);
    if (GetTaskOopTI(event_311C.nTID, &oopTI) != ERR_OK)
    {
        //如果没有读取到电表抄表标志，直接返回
        EVENT_FMT_DEBUG("event_311C[%hu] 任务%hu无效\n", mtr_id, event_311C.nTID);
        return ERR_NORMAL;
    }
    
    taskInterval = OopTIToSec(oopTI);  

    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep
    timep += COMPENSATION_TIME;
    startReadRecord.recordOAD.optional = 0;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.road.oadCols.oad[0] = oad;
    startReadRecord.recordOAD.road.oadCols.oad[0].nIndex = 0;
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = mtr_id;
    startReadRecord.cType = 0x02;                           //以结束时间筛选
    startReadRecord.cStart = timep-(taskInterval*2);        //取最新两条记录数据
    startReadRecord.cEnd = timep;
    startReadRecord.sortType = 1;
    startReadRecord.MAC.nNum = 6;
    memcpy(startReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
    //读取电表时间
    ret = db_read_record_start(gDbCenterClient, &startReadRecord, &handle, &recNum);
    if (ret != ERR_OK ||recNum < 2)
    {
        //召测数据错误，不判定为有事件发生
        EVENT_FMT_DEBUG("event_311C[%hu] db_read_record_start %08X error ret%d or recNum[%u]<2\n", mtr_id, oad.value, ret, recNum);
        return ERR_NORMAL;
    }

    tRecord.MAC = startReadRecord.MAC;
    ret = db_read_record_step(gDbCenterClient, handle, &tRecord, &stepRecordOut);
    if (ret != ERR_OK || stepRecordOut.recordnum < 1 || stepRecordOut.record[0].outDatalen == 0)
    {
        //上第1次没召测到，不判定为有事件发生
        EVENT_FMT_DEBUG("event_311C[%hu] db_read_record_step1 %08X error ret%d or read data errrecordnum[%u], outDatalen[%u]\n"
                        , mtr_id, oad.value, ret, stepRecordOut.recordnum, stepRecordOut.record[0].outDatalen);
        return ERR_NORMAL;
    }

    //总分格式的数据，需要从总的获取分数据
    if(oad.nIndex != 0)
    {   
        ret = evt_subdata_get_byoad(oad, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen, lastData, &lastLen);
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        lastLen = stepRecordOut.record[0].outDatalen;
        //事件结构体中，最大长度，防止溢出
        if(lastLen > 64)
            lastLen = 64;
        memcpy(lastData, stepRecordOut.record[0].outDataBuf, lastLen);
    } 

    ret = db_read_record_step(gDbCenterClient, handle, &tRecord, &stepRecordOut);
    if (ret != ERR_OK || stepRecordOut.recordnum < 1 || stepRecordOut.record[0].outDatalen == 0)
    {
        //上第2次没召测到，不判定为有事件发生
        EVENT_FMT_DEBUG("event_311C[%hu] db_read_record_step2 %08X error ret%d or read data errrecordnum[%u], outDatalen[%u]\n"
                        , mtr_id, oad.value, ret, stepRecordOut.recordnum, stepRecordOut.record[0].outDatalen);
        return ERR_NORMAL;
    }

    if(aftTime[mtr_id-1] == stepRecordOut.colEndTm)
    {
        //时间相同，已经判断过，
        return 0;
    }
    aftTime[mtr_id-1] = stepRecordOut.colEndTm;

    if(oad.nIndex != 0)
    {   
        ret = evt_subdata_get_byoad(oad, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen, last2Data, &last2Len);
        if(ret < 0)
        {
            return ret;
        }
    }
    else
    {
        last2Len = stepRecordOut.record[0].outDatalen;
        //事件结构体中，最大长度，防止溢出
        if(last2Len > 64)
            last2Len = 64;
        memcpy(last2Data, stepRecordOut.record[0].outDataBuf, last2Len);
    }
    
    //if(last2Len != last2Len)
    //{
        //同一个oad数据长度不一样，必须有问题
    //    return 0;
    //}
    if(memcmp(last2Data, lastData, lastLen) == 0)
    {
        return 0;
    }

    return lastLen;

}

#endif

#if DESC("终端相序异常事件",1)

/**********************************************************************
* @name      : get_evt_data_300F
* @brief     : 获取事件关联数据
* @param[in] ：int clientid    db客户端
               uint8 evt_flag   事件标志，1发生，2恢复
               OOP_EVTBUF_T* pevt_data   事件关联数据
* @param[out]：
* @return    ：0成功/<0异常
* @Create    : 梅安宁
* @Date      ：2021-03-18
* @Update    :
**********************************************************************/
int get_evt_data_item(int clientid, uint8 evt_flag, uint32 oad_value, uint8 item, OOP_EVTBUF_T* pevt_data)
{
    OOP_ENERGY_T  tenergy;
    uint8*        pbuf      = NULL;
    uint32        normal_value = 0;

    pevt_data->cols.oad[item].value = oad_value;
    pevt_data->idLen[item] = 4;

    normal_value = oad_value&0xffff0ff0;

    if(evt_flag == 1)
    {
        if(item < 16)
        {
            pbuf = pevt_data->buf + item*4;
        }
        else
        {
            memset(pevt_data->buf + item*4,0xFF,4);
        }
            
    
	}
    else
    {
        if((item) > 15 && (item < 32))
        {
            pbuf = pevt_data->buf + item*4;
        }
    
	}

    if(pbuf != NULL)
    {
        memset(&tenergy,0,sizeof(OOP_ENERGY_T));
        evt_read_nomal_data(clientid, 1, 0, normal_value, sizeof(OOP_ENERGY_T), (uint8*)&tenergy);
        if(tenergy.nNum > 0 && tenergy.rsv&0x01)
        {
            memcpy(pbuf,tenergy.nValue,sizeof(uint32));
        }
    }
    return 0;
}

/**********************************************************************
* @name      : get_evt_data_300F
* @brief     : 获取事件关联数据
* @param[in] ：int clientid    db客户端
               uint8 evt_flag   事件标志，1发生，2恢复
               OOP_EVTBUF_T* pevt_data   事件关联数据
* @param[out]：
* @return    ：0成功/<0异常
* @Create    : 梅安宁
* @Date      ：2021-03-18
* @Update    :
**********************************************************************/
int get_evt_data_300F(int clientid, uint8 evt_flag, OOP_EVTBUF_T* pevt_data)
{
    uint8         item      = 0;
    uint32        aatPro    = 0;  //事件oad属性标识
    uint32        item_oad  = 0;  
    int           i, j;
    
    //事件关联数据
    pevt_data->oadNum = 32;
    pevt_data->cols.nNum = 32;
    pevt_data->len = 4*32;

    //循环拼接关联数据项的oad
    for(i=0x00000201; i<0x00040201; i+=0x00010000)
    {
        for(j=0x00100000; j<0x00500000; j+=0x00100000)
        {
            if(item < 16)
            {
                aatPro = 0x00002000;   //属性标识 事件发生前
            
}
            else
            {
                aatPro = 0x00008000;    //属性标识 事件发生后
            
}
            item_oad = i + j + aatPro;
            get_evt_data_item(clientid, evt_flag, item_oad, item, pevt_data);
            item++;
        }
    }

    //循环拼接关联数据项的oad
    for (i = 0x00000201; i < 0x00040201; i += 0x00010000)
    {
        for (j = 0x00100000; j < 0x00500000; j += 0x00100000)
        {
            if (item < 16)
            {
                aatPro = 0x00002000;   //属性标识 事件发生前

            }
            else
            {
                aatPro = 0x00008000;    //属性标识 事件发生后

            }
            item_oad = i + j + aatPro;
            get_evt_data_item(clientid, evt_flag, item_oad, item, pevt_data);
            item++;
        }
    }

    return 0;
    
}
#endif


/*********************************************************************
功能描述：根据搜表结果判断是否在当前档案中
-			
输出参数：
-			
函数返回值：
-			0				失败
-           1               成功
内部处理流程：
********************************************************************/
BOOLEAN check_search_meter_res(OOP_SEARCHMET_RES_T *res)
{
    uint32 i;
    OOP_METER_T oopMeter ;
    for(i=1;i<=NO_VIR_MTR_MAX_NUM;i++)
    {
        if(app_get_meter_list(i, &oopMeter)!=ERR_OK)
        {
            continue;
        }
        //三个都一致才算相同
        if(CompareTSA(oopMeter.basic.tsa,res->commAdd)==0 && CompareTSA(oopMeter.extend.add,res->collAdd)==0 && oopMeter.basic.protocol == res->proto)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*******************************************************************************
所属功能单元：事件预处理函数
功能描述：调用相对应的功能处理函数预先生成告警所需的数据(读取测量点基本参数F25)
输入参数：
  mtr_id  测量点号
输出参数：无
函数返回值：无
内部处理流程：读取当前计量点的数据及其参数填写到该计量点相对应的全局信息结构体,填
	充电压电流到内存,为之后的告警程序提供数据基。(接线方式,三相电压,电流,当前正向
	有功电能,上一次编程时间电池工作状态,额定电流,电流过负荷的相对额定值比例,额定电压)
********************************************************************************/
void FillEventGlobal(mtrnum_t mtr_id, OOP_METER_T oopMeter)
{
	if (mtr_id >= NO_VIR_MTR_MAX_NUM)//-不需要记录-
    {
        EVENT_FMT_DEBUG(" mtr_id[%d] too big \n", mtr_id);
        return ;
    }
	
    if(oopMeter.basic.port.nObjID == OI_485)
    {
        //抄表成功标记
        switch (event_310F.relayState[mtr_id-1])
        {
        case RELAY_IDEL://发送消息
            event_310F.relayState[mtr_id-1] = RELAY_SEND;
            GetReadMeterStatus(oopMeter.basic.tsa, mtr_id);
            break;
        case RELAY_WATT: //等待接收
//            EVENT_FMT_TRACE("CheckERC_310F[%hu] 等待接收抄表状态中……!\n", mtr_id);
            EVENT_FMT_TRACE("CheckERC_310F[%hu] 未收到消息，重试……!\n", mtr_id);
            event_310F.relayState[mtr_id-1] = RELAY_IDEL;

//            event_310F.errNum[mtr_id-1] +=1;
//            if(event_310F.errNum[mtr_id-1] >5)
//            {
//                event_310F.relayState[mtr_id-1] = RELAY_IDEL;
//                event_310F.errNum[mtr_id-1] = 0;
//            }
            break;
        case RELAY_RECVD://接收到抄表消息
            event_310F.relayState[mtr_id-1] = RELAY_IDEL;
            break;
        case RELAY_FAIL: //重试
            event_310F.relayState[mtr_id-1] = RELAY_SEND;
            GetReadMeterStatus(oopMeter.basic.tsa, mtr_id);
            break;
        default:
            break;
        }
    }

}

/*****************************************************************************
所属功能单元：
功能描述：表时钟异常告警
输入参数：计量点号
输出参数：无
函数返回值：无
内部处理流程：获取当前计量点的时间和终端的当前时间，转换为统一分钟数判断是否超出允许的门阀值，
        越限则调用告警数据生成处理函数生成告警数据并填写主动上报队列
***************************************************************************-*/
void MetclockAbnormalEvent(int clientid, mtrnum_t mtr_id, OOP_METER_T oopMeter, uint8 bType,uint8* databuf,uint16 datalen)
{
    uint32  ERFlg = 0;
    uint8	is_now_flag = 0;
    uint16	clockerror_para = 0;//-时钟超差门限-
    uint32	met_clock_sec = 0;	//-测量点当前时间，秒-
    uint32	tmn_clock_sec = 0;	//-终端当前时间，秒-
    uint32	diff_value = 0;		//-差值-
    uint32	dwERFlgTmp = 0;
    uint32	len = 0;

    if(bType == 1)
        EVENT_FMT_TRACE("Check3105[%hu] 事件发生start\n",mtr_id);
    else
        EVENT_FMT_TRACE("Check3105[%hu] 事件恢复start\n",mtr_id);

    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据     

    OOP_DATETIME_S_T *pTemp = NULL;
    RECORD_UNIT_T  *pstepRecordOut = NULL;
    int            ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));   

    if(datalen != (uint16)sizeof(RECORD_UNIT_T))
    {
        EVENT_FMT_DEBUG("puamr_evt_check_3105[%hu] datalen[%hu] input data error\n", mtr_id, datalen);
        return;
    }
    pstepRecordOut = (RECORD_UNIT_T *)databuf;
    pTemp = (OOP_DATETIME_S_T *)pstepRecordOut->record[0].outDataBuf;

    if(((pstepRecordOut->record[0].outDataBuf[0] == 0xff)&&(pstepRecordOut->record[0].outDataBuf[1] == 0xff))|| 
            (pTemp->minute == 0 &&  //-min-
                pTemp->hour  == 0 &&	//-hour-
                pTemp->day  == 0 &&	//-day-
                pTemp->month  == 0 &&	//-month-
                pTemp->year  == 0)		//-year-
       )
    {
        //-时间无效返回-
        EVENT_FMT_DEBUG("event_3105[%hu]  电能表 时间无效 \n", mtr_id);
        return;
    }
       
    ooptm_to_systm(*pTemp,(time_t*)&met_clock_sec);
    event_3105.mettime = *pTemp;
    EVENT_FMT_TRACE("电表时间%u-%u-%u %u:%u:%u 秒数%u\n", pTemp->year, pTemp->month, pTemp->day, pTemp->hour, pTemp->minute, pTemp->second, met_clock_sec);

    //-读取终端时钟-
    tmn_clock_sec = pstepRecordOut->colEndTm; //-终端当前时刻，秒数-
    tmn_clock_sec -= COMPENSATION_TIME;
    if( tmn_clock_sec  == 0)
    {
        //-时间无效返回-
        EVENT_FMT_DEBUG("event_3105[%hu]  终端时间无效clock_sec=%u \n", mtr_id, tmn_clock_sec);
        return;
    }
    systm_to_ooptm(tmn_clock_sec, &event_3105.tmntime);
    if((event_3105.tmntime.minute == 0xff)||(event_3105.tmntime.day== 0xff)
            ||(event_3105.tmntime.minute== 0xee)||(event_3105.tmntime.day== 0xee))
    {
        EVENT_FMT_DEBUG("event_3105[%hu]  终端 时间无效clock_sec=%u \n", mtr_id, tmn_clock_sec);
        return;
    }

    EVENT_FMT_TRACE("终端时间%u-%u-%u %u:%u:%u 秒数%u\n", event_3105.tmntime.year, event_3105.tmntime.month, event_3105.tmntime.day, event_3105.tmntime.hour, event_3105.tmntime.minute, event_3105.tmntime.second, tmn_clock_sec);

    diff_value = COMMFUN_ABS(tmn_clock_sec,met_clock_sec);

    EVENT_FMT_LOGCC( "时间差值%u\n",diff_value);
    EVENT_FMT_LOGCC("判别阈值event_3105.checkPara.nValue=%hu\n",event_3105.checkPara.nValue);

    clockerror_para = event_3105.checkPara.nValue;

    // 默认
    if (clockerror_para == 0)
    {
        clockerror_para = 60*5;
    }
    
    // 容错小于60秒按60秒算
    if (clockerror_para < 60)
    {
        clockerror_para = 60;
    }
        

    //超差恢复值
    if(bType ==2)
        clockerror_para -=30;

    is_now_flag = (diff_value >clockerror_para);

    len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id-1)*(4)), 4);
    if(len<0 || dwERFlgTmp == 0XFFFFFFFF)
    {
        //-读取事件状态记录-
        EVENT_FMT_DEBUG("读取event_3105.ERFlg[%hu]=%08X事件记录标识错误或无效\n", mtr_id,dwERFlgTmp);
        dwERFlgTmp = 0;
        write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id-1)*(4)), 4);
        return;
    }


   
    EVENT_FMT_TRACE("event_3105.ERFlg[%hu]事件记录标识dwERFlgTmp=%08X\n", mtr_id,dwERFlgTmp);

    //事件发生源
    eventSource.choice = DT_TSA;
    eventSource.add= oopMeter.basic.tsa;

    //事件关联数据
    
    if((bType == 1)&&is_now_flag && (!(dwERFlgTmp & METTM_ERROR)))
    {
        //-超差发生-
        ERFlg = METTM_ERROR;

        //写数据库
        ret = WriteEventToDB(clientid, mtr_id-1, &event_3105.eventCommInfo, eventSource, eventData, 1);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("电能表时钟超差事件记录CheckERC_3105失败返回%d\n", ret);
//            return;
        }
        else
            EVENT_FMT_LOGCC("电能表时钟超差事件记录CheckERC_3105成功, storagenum=%u !\n", event_3105.eventCommInfo.storagenum);
   }
    else if((bType == 2)&&(!is_now_flag)&&(dwERFlgTmp & METTM_ERROR))
    {
        //-超差恢复发生-
        ERFlg = METTM_ERROR;
        //写数据库
        ret = WriteEventToDB(clientid, mtr_id-1, &event_3105.eventCommInfo, eventSource, eventData, 2);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("电能表时钟超差事件恢复记录CheckERC_3105失败返回%d\n", ret);
//            return;
        }
        else
            EVENT_FMT_LOGCC("电能表时钟超差事件恢复记录CheckERC_3105成功, storagenum=%u !\n", event_3105.eventCommInfo.storagenum);
    }

    if(ERFlg)
    {
        if(bType == 1)
        {
            dwERFlgTmp |= ERFlg;//-保存发生标记到此测量点全局事情标记中-
        }
        else
        {
            dwERFlgTmp &= ~ERFlg;//-保存发生标记到此测量点全局事情标记中-
        }
        write_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(mtr_id-1)*(4)), 4);
    }
}

/**********************************************************************
* @name      : get_met_en_now
* @brief     ：电能表电量检测事件 获取当前电表电量
* @param[in] ：uint16 mtr_id         信息点号              
               uint8* databuf       事件判断数据
               uint16 datalen       事件判断数据长度
               uint8  offset        电量存储地址偏移
* @param[out]：uint32* p_en_now      当前电表电量
               uint32* p_en_last    上一次电表电量

* @return    ：0成功/<0异常
* @Create    : 梅安宁
* @Date      ：2020-12-26
* @Update    :
**********************************************************************/
int get_met_en(mtrnum_t mtr_id, uint8* databuf,uint16 datalen, uint8  offset, uint32* p_en_now, uint32* p_en_last)
{
    uint32 	met_en_now = 0;
    uint32 	met_en_last = 0;
    static uint8 bfirst[MTR485_MAX_NUM] = {0};
    int16 	len = 0;
    
    //读取电表当前电量
    OOP_ENERGY_T    *Pfw_act_en = (OOP_ENERGY_T*)databuf;     //正向有功电能
    if(Pfw_act_en->nNum < 1 || (Pfw_act_en->rsv&0x00000001) != 1)
    {
        EVENT_FMT_DEBUG("event_310X[%hu] 获取 当前电量 err \n", mtr_id);
        return -1;
    }
    met_en_now = Pfw_act_en->nValue[0];
    if (met_en_now == APP_NONELWORD || met_en_now == NONE_LWORD)
    {
        EVENT_FMT_DEBUG("event_310X[%hu]  当前电量 无效 \n", mtr_id);
        return -1;									//无效数据返回
    }
    *p_en_now =	met_en_now;

    //首次,将当前值写入上次状态
    if (bfirst[mtr_id-1] == 0)
    {
        bfirst[mtr_id-1] = 1;
        EVENT_FMT_DEBUG("event_310X[%hu] 首次检测,将当前值写入上次状态\n", mtr_id);
        write_pdata_xram("^Event", (char *)&met_en_now, (MetEnLastRecAddr+(mtr_id-1)*4*4 + offset), 4);
        return -1;
    }
    
    //读取上次电量
    len = read_pdata_xram("^Event", (char *)&met_en_last, (MetEnLastRecAddr+(mtr_id-1)*4*4 + offset), 4);
    if (len<0)
    {
        EVENT_FMT_DEBUG("event_310X[%hu]读取上次电量失败\n", mtr_id);
        write_pdata_xram("^Event", (char *)&met_en_now, (MetEnLastRecAddr+(mtr_id-1)*4*4 + offset), 4);
        return -1;
    }

    if (met_en_last != met_en_now)
    {
        write_pdata_xram("^Event", (char *)&met_en_now, (MetEnLastRecAddr+(mtr_id-1)*4*4 + offset*4), 4);
    }
    if (met_en_last == APP_NONELWORD || met_en_last == NONE_LWORD)//-重新设置过电表后，底数被清为ee-
    {
        EVENT_FMT_DEBUG("CheckERC_310B 电表[%hu]上次电量数据无效\n", mtr_id);
        return -1;
    }
    *p_en_last =	met_en_last;
    
    EVENT_FMT_TRACE("event_310X[%hu]上次电量=%u,当前电量=%u\n", mtr_id, met_en_last, met_en_now);
    
    return 0;
}
/*******************************************************************************
* 所属单元：
* 功能描述：电表示度下降告警
* 输入参数：计量点号
* 输出参数：无
* 返 回 值：无
* 内部流程：根据当前的电表的读数和上一次的电表的读数判断是否发生示度下降，
* 		  	下降则调用告警数据处理函数生成告警数据并填写主动上报队列
*******************************************************************************/
void EnDownEvent(int clientid, mtrnum_t mtr_id, OOP_METER_T oopMeter, uint32 en_now, uint32 en_last)
{
//    static uint8 bTimes[NO_VIR_MTR_MAX_NUM] = {0};
//    uint32 	cb_interval = 0;
    uint32	dwflag = 0;
    uint8 	is_now_flag = 0;
    BOOL    flag_save=FALSE;
      
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;

    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    //这个程序中未考虑换表的问题，换表的话需要将初始电表读数清零
    //读取电表电量
    event_310B.nowEnergy = en_now;
    event_310B.lastEnergy = en_last;

    is_now_flag = (event_310B.nowEnergy < event_310B.lastEnergy);
    if (read_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4) < 0 || (dwflag == 0XFFFFFFFF))  //事件状态
    {
        dwflag = 0;
        write_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4);
        return;
    }
    
    EVENT_FMT_TRACE("CheckERC_310B 电表[%hu]事件状态=%08X\n", mtr_id, dwflag&MET_EN_DOWN);

    if ((!is_now_flag)&&(dwflag & MET_EN_DOWN))	//未超差
    {
        if(event_310B.nowEnergy == event_310B.lastEnergy)		//相等不判恢复
        {
            return;
        }
        dwflag &= ~MET_EN_DOWN;//恢复
        flag_save=TRUE;
    }
    
    if (is_now_flag &&(!((dwflag & MET_EN_DOWN))))
    {
        flag_save=TRUE;
        dwflag |= MET_EN_DOWN;					//更新事件状态
    }
    
    if (flag_save)
    {
        write_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4);
        
#if STD_698PRO_BETA
        //事件发生源
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter.basic.tsa;

        //事件关联数据
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X00102201;
        eventData.cols.oad[1].value = 0X00108201;
        eventData.idLen[0] = sizeof(uint32);
        eventData.idLen[1] = sizeof(uint32);
        eventData.len = eventData.idLen[0] + eventData.idLen[1];
        memcpy(eventData.buf, &event_310B.lastEnergy, eventData.idLen[0]);
        memcpy(eventData.buf+eventData.idLen[0], &event_310B.nowEnergy, eventData.idLen[1]);
        
        if(dwflag & MET_EN_DOWN)//发生
        {
            //写数据库
            ret = WriteEventToDB(clientid, mtr_id-1, &event_310B.eventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("CheckERC_310B测量点mtr_id=%hu  电能表示度下降事件记录失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_LOGCC("CheckERC_310B测量点mtr_id=%hu  电能表示度下降事件记录成功, storagenum=%u !\n", mtr_id, event_310B.eventCommInfo.storagenum);
        }
        else    
        {
            //恢复事件记录内容
            ret = WriteEventToDB(clientid, mtr_id-1, &event_310B.eventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("CheckERC_310B测量点mtr_id=%d  电能表示度下降事件恢复记录失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_LOGCC("CheckERC_310B测量点mtr_id=%hu  电能表示度下降事件恢复记录成功 , storagenum=%u !\n", mtr_id, event_310B.eventCommInfo.storagenum);
        }
#endif
    }

    EVENT_FMT_TRACE("CheckERC_310B[%hu] complete\n", mtr_id);
}

/*****************************************************************************
所属功能单元：
功能描述：电表电能量超差告警
输入参数：计量点号
输出参数：无
函数返回值：无
内部处理流程：在规定的时间段内根据当前计量点的相对电量和可能走的相对电量之差超过电表
	电能量超差判断的门限值，越限则调用告警数据生成处理函数生成告警数据并填写主动上报队列
***************************************************************************-*/
void MetJumpEvent(int clientid, mtrnum_t mtr_id, OOP_METER_T oopMeter, uint32 en_now, uint32 en_last)
{
    uint32	en_comp = 0;						//比较电量
    uint32	value_comp = 0;						//比较值
    float	a1 = 0.0;
    int     met_en_comp     = 0;
    uint32	fly_para = 0,jump_para = 0;			//电表飞走/超差阀值
    uint8	is_now_flag = 0;
    uint32	cb_interval = 0;					//抄表间隔
    uint32	dwflag = 0;
    BOOL    flag_save=FALSE;

    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    //获取电能量超差阀值
    jump_para = event_310C.eventParam.nValue/100;
    
    //获取电表飞走阀值
    fly_para = event_310D.eventParam.nValue/100;

    EVENT_FMT_TRACE("CheckERC_310C电能量超差阀值=%lu\n", jump_para);
    EVENT_FMT_TRACE("CheckERC_310C电表飞走阀值=%lu\n", fly_para);

    // 容错

    if (jump_para ==0 || fly_para == 0)
    {
        return;
    }
    
    if (jump_para > fly_para)
    {
        fly_para = 0x9999;
    }

    //读取电表电量
    event_310C.nowEnergy = en_now;
    event_310C.lastEnergy = en_last;

    //获得差值电量
    met_en_comp = (event_310C.nowEnergy - event_310C.lastEnergy);

    if(met_en_comp == 0)
    {
        EVENT_FMT_DEBUG("CheckERC_310C[%hu] 电量没有发生变化\n", mtr_id);
        return;
    }
    else if(met_en_comp < 0)		//电量不正常则返回
    {
        EVENT_FMT_DEBUG("CheckERC_310C电表[%hu]电量不正常则返回\n", mtr_id);
        return;
    }

    if(GetTaskOopTI(event_310C.eventParam.nTID, &event_310C.oopTI) < 0)
    {
        cb_interval = CB_INTERVAL;			//抄表间隔
    }
    else
    {
        cb_interval = OopTIToSec(event_310C.oopTI)/60;
        if((cb_interval == 0))
            cb_interval = CB_INTERVAL;			//抄表间隔
    }

    EVENT_FMT_TRACE("CheckERC_310C终端抄表时间间隔cb_interval=%d\n",cb_interval);

    //获取比较电量
    switch (oopMeter.basic.pwrType)
    {
    case 2:										//三相三线
        en_comp = (uint32)(oopMeter.basic.stVolt * oopMeter.basic.stAmp * cb_interval/300);
        break;

    case 1:										//单相
        en_comp = (uint32)(oopMeter.basic.stVolt * oopMeter.basic.stAmp * cb_interval/600);
        break;

    default:									//三相四线
        en_comp = (uint32)(oopMeter.basic.stVolt * oopMeter.basic.stAmp * cb_interval/200);
        break;
    }
    //两者差的绝对值
    value_comp = COMMFUN_ABS(met_en_comp,en_comp);
    a1 = ((float)met_en_comp*100)/(((float)en_comp));
    is_now_flag = ((a1 > jump_para)&&(a1 < (fly_para)));

    
    EVENT_FMT_TRACE("CheckERC_310C电表[%hu] 额定电压%d ,额定电流%u ,抄表间隔时间内的表电量%d ,抄表间隔时间内比较电量%u ,两者差的绝对值%u ,计算的比较值%f ,电表超差阀值%u, dwflag%u\n",
            mtr_id, oopMeter.basic.stVolt/10, oopMeter.basic.stAmp/10, met_en_comp, en_comp, value_comp, a1, jump_para, dwflag & MET_EN_OUT);

    //获取事件标志
    if (read_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4) < 0 || (dwflag == 0XFFFFFFFF))  //事件状态
    {
        dwflag = 0;
        write_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4);
        return;
    }
    
    //未超差
    if ((a1 <= jump_para) && (dwflag & MET_EN_OUT))
    {
        dwflag &= ~MET_EN_OUT;
        flag_save=TRUE;
    }
    if ((is_now_flag) && (!(dwflag & MET_EN_OUT)))
    {
        //更新事件状态
        dwflag |= MET_EN_OUT;
        flag_save=TRUE;
    }
  
    //下个抄表间隔再次检测，如果超差才是真正的超差，防止出现误报
    if(flag_save)
    {
        write_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4);
        
#if STD_698PRO_BETA
        //事件发生源
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter.basic.tsa;

        //事件关联数据
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X00102201;
        eventData.cols.oad[1].value = 0X00108201;
        eventData.idLen[0] = sizeof(uint32);
        eventData.idLen[1] = sizeof(uint32);
        eventData.len = eventData.idLen[0] + eventData.idLen[1];
        memcpy(eventData.buf, &event_310C.lastEnergy, eventData.idLen[0]);
        memcpy(eventData.buf+eventData.idLen[0], &event_310C.nowEnergy, eventData.idLen[1]);
        
        if(dwflag & MET_EN_OUT)//发生
        {
            //写数据库
            ret = WriteEventToDB(clientid, mtr_id-1, &event_310C.eventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG( "测量点mtr_id=%hu  电能表超差事件记录CheckERC_310C失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_LOGCC( "测量点mtr_id=%hu  电能表超差事件记录CheckERC_310C成功 storagenum=%u \n", mtr_id, event_310C.eventCommInfo.storagenum);

        }
        else
        {
            //恢复事件记录内容
            ret = WriteEventToDB(clientid, mtr_id-1, &event_310C.eventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG( "测量点mtr_id=%hu  电能表超差事件恢复记录CheckERC_310C失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_LOGCC( "测量点mtr_id=%hu  电能表超差事件恢复记录CheckERC_310C成功 storagenum=%u \n", mtr_id, event_310C.eventCommInfo.storagenum);
        }

#endif

    }

    return;
}

/*******************************************************************************
* 所属功能单元：
* 功能描述：电表飞走告警
* 输入参数：计量点号
* 输出参数：无
* 返 回 值：无
* 内部流程：在规定的时间段内根据当前计量点的相对电量和可能走的相对电量之差超过电表
* 			电能量超差判断的门限值，越限则调用告警数据生成处理函数生成告警数据并填写主动上报队列
*******************************************************************************/
void MetFlyEvent(int clientid, mtrnum_t mtr_id, OOP_METER_T oopMeter, uint32 en_now, uint32 en_last)
{
    uint32	value_comp = 0;    //比较值
    float	a1 = 0.0;
    uint32	en_comp = 0;						    //比较电量
    int     met_en_comp     = 0;
    uint32	fly_para        = 0;						    //电表飞走阀值
    uint8	is_now_flag = 0,cb_interval = 0;
    uint32	dwflag = 0;
    BOOL 	flag_save=FALSE;

    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));
    
    //获取电表飞走阀值
    fly_para = event_310D.eventParam.nValue;

    if ((fly_para == 0xffffffffL) || (fly_para == 0))	//飞走阀值无效，不判断
    {
        EVENT_FMT_DEBUG("CheckERC_310D[%hu] 飞走阀值无效，不判断\n", mtr_id);
        return;
    }

    fly_para /= 100;

    //读取电表电量
    event_310D.nowEnergy = en_now;
    event_310D.lastEnergy = en_last;
    
    //获得差值电量
    met_en_comp = event_310D.nowEnergy - event_310D.lastEnergy;
    if(met_en_comp == 0)
    {
        EVENT_FMT_DEBUG("CheckERC_310D[%hu] 电量没有发生变化\n", mtr_id);
        return;
    }
    else if(met_en_comp < 0)		//电量不正常则返回
    {
        EVENT_FMT_DEBUG("CheckERC_310D[%hu] 电量不正常则返回\n", mtr_id);
        return;
    }
    
    if(GetTaskOopTI(event_310D.eventParam.nTID, &event_310D.oopTI) < 0)
    {
        cb_interval = CB_INTERVAL;			//抄表间隔
    }
    else
    {
        cb_interval = OopTIToSec(event_310D.oopTI)/60;
        if((cb_interval == 0))
            cb_interval = CB_INTERVAL;			//抄表间隔
    }

    EVENT_FMT_TRACE("CheckERC_310D[%hu] 终端抄表时间间隔cb_interval=%d\n", mtr_id, cb_interval);

    //获得比较电量
    switch (oopMeter.basic.pwrType)
    {
    case 2:										//三相三线
        en_comp = (uint64)(oopMeter.basic.stVolt * oopMeter.basic.stAmp * cb_interval/300);
        break;

    case 1:										//单相
        en_comp = (uint64)(oopMeter.basic.stVolt * oopMeter.basic.stAmp * cb_interval/600);
        break;

    default:									//三相四线
        en_comp = (uint64)(oopMeter.basic.stVolt * oopMeter.basic.stAmp * cb_interval/200);
        break;
    }
    if(en_comp==0)//容错处理
    {
      EVENT_FMT_DEBUG("CheckERC_310D[%hu] 抄表间隔时间内比较电量=0\n", mtr_id);
      return;
    }
    value_comp = COMMFUN_ABS(met_en_comp, en_comp);					//两者差的绝对值
    a1 =  ((float)met_en_comp*100)/(((float)en_comp));
    is_now_flag = (a1 > (fly_para));		//事件标志
    
    if (read_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4) < 0 || (dwflag == 0XFFFFFFFF))  //事件状态
    {
        dwflag = 0;
        write_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4);
        return;
    }
    
    EVENT_FMT_TRACE("CheckERC_310D[%hu] 额定电压%d ,额定电流%u ,抄表间隔时间内的表电量%d ,抄表间隔时间内比较电量%u ,两者差的绝对值%u ,计算的比较值%f ,电表飞走阀值%u, dwflag%u\n",
        mtr_id, oopMeter.basic.stVolt/10, oopMeter.basic.stAmp/10, met_en_comp, en_comp, value_comp, a1, fly_para, dwflag & MET_EN_FLY);

    if ((!is_now_flag) && (dwflag & MET_EN_FLY))		//曾经飞走,现在没有飞走
    {
        //更新事件状态
        dwflag &= ~MET_EN_FLY;
        flag_save=TRUE;
    }
    if ((is_now_flag) && (!(dwflag & MET_EN_FLY)))
    {
        //更新事件状态
        dwflag |= MET_EN_FLY;
        flag_save=TRUE;
    }

    if (flag_save)		//飞走且未记录过
    {
        write_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4);           

#if STD_698PRO_BETA
        //事件发生源
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter.basic.tsa;

        //事件关联数据
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X00102201;
        eventData.cols.oad[1].value = 0X00108201;
        eventData.idLen[0] = sizeof(uint32);
        eventData.idLen[1] = sizeof(uint32);
        eventData.len = eventData.idLen[0] + eventData.idLen[1];
        memcpy(eventData.buf, &event_310D.lastEnergy, eventData.idLen[0]);
        memcpy(eventData.buf+eventData.idLen[0], &event_310D.nowEnergy, eventData.idLen[1]);
        
        if(dwflag & MET_EN_FLY)//发生
        {
            //写数据库
            ret = WriteEventToDB(clientid, mtr_id-1, &event_310D.eventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG( "测量点mtr_id=%hu  电能表飞走事件记录CheckERC_310D失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_LOGCC( "测量点mtr_id=%hu  电能表飞走事件记录CheckERC_310D成功 storagenum=%u \n", mtr_id, event_310D.eventCommInfo.storagenum);

        }
        else
        {
            //恢复事件记录内容
            ret = WriteEventToDB(clientid, mtr_id-1, &event_310D.eventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG( "测量点mtr_id=%hu  电能表飞走事件恢复记录CheckERC_310D失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_LOGCC( "测量点mtr_id=%hu  电能表飞走事件恢复记录CheckERC_310D成功 storagenum=%u \n", mtr_id, event_310D.eventCommInfo.storagenum);
        }
#endif
    }

    return;
}

/**********************************************************************
* @name      : GetCSDFromPlanByTaskID
* @brief     : 通过任务ID获取对应采集方案的采集CSD
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int GetCSDFromPlanByTaskID(uint8 taskID, OOP_RECORD_COL_T *pRecordCol)
{
    int                 i   = 0;
    OOP_PLAN_TYPE_E		planType;		//方案类型		
    OOP_TASK_T          oopTaskTemp;
    OOP_PLAN_NORMAL_T   oopPlanNormal;
    OOP_PLAN_EVENT_T    oopPlanEvent;

    if(pRecordCol == NULL)
    {
        return -1;
    }

    memset(&oopTaskTemp,   0, sizeof(OOP_TASK_T));
    memset(&oopPlanNormal, 0, sizeof(OOP_PLAN_NORMAL_T));
    memset(&oopPlanEvent,  0, sizeof(OOP_PLAN_EVENT_T));
    
    //根据任务ID taskID 找到任务配置表 
    if(app_get_task_list(taskID, &oopTaskTemp) == 0)
    {    

        planType = oopTaskTemp.planType;
        if(planType == PLAN_NORMAL)
        {
            //根据方案编号planID 找到采集方案
            if(app_get_plan_normal_list(oopTaskTemp.planID, &oopPlanNormal) == 0)
            {
                *pRecordCol = oopPlanNormal.recdCol;
                EVENT_FMT_TRACE("CheckERC_311C 获取采集方案csd num%u oad%08x \n", pRecordCol->num, pRecordCol->csd[0].oad.value);
                return 0;
            }
        }
        else if(planType == PLAN_EVENT)
        {
            //根据方案编号planID 找到采集方案
            if(app_get_plan_event_list(oopTaskTemp.planID, &oopPlanEvent) == 0)
            {   
                pRecordCol->num = oopPlanEvent.events.nNum;
                for(i=0; i<pRecordCol->num; i++)
                {
                    pRecordCol->csd[i].choice = 1;
                    pRecordCol->csd[i].road = oopPlanEvent.events.cols[i];
                }
                return 0;
            }
       }
    }

    return -1;
}

uint8 IsExistVCInPlantNamol(uint8 taskID)
{
    int                 i = 0;
    int                 j = 0;
    uint8               ExistFlag = 0;  //bit0 表示电压  bit1表示电流
    OOP_OAD_U           MainOad;
    OOP_TASK_T          oopTaskTemp;
    OOP_PLAN_NORMAL_T   oopPlanNormal;
    OOP_RECORD_COL_T    recordCol;

    memset(&oopTaskTemp,   0, sizeof(OOP_TASK_T));
    memset(&oopPlanNormal, 0, sizeof(OOP_PLAN_NORMAL_T));
    memset(&recordCol,     0, sizeof(OOP_RECORD_COL_T));
  
    if(GetCSDFromPlanByTaskID(taskID, &recordCol) == 0)
    {   
        for(i=0; i<recordCol.num; i++)
        {
            //设计依据:每个CSD应配置采集为一种数据类型,不应该配置为两种及以上的数据类型
            if(recordCol.csd[i].choice == 0)//类型为OAD即表示实时数据
            {
                //主OAD为0
                MainOad = recordCol.csd[i].oad;
                //如果为OAD类型,判断是否
                if(MainOad.nObjID == 0x2000)
                    ExistFlag |= 0x01;
                if(MainOad.nObjID == 0x2001)
                    ExistFlag |= 0x02;
            }
            else if(recordCol.csd[i].choice == 1)
            {
                for(j = 0; j < recordCol.csd[i].road.oadCols.nNum; j++)
                {
                    MainOad = recordCol.csd[i].road.oadCols.oad[j];
                    //如果为OAD类型,判断是否
                    if(MainOad.nObjID == 0x2000)
                        ExistFlag |= 0x01;
                    if(MainOad.nObjID == 0x2001)
                        ExistFlag |= 0x02;
                }
            }
            else
                break;
        }

    }

    return ExistFlag;
}

/*****************************************************************************
* 所属单元：
* 功能描述：电表停走告警
* 输入参数：计量点号
* 输出参数：无
* 返 回 值：无
* 内部流程：在规定的时间段内根据当前计量点的相对电量和可能走的相对电量之差超过电表
* 			电能量超差判断的门限值，越限则调用告警数据生成处理函数生成告警数据并填写主动上报队列
***************************************************************************-*/
void MetStopEvent(int clientid, mtrnum_t mtr_id, OOP_METER_T oopMeter, uint32 en_now, uint32 en_last)
{
    uint32	stop_para       = 0;
    uint8	is_now_flag     = 0;
    uint8   ExistFlag       = 0;
    uint32	dwflag          = 0;
    static	uint16  bTimes[NO_VIR_MTR_MAX_NUM] = {0};
    BOOL	flag_save       = FALSE;

    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;

    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));
    
    //读取电表电量
    event_310E.nowEnergy = en_now;
    event_310E.lastEnergy = en_last;

    //获取电表停走阀值
    stop_para = OopTIToSec(event_310E.event310eParam.threshold)/60;
    if(stop_para == 0)//start Common Problem
    {
        EVENT_FMT_DEBUG("CheckERC_310E[%hu] 获取电表停走阀值无效返回\n", mtr_id);
        return;
    }

    //获取电表停走对应的任务号
    if(event_310E.event310eParam.nTID == 0)
    {
        EVENT_FMT_DEBUG("CheckERC_310E[%hu] 获取电表停走对应的任务号无效返回\n", mtr_id);
        return;
    }
    
    EVENT_FMT_TRACE("CheckERC_310E[%hu] 电表停走阀值为%u分钟\n", mtr_id, stop_para);

 

    ExistFlag = IsExistVCInPlantNamol(event_310E.event310eParam.nTID);
    //判断ABC相中某相电压、电流全部存在
    //读取电表当前电量
    OOP_WORD3V_T valTemp;       //电压
    memset(&valTemp, 0, sizeof(OOP_WORD3V_T));
    if(get_meter_data_by_oad(clientid, mtr_id, 0X20000200, (uint8 *)&valTemp, sizeof(OOP_WORD3V_T))<0)
    {
        EVENT_FMT_DEBUG("event_310E[%hu]  get 0X20000200 err \n", mtr_id);
    }
    
    //读取电表当前电量
    OOP_INT3V_T   curTemp;       //电流，
    memset(&curTemp, 0, sizeof(OOP_INT3V_T));
    if(get_meter_data_by_oad(clientid, mtr_id, 0X20010200, (uint8 *)&curTemp, sizeof(OOP_INT3V_T))<0)
    {
        EVENT_FMT_DEBUG("event_310E[%hu]  get 0X20010200 err \n", mtr_id);
    }
    
    is_now_flag = ((((valTemp.nValue[0] != 0) && (curTemp.nValue[0] != 0))
                    || ((valTemp.nValue[1] != 0) && (curTemp.nValue[1] != 0))
                    || ((valTemp.nValue[2] != 0) && (curTemp.nValue[2] != 0)))
                   && (((valTemp.nValue[0] != 0xffffffff) && (curTemp.nValue[0] != 0xffffffff))
                       || ((valTemp.nValue[1] != 0xffffffff) && (curTemp.nValue[1] != 0xffffffff))
                       || ((valTemp.nValue[2] != 0xffffffff) && (curTemp.nValue[2] != 0xffffffff)))
                   && (((valTemp.nValue[2] != APP_NONELWORD) && (curTemp.nValue[0] != APP_NONELWORD))
                       || ((valTemp.nValue[2] != APP_NONELWORD) && (curTemp.nValue[1] != APP_NONELWORD))
                       || ((valTemp.nValue[2] != APP_NONELWORD) && (curTemp.nValue[2] != APP_NONELWORD))));

    if(ExistFlag != 0x03)  //方案不配置电压电流,那么不对电压电流值判断默认不为0防止台体不过
        is_now_flag = 1;

    
    //读停走记录的标志
    if (read_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4) < 0 || (dwflag == 0XFFFFFFFF))
    {
        dwflag = 0;
        write_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4);
        return;
    }
    EVENT_FMT_TRACE("CheckERC_310E[%hu] is_now_flag = %d  (dwflag & MET_EN_STOP) = %08x\n", mtr_id, is_now_flag, (dwflag & MET_EN_STOP));

    if (event_310E.nowEnergy != event_310E.lastEnergy)				//电量不等
    {
        bTimes[mtr_id-1] = 0;

        //更新事件状态
        if(dwflag&MET_EN_STOP)
        {
            flag_save=TRUE;
        }
        dwflag &= ~MET_EN_STOP;
    }
    else									//两次值相等，则判断是否停走
    {
        if (bTimes[mtr_id-1] < stop_para)//终端抄表时间未到*15
        {
            bTimes[mtr_id-1]++;

        }

        if (bTimes[mtr_id-1] >= stop_para) //*15
        {
            bTimes[mtr_id-1] = 0;
        }
        else
        {
            return;
        }

        if ((is_now_flag) && (!(dwflag & MET_EN_STOP)))
        {
            flag_save=TRUE;
            //更新事件状态
            dwflag |= MET_EN_STOP;
        }
    }

    if(flag_save)
    {
        write_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4);  
        
#if STD_698PRO_BETA
        //事件发生源
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter.basic.tsa;

        //事件关联数据
        eventData.oadNum = 1;
        eventData.cols.nNum = 1;
        eventData.cols.oad[0].value = 0X00102201;
        eventData.idLen[0] = sizeof(uint32);
        eventData.len = eventData.idLen[0];
        memcpy(eventData.buf, &event_310E.nowEnergy, eventData.idLen[0]);

        if(dwflag&MET_EN_STOP)//发生
        {
            //写数据库
            ret = WriteEventToDB(clientid, mtr_id-1, &event_310E.eventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG( "测量点mtr_id=%hu  电能表停走事件记录CheckERC_310E失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_LOGCC( "测量点mtr_id=%hu  电能表停走事件记录CheckERC_310E成功 storagenum=%u \n", mtr_id, event_310E.eventCommInfo.storagenum);

        }
        else
        {
            //恢复事件记录内容
            ret = WriteEventToDB(clientid, mtr_id-1, &event_310E.eventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG( "测量点mtr_id=%hu  电能表停走事件恢复记录CheckERC_310E失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_LOGCC( "测量点mtr_id=%hu  电能表停走事件恢复记录CheckERC_310E成功 storagenum=%u \n", mtr_id, event_310E.eventCommInfo.storagenum);
        }
#endif

    }
}

/**********************************************************************
* @name      : GetReadMeterStatus
* @brief     ：获取抄表状态
* @param[in] ：OOP_TSA_T		tsa			表计地址
               mtrnum_t     mtr_id      测量点号
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-02-26
* @Update    :
**********************************************************************/
int GetReadMeterStatus(OOP_TSA_T		  tsa, mtrnum_t mtr_id)
{
    uint16  msgLen      = 0;
    uint8   msgBuf[128] = {0};
    uint16  tsaLen      = 0;
    int     ret         = 0;

//    event_310F.recodFlag[mtr_id-1] = 0;
    //打分的下了太多485档案导致消息来不及处理 透传的消息丢了 信息点号超过10的就不透传 不影响功能台体
    if(mtr_id>10)
        return 0;
    //获取当日抄读状态
    //消息接口 1003 0013 
    tsaLen = tsa.len+1;
    msgBuf[msgLen] = tsaLen;
    msgLen++;
    memcpy(msgBuf + msgLen, tsa.add, tsaLen);
    msgLen += tsaLen;
    
    ret = appmain_send_msg(MSG_APPAMIN_CTS_SETREAD_LIST, mtr_id, msgBuf, msgLen);
    EVENT_FMT_LOGCC("获取抄表状态中……!Send to 0x%x: msg 0x%x, label %d ret=%d\n", MSG_MONITOR, MSG_APPAMIN_CTS_SETREAD_LIST, mtr_id, ret);
    EVENT_BUF_TRACE(msgBuf, msgLen, "Send msg:");
    
    event_310F.relayState[mtr_id-1] = RELAY_WATT;
    return 0;
}

/**********************************************************************
* @name      : RecvReadMeterStatus
* @brief     ：接收处理抄表状态
* @param[in] ：uint16 msglen;    //消息长度
               uint8  *pmsg;    //消息内容
               uint16 label;     //消息标记
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-02-28
* @Update    :
**********************************************************************/
int RecvReadMeterStatus(uint8 *pmsg, uint16 msglen, uint16 label)
{
     //消息接口 1003 0013 长度判断
    EVENT_BUF_TRACE(pmsg, msglen, "RecvReadMeterStatus:");

    event_310F.recodFlag[label-1] = 0;
    
    if(msglen != (1 + 2 + 7*3))
    {
        event_310F.relayState[label-1] = RELAY_FAIL;
        EVENT_FMT_DEBUG("CheckERC_310F 抄表状态消息长度错误 msglen[%u]!=%u\n", msglen, (1 + 2 + 7*3));
        return -1;
    }

    if(pmsg[0] == 0 && (pmsg[1] !=0 || pmsg[2] != 0))  //抄表状态为0，且抄表失败次数不为0，
    {
        event_310F.recodFlag[label-1] = 0xaa;
        event_310F.relayState[label-1] = RELAY_IDEL;
        return 0;
    }
    else if(pmsg[0] == 1)
    {
        event_310F.recodFlag[label-1] = 0x55;
        event_310F.relayState[label-1] = RELAY_IDEL;
        return 0;
    }

    EVENT_FMT_DEBUG("CheckERC_310F 最近一次抄表成功标志[%u]不是bool值\n", pmsg[0]);
    event_310F.relayState[label-1] = RELAY_FAIL;
    return -1;
}

/*******************************************************************************
* 所属功能单元：
* 功能描述：电表抄读失败及恢复正常抄表告警
* 输入参数：计量点号
* 输出参数：无
* 返 回 值：无
* 内部流程：
*******************************************************************************/
void MetReadFailureEvent(mtrnum_t mtr_id, OOP_METER_T oopMeter)
{
    uint8  bfgEvent = 0;	            //-通信事件类型，最高位为0表抄表成功标志，为1表示不成功-
    uint8 bfgReport = 0;				//-事件上报发生标志-
    uint32	dwflag = 0;
    uint8 retryPara = 0;   //重试轮次的参数
    uint32 cb_interval = 0;
    uint32 cb_delay = 0;
    OOP_TI_T acqDelay;
    static uint8  retryCnt[VIR_BEGIN+1] = {0};
    static uint32 lastMin[VIR_BEGIN+1] = {0};
    uint32 aMin = time(NULL)/60;

    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据     
    RECORD_UNIT_T stepRecordOut;
    int  ret = 0;

    memset(&stepRecordOut,0,sizeof(RECORD_UNIT_T));
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    if(event_310F.recodFlag[mtr_id-1] != 0x55 && event_310F.recodFlag[mtr_id-1] != 0xaa)
    {
        EVENT_FMT_DEBUG("CheckERC_310F[%hu] recodFlag=%02X error!\n", mtr_id, event_310F.recodFlag[mtr_id-1]);
        GetReadMeterStatus(oopMeter.basic.tsa, mtr_id);
        return;
    }

    if (read_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4) < 0 || (dwflag == 0XFFFFFFFF))
    {
        dwflag = 0;
        write_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4);
        return;
    }

    retryPara = event_310F.eventParam.nTicks;

    

    //间隔时间
    if(GetTaskOopTI(event_310F.eventParam.nTID, &event_310F.oopTI) < 0)
        return;
    
    cb_interval = OopTIToSec(event_310F.oopTI)/60;
    if(cb_interval == 0)
        return;

    if(GetTaskOopDelay(event_310F.eventParam.nTID, &acqDelay) < 0)
        return;
    
    cb_delay = OopTIToSec(acqDelay)/60;
    EVENT_FMT_TRACE("CheckERC_310F[%hu] 抄表间隔时间: %d分钟,延时 %d分钟\n",mtr_id,cb_interval, cb_delay);

    bfgReport = 0;
    EVENT_FMT_TRACE("CheckERC_310F[%hu] recodFlag:%2x, dwflag%0X\n",mtr_id,event_310F.recodFlag[mtr_id-1], dwflag & MET_EN_RM_FAILURE);

    if (event_310F.recodFlag[mtr_id-1] == 0x55)    //-抄表成功标志55为成功，aa表示不成功
    {  
        retryCnt[mtr_id] = 0;  //抄读通置失败累计次数为0
        //本次抄通，前面未抄通，上报抄表恢复
        if (dwflag & MET_EN_RM_FAILURE)
        {
            bfgReport = 1;                  //-上报标志置位-
            bfgEvent = 0x00;
            dwflag &= ~MET_EN_RM_FAILURE;   //更新事件状态
        }
        //本次抄通，前面抄通，不上报
        else
        {
            return;
        }
    }
    else if(event_310F.recodFlag[mtr_id-1] == 0xaa)
    {
        //本次未抄通，前面未抄通，不上报
        if (dwflag & MET_EN_RM_FAILURE)
        {
            return;
        }
        else    //本次未抄通，前面抄通，上报抄表失败
        {

            if(((aMin-cb_delay)%cb_interval) != 0 || lastMin[mtr_id] == aMin)
                return;

            lastMin[mtr_id] = aMin;

            retryCnt[mtr_id]++;
            if(retryCnt[mtr_id] < retryPara)//判断失败轮次
            {
                EVENT_FMT_TRACE("CheckERC_310F[%hu]  电表抄读失败对应的重试次数: %d, 当前失败次数:%d\n",mtr_id,retryPara, retryCnt[mtr_id]);
                return;
            }

            bfgReport = 1;					//-上报标志置位-
            bfgEvent = 0x80;
            dwflag |= MET_EN_RM_FAILURE;   //更新事件状态
        }
    }
    else
    {
        return;
    }

    if(bfgReport)
    {
        write_pdata_xram("^Event", (char *)&dwflag, (EventStatusRecAddr+(mtr_id-1)*4), 4);
        
        //最近一次抄表成功时间-  
        if(get_meter_data_by_oad(gDbCenterClient, mtr_id, 0X40000200, (uint8 *)&event_310F.recodTime, sizeof(OOP_DATETIME_S_T)) !=0)
        {   
            EVENT_FMT_LOGCC("event_310F[%hu]  get 0X40000200 err \n", mtr_id);
        }

        //最近一次抄表有功示值-
        if(get_meter_data_by_oad(gDbCenterClient, mtr_id, 0X00100200, (uint8 *)&event_310F.oopEnergy, sizeof(OOP_ENERGY_T))<0)
        {
            EVENT_FMT_LOGCC("event_310F[%hu]  get 0X00100200 err \n", mtr_id);
        }

        if(get_meter_data_by_oad(gDbCenterClient, mtr_id, 0X00300200, (uint8 *)&event_310F.oopEnergyS, sizeof(OOP_ENERGYS_T))<0)
        {
            EVENT_FMT_LOGCC("event_310F[%hu]  get 0X00300200 err \n", mtr_id);
        }
            
#if STD_698PRO_BETA
        //事件发生源
        eventSource.choice = DT_TSA;
        eventSource.add= oopMeter.basic.tsa;

        //事件关联数据
        eventData.oadNum = 3;
        eventData.cols.nNum = 3;
        eventData.cols.oad[0].value = 0X60412200;
        eventData.idLen[0] = sizeof(OOP_DATETIME_S_T);
        eventData.cols.oad[1].value = 0X00102201;
        eventData.idLen[1] = sizeof(uint32);
        eventData.cols.oad[2].value = 0X00302201;
        eventData.idLen[2] = sizeof(int32);
        
        memcpy(eventData.buf, &event_310F.recodTime, eventData.idLen[0]);
        eventData.len += eventData.idLen[0];
        memcpy(eventData.buf + eventData.len, &event_310F.oopEnergy.nValue[0], eventData.idLen[1]);
        eventData.len += eventData.idLen[1];
        memcpy(eventData.buf + eventData.len, &event_310F.oopEnergyS.nValue[0], eventData.idLen[2]);
        eventData.len += eventData.idLen[2];

        if(bfgEvent == 0x80)//事件发生
        {
            //写数据库
            ret = WriteEventToDB(gDbCenterClient, mtr_id-1, &event_310F.eventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG( "终端抄表mtr_id=%hu失败事件记录CheckERC_310F失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_LOGCC( "终端抄表mtr_id=%hu失败事件记录CheckERC_310F成功 storagenum=%u \n", mtr_id, event_310F.eventCommInfo.storagenum);

        }
        if(bfgEvent == 0x00)//事件恢复
        {
            //恢复事件记录内容
            ret = WriteEventToDB(gDbCenterClient, mtr_id-1, &event_310F.eventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG( "终端抄表mtr_id=%hu失败事件恢复记录CheckERC_310F失败返回%d\n", mtr_id, ret);
                return;
            }
            EVENT_FMT_LOGCC( "终端抄表mtr_id=%hu失败事件恢复记录CheckERC_310F成功 storagenum=%u \n", mtr_id, event_310F.eventCommInfo.storagenum);
        }
#endif

    }

}

