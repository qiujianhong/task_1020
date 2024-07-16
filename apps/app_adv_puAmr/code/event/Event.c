/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：Event.c
* 版本号：1.0
* 功能：实现事件的检测与主动上报功能
* 原始开发人及时间：董宁 2006年12月25日
* 修改人员、时间及修改内容:朴军海 2019年7月1日
            增加国网101、104协议所需的部分功能
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
#include "msg_id.h"
#include "label_id.h"
#include "storage.h"
#include "oadInfoList.h"
#include "appmsg_split.h"

#include "area.h"
#include "hal.h"
#include "adc.h"
#include "power.h"

/*-******************************************************************-*/
/*-                             全局变量                             -*/
/*-******************************************************************-*/


EVENT_3100_INFO event_3100;   //终端初始化事件
EVENT_3101_INFO event_3101;   //终端版本号变更事件
EVENT_3104_INFO event_3104;   //终端状态量变位事件
EVENT_3105_INFO event_3105;   //电能表时间超差事件
EVENT_3106_INFO event_3106;   //终端停/上电事件
EVENT_31XX_INFO event_3109;   //终端消息认证错误事件
EVENT_31XX_INFO event_310A;   //设备故障事件
EVENT_310X_INFO event_310B;   //电能表示度下降事件
EVENT_310X_INFO event_310C;   //电能表超差事件
EVENT_310X_INFO event_310D;   //电能表飞走事件
EVENT_310X_INFO event_310E;   //电能表停走事件
EVENT_310F_INFO event_310F;   //抄表失败事件
EVENT_3110_INFO event_3110;   //终端流量超限事件
EVENT_3111_INFO event_3111;    //未知电表事件
EVENT_31XX_INFO event_3114;   //终端对时事件
EVENT_3118_INFO event_3118;   //终端编程事件
EVENT_3119_INFO event_3119;   //电流回路异常事件
EVENT_300F_INFO event_300F;   //终端相序异常事件
EVENT_311C_INFO event_311C;   //电能表数据变更监控记录
EVENT_3140_INFO event_3140;   //安全事件变更记录
EVENT_3115_INFO event_3115;   //遥控跳闸事件记录
EVENT_3200_INFO event_3200;   //功控跳闸事件记录
EVENT_3201_INFO event_3201;   //电控跳闸事件记录
EVENT_3202_INFO event_3202;   //购电参数设置事件记录
EVENT_3203_INFO event_3203;   //电控告警事件记录
EVENT_311B_INFO event_311B;   //终端对电能表校时记录
EVENT_311F_INFO event_311F;    //拒绝从节点入网记录
EVENT_3112_INFO event_3112;    //垮台区电表事件记录
EVENT_3030_INFO event_3030;    //模块变更事件
EVENT_3E03_INFO event_3E03;    //拓扑识别
EVENT_31XX_INFO event_3117;   //输出回路接入状态变位事件
EVENT_31XX_INFO event_3327;   //系统监测事件单元

EVENT_360X_INFO event_3600;
EVENT_360X_INFO event_3601;
EVENT_360X_INFO event_3602;



DB_CLIENT           gDbCenterClient;                    //和数据中心交互的句柄(新)
OOP_OCTETVAR16_T    tmnAddr;                            //终端通信地址
OOP_WORD3V_T        tmnVoltage;                         //终端电压
OOP_OCTETVAR64_T    MAC;                                //校验码
OOP_METER_INFO      oopMeterInfo[NO_VIR_MTR_MAX_NUM];   //采集档案配置
OOP_TASK_T          oopTask[TASK_MAX_NUM];              //任务配置表
OOP_CHANNEL_T       g_tnmPort;                          //终端上报端口
BOOLEAN             g_tnmEventReport = 0;               //允许终端主动上报标志,1允许,0不允许
int8                g_PowerOff     = 0;                 //0x00表示未知，正数表示终端正常供电，负数表示终端停电由电池供电
extern OOP_PERCEPTION_TERMINAL_LIST_T perceptionterminal;
extern void rpt_evtstat_clear(void);

/**********************************************************************
* @brief     ：获取事件相关操作数据库中句柄
* @param[in] ：

* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2022-03-01
* @Update    :
**********************************************************************/
int get_evt_dbclient()
{
    return gDbCenterClient;
}


#if DESC("事件初始化",1)
/**********************************************************************
* @name      : CleanEventStatusFlag
* @brief     ：初始化测量点事件标志
* @param[in] ：

* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-24
* @Update    :
**********************************************************************/
void CleanEventStatusFlag()
{
    uint8 bData[EventMaxFlagLen] = {0};
    
    EVENT_FMT_TRACE("InitEventStatusFlag set 0!\n");
	
#ifdef AREA_SHAANXI
	event_3199.isFirst = 0;
	event_3199.iccidBef.nNum = 0;

#endif
    //直接初始化成0有些数据清不掉 先写全ff再写0
    memset(bData,0xFF,sizeof(bData));
    write_pdata_xram("^Event", (const char *)bData, EventStatusRecAddr, EventMaxFlagLen);
    write_pdata_xram("^Event", (const char *)bData, ProgDownAddr, ProgDownRecLen);
    sleep(1);
    memset(bData,0x00,sizeof(bData));
    write_pdata_xram("^Event", (const char *)bData, EventStatusRecAddr, EventMaxFlagLen);
    write_pdata_xram("^Event", (const char *)bData, ProgDownAddr, ProgDownRecLen);
    int len;
    uint32 dwERFlgTmp;
    int i = 501;
    for(i=501;i<510;i++)
    {
        len =  read_pdata_xram("^Event", (char *)&dwERFlgTmp, (EventStatusRecAddr+(i-1)*(4)), 4);
        if(len<0 )
        {
            //-读取事件状态记录-
            EVENT_FMT_DEBUG("读取event_3105.ERFlg[%d]=%08X事件记录标识错误或无效\n",i, dwERFlgTmp);
        }
        EVENT_FMT_DEBUG("读取event_3105.ERFlg[%d]=%08X %d \n",i, dwERFlgTmp,EventMaxFlagLen);
    }
#ifdef AREA_FUJIAN
	bData[0] = 0xaa;
	write_pdata_xram("^Event", (const char *)bData, EventStopRpt, 1);
	bData[0] = 0x55;
	write_pdata_xram("^Event", (const char *)bData, EventStopTm, 1);
	memset(&gTransEvtStatus, 0, sizeof(OOP_TRANS_EVENT_STATUS_T));
    rpt_evtstat_clear();
#endif
}

/**********************************************************************
* @name      : CleanEventCommInfo
* @brief     ：初始化事件公共信息
* @param[in] ：EVENT_COMM_INFO *eventCommInfo
               
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-03-03
* @Update    :
**********************************************************************/
void CleanEventCommInfo(EVENT_COMM_INFO *eventCommInfo)
{
    uint8 bData[2048];
    memset(bData, 0, sizeof(bData));
    
    eventCommInfo->eventNum = 0;
    eventCommInfo->recordnum = 0;
    memset(&eventCommInfo->eventState, 0, sizeof(EVTSTATE));
    memset(&eventCommInfo->eventTime, 0, sizeof(EVTTIME));
    
    if(eventCommInfo->EventFlagLen > 0)
    {
        write_pdata_xram("^Event", (const char *)bData, eventCommInfo->EventFlagAddr, eventCommInfo->EventFlagLen);
    }
    return;
}


/**********************************************************************
* @name      : MntVersionCheck
* @brief     ：终端通信地址更改
* @param[in] ：OOP_OCTETVAR16_T tLocalAddr 当前通信地址
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-16
* @Update    :
**********************************************************************/
void GetMntAddr(OOP_OCTETVAR16_T tLocalAddr)
{
    //终端通信地址
    tmnAddr = tLocalAddr;
    MAC.nNum = 6;
    memcpy(MAC.value, tmnAddr.value, MAC.nNum);
    EVENT_FMT_TRACE("MAC=%02X %02X %02X %02X %02X %02X \n", MAC.value[0], MAC.value[1], MAC.value[2], MAC.value[3], MAC.value[4], MAC.value[5]);
    return ;
}

/**********************************************************************
* @name      : readMntAddr
* @brief     ：读取终端地址
* @param[in] ：int             clientid     数据中心句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2021-1-26
* @Update    :
**********************************************************************/
int readMntAddr(int clientid)
{
    int         ret     = -1;
    OOP_OCTETVAR16_T tLocalAddr;

    ret = evt_read_nomal_data(clientid, 0, 0, 0x40010200, sizeof(OOP_OCTETVAR16_T), (uint8 *)&tLocalAddr);
    if (ret == ERR_OK)
    {
        GetMntAddr(tLocalAddr);
    }

    return ret;
}


/**********************************************************************
* @name      : GetEventNumAndIndex
* @brief     ：获取事件的记录数和记录序号
* @param[in] ：int             clientid     数据中心句柄
               uint32         eventOAD     事件OAD
* @param[out]：uint16*         nEventNum     事件记录数
               uint32*        nIndex        记录序号
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int GetEventNumAndIndex(int clientid, uint32 eventOAD, uint16* nEventNum, uint32* nIndex)
{
    READ_RECORD_T startReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T stepRecordOut;
    OOP_EVENT_T *pTemp = NULL;
    uint32 handle = 0;
    uint32 recNum = 0;
    int  ret = 0;
    time_t timep;

    memset(&startReadRecord,0,sizeof(READ_RECORD_T));
    memset(&stepReadRecord,0,sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut,0,sizeof(RECORD_UNIT_T));

    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep
    timep += COMPENSATION_TIME;
    startReadRecord.recordOAD.road.oadMain.value = 0;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.road.oadCols.oad[0].value = eventOAD;
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = 0;
    startReadRecord.cType = 0x02;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = timep;
    startReadRecord.sortType = 0x01;
   
    ret = db_read_record_start(clientid, &startReadRecord, &handle, &recNum);
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_read_record_start, OAD=%8X error ret %d\n", eventOAD, ret);
        return ret;

    }

    *nEventNum = (uint16)recNum;
    
    if(recNum == 0)
    {
        db_read_record_end(clientid, handle);
        return ERR_OK;
    }

    if(MAC.nNum != 6)
    {
        readMntAddr(clientid);
    }

    stepReadRecord.MAC = MAC;
    
    ret = db_read_record_step(clientid, handle, &stepReadRecord, &stepRecordOut);   
    db_read_record_end(clientid, handle);
    
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_read_record_step, OAD=%8X error ret %d\n", eventOAD, ret);
        return ret;

    }
    else
    {
        pTemp = (OOP_EVENT_T *)stepRecordOut.record[0].outDataBuf;
        *nIndex = pTemp->nIndex;
    }

    return ERR_OK;
}

/**********************************************************************
* @name      : Uint16To32
* @brief     ：两字节拼接成四字节
* @param[in] ：uint16 front
               uint16 rear  
* @param[out]：
* @return    ：拼接成四字节数据
* @Create    : 梅安宁
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
uint32 Uint16To32(uint16 front, uint16 rear)
{
    uint32 value = 0;
    value = (0xffffffff&front)<<16;
    value = value|rear;
    return value;
}


/**********************************************************************
* @name      : IniEventCommInfo
* @brief     ：事件公共信息初始化
* @param[in] ：EVENT_COMM_INFO *eventCommInfo      
* @param[out]：EVENT_COMM_INFO *eventCommInfo   
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int IniEventCommInfo(EVENT_COMM_INFO *eventCommInfo)
{
    if(eventCommInfo == NULL)
    {
        EVENT_FMT_DEBUG("IniEventCommInfo error, eventCommInfo is  null.\n");
        return -1;
    }

    //获取 事件有效标识
    evt_read_nomal_data(gDbCenterClient, 0, 0, Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0900), sizeof(uint8), &eventCommInfo->checkflag);

    //获取 事件上报标识
    evt_read_nomal_data(gDbCenterClient, 0, 0, Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0800), sizeof(uint8), &eventCommInfo->reportflag);

    // 事件记录序号和记录数
    GetEventNumAndIndex(gDbCenterClient, eventCommInfo->eventOAD.value, &eventCommInfo->eventNum, &eventCommInfo->recordnum);

    EVENT_FMT_DEBUG("event[%8X] eventNum=%u ,recordnum=%u!\n", eventCommInfo->eventOAD.value, eventCommInfo->eventNum, eventCommInfo->recordnum);
    
  
    return ERR_OK;
}

/**********************************************************************
* @name      : CompareVersion
* @brief     ：比较event_310的版本号
* @param[in] ：
* @param[out]：
* @return    ：0-相同/-1不相同
* @Create    : 梅安宁
* @Date      ：2019-12-26
* @Update    :
**********************************************************************/
int CompareVersion()
{
    int i = 0;
    for(i=0; i<8; i++)
    {
        if(event_3101.lastversion.szSoftVer.value[i] != event_3101.version.szSoftVer.value[i])
        {
            
            EVENT_FMT_DEBUG("Event[3101] version=[%s] ,lastversion=[%s] \n"
                            , event_3101.version.szSoftVer.value
                            , event_3101.lastversion.szSoftVer.value);
            return -1;
        }
    }

    return 0;
}

/**********************************************************************
* @name      : MntVersionCheck
* @brief     ：终端版本号变更检查
* @param[in] ：OOP_VERSION_T        tVersion 当前版本号
* @param[out]：event_3101.ishappen
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-11
* @Update    :
**********************************************************************/
void MntVersionCheck(OOP_VERSION_T        tVersion)
{
    event_3101.version = tVersion;
     
    //读取历史版本号
    if (read_pdata_xram("^Event", event_3101.lastversion.szSoftVer.value, LastVersionAddr, LastVersionLen) < 0)
    {
        event_3101.lastversion = event_3101.version;
        write_pdata_xram("^Event", event_3101.version.szSoftVer.value, LastVersionAddr, LastVersionLen);
        return ;
    }

    if (CompareVersion() < 0)   //-终端软件版本变更,且读取成功
    {
        evt_check_3101(gDbCenterClient);
        write_pdata_xram("^Event", event_3101.version.szSoftVer.value, LastVersionAddr, LastVersionLen);
    }

    return ;
}


/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取终端初始化事件参数
输入参数：

输出参数：
  event_3100   终端初始化事件
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
********************************************************************/
int Init3100Para()
{
    memset(&event_3100, 0, sizeof(EVENT_3100_INFO));

    OOP_RCSD_T eventRCSD={
        5, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态
            }
    };

    event_3100.eventCommInfo.eventRCSD = eventRCSD;

    event_3100.eventCommInfo.eventOAD.nObjID = 0X3100;
    event_3100.eventCommInfo.eventOAD.value = 0X31000200;

    // 公共信息
    IniEventCommInfo(&event_3100.eventCommInfo);

    event_3100.initflag = 1;
    return ERR_OK;
}


/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取终端版本号变更事件参数
输入参数：

输出参数：
  event_3101   终端版本号变更事件全局变量
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
    从数据中心获取终端版本号变更事件参数，写入全局变量中。
********************************************************************/
int Init3101Para()
{
//    int             ret     = -1;
//    uint32          len     = 0;
//    NOMAL_OAD_T     nomalOAD;
    OOP_VERSION_T   tVersion;

//    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
    memset(&tVersion,0,sizeof(OOP_VERSION_T));
    memset(&event_3101, 0, sizeof(EVENT_3101_INFO));

    OOP_RCSD_T eventRCSD={
        7, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x43002302},}},//事件发生前软件版本号
                {0,{{0x43004302},}},//事件发生前软件版本号
            }
    };

    event_3101.eventCommInfo.eventRCSD = eventRCSD;

    event_3101.eventCommInfo.eventOAD.nObjID= 0X3101;
    event_3101.eventCommInfo.eventOAD.value= 0X31010200;

    // 公共信息
    IniEventCommInfo(&event_3101.eventCommInfo);

    // 终端版本号
/*    nomalOAD.oad.value = 0x43000300;
    ret = db_read_nomal(gDbCenterClient, &nomalOAD, sizeof(OOP_VERSION_T), (uint8*)&tVersion, &len);
    if (ret != ERR_OK || len == 0)
    {
        EVENT_FMT_DEBUG("nomal_data_read OAD=%8X error ret=%d,len=%u\n", nomalOAD.oad.value, ret, len);
        return ret;
    }
*/
    snprintf(tVersion.szSoftVer.value, 8, "%s", g_tVer.softVer);

    event_3101.initflag = 1;

    MntVersionCheck(tVersion);
 
    return 0;
}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取状态量变位事件参数
输入参数：

输出参数：
  event_3104   状态量变位事件
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
********************************************************************/
int Init3104Para()
{
    memset(&event_3104, 0, sizeof(EVENT_3104_INFO));

    event_3104.eventCommInfo.eventOAD.nObjID = 0X3104;
    event_3104.eventCommInfo.eventOAD.value = 0X31040200;

#ifdef PRODUCT_ZCU_1

#ifdef AREA_JIANGSU
    OOP_RCSD_T eventRCSD={
            22, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
    
                    {0,{{0x201E4200},}},//事件发生时间
                    {0,{{0xF2034201},}},//第1路状态量变位发生后状态
                    {0,{{0xF2034202},}},//第2路状态量变位发生后状态
                    {0,{{0xF2034203},}},//第3路状态量变位发生后状态
                    {0,{{0xF2034204},}},//第4路状态量变位发生后状态
                    {0,{{0xF2034205},}},//第5路状态量变位发生后状态
                    {0,{{0xF2034206},}},//第6路状态量变位发生后状态
                    {0,{{0xF2034207},}},//第7路状态量变位发生后状态
                    {0,{{0xF2034208},}},//第8路状态量变位发生后状态
                    {0,{{0xF2034209},}},//第9路状态量变位发生后状态
                    {0,{{0xF203420A},}},//第10路状态量变位发生后状态
                    {0,{{0xF203420B},}},//第11路状态量变位发生后状态
                    {0,{{0xF203420C},}},//第12路状态量变位发生后状态
                    {0,{{0xF203420D},}},//第13路状态量变位发生后状态
                    {0,{{0xF203420E},}},//第14路状态量变位发生后状态
                    {0,{{0xF203420F},}},//第15路状态量变位发生后状态
                    {0,{{0xF2034210},}},//第16路状态量变位发生后状态
                }
        };
#else
    OOP_RCSD_T eventRCSD={
        14, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x201E4200},}},//事件发生时间
                {0,{{0xF2034201},}},//第1路状态量变位发生后状态
                {0,{{0xF2034202},}},//第2路状态量变位发生后状态
                {0,{{0xF2034203},}},//第3路状态量变位发生后状态
                {0,{{0xF2034204},}},//第4路状态量变位发生后状态
                {0,{{0xF2034205},}},//第5路状态量变位发生后状态
                {0,{{0xF2034206},}},//第6路状态量变位发生后状态
                {0,{{0xF2034207},}},//第7路状态量变位发生后状态
                {0,{{0xF2034208},}},//第8路状态量变位发生后状态
            }
    };
#endif
#endif

#ifdef PRODUCT_CCU
    OOP_RCSD_T eventRCSD={
        9, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x201E4200},}},//事件发生时间
                {0,{{0xF2034201},}},//第1路状态量变位发生后状态
                {0,{{0xF2034202},}},//第2路状态量变位发生后状态
                {0,{{0xF2034208},}},//第8路状态量变位发生后状态,门节点
            }
    };
#endif
#ifdef PRODUCT_ECU
    OOP_RCSD_T eventRCSD={
        10, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x201E4200},}},//事件发生时间
                {0,{{0xF2034201},}},//第1路状态量变位发生后状态
                {0,{{0xF2034202},}},//第2路状态量变位发生后状态
                {0,{{0xF2034203},}},//第3路状态量变位发生后状态
                {0,{{0xF2034204},}},//第4路状态量变位发生后状态
            }
    };
#endif
#ifdef PRODUCT_ZCU_3
    OOP_RCSD_T eventRCSD={
        11, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x201E4200},}},//事件发生时间
                {0,{{0xF2034201},}},//第1路状态量变位发生后状态
                {0,{{0xF2034202},}},//第2路状态量变位发生后状态
                {0,{{0xF2034203},}},//第3路状态量变位发生后状态
                {0,{{0xF2034204},}},//第4路状态量变位发生后状态
                {0,{{0xF2034208},}},//第8路状态量变位发生后状态,门节点
            }
    };
#endif
    event_3104.eventCommInfo.eventRCSD = eventRCSD;
    
    // 公共信息
    IniEventCommInfo(&event_3104.eventCommInfo);

    GetEventNomalParam(gDbCenterClient, 0, 0, 0xf2030200);
    GetEventNomalParam(gDbCenterClient, 0, 0, 0xf2030400);

    event_3104.initflag = 1;
    return ERR_OK;

}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取电能表时间超差事件参数，写入全局变量中。
输入参数：
输出参数：
  event_3105   电能表时间超差事件全局变量
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
    从数据中心获取电能表时间超差事件参数，写入全局变量中。
********************************************************************/
int Init3105Para()
{
    memset(&event_3105, 0, sizeof(EVENT_3105_INFO));

    event_3105.eventCommInfo.eventOAD.nObjID= 0X3105;
    event_3105.eventCommInfo.eventOAD.value= 0X31050200;
    event_3105.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr3105;
    event_3105.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr3105;
	event_3105.eventCommInfo.EventStoragIndexAddr = EventStoragIndex3105;

    OOP_RCSD_T eventRCSD={
        7, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x33130206},}},//电能表时钟
                {0,{{0x33130207},}},//终端当前时钟
            }
    };

    event_3105.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3105.eventCommInfo);
 
    //获取 时钟超差门限，关联采集任务号
    evt_read_nomal_data(gDbCenterClient, 0, 0, 0x31050600, sizeof(OOP_EVTCFG_3105_T), (uint8 *)&event_3105.checkPara);

    EVENT_FMT_TRACE("event_3105.checkPara.nValue=%04X \n", event_3105.checkPara.nValue);
    EVENT_FMT_TRACE("event_3105.checkPara.nTID=%u \n", event_3105.checkPara.nTID);

    event_3105.initflag = 1;
    return ERR_OK;
}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取电能表时间超差事件参数，写入全局变量中。
输入参数：
输出参数：
  event_3105   电能表时间超差事件全局变量
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
    从数据中心获取电能表时间超差事件参数，写入全局变量中。
********************************************************************/
int Init3106Para()
{
    int         i       = 0;
      
    memset(&event_3106, 0, sizeof(EVENT_3106_INFO));

    event_3106.eventCommInfo.eventOAD.nObjID= 0X3106;
    event_3106.eventCommInfo.eventOAD.value= 0X31060200;

    event_3106.eventCommInfo.EventFlagAddr = TmnPowerOffAddr;
    event_3106.eventCommInfo.EventFlagLen = TmnPowerOffLen;

    OOP_RCSD_T eventRCSD={
        6, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x33090206},}},//属性标志bit-string(SIZE(8))
//                {0,{{0x33090204},}},//终端当前时钟
            }
    };

    event_3106.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3106.eventCommInfo);

    //获取 终端电压
    GetEventNomalParam(gDbCenterClient, 1, 0, 0x20000200);

    for(i=0; i<event_3106.tmnVoltage.nNum; i++)
    {
        if(event_3106.tmnVoltage.nValue[i] == 0XFFFF)
            event_3106.tmnVoltage.nValue[i] = 0;
    }

    //获取 事件配置参数
    GetEventNomalParam(gDbCenterClient, 0, 0, 0x31060600);
	
#ifdef AREA_FUJIAN
	//停电上报标记       0x3c 等待上报， 0x55 立即上报，0xaa 已经上报
	//终端上电时，等待上报的，立即上报
	read_pdata_xram("^Event", (char *)&event_3106.rptFlag, EventStopRpt, EventStopRptLen);
	if(event_3106.rptFlag <= 0x3c)
    {
        event_3106.rptFlag = 0x55;
		read_pdata_xram("^Event", (char *)&event_3106.tmnStarttime,ProgDownAddr, ProgDownRecLen);
    }

	read_pdata_xram("^Event", (char *)&event_3106.tmFlag, EventStopTm, EventStopRptLen);
	if(event_3106.tmFlag != 0xaa && event_3106.tmFlag != 0x55)
    {
        event_3106.tmFlag = 0xaa;
		write_pdata_xram("^Event", (char *)&event_3106.tmFlag, EventStopTm, EventStopRptLen);
    }
	evt_read_nomal_data(gDbCenterClient, 0, 0, 0x4e010200, sizeof(OOP_BITSTR16_T), event_3106.powerStatus.nValue);
	evt_read_nomal_data(gDbCenterClient, 0, 0, 0x4e020200, sizeof(OOP_BATTERY_STATUS_T), (uint8 *)&event_3106.batteryStatus);
	check_battery_status(gDbCenterClient);
#endif
    event_3106.initflag = 1;
    return ERR_OK;
}


/**********************************************************************
* @name      : Init3109Para
* @brief     : 终端对时事件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init3109Para()
{
    memset(&event_3109, 0, sizeof(EVENT_31XX_INFO));

    event_3109.eventCommInfo.eventOAD.nObjID= 0X3109;
    event_3109.eventCommInfo.eventOAD.value= 0X31090200;

    OOP_RCSD_T eventRCSD={
            7, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

                    {0,{{0X44012201},}},//事件发生前安全认证密码
                }
        };
    
    event_3109.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3109.eventCommInfo);

    event_3109.initflag = 1;
    return ERR_OK;
}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取设备故障事件参数
输入参数：

输出参数：
  event_3100   设备故障记录事件
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
********************************************************************/
int Init310APara()
{
    memset(&event_310A, 0, sizeof(EVENT_31XX_INFO));

    event_310A.eventCommInfo.eventOAD.nObjID = 0X310A;
    event_310A.eventCommInfo.eventOAD.value = 0X310A0200;

    event_310A.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr310A;
    event_310A.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr310A;
    event_310A.eventCommInfo.EventFlagAddr = EventFlagRecAddr;
    event_310A.eventCommInfo.EventFlagLen = EventFlagRecLen;

    OOP_RCSD_T eventRCSD={
        5, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态
            }
    };

    event_310A.eventCommInfo.eventRCSD = eventRCSD;
    
    // 公共信息
    IniEventCommInfo(&event_310A.eventCommInfo);

    event_310A.initflag = 1;
    return ERR_OK;

}


/**********************************************************************
* @name      : Init310BPara
* @brief     : 电能表示度下降事件初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init310BPara()
{
    memset(&event_310B, 0, sizeof(EVENT_310X_INFO));

    event_310B.eventCommInfo.eventOAD.nObjID= 0X310B;
    event_310B.eventCommInfo.eventOAD.value= 0X310B0200;
    event_310B.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr310B;
    event_310B.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr310B;
    event_310B.eventCommInfo.EventFlagAddr = EventStoragTmRecAddr310B;
    event_310B.eventCommInfo.EventFlagLen = EventStoragTmRecLen;
	event_310B.eventCommInfo.EventStoragIndexAddr = EventStoragIndex310B;

    OOP_RCSD_T eventRCSD={
        7, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0X00102201},}},//事件发生前正向有功总电能
                {0,{{0X00108201},}},//事件结束后正向有功总电能
            }
    };

    event_310B.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_310B.eventCommInfo);
 
    //关联采集任务号
    evt_read_nomal_data(gDbCenterClient, 0, 0, 0x310B0600, sizeof(uint8), &event_310B.nTID);
    
    EVENT_FMT_TRACE("event_310B.nTID=%d!\n", event_310B.nTID);

    event_310B.initflag = 1;
    return ERR_OK;
}


/**********************************************************************
* @name      : Init310CPara
* @brief     : 电能表超差事件初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init310CPara()
{
    memset(&event_310C, 0, sizeof(EVENT_310X_INFO));

    event_310C.eventCommInfo.eventOAD.nObjID= 0X310C;
    event_310C.eventCommInfo.eventOAD.value= 0X310C0200;
    event_310C.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr310C;
    event_310C.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr310C;
    event_310C.eventCommInfo.EventFlagAddr = EventStoragTmRecAddr310C;
    event_310C.eventCommInfo.EventFlagLen = EventStoragTmRecLen;
	event_310C.eventCommInfo.EventStoragIndexAddr = EventStoragIndex310C;

    OOP_RCSD_T eventRCSD={
        7, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0X00102201},}},//事件发生前正向有功总电能
                {0,{{0X00108201},}},//事件结束后正向有功总电能
            }
    };

    event_310C.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_310C.eventCommInfo);

    //获取 配置参数
    evt_read_nomal_data(gDbCenterClient, 0, 0, 0x310C0600, sizeof(OOP_EVTCFG_310X_T), (uint8 *)&event_310C.eventParam);
    
    EVENT_FMT_TRACE("Get event_310C.eventParam.nValue = %u OK!\n", event_310C.eventParam.nValue);
    EVENT_FMT_TRACE("Get event_310C.eventParam.nTID = %hu OK!\n", event_310C.eventParam.nTID);

    event_310C.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init310DPara
* @brief     : 电能表飞走事件初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init310DPara()
{
    memset(&event_310D, 0, sizeof(EVENT_310X_INFO));

    event_310D.eventCommInfo.eventOAD.nObjID= 0X310D;
    event_310D.eventCommInfo.eventOAD.value= 0X310D0200;
    event_310D.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr310D;
    event_310D.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr310D;
    event_310D.eventCommInfo.EventFlagAddr = EventStoragTmRecAddr310D;
    event_310D.eventCommInfo.EventFlagLen = EventStoragTmRecLen;
	event_310D.eventCommInfo.EventStoragIndexAddr = EventStoragIndex310D;
    
    OOP_RCSD_T eventRCSD={
            7, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
    
                    {0,{{0X00102201},}},//事件发生前正向有功总电能
                    {0,{{0X00108201},}},//事件结束后正向有功总电能
                }
        };
    
    event_310D.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_310D.eventCommInfo);

    //获取 配置参数
    evt_read_nomal_data(gDbCenterClient, 0, 0, 0x310D0600, sizeof(OOP_EVTCFG_310X_T), (uint8 *)&event_310D.eventParam);
    
    EVENT_FMT_TRACE("Get event_310D.eventParam.nValue = %u OK!\n", event_310D.eventParam.nValue);
    EVENT_FMT_TRACE("Get event_310D.eventParam.nTID = %hu OK!\n", event_310D.eventParam.nTID);


    event_310D.initflag = 1;
    return ERR_OK;
}


/**********************************************************************
* @name      : Init310EPara
* @brief     : 电能表停走事件初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init310EPara()
{
    memset(&event_310E, 0, sizeof(EVENT_310X_INFO));

    event_310E.eventCommInfo.eventOAD.nObjID= 0X310E;
    event_310E.eventCommInfo.eventOAD.value= 0X310E0200;
    event_310E.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr310E;
    event_310E.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr310E;
    event_310E.eventCommInfo.EventFlagAddr = EventStoragTmRecAddr310E;
    event_310E.eventCommInfo.EventFlagLen = EventStoragTmRecLen;
	event_310E.eventCommInfo.EventStoragIndexAddr = EventStoragIndex310E;

    OOP_RCSD_T eventRCSD={
            6, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
                    
                    {0,{{0X00102201},}},//事件发生前正向有功总电能
                }
        };
    
    event_310E.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_310E.eventCommInfo);

    //获取 配置参数
    evt_read_nomal_data(gDbCenterClient, 0, 0, 0x310E0600, sizeof(OOP_EVTCFG_310E_T), (uint8 *)&event_310E.event310eParam);

    EVENT_FMT_TRACE("Get event_310E.eventParam.threshold.value = %u OK!\n", event_310E.event310eParam.threshold.value);
    EVENT_FMT_TRACE("Get event_310E.eventParam.nTID = %hu OK!\n", event_310E.event310eParam.nTID);

    event_310E.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init310FPara
* @brief     : 抄表失败事件初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init310FPara()
{
    memset(&event_310F, 0, sizeof(EVENT_310F_INFO));

    event_310F.eventCommInfo.eventOAD.nObjID= 0X310F;
    event_310F.eventCommInfo.eventOAD.value= 0X310F0200;
    event_310F.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr310F;
    event_310F.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr310F;
    event_310F.eventCommInfo.EventFlagAddr = EventStoragTmRecAddr310F;
    event_310F.eventCommInfo.EventFlagLen = EventStoragTmRecLen;
	event_310F.eventCommInfo.EventStoragIndexAddr = EventStoragIndex310F;

    OOP_RCSD_T eventRCSD={
            8, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

                    {0,{{0x60410200},}},//事件发生前最近一次抄表成功时间
                    {0,{{0X00102201},}},//事件发生前正向有功总电能
                    {0,{{0x00302201},}},//事件发生前最近一次抄表成功正向无功总电能示值
                }
        };
    
    event_310F.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_310F.eventCommInfo);

    //获取 配置参数
    evt_read_nomal_data(gDbCenterClient, 0, 0, 0x310F0600, sizeof(OOP_EVTCFG_310F_T), (uint8 *)&event_310F.eventParam);

    EVENT_FMT_TRACE("Get event_310F.eventParam.nTicks = %u OK!\n", event_310F.eventParam.nTicks);
    EVENT_FMT_TRACE("Get event_310F.eventParam.nTID = %hu OK!\n", event_310F.eventParam.nTID);

    event_310F.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3110Para
* @brief     : 流量超标事件初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init3110Para()
{
    memset(&event_3110, 0, sizeof(EVENT_3110_INFO));

    event_3110.eventCommInfo.eventOAD.nObjID = 0X3110;
    event_3110.eventCommInfo.eventOAD.value = 0X31100200;
    event_3110.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr3110;
    event_3110.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr3110;
    event_3110.eventCommInfo.EventFlagAddr = FluxOverTopAddr;
    event_3110.eventCommInfo.EventFlagLen = FluxOverTopLen;

    OOP_RCSD_T eventRCSD={
            7, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

                    {0,{{0x22004202},}},//事件发生后当月已发生通信流量
                    {0,{{0x31100601},}},//月通信流量门限
                }
        };
    
    event_3110.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3110.eventCommInfo);

    //获取 配置参数
    evt_read_nomal_data(gDbCenterClient, 0, 0, 0x31100600, sizeof(OOP_EVTCFG_3110_T), (uint8 *)&event_3110.eventParam);

    evt_read_nomal_data(gDbCenterClient, 0, 0, 0x22000200, sizeof(OOP_DWORD2_T), (uint8*)&event_3110.recvdataFlowrate);

    EVENT_FMT_TRACE("event_3110.eventParam.nValue = %u!\n", event_3110.eventParam.nValue);

    event_3110.initflag = 1;
    return ERR_OK;

}

/**********************************************************************
* @name      : Init3111Para
* @brief     : 发现未知电表事件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init3111Para()
{
    memset(&event_3111, 0, sizeof(EVENT_3111_INFO));

    event_3111.eventCommInfo.eventOAD.nObjID = 0X3111;
    event_3111.eventCommInfo.eventOAD.value = 0X31110200;

    OOP_RCSD_T eventRCSD={
            6, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

                    {0,{{0x33030206},}},//ARRAY一个搜表结果
                }
    };
    
    event_3111.eventCommInfo.eventRCSD = eventRCSD;
    
    // 公共信息
    IniEventCommInfo(&event_3111.eventCommInfo);

    //搜表状态
    GetEventNomalParam(gDbCenterClient, 0, 0, 0x60020A00);

    event_3111.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3114Para
* @brief     : 终端对时事件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init3114Para()
{
    memset(&event_3114, 0, sizeof(EVENT_31XX_INFO));

    event_3114.eventCommInfo.eventOAD.nObjID= 0X3114;
    event_3114.eventCommInfo.eventOAD.value= 0X31140200;

    OOP_RCSD_T eventRCSD={
            7, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

                    {0,{{0x40002200},}},//事件发生前对时时间
                    {0,{{0x40008200},}},//事件结束后对时时间
                }
        };
    
    event_3114.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3114.eventCommInfo);

    event_3114.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3118Para
* @brief     : 终端编程事件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init3118Para()
{
    memset(&event_3118, 0, sizeof(EVENT_3118_INFO));

    event_3118.eventCommInfo.eventOAD.nObjID= 0X3118;
    event_3118.eventCommInfo.eventOAD.value= 0X31180200;
    OOP_RCSD_T eventRCSD={
            6, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

                    {0,{{0x33020206},}},//编程对象列表array OAD
                }
        };
    event_3118.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3118.eventCommInfo);

    event_3118.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3119Para
* @brief     : 电流回路异常检测
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init3119Para()
{
    memset(&event_3119, 0, sizeof(EVENT_3119_INFO));

    event_3119.eventCommInfo.eventOAD.nObjID= 0X3119;
    event_3119.eventCommInfo.eventOAD.value= 0X31190200;
    event_3119.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr3119;
    event_3119.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr3119;

    OOP_RCSD_T eventRCSD={
            7, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

                    {0,{{0x20034200},}},//事件发生时电压电流相角
                    {0,{{0x00104201},}},//事件发生时正向有功总电能
                }
        };
    
    event_3119.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3119.eventCommInfo);

    event_3119.initflag = 1;
    return ERR_OK;
}
/**********************************************************************
* @name      : Init311BPara
* @brief     : 终端对时事件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init311BPara()
{
    memset(&event_311B, 0, sizeof(EVENT_311B_INFO));

    event_311B.eventCommInfo.eventOAD.nObjID= 0X311B;
    event_311B.eventCommInfo.eventOAD.value= 0X311B0200;

    OOP_RCSD_T eventRCSD={
            7, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

                    {0,{{0x330D0206},}},//校时前时钟
                    {0,{{0x330D0207},}},//时钟误差
                }
        };
    
    event_311B.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_311B.eventCommInfo);

    event_311B.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init311FPara
* @brief     : 终端对时事件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init311FPara()
{
    memset(&event_311F, 0, sizeof(EVENT_311F_INFO));

    event_311F.eventCommInfo.eventOAD.nObjID= 0X311F;
    event_311F.eventCommInfo.eventOAD.value= 0X311F0200;

    OOP_RCSD_T eventRCSD={
            6, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

                    {0,{{0x332F0206},}},//拒绝从节点列表

                }
        };
    
    event_311F.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_311F.eventCommInfo);

    event_311F.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3E03Para
* @brief     : 终端拓扑识别事件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init3E03Para()
{
    memset(&event_3E03, 0, sizeof(EVENT_3E03_INFO));

    event_3E03.eventCommInfo.eventOAD.nObjID= 0X3E03;
    event_3E03.eventCommInfo.eventOAD.value= 0X3E030200;

    OOP_RCSD_T eventRCSD={
            6, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

                    {0,{{0x3E830206},}},//识别结果信息

                }
        };
    
    event_3E03.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3E03.eventCommInfo);

    event_3E03.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3112Para
* @brief     : 跨台区电表事件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init3112Para()
{
    memset(&event_3112, 0, sizeof(EVENT_3112_INFO));

    event_3112.eventCommInfo.eventOAD.nObjID= 0X3112;
    event_3112.eventCommInfo.eventOAD.value= 0X31120200;

    OOP_RCSD_T eventRCSD={
            6, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

                    {0,{{0x33040206},}},//跨台区电能表事件

                }
        };
    
    event_3112.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3112.eventCommInfo);

    event_3112.initflag = 1;
    return ERR_OK;
}
/**********************************************************************
* @name      : Init3030Para
* @brief     : 跨台区电表事件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init3030Para()
{
    memset(&event_3030, 0, sizeof(EVENT_3112_INFO));

    event_3030.eventCommInfo.eventOAD.nObjID= 0X3030;
    event_3030.eventCommInfo.eventOAD.value= 0X30300200;

    OOP_RCSD_T eventRCSD={
            8, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

                    {0,{{0x33120206},}},//地址
                    {0,{{0x33120207},}},//变更前的模块描述符
                    {0,{{0x33120208},}},//变更后的模块描述符

                }
        };
    
    event_3030.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3030.eventCommInfo);

    event_3030.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init300FPara
* @brief     : 终端相序异常检测
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init300FPara()
{
    memset(&event_300F, 0, sizeof(EVENT_300F_INFO));

    event_300F.eventCommInfo.eventOAD.nObjID= 0X300F;
    event_300F.eventCommInfo.eventOAD.value= 0X300F0200;
    event_300F.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr300F;
    event_300F.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr300F;
    event_300F.eventCommInfo.EventFlagAddr = EventFlagPhaseAbRecAddr;
    event_300F.eventCommInfo.EventFlagLen = EventFlagRecLen;

    OOP_RCSD_T eventRCSD={
            37, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
                    {0,{{0x00102201},}},//事件发生时刻正向有功总电能
                    {0,{{0x00202201},}},//事件发生时刻反向有功总电能
                    {0,{{0x00302201},}},//事件发生时刻组合无功1总电能
                    {0,{{0x00402201},}},//事件发生时刻组合无功2总电能
                    {0,{{0x00112201},}},//事件发生时刻A相正向有功电能
                    {0,{{0x00212201},}},//事件发生时刻A相反向有功电能
                    {0,{{0x00312201},}},//事件发生时刻A相组合无功1电能
                    {0,{{0x00412201},}},//事件发生时刻A相组合无功2电能
                    {0,{{0x00122201},}},//事件发生时刻B相正向有功电能
                    {0,{{0x00222201},}},//事件发生时刻B相反向有功电能
                    {0,{{0x00322201},}},//事件发生时刻B相组合无功1电能
                    {0,{{0x00422201},}},//事件发生时刻B相组合无功2电能
                    {0,{{0x00132201},}},//事件发生时刻C相正向有功电能
                    {0,{{0x00232201},}},//事件发生时刻C相反向有功电能
                    {0,{{0x00332201},}},//事件发生时刻C相组合无功1电能
                    {0,{{0x00432201},}},//事件发生时刻C相组合无功2电能
                    {0,{{0x00108201},}},//事件结束后正向有功总电能
                    {0,{{0x00208201},}},//事件结束后反向有功总电能
                    {0,{{0x00308201},}},//事件结束后组合无功1总电能
                    {0,{{0x00408201},}},//事件结束后组合无功2总电能
                    {0,{{0x00118201},}},//事件结束后A相正向有功电能
                    {0,{{0x00218201},}},//事件结束后A相反向有功电能
                    {0,{{0x00318201},}},//事件结束后A相组合无功1电能
                    {0,{{0x00418201},}},//事件结束后A相组合无功2电能
                    {0,{{0x00128201},}},//事件结束后B相正向有功电能
                    {0,{{0x00228201},}},//事件结束后B相反向有功电能
                    {0,{{0x00328201},}},//事件结束后B相组合无功1电能
                    {0,{{0x00428201},}},//事件结束后B相组合无功2电能
                    {0,{{0x00138201},}},//事件结束后C相正向有功电能
                    {0,{{0x00238201},}},//事件结束后C相反向有功电能
                    {0,{{0x00338201},}},//事件结束后C相组合无功1电能
                    {0,{{0x00438201},}},//事件结束后C相组合无功2电能
                }
        };
    
    event_300F.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_300F.eventCommInfo);

    //判定延时
    evt_read_nomal_data(gDbCenterClient, 0, 0, 0x300F0600, sizeof(uint8), &event_300F.nDelay);

    event_300F.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init311CPara
* @brief     : 电能表数据变更
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init311CPara()
{
    memset(&event_311C, 0, sizeof(EVENT_311C_INFO));

    event_311C.eventCommInfo.eventOAD.nObjID= 0X311C;
    event_311C.eventCommInfo.eventOAD.value= 0X311C0200;

    OOP_RCSD_T eventRCSD={
            8, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
                    
                    {0,{{0x330F0206},}},//监控数据对象
                    {0,{{0x330F0207},}},//变化前数据
                    {0,{{0x330F0208},}},//变化后数据
                }
        };
    
    event_311C.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_311C.eventCommInfo);

    //关联任务号
    GetEventNomalParam(gDbCenterClient, 0, 0, 0x311c0600);
    event_311C.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3140Para
* @brief     : 安全事件变更记录
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int Init3140Para()
{
    memset(&event_3140, 0, sizeof(EVENT_3140_INFO));

    event_3140.eventCommInfo.eventOAD.nObjID= 0X3140;
    event_3140.eventCommInfo.eventOAD.value= 0X31400200;

    OOP_RCSD_T eventRCSD={
            21, 0,{
                    {0,{{0x20220200},}},//事件记录序号
                    {0,{{0x201E0200},}},//事件发生时间
                    {0,{{0x20200200},}},//事件结束时间
                    {0,{{0x20240200},}},//事件发生源
                    {0,{{0x33000200},}},//通道上报状态
                    {0,{{0x33160206},}},//异常标志
                    {0,{{0x33160207},}},//以太网端口开放信息
                    {0,{{0x33160208},}},//以太网外联信息
                    {0,{{0x33160209},}},//远程通信模块端口开放信息
                    {0,{{0x3316020A},}},//远程通信模块外联信息
                    {0,{{0x3316020B},}},//USB设备接入信息
                    {0,{{0x3316020C},}},//本地维护端口信息
                    {0,{{0x3316020D},}},//关键文件目录变更信息
                    {0,{{0x3316020E},}},//以太网登录控制台信息
                    {0,{{0x3316020F},}},//串口登录控制台信息
                    {0,{{0x33160210},}},//非法操作命令信息
                    {0,{{0x33160211},}},//SSH服务异常类别
                    {0,{{0x33160212},}},//流量会话异常信息
                    {0,{{0x33160213},}},//流量带宽异常信息
                    {0,{{0x33160214},}},//进程异常信息
                    {0,{{0x33160215},}},//用户登录异常信息

                }
        };
    
    event_3140.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3140.eventCommInfo);

    return ERR_OK;
}

/**********************************************************************
* @name      : Init3115Para
* @brief     : 遥控跳闸事件参数初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
int Init3115Para()
{
    memset(&event_3115, 0, sizeof(EVENT_3115_INFO));

    event_3115.eventCommInfo.eventOAD.nObjID= 0X3115;
    event_3115.eventCommInfo.eventOAD.value= 0X31150200;
    event_3115.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr3115;
    event_3115.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr3115;
    event_3115.eventCommInfo.EventFlagAddr = EventStoragTmRecAddr3115;
    event_3115.eventCommInfo.EventFlagLen = EventStoragMntRecLen;

    OOP_RCSD_T eventRCSD={
            8, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态

					{0,{{0X330A0206},}},//遥控命令类型
                    {0,{{0X330A0207},}},//跳闸后2分钟功率
                    {0,{{0X330A0208},}},//跳闸时2分钟功率
                }
        };
    
    event_3115.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3115.eventCommInfo);

    event_3115.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3117Para
* @brief     : 输出回路接入状态变位事件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
int Init3117Para()
{
    memset(&event_3117, 0, sizeof(EVENT_31XX_INFO));

    event_3117.eventCommInfo.eventOAD.nObjID= 0X3117;
    event_3117.eventCommInfo.eventOAD.value= 0X31170200;

    OOP_RCSD_T eventRCSD={
            6, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
                    
                    {0,{{0xf2050200},}},//继电器输出
                }
        };
    
    event_3117.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3117.eventCommInfo);

    event_3117.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3200Para
* @brief     : 功控跳闸事件参数初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
int Init3200Para()
{
    memset(&event_3200, 0, sizeof(EVENT_3200_INFO));

    event_3200.eventCommInfo.eventOAD.nObjID= 0X3200;
    event_3200.eventCommInfo.eventOAD.value= 0X32000200;
    event_3200.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr3200;
    event_3200.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr3200;
    event_3200.eventCommInfo.EventFlagAddr = EventStoragTmRecAddr3200;
    event_3200.eventCommInfo.EventFlagLen = EventStoragMntRecLen;

    OOP_RCSD_T eventRCSD={
            10, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
                    
                    {0,{{0x33050206},}},//事件发生后2分钟功率long64
                    {0,{{0x810E0200},}},//控制对象
                    {0,{{0x810F0200},}},//跳闸轮次
                    {0,{{0x33050209},}},//功控定值long64(无OAD)
                    {0,{{0x3305020A},}},//跳闸发生前总加有功功率long64
                }
        };
    
    event_3200.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3200.eventCommInfo);

    event_3200.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3201Para
* @brief     : 电控跳闸事件参数初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
int Init3201Para()
{
    memset(&event_3201, 0, sizeof(EVENT_3201_INFO));

    event_3201.eventCommInfo.eventOAD.nObjID= 0X3201;
    event_3201.eventCommInfo.eventOAD.value= 0X32010200;

    OOP_RCSD_T eventRCSD={
            9, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
                    
                    {0,{{0x810E0200},}},//控制对象
                    {0,{{0x810F0200},}},//跳闸轮次
                    {0,{{0x81100200},}},//电控定值
                    {0,{{0x33060209},}},//跳闸发生时总加总加电能量long64
                }
        };
    
    event_3201.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3201.eventCommInfo);

    event_3201.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3202Para
* @brief     : 购电参数设置事件参数初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
int Init3202Para()
{
    memset(&event_3202, 0, sizeof(EVENT_3202_INFO));

    event_3202.eventCommInfo.eventOAD.nObjID= 0X3202;
    event_3202.eventCommInfo.eventOAD.value= 0X32020200;

    OOP_RCSD_T eventRCSD={
            6, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
                    
                    {0,{{0X810C2200},}},//事件发生前购电控配置单元
                }
        };
    
    event_3202.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3202.eventCommInfo);

    event_3202.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3203Para
* @brief     : 电控跳闸事件参数初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
int Init3203Para()
{
    memset(&event_3203, 0, sizeof(EVENT_3203_INFO));

    event_3203.eventCommInfo.eventOAD.nObjID= 0X3203;
    event_3203.eventCommInfo.eventOAD.value= 0X32030200;

    OOP_RCSD_T eventRCSD={
            7, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
                    
                    {0,{{0x810E0200},}},//控制对象
                    {0,{{0x81100200},}},//电控定值
                }
        };
    
    event_3203.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3203.eventCommInfo);

    event_3203.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3600Para
* @brief     : 系统监测事件参数初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
int Init3600Para()
{
    memset(&event_3600, 0, sizeof(EVENT_360X_INFO));

    event_3600.eventCommInfo.eventOAD.nObjID= 0X3600;
    event_3600.eventCommInfo.eventOAD.value= 0X36000200;
	event_3600.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr3600;
    event_3600.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr3600;
	event_3600.eventCommInfo.EventStoragIndexAddr = EventStoragIndex3600;

    OOP_RCSD_T eventRCSD={
            11, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
                    
                    {0,{{0x33270206},}},//告警类别
				    {0,{{0x33270207},}},//实际值
				    {0,{{0x33270208},}},//设置阈值
				    {0,{{0x33270209},}},//容器名称
				    {0,{{0x3327020a},}},//应用名称
				    {0,{{0x3327020b},}},//服务名称
                }
        };
    
    event_3600.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3600.eventCommInfo);

    event_3600.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3600Para
* @brief     : 系统监测事件参数初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
int Init3601Para()
{
    memset(&event_3601, 0, sizeof(EVENT_360X_INFO));

    event_3601.eventCommInfo.eventOAD.nObjID= 0X3601;
    event_3601.eventCommInfo.eventOAD.value= 0X36010200;
	event_3601.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr3601;
    event_3601.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr3601;
	
	event_3601.eventCommInfo.EventStoragIndexAddr = EventStoragIndex3601;

    OOP_RCSD_T eventRCSD={
            11, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
                    
                    {0,{{0x33270206},}},//告警类别
				    {0,{{0x33270207},}},//实际值
				    {0,{{0x33270208},}},//设置阈值
				    {0,{{0x33270209},}},//容器名称
				    {0,{{0x3327020a},}},//应用名称
				    {0,{{0x3327020b},}},//服务名称
                }
        };
    
    event_3601.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3601.eventCommInfo);

    event_3601.initflag = 1;
    return ERR_OK;
}

/**********************************************************************
* @name      : Init3600Para
* @brief     : 系统监测事件参数初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
int Init3602Para()
{
    memset(&event_3602, 0, sizeof(EVENT_360X_INFO));

    event_3602.eventCommInfo.eventOAD.nObjID= 0X3602;
    event_3602.eventCommInfo.eventOAD.value= 0X36020200;
	event_3602.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr3602;
    event_3602.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr3602;
	
	event_3602.eventCommInfo.EventStoragIndexAddr = EventStoragIndex3602;

    OOP_RCSD_T eventRCSD={
            11, 0,{
                    {0,{{0X20220200},}},//事件记录序号
                    {0,{{0X201E0200},}},//事件发生时间
                    {0,{{0X20200200},}},//事件结束时间
                    {0,{{0X20240200},}},//事件发生源
                    {0,{{0X33000200},}},//通道上报状态
                    
                    {0,{{0x33270206},}},//告警类别
				    {0,{{0x33270207},}},//实际值
				    {0,{{0x33270208},}},//设置阈值
				    {0,{{0x33270209},}},//容器名称
				    {0,{{0x3327020a},}},//应用名称
				    {0,{{0x3327020b},}},//服务名称
                }
        };
    
    event_3602.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    IniEventCommInfo(&event_3602.eventCommInfo);

    event_3602.initflag = 1;
    return ERR_OK;
}



//事件参数初始化及清除列表
EVENT_MODULE_T g_event_modul_list[] = 
{
    {&event_3100.eventCommInfo, Init3100Para, CleanEventCommInfo},
    {&event_3101.eventCommInfo, Init3101Para, CleanEventCommInfo},
    {&event_3104.eventCommInfo, Init3104Para, CleanEventCommInfo},
    {&event_3105.eventCommInfo, Init3105Para, CleanEventCommInfo},
    {&event_3106.eventCommInfo, Init3106Para, CleanEventCommInfo},
    {&event_3109.eventCommInfo, Init3109Para, CleanEventCommInfo},
    {&event_300F.eventCommInfo, Init300FPara, CleanEventCommInfo},
    {&event_310A.eventCommInfo, Init310APara, CleanEventCommInfo},
    {&event_310B.eventCommInfo, Init310BPara, CleanEventCommInfo},
    {&event_310C.eventCommInfo, Init310CPara, CleanEventCommInfo},
    {&event_310D.eventCommInfo, Init310DPara, CleanEventCommInfo},
    {&event_310E.eventCommInfo, Init310EPara, CleanEventCommInfo},
    {&event_310F.eventCommInfo, Init310FPara, CleanEventCommInfo},
    {&event_3110.eventCommInfo, Init3110Para, CleanEventCommInfo},
    {&event_3111.eventCommInfo, Init3111Para, CleanEventCommInfo},
    {&event_3114.eventCommInfo, Init3114Para, CleanEventCommInfo}, 
    {&event_3117.eventCommInfo, Init3117Para, CleanEventCommInfo}, 
    {&event_3118.eventCommInfo, Init3118Para, CleanEventCommInfo},  
    {&event_311C.eventCommInfo, Init311CPara, CleanEventCommInfo}, 
    {&event_3140.eventCommInfo, Init3140Para, CleanEventCommInfo},
    {&event_3115.eventCommInfo, Init3115Para, CleanEventCommInfo},
    {&event_3200.eventCommInfo, Init3200Para, CleanEventCommInfo},
    {&event_3201.eventCommInfo, Init3201Para, CleanEventCommInfo},
    {&event_3202.eventCommInfo, Init3202Para, CleanEventCommInfo},
    {&event_3203.eventCommInfo, Init3203Para, CleanEventCommInfo},
    {&event_311B.eventCommInfo, Init311BPara, CleanEventCommInfo},
    {&event_311F.eventCommInfo, Init311FPara, CleanEventCommInfo},
    {&event_3112.eventCommInfo, Init3112Para, CleanEventCommInfo},  
    {&event_3030.eventCommInfo, Init3030Para, CleanEventCommInfo},
    {&event_3E03.eventCommInfo, Init3E03Para, CleanEventCommInfo},
    {&event_3600.eventCommInfo, Init3600Para, CleanEventCommInfo},
    {&event_3601.eventCommInfo, Init3601Para, CleanEventCommInfo},
    {&event_3602.eventCommInfo, Init3602Para, CleanEventCommInfo},
#ifdef AREA_HUNAN
    {&event_3360.eventCommInfo, Init3360Para, CleanEventCommInfo},
    {&event_3361.eventCommInfo, Init3361Para, CleanEventCommInfo},
    {&event_3362.eventCommInfo, Init3362Para, CleanEventCommInfo},
    {&event_3363.eventCommInfo, Init3363Para, CleanEventCommInfo},
#endif
#ifdef AREA_SHAANXI
	{&event_3199.eventCommInfo, Init3199Para, CleanEventCommInfo},
#endif

};

uint16 g_event_modul_num = sizeof(g_event_modul_list)/sizeof(EVENT_MODULE_T);

/**********************************************************************
* @name      : InitEventPara
* @brief     ：事件参数初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-14
* @Update    :
**********************************************************************/
void InitEventPara(void)
{
    int         ret     = -1;
    int         i       = 0;
    EVENT_FMT_DEBUG("Start InitEventPara.......\n");

    memset(&g_tnmPort, 0, sizeof(OOP_CHANNEL_T));
    memset(&tmnAddr, 0, sizeof(OOP_OCTETVAR16_T));
    memset(&MAC, 0, sizeof(OOP_OCTETVAR64_T));

    if ((ret = reg_pdata_xram(ADV_APP_NAME, "^Event")) != 0)
    {
        EVENT_FMT_DEBUG("event_init reg error1 ret=%d\n", ret);
    }

    gDbCenterClient  = db_client_create(DB_YC_AGENT, DB_CLIENT_EVENT);
    if(gDbCenterClient <= 0)
    {
        EVENT_FMT_DEBUG("gDbCenterClient创建失败, 返回! \n");
        return ;        
    }
    
    readMntAddr(gDbCenterClient);

    GetEventNomalParam(gDbCenterClient, 0, 0, 0x43000800);
    GetEventNomalParam(gDbCenterClient, 0, 0, 0x43000a00);
    GetEventNomalParam(gDbCenterClient, 1, 0, 0x20000200);
    GetEventNomalParam(gDbCenterClient, 1, 0, 0x20140200);
    GetEventNomalParam(gDbCenterClient, 1, 0, 0x810c0200);

#ifdef AREA_FUJIAN
	gGather = OOP_GATHER_FJ;		//没有配置，默认福建模式

	GetEventNomalParam(gDbCenterClient, 0, 0, 0x6E000200); //外部设备数据采集模式
	GetEventNomalParam(gDbCenterClient, 0, 0, 0x6E310300); //透明事件当前状态
#endif

	for(i=0; i<g_event_modul_num; i++)
    {
        g_event_modul_list[i].initFp();
    }

    EVENT_FMT_TRACE("g_tnmPort num %u port1=%08X\n", g_tnmPort.nNum, g_tnmPort.port[0].value);

    return;
}

#endif


#if DESC("事件关联普通型数据更新",1)

//普通型数据更新列表
EventUpdateDataInfo g_EventUpdateNormalDataList[] = 
{/*     数据标识,   数据长度,     数据内容 */
    {0x31000800,    sizeof(uint8),                  &event_3100.eventCommInfo.reportflag},
    {0x31000900,    sizeof(uint8),                  &event_3100.eventCommInfo.checkflag},
    {0x31010800,    sizeof(uint8),                  &event_3101.eventCommInfo.reportflag},
    {0x31010900,    sizeof(uint8),                  &event_3101.eventCommInfo.checkflag},
    {0xf2030200,    sizeof(OOP_SWITCHIN_STATE_T),   (uint8 *)&event_3104.switchState},
    {0xf2030400,    sizeof(OOP_SWITCHIN_FLAG_T),    (uint8 *)&event_3104.switchFlag},
    {0x31040800,    sizeof(uint8),                  &event_3104.eventCommInfo.reportflag},
    {0x31040900,    sizeof(uint8),                  &event_3104.eventCommInfo.checkflag},
    {0x31050600,    sizeof(OOP_EVTCFG_3105_T),      (uint8 *)&event_3105.checkPara},
    {0x31050800,    sizeof(uint8),                  &event_3105.eventCommInfo.reportflag},
    {0x31050900,    sizeof(uint8),                  &event_3105.eventCommInfo.checkflag},
    {0x20000200,    sizeof(OOP_WORD3V_T),          (uint8 *)&tmnVoltage},
    {0x31060600,    sizeof(OOP_POWERONCUT_T),       (uint8 *)&event_3106.eventParam},
    {0x31060800,    sizeof(uint8),                  &event_3106.eventCommInfo.reportflag},
    {0x31060900,    sizeof(uint8),                  &event_3106.eventCommInfo.checkflag},
    {0x310a0800,    sizeof(uint8),                  &event_310A.eventCommInfo.reportflag},
    {0x310a0900,    sizeof(uint8),                  &event_310A.eventCommInfo.checkflag},
    {0x310b0600,    sizeof(uint8),                  &event_310B.nTID},
    {0x310b0800,    sizeof(uint8),                  &event_310B.eventCommInfo.reportflag},
    {0x310b0900,    sizeof(uint8),                  &event_310B.eventCommInfo.checkflag},
    {0x310c0600,    sizeof(OOP_EVTCFG_310X_T),      (uint8 *)&event_310C.eventParam},
    {0x310c0800,    sizeof(uint8),                  &event_310C.eventCommInfo.reportflag},
    {0x310c0900,    sizeof(uint8),                  &event_310C.eventCommInfo.checkflag},
    {0x310d0600,    sizeof(OOP_EVTCFG_310X_T),      (uint8 *)&event_310D.eventParam},
    {0x310d0800,    sizeof(uint8),                  &event_310D.eventCommInfo.reportflag},
    {0x310d0900,    sizeof(uint8),                  &event_310D.eventCommInfo.checkflag},
    {0x310e0600,    sizeof(OOP_EVTCFG_310E_T),      (uint8 *)&event_310E.event310eParam},
    {0x310e0800,    sizeof(uint8),                  &event_310E.eventCommInfo.reportflag},
    {0x310e0900,    sizeof(uint8),                  &event_310E.eventCommInfo.checkflag},
    {0x310f0600,    sizeof(OOP_EVTCFG_310F_T),      (uint8 *)&event_310F.eventParam},
    {0x310f0800,    sizeof(uint8),                  &event_310F.eventCommInfo.reportflag},
    {0x310f0900,    sizeof(uint8),                  &event_310F.eventCommInfo.checkflag},
    {0x22000200,    sizeof(OOP_DWORD2_T),           NULL,   NULL,evt_check_3110},
    {0x31100600,    sizeof(OOP_EVTCFG_3110_T),      (uint8 *)&event_3110.eventParam},
    {0x31100800,    sizeof(uint8),                  &event_3110.eventCommInfo.reportflag},
    {0x31100900,    sizeof(uint8),                  &event_3110.eventCommInfo.checkflag},
    {0x60020A00,    sizeof(uint8),                  &event_3111.SerchStat},
    {0x31110800,    sizeof(uint8),                  &event_3111.eventCommInfo.reportflag},
    {0x31110900,    sizeof(uint8),                  &event_3111.eventCommInfo.checkflag},
    {0x31140800,    sizeof(uint8),                  &event_3114.eventCommInfo.reportflag},
    {0x31140900,    sizeof(uint8),                  &event_3114.eventCommInfo.checkflag},
    {0x31170800,    sizeof(uint8),                  &event_3117.eventCommInfo.reportflag},
    {0x31170900,    sizeof(uint8),                  &event_3117.eventCommInfo.checkflag},
    {0x31180800,    sizeof(uint8),                  &event_3118.eventCommInfo.reportflag},
    {0x31180900,    sizeof(uint8),                  &event_3118.eventCommInfo.checkflag},
    {0x31190800,    sizeof(uint8),                  &event_3119.eventCommInfo.reportflag},
    {0x31190900,    sizeof(uint8),                  &event_3119.eventCommInfo.checkflag},
    {0x43000800,    sizeof(BOOLEAN),                (uint8 *)&g_tnmEventReport}, 
    {0x43000a00,    sizeof(OOP_CHANNEL_T),          (uint8 *)&g_tnmPort}, 
    {0x300f0600,    sizeof(uint8),                  &event_300F.nDelay},
    {0x300f0800,    sizeof(uint8),                  &event_300F.eventCommInfo.reportflag},
    {0x300f0900,    sizeof(uint8),                  &event_300F.eventCommInfo.checkflag},
    {0x20140200,    sizeof(OOP_METWORD_T),          (uint8 *)&event_300F.meterRsw},
    {0x311c0600,    sizeof(uint8),                  &event_311C.nTID},
    {0x311c0800,    sizeof(uint8),                  &event_311C.eventCommInfo.reportflag},
    {0x311c0900,    sizeof(uint8),                  &event_311C.eventCommInfo.checkflag},
    {0x31400800,    sizeof(uint8),                  &event_3140.eventCommInfo.reportflag},
    {0x31400900,    sizeof(uint8),                  &event_3140.eventCommInfo.checkflag},
    {0x31150800,    sizeof(uint8),                  &event_3115.eventCommInfo.reportflag},
    {0x31150900,    sizeof(uint8),                  &event_3115.eventCommInfo.checkflag},
    {0x32000800,    sizeof(uint8),                  &event_3200.eventCommInfo.reportflag},
    {0x32000900,    sizeof(uint8),                  &event_3200.eventCommInfo.checkflag},
    {0x32010800,    sizeof(uint8),                  &event_3201.eventCommInfo.reportflag},
    {0x32010900,    sizeof(uint8),                  &event_3201.eventCommInfo.checkflag},
    {0x32020800,    sizeof(uint8),                  &event_3202.eventCommInfo.reportflag},
    {0x32020900,    sizeof(uint8),                  &event_3202.eventCommInfo.checkflag},
//    {0x810c0200,    sizeof(OOP_CFGUNIT_810C_T),     (uint8 *)&event_3202.cfgunit},
    {0x32030800,    sizeof(uint8),                  &event_3203.eventCommInfo.reportflag},
    {0x32030900,    sizeof(uint8),                  &event_3203.eventCommInfo.checkflag},
    {0x31090800,    sizeof(uint8),                  &event_3109.eventCommInfo.reportflag},
    {0x31090900,    sizeof(uint8),                  &event_3109.eventCommInfo.checkflag},
    {0x311B0800,    sizeof(uint8),                  &event_311B.eventCommInfo.reportflag},
    {0x311B0900,    sizeof(uint8),                  &event_311B.eventCommInfo.checkflag},
    {0x311F0800,    sizeof(uint8),                  &event_311F.eventCommInfo.reportflag},
    {0x311F0900,    sizeof(uint8),                  &event_311F.eventCommInfo.checkflag},
    {0x31120800,    sizeof(uint8),                  &event_3112.eventCommInfo.reportflag},
    {0x31120900,    sizeof(uint8),                  &event_3112.eventCommInfo.checkflag},
    {0x30300800,    sizeof(uint8),                  &event_3030.eventCommInfo.reportflag},
    {0x30300900,    sizeof(uint8),                  &event_3030.eventCommInfo.checkflag},
    {0x3E030800,    sizeof(uint8),                  &event_3E03.eventCommInfo.reportflag},
    {0x3E030900,    sizeof(uint8),                  &event_3E03.eventCommInfo.checkflag},
#ifdef AREA_HUNAN
    {0x33600800,    sizeof(uint8),                  &event_3360.eventCommInfo.reportflag},
    {0x33600900,    sizeof(uint8),                  &event_3360.eventCommInfo.checkflag},
    {0x33610600,    sizeof(OOP_TemperatureWarningParam_T),   (uint8 *)&event_3361.param},
    {0x33610800,    sizeof(uint8),                  &event_3361.eventCommInfo.reportflag},
    {0x33610900,    sizeof(uint8),                  &event_3361.eventCommInfo.checkflag},
    {0x33620800,    sizeof(uint8),                  &event_3362.eventCommInfo.reportflag},
    {0x33620900,    sizeof(uint8),                  &event_3362.eventCommInfo.checkflag},
    {0x33630800,    sizeof(uint8),                  &event_3363.eventCommInfo.reportflag},
    {0x33630900,    sizeof(uint8),                  &event_3363.eventCommInfo.checkflag},
#endif
#ifdef AREA_SHAANXI
	{0x31990800,	sizeof(uint8),					&event_3199.eventCommInfo.reportflag},
	{0x31190900,	sizeof(uint8),					&event_3199.eventCommInfo.checkflag},
	
//	{0x45000700,	sizeof(OOP_VISIBLEVAR32_T),		(uint8 *)&event_3199.iccidBef},
#endif
	{0x36000800,	sizeof(uint8),					&event_3600.eventCommInfo.reportflag},
	{0x36000900,	sizeof(uint8),					&event_3600.eventCommInfo.checkflag},
	{0x36010800,	sizeof(uint8),					&event_3601.eventCommInfo.reportflag},
	{0x36010900,	sizeof(uint8),					&event_3601.eventCommInfo.checkflag},
	{0x36020800,	sizeof(uint8),					&event_3602.eventCommInfo.reportflag},
	{0x36020900,	sizeof(uint8),					&event_3602.eventCommInfo.checkflag},

#ifdef AREA_FUJIAN
	//外部设备数据采集模式
	{0x6E000200,	sizeof(uint8),			(uint8 *)&gGather},
	//透明事件当前状态
	{0x6E310300,	sizeof(OOP_TRANS_EVENT_STATUS_T),(uint8 *)&gTransEvtStatus},
#endif

};

uint16 g_EventUpdateNormalDataNum = sizeof(g_EventUpdateNormalDataList)/sizeof(EventUpdateDataInfo);


/**********************************************************************
* @name      : GetEventNomalParam
* @brief     ：事件数据变更
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-11
* @Update    :
**********************************************************************/
int GetEventNomalParam(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                i = 0;
    int                ret = 0;
    uint32             readlen = 0;
    uint8              pDataBuf[1024] = {0};
    NOMAL_OAD_T        nomalOAD;
    OOP_METER_T        oopMeter;

    memset(&nomalOAD, 0, sizeof(NOMAL_OAD_T));

    nomalOAD.logicId = logicId;
    nomalOAD.infoNum = infoNum;
    nomalOAD.oad.value = value;

    for(i=0; i<g_EventUpdateNormalDataNum; i++)
    {
        if(g_EventUpdateNormalDataList[i].oadValue == value)
        {
            readlen = g_EventUpdateNormalDataList[i].dateLen;
            ret = evt_read_nomal_data(clientid, logicId, infoNum, value, readlen, pDataBuf);
            if (ret != ERR_OK )
            {
                return ret;
            }
            
            if(g_EventUpdateNormalDataList[i].pBuf != NULL)
            {
                memcpy(g_EventUpdateNormalDataList[i].pBuf, pDataBuf, readlen);
            }
                
            EVENT_BUF_TRACE(pDataBuf, readlen, "读取数据 logicId=%u, infoNum=%u, OAD=%08X OK! buf:", logicId, infoNum, value);

            if(g_EventUpdateNormalDataList[i].func != NULL)
            {
                g_EventUpdateNormalDataList[i].func(clientid, logicId, infoNum, oopMeter, pDataBuf, readlen);
            }
            return 0;
        }
        
    }
    
    return -1;
}


#endif

#if DESC("事件关联记录型数据更新",1)

//最新抄表数据
LatestMeterReadData g_LatestMeterReadData;

//记录型数据更新列表
EventUpdateDataInfo g_EventUpdateRecordDataList[] = 
{/*     数据标识,   数据长度,     数据内容 */
    {0x40000200,    sizeof(OOP_DATETIME_S_T),    (uint8*)g_LatestMeterReadData.meterReadTime,   g_LatestMeterReadData.time_vaild, evt_check_3105},
    {0x20010200,    sizeof(OOP_INT3V_T),         (uint8*)g_LatestMeterReadData.curTemp,         g_LatestMeterReadData.cur_vaild,  NULL},
    {0x20000200,    sizeof(OOP_WORD3V_T),        (uint8*)g_LatestMeterReadData.valTemp,         g_LatestMeterReadData.val_vaild,  NULL},    
    {0x00100200,    sizeof(OOP_ENERGY_T),        (uint8*)g_LatestMeterReadData.forwardActive,   g_LatestMeterReadData.Active_vaild, evt_check_310X},
    {0x00300200,    sizeof(OOP_ENERGYS_T),       (uint8*)g_LatestMeterReadData.EnergyS,         g_LatestMeterReadData.EnergyS_vaild,NULL},
#ifdef AREA_HUNAN
//    {0x28000200,    sizeof(uint8),               NULL, NULL,CheckERC_3360}, 
    {0x26000200,    sizeof(OOP_LONG3V_T),        NULL, NULL,CheckERC_3361}, 
//    {0x2B010200,    sizeof(uint8),               NULL, NULL,CheckERC_3362}, 
//    {0x2B020200,    sizeof(uint8),               NULL, NULL,CheckERC_3363}, 
#endif
};

uint16 g_EventUpdateRecordDataNum = sizeof(g_EventUpdateRecordDataList)/sizeof(EventUpdateDataInfo);


/**********************************************************************
* @name      : InitLatestMeterReadDataLock
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
void InitLatestMeterReadDataLock(void)
{
    static int firstFlag = 0;
    /* 初始化锁 */
    if(firstFlag == 0)
    {
        firstFlag = 1;
        memset(&g_LatestMeterReadData, 0, sizeof(LatestMeterReadData));
        pthread_mutex_init(&g_LatestMeterReadData.mutex, NULL);
    }
}

/**
*********************************************************************
* @name：      InitLatestMeterReadDataList
* @brief：     最新实时抄表数据初始化
* @param[in] ：clientid  数据库句柄
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
*********************************************************************
*/
void InitLatestMeterReadDataList(int clientid)
{   
    int                 ret         = 0;
    int                 i           = 0;
    int                 offset      = 0;
    time_t              timep       = 0;
    mtrnum_t            mtr_id      = 0;      
    READ_RECORD_T       startReadRecord;
    RECORD_UNIT_T       stepRecordOut;
    OOP_METER_T         oopMeter;
    
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&startReadRecord,0,sizeof(READ_RECORD_T));
    memset(&stepRecordOut,0,sizeof(RECORD_UNIT_T));   
    memset(&g_LatestMeterReadData, 0, sizeof(LatestMeterReadData));

    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep
    startReadRecord.recordOAD.road.oadCols.nNum = 1;    
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.cType = 0x02;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = timep + COMPENSATION_TIME;;
    startReadRecord.sortType = 1;
    startReadRecord.MAC.nNum = 6;

//    EVENT_FMT_DEBUG("Initializing latest meter reading data. please wait......\n");
    for (i = 0; i < g_EventUpdateRecordDataNum; i++)
    {
        if (g_EventUpdateRecordDataList[i].pBuf == NULL)
        {
            continue;
        }
        
        startReadRecord.recordOAD.road.oadCols.oad[0].value = g_EventUpdateRecordDataList[i].oadValue;
        for (mtr_id = 1; mtr_id <= MTR485_MAX_NUM; mtr_id++)
        {
            if(app_get_meter_list(mtr_id, &oopMeter) == ERR_OK)
            {
                startReadRecord.recordOAD.infoNum = mtr_id;
                memcpy(startReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
                
                ret = db_read_record(clientid, &startReadRecord, g_EventUpdateRecordDataList[i].dateLen, &stepRecordOut);
                if (ret != ERR_OK)
                {
                    //如果没有读取到电表抄表标志，直接返回
                    if(ret != ERR_NORESOURCE)
                    {
                        EVENT_FMT_DEBUG("db_read_record infoNum=%u oad=%08X error ret%d\n", mtr_id, g_EventUpdateRecordDataList[i].oadValue, ret);
                    }
                    continue;
                }
                else
                {   
                    if(stepRecordOut.record[0].outDatalen == g_EventUpdateRecordDataList[i].dateLen)
                    {
                        offset = g_EventUpdateRecordDataList[i].dateLen*(mtr_id - 1);
                        memcpy(g_EventUpdateRecordDataList[i].pBuf + offset, stepRecordOut.record[0].outDataBuf, g_EventUpdateRecordDataList[i].dateLen);

                        g_EventUpdateRecordDataList[i].pvaild[mtr_id-1] = 1;
                    }
                }
                
            }
                
        }
    }
    EVENT_FMT_DEBUG("Initializing latest meter reading data end.\n");
    return;
}

extern int rpt_term_evt_update(int clientid, uint8 logicId, uint16 infoNum, uint32 oadMain, uint8 oadColsNum, uint32* oadCols);

/**********************************************************************
* @name      : UpdateLatestMeterReadDataList
* @brief     ：更新最新实时抄表数据到缓存表
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
int UpdateLatestMeterReadDataList(int clientid, uint8 logicId, uint16 infoNum, uint32 oadMain, uint8 oadColsNum, uint32* oadCols)
{
    int                 ret         = 0;
    int                 i           = 0;
    int                 j           = 0;
    int                 offset      = 0;
    time_t              timep       = 0;
    READ_RECORD_T       startReadRecord;
    RECORD_UNIT_T       stepRecordOut;
    OOP_METER_T         oopMeter;

	//TODO 所有事件都采用该机制上报后，则使用表格和掩码
    if((oadColsNum == 1) && ((oadCols[0] == 0x31200200) || (oadCols[0] == 0x302A0200) ))
    {
        return rpt_term_evt_update(clientid, logicId, infoNum, oadMain, oadColsNum, oadCols);
    }
    
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&startReadRecord,0,sizeof(READ_RECORD_T));
    memset(&stepRecordOut,0,sizeof(RECORD_UNIT_T));
    
    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep
    startReadRecord.recordOAD.optional = 0;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.logicId = logicId;
    startReadRecord.recordOAD.infoNum = infoNum;
    startReadRecord.cType = 0x00;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = timep + COMPENSATION_TIME+1;
    startReadRecord.sortType = 1;
    startReadRecord.MAC.nNum = 6;
    
    if(app_get_meter_list(infoNum, &oopMeter) == ERR_OK)
    {
        memcpy(startReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
        for(j = 0; j < oadColsNum; j++)
        {
            startReadRecord.recordOAD.road.oadCols.oad[0].value = oadCols[j];
            for (i = 0; i < g_EventUpdateRecordDataNum; i++)
            {
                if(g_EventUpdateRecordDataList[i].oadValue == oadCols[j])
                {
                    ret = db_read_record(clientid, &startReadRecord, g_EventUpdateRecordDataList[i].dateLen, &stepRecordOut);
                    if (ret != ERR_OK)
                    {
                        //如果没有读取到电表抄表标志，直接返回
                        EVENT_FMT_DEBUG("db_read_record infoNum=%u oad=%08X error ret%d\n", infoNum, g_EventUpdateRecordDataList[i].oadValue, ret);
                        if(NULL!=g_EventUpdateRecordDataList[i].pvaild)
                            g_EventUpdateRecordDataList[i].pvaild[infoNum-1] = 0;
                        return -1;
                    }
                   
                    /* 使用互斥量进行加锁 */
                    PTHREAD_LOCK_SAFE(&g_LatestMeterReadData.mutex);
                    
                    if(stepRecordOut.record[0].outDatalen == g_EventUpdateRecordDataList[i].dateLen && infoNum <= NO_VIR_MTR_MAX_NUM)
                    {
                        if(NULL!=g_EventUpdateRecordDataList[i].pvaild&&NULL!=g_EventUpdateRecordDataList[i].pBuf)
                        {
                            offset = g_EventUpdateRecordDataList[i].dateLen*(infoNum - 1);
                            memcpy(g_EventUpdateRecordDataList[i].pBuf + offset, stepRecordOut.record[0].outDataBuf, g_EventUpdateRecordDataList[i].dateLen);

                            g_EventUpdateRecordDataList[i].pvaild[infoNum-1] = 1;
                        }
                    }
                    /* 使用互斥量进行解锁 */
                    PTHREAD_UNLOCK_SAFE(&g_LatestMeterReadData.mutex);
                    //估计记录型数据变更事件判断是否生成电表事件
                    if(NULL!=g_EventUpdateRecordDataList[i].func)
                    {
                        EVENT_FMT_TRACE("Updata record logicId=%u, infoNum=%u, OAD=0X%08X  check event  !\n", logicId, infoNum, oadCols[j]);
                        if(g_EventUpdateRecordDataList[i].oadValue == 0x40000200)
                        {
                            g_EventUpdateRecordDataList[i].func(clientid,logicId,infoNum,oopMeter,(uint8*)&stepRecordOut, (uint16)sizeof(RECORD_UNIT_T));
                    
                        }
                        else
                            g_EventUpdateRecordDataList[i].func(clientid,logicId,infoNum,oopMeter,stepRecordOut.record[0].outDataBuf,g_EventUpdateRecordDataList[i].dateLen);
                    }
                    EVENT_BUF_TRACE(stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen, "Updata record logicId=%u, infoNum=%u, OAD=0X%08X  OK !\n", logicId, infoNum, oadCols[j]);
                    return 0;
                }
                
            }
        }
    }
   
   return -1;
}

/**********************************************************************
* @name      : get_meter_data_by_oad
* @brief     ：根据oad从最新实时抄表数据缓存表取数据
* @param[in] ：int clientid    socket句柄
               uint16 infoNum  信息点号
               uint32 oadvalue    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-2-27
* @Update    :
**********************************************************************/
int get_meter_data_by_oad(int clientid, uint16 infoNum, uint32 oadvalue, uint8 *poutbuf, uint32 max_len)
{
    int    i           = 0;
    int    offset      = 0;
    uint32 datalen    = 0;

    if(infoNum > NO_VIR_MTR_MAX_NUM)
    {
        EVENT_FMT_TRACE("只处理485数据, infoNum[%u]>%u!\n", infoNum, NO_VIR_MTR_MAX_NUM);
        return 0;
    }

    for (i = 0; i < g_EventUpdateRecordDataNum; i++)
    {
        if(g_EventUpdateRecordDataList[i].oadValue == oadvalue)
        {
            if(g_EventUpdateRecordDataList[i].pvaild[infoNum-1] == 0)
            {
                UpdateLatestMeterReadDataList(clientid, 0, infoNum, 0, 1, &oadvalue);
            }
            
            if(g_EventUpdateRecordDataList[i].pvaild[infoNum-1] == 1)
            {
                datalen = g_EventUpdateRecordDataList[i].dateLen;
                offset = g_EventUpdateRecordDataList[i].dateLen*(infoNum - 1);
                
                if(datalen > max_len)
                {
                    EVENT_FMT_DEBUG("datalen[%d] > max_len[%d]\n", datalen, max_len);
                    return -1;
                }
                
                memcpy(poutbuf, g_EventUpdateRecordDataList[i].pBuf + offset, datalen);
                return 0;
            }

            return -1;
        }
    }
   
   return -1;
}

#endif

/**********************************************************************
* @name      : evt_init_event_deal
* @brief     ：初始化事件处理
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-14
* @Update    :
**********************************************************************/
int evt_init_event_deal(int clientid,uint8 initType, uint8 logicId,uint16 classTag)
{
    int i = 0;
    
    EVENT_FMT_DEBUG("Init event initType=%u, logicId=%u, classTag=%u!\n", initType, logicId, classTag);
    if(logicId == 0)
    {
        //清除实时数据
        if(initType == 1 || classTag == 0 || classTag == CLASS_DATA_COLLEC)
        {
            memset(&g_LatestMeterReadData, 0, sizeof(g_LatestMeterReadData));
        }
        
        //清除事件数据
        if(initType == 1 || classTag == 0 || classTag == CLASS_DATA_EVENT)
        {
            for(i=0; i<g_event_modul_num; i++)
            {
                (*g_event_modul_list[i].cleanFp)(g_event_modul_list[i].peventCommInfo);
            }

            //清除事件状态
            CleanEventStatusFlag();
        }

        if(initType == 1 || classTag == 0)
        {
        	usleep(1000);
            evt_check_3100(clientid);
        }

        //清除参数数据
        if(initType == 1)
        {
            for(i=0; i<g_EventUpdateNormalDataNum; i++)
            {
                if(g_EventUpdateNormalDataList[i].oadValue == 0x31000900)
                {
                    continue;
                }

                if(g_EventUpdateNormalDataList[i].pBuf != NULL)
                {
                    memset(g_EventUpdateNormalDataList[i].pBuf, 0, g_EventUpdateNormalDataList[i].dateLen);
                }
            }
            set_reset_flag(0, TMN_RBT_DELAY_20S);
        }

		
    }
    else if(logicId == 1) //清除交采数据
    {
        memset(&tmnVoltage, 0, sizeof(tmnVoltage));
    }
        
    return 0;
}


/**********************************************************************
* @name      : GetTnmEventReport
* @brief     ：获取允许终端上报标志
* @param[in] ：
               
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-24
* @Update    :
**********************************************************************/
BOOLEAN GetTnmEventReport()
{
    return g_tnmEventReport;
}

#if GB101_104_EN 
uint8 bfgCommType = NOMMAL_PROT_TYPE;

/**********************************************************************
* @name      : GetCommProType
* @brief     ：获取通信规约类型-
* @param[in] ：mtrnum_t mtr_id         0标识终端，>0电能表
* @param[out]：uint8 *pbfgCommType    规约类型
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-03-02
* @Update    :
**********************************************************************/
void GetCommProType(mtrnum_t mtr_id, uint8 *pbfgCommType)
{
    OOP_METER_T oopMeter;
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    if(app_get_meter_list(mtr_id, &oopMeter) != ERR_OK)
    {
        *pbfgCommType = NOMMAL_PROT_TYPE;
        return;
    }

    *pbfgCommType = oopMeter.basic.protocol;
    return;
}

#endif

/*********************************************************************
所属功能单元：事件处理模块终端运行检测子模块
功能描述：
  完成终端初始化/版本变更、终端停/上电、遥信变位、总加组电量差动越限事件的检测功能。
输入参数：
  mtr_id  测量点号
输出参数：无
函数返回值：无
内部处理流程：
  顺序完成终端初始化/版本变更、终端停/上电、遥信变位、总加组电量差动越限事件的检测。
********************************************************************/
void RtuExam(void)
{
    evt_check_3106();       //停电时间检测
    evt_check_300F();
}

/*******************************************************************************
* 所属单元：事件处理模块用电现场检测子模块
* 功能描述：测量点告警处理接口。
* 输入参数：mtr_id  测量点号
* 输出参数：无
* 返 回 值：无
* 内部流程：根据当前告警配置信息，调用告警处理函数，处理告警数据。
*******************************************************************************/
void MetExam(mtrnum_t mtr_id, OOP_METER_T oopMeter)
{
    FillEventGlobal(mtr_id, oopMeter);                  //填充全局结构体


//  CheckERC_3105(mtr_id, oopMeter);            //电能表时间超差检测
//  CheckERC_310B(mtr_id, oopMeter);            //电能表示度下降检测
//  CheckERC_310C(mtr_id, oopMeter);            //电能表超差检测
//  CheckERC_310D(mtr_id, oopMeter);            //电能表飞走检测
//  CheckERC_310E(mtr_id, oopMeter);            //电能表停走检测
    evt_check_310X(gDbCenterClient, 0, mtr_id, oopMeter, NULL, 0); //电能表停走检测
    evt_check_310F(mtr_id, oopMeter);           //抄表失败检测
    evt_check_311C(mtr_id, oopMeter);           //电能表数据变更监控


}

/*********************************************************************
功能描述：根据搜表文件，判断是否有端口配置错误事件和采集器地址变更事件发生
输入参数：
-           无
输出参数：
-           无
函数返回值：
-           0               失败
-           1               成功
内部处理流程：
********************************************************************/
int CheckERC_3111(int fd)
{
    
    uint32              i;
    int                 Ret = -1;
    OOP_SEARCHMET_RES_T oOP_SEARCHMET_RES_T;
    OOP_EVTSOURCE_T     eventSource; //事件发生源
    OOP_EVTBUF_T        eventData;   //事件关联数据
    OOP_METER_T         oopMeter;
    
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&oOP_SEARCHMET_RES_T, 0, sizeof(OOP_SEARCHMET_RES_T));
    memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
    memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
    
    if(!event_3111.eventCommInfo.checkflag)
    {
        EVENT_FMT_LOGCC("CheckERC_3111 checkflag 0\n");
        return 0 ;
    }
/*
#ifndef AREA_ZHEJIANG
    static uint8      LastSearchFlag = 0xff;
    uint8             SerchStat;

    //搜表状态
    SerchStat = event_3111.SerchStat;
    if (SerchStat != 0xff && SerchStat != LastSearchFlag)
    {
        
        if (SerchStat != 0 || LastSearchFlag != 1)
        {
            LastSearchFlag = SerchStat;
            return 0;
        }

        LastSearchFlag = SerchStat;
    }
    else {
        return 0;
    }
#endif 
*/
    eventSource.choice = DT_NULL;
    //搜表个数
    EVENT_FMT_TRACE("搜表结束 判断发现未知电表事件 \n");


    for(i=1;i<=NO_VIR_MTR_MAX_NUM;i++)
    {
        Ret = app_get_seek_meter_list(i, &oOP_SEARCHMET_RES_T);
        if(Ret!=ERR_OK)
        {
            continue;
        }
        if(app_check_seek_meter_isreport(i)==TRUE)
        {
            EVENT_FMT_LOGCC("搜表结果 %d 已上报\n", i);
            continue;
        }
        if(check_search_meter_res(&oOP_SEARCHMET_RES_T)==TRUE)
        {
            app_set_seek_meter_isreport(i);
            EVENT_FMT_DEBUG("搜表结果 %d 存在当前档案中\n", i);
            continue;
        }
        app_set_seek_meter_isreport(i);
        event_3111.Res[event_3111.MetNum] = oOP_SEARCHMET_RES_T;
        event_3111.MetNum++;
        if(event_3111.MetNum>=OOP_MAX_SMREPORT)
        {
            //写数据库
            Ret = WriteEventToDB(fd, 0, &event_3111.eventCommInfo, eventSource, eventData, 0);
            if (Ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("发现未知电能表事件CheckERC_3111记录失败返回%d\n", Ret);
                return 0;
            }
            EVENT_FMT_LOGCC("发现未知电能表事件记录CheckERC_3111成功, storagenum=%u !\n", event_3111.eventCommInfo.storagenum);
            event_3111.MetNum =0;
            memset(event_3111.Res,0x00,sizeof(OOP_SEARCHMET_RES_T)*OOP_MAX_SMREPORT);
        }
    }
    
    if(event_3111.MetNum > 0)
    {
        //写数据库
        Ret = WriteEventToDB(fd, 0, &event_3111.eventCommInfo, eventSource, eventData, 0);
        if (Ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("发现未知电能表事件CheckERC_3111记录失败返回%d\n", Ret);
            return 0;
        }
        EVENT_FMT_LOGCC("发现未知电能表事件记录CheckERC_3111成功, storagenum=%u !\n", event_3111.eventCommInfo.storagenum);
        event_3111.MetNum =0;
        memset(event_3111.Res,0x00,sizeof(OOP_SEARCHMET_RES_T)*OOP_MAX_SMREPORT);
    }
    return 0;
  
}


#if DESC("终端初始化事件",1)
/**********************************************************************
* @name      : evt_check_3100
* @brief     ：电能表时间超差检测、记录函数
* @param[in] ：int clientid    db客户端
               uint8 logicId   逻辑设备号
               uint16 mtr_id   信息点号
               OOP_METER_T oopMeter 电表档案
               uint8* databuf       事件判断数据
               uint16 datalen       事件判断数据长度
* @param[out]：
* @return    ：0成功/<0异常
* @Create    : 梅安宁
* @Date      ：2020-12-26
* @Update    :
**********************************************************************/
void  evt_check_3100(int clientid)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
    
    memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
    memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
    
    EVENT_FMT_TRACE("evt_check_3100 starts\n");
        
    if (event_3100.eventCommInfo.checkflag)
    {
        //事件发生源NULL
        eventSource.choice = DT_NULL;
        //关联数据
        
        //写数据库
        ret = WriteEventToDB(clientid, 0, &event_3100.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("evt_check_3100失败返回%d\n", ret);
            return;
        }
        EVENT_FMT_LOGCC("evt_check_3100成功, storagenum=%u !\n", event_3100.eventCommInfo.storagenum);
        
    }
    else
        EVENT_FMT_LOGCC("evt_check_3100 checkflag 0\n");

    return;
}

#endif

#if DESC("终端版本变更事件",1)
/**********************************************************************
* @name      : evt_check_3101
* @brief     ：终端版本变更事件
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-26
* @Update    :
**********************************************************************/
void  evt_check_3101(int clientid)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    EVENT_FMT_TRACE("evt_check_3101 starts\n");
    
    if (event_3101.eventCommInfo.checkflag)                                 //需要记录
    {
        
        //事件发生源NULL
        eventSource.choice = DT_NULL;

        //事件关联数据
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X43002302;
        eventData.cols.oad[1].value = 0X43004302;
        eventData.idLen[0] = sizeof(event_3101.lastversion.szSoftVer);
        eventData.idLen[1] = sizeof(event_3101.version.szSoftVer);
        eventData.len = eventData.idLen[0] + eventData.idLen[1];
        memcpy(eventData.buf, event_3101.lastversion.szSoftVer.value, eventData.idLen[0]);
        memcpy(eventData.buf+eventData.idLen[0], event_3101.version.szSoftVer.value, eventData.idLen[1]);

        //写数据库
        ret = WriteEventToDB(clientid, 0, &event_3101.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("evt_check_3101失败返回%d\n", ret);
            return;
        }
        EVENT_FMT_LOGCC("evt_check_3101成功, storagenum=%u !\n", event_3101.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_LOGCC("evt_check_3101 checkflag 0\n");

    return;
}
#endif

#if DESC("360X检测事件",1)

/**********************************************************************
* @name      : evt_check_3101
* @brief     ：终端版本变更事件
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-26
* @Update    :
**********************************************************************/
void  evt_check_360X(int clientid, uint8 evtSource, OOP_DATETIME_S_T data, uint8 warnType, uint8 bType, char *pContainerName, char *pAppName)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    EVENT_360X_INFO *pInfo;
    int ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    EVENT_FMT_TRACE("evt_check_3600 starts\n");

	switch(evtSource)
	{
		case 0:
			pInfo = &event_3600;
			break;
		case 1:
			pInfo = &event_3601;
			break;
		case 2:
			pInfo = &event_3602;
			break;
	}

	pInfo->warnType = warnType;

	pInfo->containerName.nNum = 0;
	if (pContainerName != NULL && strlen(pContainerName) > 0)
    {
    	pInfo->containerName.nNum = strlen(pContainerName)<64 ? strlen(pContainerName) : 64;
        memcpy(pInfo->containerName.value, pContainerName, pInfo->containerName.nNum);
    }

	pInfo->appName.nNum = 0;
	if (pAppName != NULL && strlen(pAppName) > 0)
    {
    	pInfo->appName.nNum = strlen(pAppName)<64 ? strlen(pAppName) : 64;
        memcpy(pInfo->appName.value, pAppName, pInfo->appName.nNum);
    }
    
    if (pInfo->eventCommInfo.checkflag)                                 //需要记录
    {
        
        //事件发生源NULL
        eventSource.choice = DT_ENUM;
		eventSource.nValue = evtSource;

        //事件关联数据

        //写数据库
        ret = WriteEventToDB(clientid, 0, &pInfo->eventCommInfo, eventSource, eventData, bType);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("evt_check_360X失败返回%d\n", ret);
            return;
        }
        EVENT_FMT_LOGCC("evt_check_360X成功, storagenum=%u !\n", pInfo->eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_LOGCC("evt_check_360X checkflag 0\n");

    return;
}

/**********************************************************************
* @name      : advance_iot_evt_msg
* @brief     ：物联管理事件消息处理
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2022-7-7
* @Update    :
**********************************************************************/
int advance_iot_evt_msg(int cid, uint16 label, uint8* pMsg, uint16 msglen, uint32* nextStep)
{
	int    ret 			= 0;
	uint8 bytes			= 0;
	uint16 byteLen		= 0;
    uint32 offset 		= 0;
	uint8 evtSource		= 0;
	uint8 warnType		= 0;
	uint8 bType			= 0;
	uint8 result		= 0;
	OOP_DATETIME_S_T data;
	uint8 *ptmp			= NULL;
	char containerName[64] = {0};
	char appName[64] = {0};

	MEMZERO(&data, sizeof(data));
    
    if(pMsg == NULL)
    {
        return -1;
    }
	ptmp = pMsg;
//	byteLen = appmsg_get_len_offset(ptmp, &bytes);
//    ptmp += bytes;
    
    switch(label)
    {
        case IOT_IOP_DOCKER_ACTION: 
		{
			evtSource = 1;
			byteLen = appmsg_get_len_offset(ptmp, &bytes);
    		offset += bytes;
			if(byteLen > 64)
			{
				memcpy(containerName, &ptmp[offset], 64);
			}
			else if(byteLen > 0)
			{
				memcpy(containerName, &ptmp[offset], byteLen);
			}

			offset += byteLen;
			memcpy_r(&data.year, &ptmp[offset], sizeof(uint16));
		    offset += sizeof(uint16);

		    data.month  = ptmp[offset++];  
		    data.day    = ptmp[offset++];           
		    data.hour   = ptmp[offset++]; 
		    data.minute = ptmp[offset++]; 
		    data.second = ptmp[offset++];

			warnType = ptmp[offset++];
			result = ptmp[offset++];
			if(result == 0)		//成功
			{
				if(warnType == 0 || warnType == 1|| warnType == 2)	//安装，升级，卸载
				{
					warnType += 14;
				}
				else if(warnType == 3 || warnType == 4)	//启动，停止
				{
					warnType += 15;
				}

				if(warnType > 13)	//安装，升级，卸载，启动，停止
				{
					//warnType += 14;
					evt_check_360X(cid, evtSource, data, warnType, bType, containerName, NULL);
				}
			}
			else
			{
				YCMSG_FMT_DEBUG("advance_iot_evt_msg msg iop %d, result error ", label);
			}
			
			break;
		}
        case IOT_IOP_DOCKER_WARRING:
		{
			evtSource = 1;
			byteLen = appmsg_get_len_offset(ptmp, &bytes);
    		offset += bytes;
			if(byteLen > 64)
			{
				memcpy(containerName, &ptmp[offset], 64);
			}
			else if(byteLen > 0)
			{
				memcpy(containerName, &ptmp[offset], byteLen);
			}

			offset += byteLen;
			memcpy_r(&data.year, &ptmp[offset], sizeof(uint16));
		    offset += sizeof(uint16);

		    data.month  = ptmp[offset++];  
		    data.day    = ptmp[offset++];           
		    data.hour   = ptmp[offset++]; 
		    data.minute = ptmp[offset++]; 
		    data.second = ptmp[offset++];

			bType = ptmp[offset++];
			warnType = ptmp[offset++];
			
			event_3601.index = warnType;
			warnType += 4;
			evt_check_360X(cid, evtSource, data, warnType, bType, containerName, NULL);
		
			break;
		}
        case IOT_IOP_APP_ACTION:
		{
			evtSource = 2;
			byteLen = appmsg_get_len_offset(ptmp, &bytes);
    		offset += bytes;
			if(byteLen > 64)
			{
				memcpy(containerName, &ptmp[offset], 64);
			}
			else if(byteLen > 0)
			{
				memcpy(containerName, &ptmp[offset], byteLen);
			}
			offset += byteLen;

			byteLen = appmsg_get_len_offset(&ptmp[offset], &bytes);
    		offset += bytes;
			if(byteLen > 64)
			{
				memcpy(appName, &ptmp[offset], 64);
			}
			else if(byteLen > 0)
			{
				memcpy(appName, &ptmp[offset], byteLen);
			}

			offset += byteLen;
			memcpy_r(&data.year, &ptmp[offset], sizeof(uint16));
		    offset += sizeof(uint16);

		    data.month  = ptmp[offset++];  
		    data.day    = ptmp[offset++];           
		    data.hour   = ptmp[offset++]; 
		    data.minute = ptmp[offset++]; 
		    data.second = ptmp[offset++];

			warnType = ptmp[offset++];
			result = ptmp[offset++];
			if(result == 0)		//成功
			{
				if(warnType == 0 || warnType == 1|| warnType == 2)	//安装，升级，卸载
				{
					warnType += 14;
				}
				else if(warnType == 3 || warnType == 4)	//启动，停止
				{
					warnType += 15;
				}

				if(warnType > 13)	//安装，升级，卸载，启动，停止
				{
//					warnType += 14;
					evt_check_360X(cid, evtSource, data, warnType, bType, containerName, appName);
				}
			}
#ifdef BOARD_3358
			else if(warnType == 8)
			{
                warnType+=12;
                evt_check_360X(cid, evtSource, data, warnType, bType, containerName, appName);
				YCMSG_FMT_DEBUG("advance_iot_evt_msg msg iop %d, result error ", label);
			}
#endif
#ifdef BOARD_801
            else if(warnType == 3)
            {
                warnType+=17;
                evt_check_360X(cid, evtSource, data, warnType, bType, containerName, appName);
                YCMSG_FMT_DEBUG("advance_iot_evt_msg msg iop %d, result error ", label);
            }
#endif
			break;
		}
        case IOT_IOP_APP_WARRING:
		{
			evtSource = 2;
			byteLen = appmsg_get_len_offset(ptmp, &bytes);
    		offset += bytes;
			if(byteLen > 64)
			{
				memcpy(containerName, &ptmp[offset], 64);
			}
			else if(byteLen > 0)
			{
				memcpy(containerName, &ptmp[offset], byteLen);
			}
			offset += byteLen;

			byteLen = appmsg_get_len_offset(&ptmp[offset], &bytes);
    		offset += bytes;
			if(byteLen > 64)
			{
				memcpy(appName, &ptmp[offset], 64);
			}
			else if(byteLen > 0)
			{
				memcpy(appName, &ptmp[offset], byteLen);
			}

			offset += byteLen;
			memcpy_r(&data.year, &ptmp[offset], sizeof(uint16));
		    offset += sizeof(uint16);

		    data.month  = ptmp[offset++];  
		    data.day    = ptmp[offset++];           
		    data.hour   = ptmp[offset++]; 
		    data.minute = ptmp[offset++]; 
		    data.second = ptmp[offset++];

			bType = ptmp[offset++];
			warnType = ptmp[offset++];
			event_3602.index = warnType;

			if(warnType == 0 || warnType == 1)	//CPU，内存
			{
				warnType += 4;
			}
			else if(warnType == 2 || warnType == 3)	//重启，异常
			{
				warnType += 7;
			}

			evt_check_360X(cid, evtSource, data, warnType, bType, containerName, appName);

			break;
		}
        case IOT_IOP_SYS_WARRING:
		{
			evtSource = 0;

			memcpy_r(&data.year, &ptmp[offset], sizeof(uint16));
		    offset += sizeof(uint16);

		    data.month  = ptmp[offset++];  
		    data.day    = ptmp[offset++];           
		    data.hour   = ptmp[offset++]; 
		    data.minute = ptmp[offset++]; 
		    data.second = ptmp[offset++];

			bType = ptmp[offset++];
			warnType = ptmp[offset++];
			event_3600.index = warnType;

			if(warnType == 4)	//异常
			{
				break;
			}
			
			if(warnType == 0 || warnType == 1 || warnType == 2)	//CPU，内存，存储
			{
				warnType += 4;
			}
			
			evt_check_360X(cid, evtSource, data, warnType, bType, NULL, NULL);
			break;
		}
        case IOT_IOP_SYS_MOVE_UP: 
        {

			break;
		}
        default: return -1;
    }

	return ret;
}

#endif


#if DESC("状态量变位事件",1)

/*******************************************************************************
* 函数名称: CheckRmtSigStable
* 函数功能: 监测遥信状态前稳定时间设置
* 输入参数: op   读取0  输入1
            val  值
* 输出参数:
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
void CheckRmtSigStable(uint8 op, uint8 *val)
{
    static uint8 bFirstFlag = 0x0;
    if (op == 0)
    {
        *val = bFirstFlag;
        return;
    }

    if (op == 1)
    {
        bFirstFlag = *val;
        return;
    }
}

/*********************************************************************
所属功能单元：提供给终端运行检测子模块使用
名称：状态量变位检测、记录函数
功能描述：状态量变位检测、记录
输入参数：测量点号
输出参数：无
函数返回值：无
内部处理流程：
  读取开关及变位状态,改变持续2秒时间，则判为发生变化。判断是否改变。

修改：2007-11-15 9:17  wlm
      福建要求在断电时仍能够上报门禁变位，现硬件设计原理为门禁变位时自动连接电池回路，
      由电池供电，则软件在这种状态下重新连接网络，并在上报完门禁变位事件后负责切断
      电池供电。
      CheckERC04在处理该逻辑时要注意上述电池上电供电软件运行时，只需检测门禁，专门调用
      u-boot驱动提供的read_mjd()接口检测门禁状态。但若是从正常供电状态断电后切换到电池
      供电，因软件并未重启，u-boot未重启则无法提供read_mjd()，因此仍应采用原来的遥信检
      测逻辑，且注意也只检测门禁状态。
********************************************************************/

void CheckERC_3104(DB_CLIENT fd, OOP_SWITCHIN_STATE_T switchTemp)
{
    /*- 数据内容                数据格式    字节数
        ERC=4                   BIN         1
        长度Le                  BIN         1
        变位时间：分时日月年    数据格式15  5
        状态变位                BS8         1
        变位后状态              BS8         1-*/
    int ret = 0;
    OOP_DATETIME_S_T      dateTime;
    OOP_EVTSOURCE_T       eventSource; //事件发生源
    OOP_EVTBUF_T          eventData;   //事件关联数据

    memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
    memset(&eventData, 0, sizeof(OOP_EVTBUF_T));

    EVENT_FMT_TRACE("CheckERC_3104 starts\n");
    
    //-不需要记录-
    if(!event_3104.eventCommInfo.checkflag)
    {
        EVENT_FMT_LOGCC("CheckERC_3104 checkflag 0\n");
        return;
    }

    //-发生变位-
    event_3104.ishappen = 0;
    
//          switchState = switchTemp;//-生成事件以后，保存当前的状态-

    //事件发生源NULL
    eventSource.choice = DT_NULL;

    //事件关联数据
    current_tm_to_ooptm(&dateTime);
    eventData.cols.oad[0].value = 0X201E4200;
    eventData.cols.oad[1].value = 0XF2034201;
    eventData.cols.oad[2].value = 0XF2034202;
    
    eventData.idLen[0] = sizeof(OOP_DATETIME_S_T);
    eventData.idLen[1] = sizeof(INPUTSTATE);
    eventData.idLen[2] = sizeof(INPUTSTATE);
    
    eventData.len = 0;
    memcpy(eventData.buf + eventData.len, &dateTime, eventData.idLen[0]);
    eventData.len += eventData.idLen[0];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[0], eventData.idLen[1]);
    eventData.len += eventData.idLen[1];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[1], eventData.idLen[2]);
    eventData.len += eventData.idLen[2];

#ifdef PRODUCT_ZCU_1
    eventData.oadNum = 9;
    eventData.cols.nNum = 9;

    eventData.cols.oad[3].value = 0XF2034203;
    eventData.cols.oad[4].value = 0XF2034204;
    eventData.cols.oad[5].value = 0XF2034205;
    eventData.cols.oad[6].value = 0XF2034206;
    eventData.cols.oad[7].value = 0XF2034207;
    eventData.cols.oad[8].value = 0XF2034208;
    
    eventData.idLen[3] = sizeof(INPUTSTATE);
    eventData.idLen[4] = sizeof(INPUTSTATE);
    eventData.idLen[5] = sizeof(INPUTSTATE);
    eventData.idLen[6] = sizeof(INPUTSTATE);
    eventData.idLen[7] = sizeof(INPUTSTATE);
    eventData.idLen[8] = sizeof(INPUTSTATE);

    memcpy(eventData.buf + eventData.len, &switchTemp.state[2], eventData.idLen[3]);
    eventData.len += eventData.idLen[3];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[3], eventData.idLen[4]);
    eventData.len += eventData.idLen[4];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[4], eventData.idLen[5]);
    eventData.len += eventData.idLen[5];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[5], eventData.idLen[6]);
    eventData.len += eventData.idLen[6];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[6], eventData.idLen[7]);
    eventData.len += eventData.idLen[7];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[7], eventData.idLen[8]);
    eventData.len += eventData.idLen[8];

#ifdef AREA_JIANGSU
    eventData.oadNum = 9 + 8;
    eventData.cols.nNum = 9 + 8;

    eventData.cols.oad[9].value  = 0XF2034209;
    eventData.cols.oad[10].value = 0XF203420A;
    eventData.cols.oad[11].value = 0XF203420B;
    eventData.cols.oad[12].value = 0XF203420C;
    eventData.cols.oad[13].value = 0XF203420D;
    eventData.cols.oad[14].value = 0XF203420E;
    eventData.cols.oad[15].value = 0XF203420F;
    eventData.cols.oad[16].value = 0XF2034210;
    
    
    eventData.idLen[9] = sizeof(INPUTSTATE);
    eventData.idLen[10] = sizeof(INPUTSTATE);
    eventData.idLen[11] = sizeof(INPUTSTATE);
    eventData.idLen[12] = sizeof(INPUTSTATE);
    eventData.idLen[13] = sizeof(INPUTSTATE);
    eventData.idLen[14] = sizeof(INPUTSTATE);
    eventData.idLen[15] = sizeof(INPUTSTATE);
    eventData.idLen[16] = sizeof(INPUTSTATE);

    memcpy(eventData.buf + eventData.len, &switchTemp.state[8], eventData.idLen[9]);
    eventData.len += eventData.idLen[9];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[9], eventData.idLen[10]);
    eventData.len += eventData.idLen[10];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[10], eventData.idLen[11]);
    eventData.len += eventData.idLen[11];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[11], eventData.idLen[12]);
    eventData.len += eventData.idLen[12];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[12], eventData.idLen[13]);
    eventData.len += eventData.idLen[13];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[13], eventData.idLen[14]);
    eventData.len += eventData.idLen[14];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[14], eventData.idLen[15]);
    eventData.len += eventData.idLen[15];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[15], eventData.idLen[16]);
    eventData.len += eventData.idLen[16];
#endif

#else

#ifdef PRODUCT_CCU
    /* 第8路门节点 */
    eventData.oadNum = 4;
    eventData.cols.nNum = 4;

    eventData.cols.oad[3].value = 0XF2034208;
    eventData.idLen[3] = sizeof(INPUTSTATE);
    memcpy(eventData.buf + eventData.len, &switchTemp.state[7], eventData.idLen[3]);
    eventData.len += eventData.idLen[3];
#else
    eventData.oadNum = 5;
    eventData.cols.nNum = 5;

    eventData.cols.oad[3].value = 0XF2034203;
    eventData.cols.oad[4].value = 0XF2034204;

    eventData.idLen[3] = sizeof(INPUTSTATE);
    eventData.idLen[4] = sizeof(INPUTSTATE);

    memcpy(eventData.buf + eventData.len, &switchTemp.state[2], eventData.idLen[3]);
    eventData.len += eventData.idLen[3];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[3], eventData.idLen[4]);
    eventData.len += eventData.idLen[4];
#endif

#ifdef PRODUCT_ZCU_3
    eventData.oadNum = 6;
    eventData.cols.nNum = 6;

    eventData.cols.oad[5].value = 0XF2034208;
    eventData.idLen[5] = sizeof(INPUTSTATE);
    memcpy(eventData.buf + eventData.len, &switchTemp.state[7], eventData.idLen[3]);
    eventData.len += eventData.idLen[5];
#endif

#endif
    //写数据库
    ret = WriteEventToDB(fd, 0, &event_3104.eventCommInfo, eventSource, eventData, 0);
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("状态量变位事件记录CheckERC_3104失败返回%d\n", ret);
        return;
    }
    EVENT_FMT_LOGCC("状态量变位事件记录CheckERC_3104成功, storagenum=%u !\n", event_3104.eventCommInfo.storagenum);
}


/*********************************************************************
所属功能单元：提供给终端运行检测子模块使用 , 福建特殊要求，事件开始时间由入参带入
名称：状态量变位检测、记录函数
功能描述：状态量变位检测、记录
输入参数：测量点号
输出参数：无
函数返回值：无
内部处理流程：
  读取开关及变位状态,改变持续2秒时间，则判为发生变化。判断是否改变。

修改：2007-11-15 9:17  wlm
      福建要求在断电时仍能够上报门禁变位，现硬件设计原理为门禁变位时自动连接电池回路，
      由电池供电，则软件在这种状态下重新连接网络，并在上报完门禁变位事件后负责切断
      电池供电。
      CheckERC04在处理该逻辑时要注意上述电池上电供电软件运行时，只需检测门禁，专门调用
      u-boot驱动提供的read_mjd()接口检测门禁状态。但若是从正常供电状态断电后切换到电池
      供电，因软件并未重启，u-boot未重启则无法提供read_mjd()，因此仍应采用原来的遥信检
      测逻辑，且注意也只检测门禁状态。
********************************************************************/
void CheckERC_3104_seu(DB_CLIENT fd, OOP_DATETIME_S_T evtStart, OOP_SWITCHIN_STATE_T switchTemp)
{
    /*- 数据内容                数据格式    字节数
        ERC=4                   BIN         1
        长度Le                  BIN         1
        变位时间：分时日月年    数据格式15  5
        状态变位                BS8         1
        变位后状态              BS8         1-*/
    int ret = 0;
    OOP_DATETIME_S_T      dateTime;
    OOP_EVTSOURCE_T       eventSource; //事件发生源
    OOP_EVTBUF_T          eventData;   //事件关联数据

    memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
    memset(&eventData, 0, sizeof(OOP_EVTBUF_T));

    EVENT_FMT_TRACE("CheckERC_3104 starts\n");
    
    //-不需要记录-
    if(!event_3104.eventCommInfo.checkflag)
    {
        EVENT_FMT_LOGCC("CheckERC_3104 checkflag 0\n");
        return;
    }

	event_3104.eventCommInfo.starttime = evtStart;

    //-发生变位-
    event_3104.ishappen = 0;
    
//          switchState = switchTemp;//-生成事件以后，保存当前的状态-

    //事件发生源NULL
    eventSource.choice = DT_NULL;

    //事件关联数据
    current_tm_to_ooptm(&dateTime);
    eventData.cols.oad[0].value = 0X201E4200;
    eventData.cols.oad[1].value = 0XF2034201;
    eventData.cols.oad[2].value = 0XF2034202;
    
    eventData.idLen[0] = sizeof(OOP_DATETIME_S_T);
    eventData.idLen[1] = sizeof(INPUTSTATE);
    eventData.idLen[2] = sizeof(INPUTSTATE);
    
    eventData.len = 0;
    memcpy(eventData.buf + eventData.len, &dateTime, eventData.idLen[0]);
    eventData.len += eventData.idLen[0];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[0], eventData.idLen[1]);
    eventData.len += eventData.idLen[1];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[1], eventData.idLen[2]);
    eventData.len += eventData.idLen[2];

#ifdef PRODUCT_ZCU_1
    eventData.oadNum = 9;
    eventData.cols.nNum = 9;

    eventData.cols.oad[3].value = 0XF2034203;
    eventData.cols.oad[4].value = 0XF2034204;
    eventData.cols.oad[5].value = 0XF2034205;
    eventData.cols.oad[6].value = 0XF2034206;
    eventData.cols.oad[7].value = 0XF2034207;
    eventData.cols.oad[8].value = 0XF2034208;
    
    eventData.idLen[3] = sizeof(INPUTSTATE);
    eventData.idLen[4] = sizeof(INPUTSTATE);
    eventData.idLen[5] = sizeof(INPUTSTATE);
    eventData.idLen[6] = sizeof(INPUTSTATE);
    eventData.idLen[7] = sizeof(INPUTSTATE);
    eventData.idLen[8] = sizeof(INPUTSTATE);

    memcpy(eventData.buf + eventData.len, &switchTemp.state[2], eventData.idLen[3]);
    eventData.len += eventData.idLen[3];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[3], eventData.idLen[4]);
    eventData.len += eventData.idLen[4];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[4], eventData.idLen[5]);
    eventData.len += eventData.idLen[5];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[5], eventData.idLen[6]);
    eventData.len += eventData.idLen[6];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[6], eventData.idLen[7]);
    eventData.len += eventData.idLen[7];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[7], eventData.idLen[8]);
    eventData.len += eventData.idLen[8];

#ifdef AREA_JIANGSU
    eventData.oadNum = 9 + 8;
    eventData.cols.nNum = 9 + 8;

    eventData.cols.oad[9].value  = 0XF2034209;
    eventData.cols.oad[10].value = 0XF203420A;
    eventData.cols.oad[11].value = 0XF203420B;
    eventData.cols.oad[12].value = 0XF203420C;
    eventData.cols.oad[13].value = 0XF203420D;
    eventData.cols.oad[14].value = 0XF203420E;
    eventData.cols.oad[15].value = 0XF203420F;
    eventData.cols.oad[16].value = 0XF2034210;
    
    
    eventData.idLen[9] = sizeof(INPUTSTATE);
    eventData.idLen[10] = sizeof(INPUTSTATE);
    eventData.idLen[11] = sizeof(INPUTSTATE);
    eventData.idLen[12] = sizeof(INPUTSTATE);
    eventData.idLen[13] = sizeof(INPUTSTATE);
    eventData.idLen[14] = sizeof(INPUTSTATE);
    eventData.idLen[15] = sizeof(INPUTSTATE);
    eventData.idLen[16] = sizeof(INPUTSTATE);

    memcpy(eventData.buf + eventData.len, &switchTemp.state[8], eventData.idLen[9]);
    eventData.len += eventData.idLen[9];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[9], eventData.idLen[10]);
    eventData.len += eventData.idLen[10];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[10], eventData.idLen[11]);
    eventData.len += eventData.idLen[11];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[11], eventData.idLen[12]);
    eventData.len += eventData.idLen[12];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[12], eventData.idLen[13]);
    eventData.len += eventData.idLen[13];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[13], eventData.idLen[14]);
    eventData.len += eventData.idLen[14];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[14], eventData.idLen[15]);
    eventData.len += eventData.idLen[15];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[15], eventData.idLen[16]);
    eventData.len += eventData.idLen[16];
#endif

#else

#ifdef PRODUCT_CCU
    /* 第8路门节点 */
    eventData.oadNum = 4;
    eventData.cols.nNum = 4;

    eventData.cols.oad[3].value = 0XF2034208;
    eventData.idLen[3] = sizeof(INPUTSTATE);
    memcpy(eventData.buf + eventData.len, &switchTemp.state[7], eventData.idLen[3]);
    eventData.len += eventData.idLen[3];
#else
    eventData.oadNum = 5;
    eventData.cols.nNum = 5;

    eventData.cols.oad[3].value = 0XF2034203;
    eventData.cols.oad[4].value = 0XF2034204;

    eventData.idLen[3] = sizeof(INPUTSTATE);
    eventData.idLen[4] = sizeof(INPUTSTATE);

    memcpy(eventData.buf + eventData.len, &switchTemp.state[2], eventData.idLen[3]);
    eventData.len += eventData.idLen[3];
    memcpy(eventData.buf + eventData.len, &switchTemp.state[3], eventData.idLen[4]);
    eventData.len += eventData.idLen[4];
#endif

#ifdef PRODUCT_ZCU_3
    eventData.oadNum = 6;
    eventData.cols.nNum = 6;

    eventData.cols.oad[5].value = 0XF2034208;
    eventData.idLen[5] = sizeof(INPUTSTATE);
    memcpy(eventData.buf + eventData.len, &switchTemp.state[7], eventData.idLen[3]);
    eventData.len += eventData.idLen[5];
#endif

#endif
    //写数据库
    ret = WriteEventToDB_fj(fd, 0, &event_3104.eventCommInfo, eventSource, eventData, 0);
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("状态量变位事件记录CheckERC_3104失败返回%d\n", ret);
        return;
    }
    EVENT_FMT_LOGCC("状态量变位事件记录CheckERC_3104成功, storagenum=%u !\n", event_3104.eventCommInfo.storagenum);
}

#endif

#if DESC("电能表时间超差事件",1)
/**********************************************************************
* @name      : evt_check_3105
* @brief     ：电能表时间超差检测、记录函数
* @param[in] ：int clientid    db客户端
               uint8 logicId   逻辑设备号
               uint16 mtr_id   信息点号
               OOP_METER_T oopMeter 电表档案
               uint8* databuf       事件判断数据
               uint16 datalen       事件判断数据长度
* @param[out]：
* @return    ：0成功/<0异常
* @Create    : 梅安宁
* @Date      ：2020-12-26
* @Update    :
**********************************************************************/
int evt_check_3105(int clientid,uint8 logicid,uint16 mtr_id, OOP_METER_T oopMeter,uint8* databuf,uint16 datalen)
{
    /*- 数据内容                                        数据格式    字节数
    ERC=12                                          BIN         1
    长度Le                                          BIN         1
    发生时间：分时日月年                            数据格式15  5
    D7：起/止标志   D6=0  D5~D0：mtr_id（测量点号） BIN         1-*/

    if (event_3105.eventCommInfo.checkflag == 0)//-不需要记录-
    {
        EVENT_FMT_LOGCC("event_3105.checkflag 0\n");
        return 0;
    }

    if (mtr_id >= NO_VIR_MTR_MAX_NUM)//-不需要记录-
    {
        EVENT_FMT_DEBUG("evt_check_3105 mtr_id[%d] too big \n", mtr_id);
        return 0;
    }

    if(oopMeter.basic.port.nObjID == OI_485||oopMeter.basic.port.nObjID == OI_PLC)      //只判485表，交采不判时间超差
    {
        MetclockAbnormalEvent(clientid, mtr_id, oopMeter, 1, databuf, datalen);//-电能表时间超差检测、记录-
        MetclockAbnormalEvent(clientid, mtr_id, oopMeter, 2, databuf, datalen);//-电能表时间超差恢复检测、记录-
    }

    EVENT_FMT_TRACE("CheckERC_3105[%hu] complete \n", mtr_id);
    return 0;
}
#endif

#if DESC("停上电事件",1)
extern METER_HALT_STAT_STRUCT gMeterHalt_Info;//需要读取停电事件的结构信息

/**********************************************************************
* @name      : Pro_RelayHalt
* @brief     : 电表停电记录中继接收处理函数
* @param[in] ：
    -           recvBuf     报文
    -           recvLen     报文长度
    -           recvID      报文类型
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-01-19
* @Update    :
**********************************************************************/
int Pro_RelayHalt(uint8 *recvBuf, uint16 recvLen)
{
    uint8 offSet = 0;
    uint16 msgLen = 0;

    if(recvLen <= 0)
    {
        EVENT_FMT_DEBUG("recvLen=0 error!\n");
        gMeterHalt_Info.Relaydstat = RELAY_FAIL;
        return -1;
    }

    
    EVENT_BUF_DEBUG(recvBuf, recvLen, "CheckERC_3106 recvBuf:");

    msgLen = getMsgLenOffset(recvBuf, &offSet);

    if(msgLen != recvLen - offSet)
    {
        EVENT_FMT_DEBUG("recvLen[%hu]- offSet[%hu]!=  msgLen[%hu] error!\n", recvLen, offSet ,msgLen);
        gMeterHalt_Info.Relaydstat = RELAY_FAIL;
        return -1;
    }
    
    if(dealMsgBuf(recvBuf+offSet, msgLen) < 0)
    {
        gMeterHalt_Info.Relaydstat = RELAY_FAIL;
        return -1;
    }

    return 0;
    
}

/*******************************************************************************
* 函数名称: CheckERC_3106
* 函数功能: 停上电检测
* 输入参数: void
* 输出参数: NULL
* 内部处理:
根据新的用电信息采集系统停电事件采集要求，终端停/上电事件进行检测
判断逻辑步骤:
STEP-1:
终端停上电时间每秒检测:
    终端电压大于掉电限值，每个一段时间间隔(暂定为20S)刷新下当前时间。
    如果产生新的终端停电记录,那么终端实时产生一条停电记录，终端实时存储，进行上报.
函数返回。
    如果产生新的终端上电记录,那么终端实时产生一条上电记录，终端记录但是为，进入下一步处理.
    如果没有产生终端停上电记录,那么进入下一步处理。
STEP-2:
    终端根据停电数据采集标志决定是否需要穿透抄读掉电记录。
    依据下发的停电事件抄读时间间隔,如果是刚上电或者到抄读间隔进入穿透抄读掉电记录.
    根据穿透返回的电表掉电记录，依据判断电表的掉电记录。
   -

* 返 回 值: 无
*******************************************************************************/
void evt_check_3106(void)
{
    int  ret = -1; //终端停上电返回

    //每秒中进行判断终端是否产生新的停上电事件
    ret = CheckTmn_Halt();

#ifdef AREA_FUJIAN
	//停电上报标记 <      0x3c 等待上报， 0x55 立即上报，0xaa 已经上报
	if(event_3106.rptFlag == 0x55 || event_3106.rptFlag == 0)
    {
		//记录事件及上报
		evt3106_write_data_FJ(gDbCenterClient, ProgDownAddr);
		event_3106.rptFlag = 0xaa;
		write_pdata_xram("^Event", (char *)&event_3106.rptFlag, EventStopRpt, 1);
    }
	
	if(event_3106.rptFlag < 0x55)
    {
        event_3106.rptFlag --;
    }

	static uint32 stopTm = 0;
	//停电后持续工作时间超过3分钟标记， 0x55 超过3分钟，0xaa 未超过, 终端上电读取判断电容
	if(stopTm >= 175)	//停电防抖已经5秒钟
    {
        event_3106.tmFlag = 0x55;
		write_pdata_xram("^Event", (char *)&event_3106.tmFlag, EventStopTm, 1);
    }
	
	if(event_3106.tmFlag == 0xaa && event_3106.ishappen == 0x55)
    {
        stopTm ++;
		EVENT_FMT_TRACE("CheckERC_3106 终端停电时间%d\n", stopTm);
    }

#endif
	

    if (ret < 0)//终端停电
    {
#ifndef AREA_HUNAN

        return;
#else
        memset(g_val_a, 0, sizeof(g_val_a));
        gMeterHalt_Info.MeterHalt_Stat = HALT_INIT;
        gMeterHalt_Info.CheckMeterHalt = 1;//初始化结构体
#endif

    }


    if (ret == 0)//新的停上电记录或者抄读时间到
    {
        // 先判断停电的属性是否异常如果异常则不再进行后面的上电抄读逻辑
        if (MergePowerOffTime(2) == 0)
        {
            gMeterHalt_Info.MeterHalt_Stat = HALT_INIT;
            gMeterHalt_Info.CheckMeterHalt = 1;//初始化结构体
            EVENT_FMT_TRACE("CheckERC_3106 终端停上电开始启动对电表记录的判断逻辑抄读\n");
        }
    }

    //判断电表的停上电事件
    CheckMeter_Halt();

}

#endif

/*********************************************************************
所属功能单元：
功能描述：终端消息认证错误事件
输入参数：
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_3109(int fd, uint8* wPW,uint32 PWlen)
{
    int                 ret = 0;
    OOP_EVTSOURCE_T     eventSource; //事件发生源
    OOP_EVTBUF_T        eventData;   //事件关联数据
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));


    EVENT_FMT_TRACE("CheckERC_3109 starts\n");
    
    if (event_3109.eventCommInfo.checkflag)         //需要记录
    {    
        //事件发生源NULL
        eventSource.choice = DT_NULL;

        //事件关联数据
        if(PWlen>32)
        {
            PWlen=32;
        }
		
        eventData.oadNum = 1;
        eventData.cols.nNum = 1;
        eventData.cols.oad[0].value = 0X44012201;
        eventData.idLen[0] = PWlen;
        eventData.len = eventData.idLen[0];
        memcpy(eventData.buf, wPW, PWlen);

        //写数据库
        ret = WriteEventToDB(fd, 0, &event_3109.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3109 终端消息认证错误事件记录失败返回%d\n", ret);
            return;
        }

        EVENT_FMT_DEBUG("CheckERC_3109 终端消息认证错误事件 记录成功, storagenum=%u !\n", event_3109.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_DEBUG("CheckERC_3109 checkflag 0\n");

    return;
}

#if DESC("电池拔插状态和电压",1)

#if (defined PRODUCT_ECU) || (defined PRODUCT_ECU_SC) 
    
/**********************************************************************
* @name      : get_battery_status
* @brief     ：检查终端电池状态插拔状态
* @param[in] ：
* @param[out]：
* @return    ：插入为低0，拔掉为高1， <0 失败
* @Create    : 梅安宁
* @Date      ：2022-06-25
* @Update    :
**********************************************************************/
int battery_insert_check(uint8* status)
{
    int sta = 0;
    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
    if(NULL == dev)
    {
        EVENT_FMT_DEBUG("dev is null!\n");
        return -1;
    }
    
    if(dev->power_battery_state_get(dev,&sta) < 0)
    {
        EVENT_FMT_DEBUG("set_battery_charge failed!\n");
        return -1;
    }

    hal_device_release((HW_DEVICE *)dev);

    *status = sta&0xff;
    EVENT_FMT_TRACE("battery_insert_check status %d!\n", sta);
    
    return sta;
}
#else

/**********************************************************************
* @name      : get_battery_status
* @brief     ：检查终端电池状态插拔状态
* @param[in] ：status 		0 有电池 ，1 电池被拔出
* @param[out]：
* @return    ：0 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2022-06-25
* @Update    :
**********************************************************************/
int battery_insert_check(uint8* status)
{
    int32 fd = -1;
    uint32 uData;
    int32 iRet = ERR_OK;
    
    /* open device file */
#ifdef PRODUCT_ECU
    fd = open("/dev/gpio", O_RDWR);
#else
    fd = open("/dev/ym", O_RDONLY|O_NONBLOCK);
#endif
    if(fd < 0)
    {
        EVENT_FMT_DEBUG("open get_battery_status file error!\n");
        return ERR_NORMAL;
    }

    /* 读掉电检测状态 */
    if(read(fd, &uData, sizeof(uData)) 
        != sizeof(uData))
    {
       iRet = ERR_IO;
       close(fd);
       return iRet;  
    }

	*status = (uData>>8)&0x01;
    //1拔出电池 0有电池

	EVENT_FMT_TRACE(" get_battery_status [%x]!\n", uData);
	EVENT_FMT_TRACE(" get_battery_status [%x]!\n", *status);

  //  printf("power.data = %d\n", data);
    close(fd);
    return iRet;    
}
#endif


/**********************************************************************
* @brief     ：备用电源充电控制
* @param[in] val: 0： 不充电， 1： 充电
* @return    ：1 成功， <0 失败
* @Create    : 梅安宁
* @Date      ：2020-07-27
* @Update    :
**********************************************************************/
int set_battery_charge(uint8 val)
{
#ifdef AREA_HUNAN   

    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
    if(NULL == dev)
    {
        EVENT_FMT_DEBUG("dev is null!\n");
        return -1;
    }
    
    if(dev->power_battery_charge_set(dev, val) < 0)
    {
        EVENT_FMT_DEBUG("set_battery_charge failed!\n");
        return -1;
    }

    //等待与电池并联的电容电量耗尽，硬件实测至少要300ms
    usleep(300000);
    hal_device_release((HW_DEVICE *)dev);
#endif    
    return 1;
}

/**********************************************************************
* @brief     ：获取电池电压
* @param[in] ： 

* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
int battery_vol_get(int *pVol)
{
    int vol = 0;

    set_battery_charge(0);

    ADC_DEVICE_T* dev = (ADC_DEVICE_T*) hal_device_get(HW_DEVICE_ID_ADC);
    if(NULL == dev)
    {
        EVENT_FMT_DEBUG("dev is null!\n");
        return -1;
    }
    
    if(dev->adc_volt_read(dev,EMADC_BATVOL_CHANNEL,&vol) < 0)
    {
        EVENT_FMT_DEBUG("get power state failed!\n");
        return -1;
    }
 //单位0.001V

    EVENT_FMT_TRACE("get power vol %d! \n", vol);
    
    hal_device_release((HW_DEVICE *)dev);

    set_battery_charge(1);

    *pVol = vol ;
    return 0;

}

/**********************************************************************
* @brief     ：获取电容电压
* @param[in] ： 

* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
int supercap_vol_get(int *pVol)
{
    int vol = 0;

    //set_battery_charge(0);

    ADC_DEVICE_T* dev = (ADC_DEVICE_T*) hal_device_get(HW_DEVICE_ID_ADC);
    if(NULL == dev)
    {
        EVENT_FMT_DEBUG("dev is null!\n");
        return -1;
    }
    
    if(dev->adc_volt_read(dev,EMADC_CAPVOL_CHANNEL,&vol) < 0)
    {
        EVENT_FMT_DEBUG("get adc capvol failed!\n");
        return -1;
    }
 //单位0.001V

    EVENT_FMT_TRACE("get adc vol %d! \n", vol);
    
    hal_device_release((HW_DEVICE *)dev);

    //set_battery_charge(1);

    *pVol = vol ;
    return 0;

}

#endif


#if DESC("终端电池故障",1)


/*******************************************************************************
* 所属功能单元：
* 功能描述：终端故障检测、记录函数
* 输入参数：测量点号
* 输出参数：无
* 返 回 值：无
* 内部流程：
* 数据内容                  数据格式    字节数
*       ERC=21                  BIN         1
*       长度Le                  BIN         1
*       发生时间：分时日月年    数据格式15  5
* 备注说明: 终端故障编码，只判断485
*******************************************************************************/
void evt_check_310A_battery(mtrnum_t mtr_id)
{
    int             Vol         = 0;
    int             ret         = 0;
    uint8           cb_flag     = 0;    //故障记录标志
    mtrnum_t        successflag = 0;    //终端电池故障标志，
    uint8           status = 0;
    int             powerState = -1;

    OOP_EVTSOURCE_T eventSource;        //事件发生源
    OOP_EVTBUF_T    eventData;          //事件关联数据 
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    struct timespec ts;

    if (event_310A.eventCommInfo.checkflag == 0)//-不需要记录-
    {
        EVENT_FMT_LOGCC("event_310A.checkflag 0\n");
        return;
    }

    if (get_power_state(&powerState) && powerState == 0)
    {
        EVENT_FMT_TRACE("CheckERC_310A 电池故障判断, 停电不判断\n");
        return;
    }

    ret = battery_insert_check(&status);        //检查电池拔插状态
    if (ret != 0)
    {
        EVENT_FMT_DEBUG("battery_insert_check err\n");
    }

    if (status== 0) 
    {
        ret = battery_vol_get(&Vol);
        if (ret != 0)
        {
            EVENT_FMT_DEBUG("battery_vol_get err\n");
            return;
        }
    }

    if(read_pdata_xram("^Event", (char *)&cb_flag, EventFlagRecAddr, 1)<0 || cb_flag == 0xff)
    {
        EVENT_FMT_DEBUG("CheckERC_310A 故障记录标志无效,设置为默认值0\n");
        cb_flag = 0;
        write_pdata_xram("^Event", (char *)&cb_flag, EventFlagRecAddr, 1);
    }

    if (status== 0 && Vol >= 4000)             //有电池，且 电池电压超过4V,
    {
        successflag = 1;        //电池正常
    }
    else if(status== 0)                         //有电池，且 电池电压低于4V, 判断终端上电时间是否超过2小时
    {
        clock_gettime(CLOCK_MONOTONIC, &ts);
        if(ts.tv_sec < 2*60*60)
        {
            EVENT_FMT_TRACE("CheckERC_310A 电池故障判断，终端运行时间小于2小时，不判断\n");
            return;
        }
    }

    if (successflag != 0 && (cb_flag & BT_ERROR_EVENT))   //电池正常 且上次事件状态为发生，判定事件恢复
    {
        successflag = 0;                   
        //清故障记录标志
        cb_flag &= ~BT_ERROR_EVENT;
        write_pdata_xram("^Event", (char *)&cb_flag, EventFlagRecAddr, 1);

        //事件发生源
        eventSource.choice = DT_ENUM;
        eventSource.nValue= 0X09;

        //事件关联数据
        EVENT_FMT_DEBUG("CheckERC_310A status=%d Vol=%d, successflag%d, 事件状态cb_flag = %d, 抄表失败电表数failflag = %d\n", status, Vol, cb_flag);
        //写数据库
        ret = WriteEventToDB(gDbCenterClient, 1, &event_310A.eventCommInfo, eventSource, eventData, 2);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG( "CheckERC_310A 终端电池故障恢复记录事件 失败返回%d\n", ret);
            return;
        }
        EVENT_FMT_LOGCC( "CheckERC_310A 终端电池故障恢复记录事件 成功 num=%u \n",             event_310A.eventCommInfo.eventNum);
    }
    else if (successflag == 0 && (!(cb_flag & BT_ERROR_EVENT)))   //所有电表都未抄通 未记录过故障，判定事件发生
    {
        //发生故障
        cb_flag |= BT_ERROR_EVENT;
        write_pdata_xram("^Event", (char *)&cb_flag, EventFlagRecAddr, 1);

        //事件发生源
        eventSource.choice = DT_ENUM;
        eventSource.nValue= 0X09;

        //事件关联数据
        EVENT_FMT_DEBUG("CheckERC_310A status=%d Vol=%d, successflag%d, 事件状态cb_flag = %d, 抄表失败电表数failflag = %d\n", status, Vol, cb_flag);
        //写数据库
        ret = WriteEventToDB(gDbCenterClient, 1, &event_310A.eventCommInfo, eventSource, eventData, 1);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG( "CheckERC_310A 终端电池故障记录事件 失败返回%d\n", ret);
            return;
        }
        EVENT_FMT_LOGCC( "CheckERC_310A 终端电池故障记录事件 成功 num=%u \n",             event_310A.eventCommInfo.eventNum);
    }
    return;

}


#endif

/*******************************************************************************
* 所属功能单元：
* 功能描述：终端故障检测、记录函数
* 输入参数：测量点号
* 输出参数：无
* 返 回 值：无
* 内部流程：
* 数据内容                  数据格式    字节数
*       ERC=21                  BIN         1
*       长度Le                  BIN         1
*       发生时间：分时日月年    数据格式15  5
* 备注说明: 终端故障编码，只判断485
*******************************************************************************/
void CheckERC_310A(mtrnum_t mtr_id)
{
    int             i           = 0;
    int             ret         = 0;
    uint8           cb_flag     = 0;    //485故障记录标志，为1时表示发生故障
    mtrnum_t        successflag = 0;    //终端抄表抄通的电表数量
    mtrnum_t        failflag = 0;
    mtrnum_t        cb_number   = 0;    //当前抄表数
    mtrnum_t        CurrMtrCnt  = 0;    //有效485表总数
    OOP_EVTSOURCE_T eventSource;        //事件发生源
    OOP_EVTBUF_T    eventData;          //事件关联数据 
    OOP_METER_T     oopMeter;           //采集档案   

    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    if (event_310A.eventCommInfo.checkflag == 0)//-不需要记录-
    {
        EVENT_FMT_LOGCC("event_310A.checkflag 0\n");
        return;
    }

    evt_check_310A_battery(mtr_id);

    CurrMtrCnt = 0;
    for (i = MTR_ID_485; i < MTR_ID_485+MTR485_MAX_NUM; i++)
    {
        if(app_get_meter_list(i, &oopMeter) != ERR_OK || oopMeter.basic.port.nObjID != OI_485)
        {
            continue;
        }
        CurrMtrCnt ++;    //-485总数-
        if(event_310F.recodFlag[i-1] != 0x55 && event_310F.recodFlag[i-1] != 0xaa)
        {
            GetReadMeterStatus(oopMeter.basic.tsa, i);
            continue;
        }
        
        cb_number++;
        if (event_310F.recodFlag[i-1]== 0xaa)                           //电表未抄通
        {
            failflag++;                                                 //抄通失败的电表数加1
        }
    }

    
    if(read_pdata_xram("^Event", (char *)&cb_flag, EventFlagRecAddr, 1)<0 || cb_flag == 0xff) 
    {
        EVENT_FMT_DEBUG("CheckERC_310A 故障记录标志无效,默认无效n");
        cb_flag = 0;
        write_pdata_xram("^Event", (char *)&cb_flag, EventFlagRecAddr, 1);
    }
    
    EVENT_FMT_TRACE("CheckERC_310A 当前设置总的表数CurrMtrCnt=%d cb_number=%d, successflag%d，事件状态cb_flag = %d\n", CurrMtrCnt, cb_number, successflag, cb_flag );

    if (!CurrMtrCnt)
    {
        return;
    }
    
    if (cb_number < CurrMtrCnt)             //未抄完全部电表，当前抄表个数小于设置的电表总数
    {
        return;
    }
    else if(cb_number > CurrMtrCnt)         //容错
    {
        return;
    }
    else                                        //抄完全部电表
    {
        successflag = cb_number - failflag;
        cb_number = 0;                          //清当前抄表个数
        if (successflag != 0 && cb_flag == 1)   //至少有一块电表抄通 且上次事件状态为发生，判定事件恢复
        {
            successflag = 0;                    //-清抄通表的个数-
            cb_flag = 0;                        //清故障记录标志
            write_pdata_xram("^Event", (char *)&cb_flag, EventFlagRecAddr, 1);

            //事件发生源
            eventSource.choice = DT_ENUM;
            eventSource.nValue= 0X03;

            //事件关联数据
        
            //写数据库
            ret = WriteEventToDB(gDbCenterClient, 0, &event_310A.eventCommInfo, eventSource, eventData, 2);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG( "CheckERC_310A 终端485通信故障恢复记录事件 失败返回%d\n", ret);
                return;
            }
            EVENT_FMT_LOGCC( "CheckERC_310A 终端485通信故障恢复记录事件 成功 num=%u \n",             event_310A.eventCommInfo.eventNum);
        }
        else if (successflag == 0 && (cb_flag == 0||cb_flag == 0xff))   //所有电表都未抄通 未记录过故障，判定事件发生
        {
            cb_flag = 1;                    //发生故障
            write_pdata_xram("^Event", (char *)&cb_flag, EventFlagRecAddr, 1);

            //事件发生源
            eventSource.choice = DT_ENUM;
            eventSource.nValue= 0X03;

            //事件关联数据
        
            //写数据库
            ret = WriteEventToDB(gDbCenterClient, 0, &event_310A.eventCommInfo, eventSource, eventData, 1);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG( "CheckERC_310A 终端485通信故障记录事件 失败返回%d\n", ret);
                return;
            }
            EVENT_FMT_LOGCC( "CheckERC_310A 终端485通信故障记录事件 成功 num=%u \n",             event_310A.eventCommInfo.eventNum);
        }

    }

    EVENT_FMT_TRACE("CheckERC_310A complete \n");
    return;

}

#if DESC("电能表电量检测事件",1)
/**********************************************************************
* @name      : evt_check_310X
* @brief     ：电能表电量检测事件 301B电能表示度下降 ,310C电能量超差 ,310D电能表飞走 ,310E电能表停走
* @param[in] ：int clientid    db客户端
               uint8 logicId   逻辑设备号
               uint16 mtr_id   信息点号
               OOP_METER_T oopMeter 电表档案
               uint8* databuf       事件判断数据
               uint16 datalen       事件判断数据长度
* @param[out]：
* @return    ：0成功/<0异常
* @Create    : 梅安宁
* @Date      ：2020-12-26
* @Update    :
**********************************************************************/
int evt_check_310X(int clientid,uint8 logicid,uint16 mtr_id, OOP_METER_T oopMeter,uint8* databuf,uint16 datalen)
{       
    
    uint32  met_en_now = 0;
    uint32  met_en_last = 0;
    int16   len = 0;
    static uint8 bfirst[NO_VIR_MTR_MAX_NUM] = {0};
    OOP_ENERGY_T    *Pfw_act_en = NULL;

    if(mtr_id > NO_VIR_MTR_MAX_NUM)
    {
        EVENT_FMT_TRACE("只处理485数据, infoNum[%u]>%u!\n", mtr_id, NO_VIR_MTR_MAX_NUM);
        return 0;
    }
    if((event_310B.eventCommInfo.checkflag==0x00)&&(event_310C.eventCommInfo.checkflag==0x00)&&(event_310D.eventCommInfo.checkflag==0x00)&&(event_310E.eventCommInfo.checkflag==0x00))
    {
        return -1;
    }
    EVENT_FMT_TRACE("evt_check_310X[%hu] starts\n", mtr_id);
    
    //读取电表当前电量, databuf == NULL表示是过分检查电表停走事件，
    if (databuf == NULL)
    {
        OOP_ENERGY_T    fw_act_en;
        len = get_meter_data_by_oad(clientid, mtr_id, 0x00100200, (uint8*)&fw_act_en, sizeof(OOP_ENERGY_T));
        if (len<0)
        {
            EVENT_FMT_DEBUG("get_meter_data_by_oad[0x00100200]失败info%u\n", mtr_id);
            return -1;
        }
        Pfw_act_en = &fw_act_en;     //正向有功电能
    }
    else //数据变更通知，检查事件
    {
        Pfw_act_en = (OOP_ENERGY_T*)databuf;     //正向有功电能
    }
    
    //互换测试时，有些厂家没有规范填写rsv
    if(Pfw_act_en->nNum < 1 /*|| (Pfw_act_en->rsv&0x00000001) != 1*/)
    {
        EVENT_FMT_DEBUG("evt_check_310X[%hu]  get 0X00100200 err \n", mtr_id);
        return 0;
    }

    met_en_now = Pfw_act_en->nValue[0];
    if (met_en_now == APP_NONELWORD || met_en_now == NONE_LWORD)
    {
        EVENT_FMT_DEBUG("event_310X[%hu]  当前电量 无效 \n", mtr_id);
        return -1;                                  //无效数据返回
    }
    
    //首次,将当前值写入上次状态
    if (bfirst[mtr_id-1] == 0)
    {
        bfirst[mtr_id-1] = 1;
        EVENT_FMT_DEBUG("event_310X[%hu] 首次检测,将当前值写入上次状态\n", mtr_id);
        write_pdata_xram("^Event", (char *)&met_en_now, (MetEnLastRecAddr+(mtr_id-1)*4*4), 4);
        return -1;
    }
    
    //读取上次电量
    len = read_pdata_xram("^Event", (char *)&met_en_last, (MetEnLastRecAddr+(mtr_id-1)*4*4), 4);
    if (len<0)
    {
        EVENT_FMT_DEBUG("event_310X[%hu]读取上次电量失败\n", mtr_id);
        write_pdata_xram("^Event", (char *)&met_en_now, (MetEnLastRecAddr+(mtr_id-1)*4*4), 4);
        return -1;
    }

    if (met_en_last != met_en_now)
    {
        write_pdata_xram("^Event", (char *)&met_en_now, (MetEnLastRecAddr+(mtr_id-1)*4*4), 4);
    }
    if (met_en_last == APP_NONELWORD || met_en_last == NONE_LWORD)//-重新设置过电表后，底数被清为ee-
    {
        EVENT_FMT_DEBUG("CheckERC_310B 电表[%hu]上次电量数据无效\n", mtr_id);
        return -1;
    }
    
    EVENT_FMT_TRACE("event_310X[%hu]上次电量=%u,当前电量=%u\n", mtr_id, met_en_last, met_en_now);

    if(oopMeter.basic.port.nObjID == OI_AC || oopMeter.basic.port.nObjID == OI_485 || oopMeter.basic.port.nObjID == OI_PLC)
    {
        if(oopMeter.basic.port.nObjID == OI_AC )
        {
#if AC261_CALC_EN == 1
            return 0;
#endif
        }
        if(databuf != NULL)
        {
            if(oopMeter.basic.port.nObjID == OI_AC || event_310B.eventCommInfo.checkflag == 0)
            {
                EVENT_FMT_LOGCC("event_310B.checkflag is 0 or 310B[%hu] is not 485[%hu]\n", mtr_id, oopMeter.basic.port.nObjID);
            }
            else
            {
                EnDownEvent(clientid, mtr_id, oopMeter, met_en_now, met_en_last);               //电能表示度下降检测、记录
            }

            if(event_310C.eventCommInfo.checkflag == 0)
            {
                EVENT_FMT_LOGCC("event_310C.checkflag 0\n");
            }
            else
            {
                MetJumpEvent(clientid, mtr_id, oopMeter, met_en_now, met_en_last);              //电表电能量超差检测、记录
            }

            if(event_310D.eventCommInfo.checkflag == 0)
            {
                EVENT_FMT_LOGCC("event_310D.checkflag 0\n");
            }
            else
            {
                MetFlyEvent(clientid, mtr_id, oopMeter, met_en_now, met_en_last);               //电能表飞走检测、记录
            }
        }
        else if(event_310E.eventCommInfo.checkflag != 0)        //电能表停走检测，需要持续一段时间，不能在数据变更通知中检查，需定时检查
        {
            MetStopEvent(clientid, mtr_id, oopMeter, met_en_now, met_en_last);              //电能表停走检测、记录
        }
        else
        {
            EVENT_FMT_LOGCC("event_310E.checkflag 0\n");
        }

    }
    EVENT_FMT_TRACE(" evt_check_310X complete\n");
    return 0;
}

#endif

#if DESC("电能表抄表失败事件",1)
/**********************************************************************
* @name      : evt_check_310F
* @brief     ：电能表抄表失败
* @param[in] ：uint16 mtr_id   信息点号
               OOP_METER_T oopMeter 电表档案
* @param[out]：
* @return    ：0成功/<0异常
* @Create    : 梅安宁
* @Date      ：2020-12-26
* @Update    :
**********************************************************************/
void evt_check_310F(mtrnum_t mtr_id, OOP_METER_T oopMeter)
{
    static uint8 firstRun[VIR_BEGIN] = {0};

    if(firstRun[mtr_id] == 0)
    {
        firstRun[mtr_id] = 1;
        return;
    }

    if(event_310F.eventCommInfo.checkflag == 0)
    {
        EVENT_FMT_LOGCC("event_310F.checkflag 0\n");
        return;
    }

    if(oopMeter.basic.port.nObjID == OI_485)
    {
        //-读计量点类型失败或是测量点不为485不判断-
        EVENT_FMT_TRACE("CheckERC_310F[%hu] starts\n", mtr_id);
        MetReadFailureEvent(mtr_id, oopMeter);
    }

    return;
}
#endif


#if DESC("终端与主站通信流量超门限事件",1)
/**********************************************************************
* @name      : evt_check_3110
* @brief     ：终端与主站通信流量超门限事件
* @param[in] ：int clientid    db客户端
               uint8 logicId   逻辑设备号
               uint16 mtr_id   信息点号
               OOP_METER_T oopMeter 电表档案
               uint8* databuf       事件判断数据
               uint16 datalen       事件判断数据长度
* @param[out]：
* @return    ：0成功/<0异常
* @Create    : 梅安宁
* @Date      ：2020-12-26
* @Update    :
**********************************************************************/
int evt_check_3110(int clientid,uint8 logicid,uint16 mtr_id, OOP_METER_T oopMeter,uint8* databuf,uint16 datalen)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    NOMAL_OAD_T     nomalOAD;
    int ret = 0;
    uint8   flow_flux_flag = 0;
    static  uint8 firstRun = 0;
    char  last_flag = 0;            //上一次超流量标志
    
    memset(&eventSource, 0, sizeof(OOP_EVTSOURCE_T));
    memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
    memset(&nomalOAD, 0, sizeof(NOMAL_OAD_T));

    if(event_3110.eventCommInfo.checkflag == 0)
    {
        EVENT_FMT_LOGCC("event_3110.checkflag 0\n");
        return ret;
    }

    if(event_3110.eventParam.nValue == 0)
    {
        EVENT_FMT_TRACE("event_3110.eventParam 0\n");
        return ret;
    }   
   
    //终端流量超标标志
    OOP_DWORD2_T        *precvdataFlowrate = (OOP_DWORD2_T *)databuf;;   //通信流量
    if(precvdataFlowrate->nValue2 < event_3110.eventParam.nValue)
    {
        flow_flux_flag = 0;
    }
    else
    {
        flow_flux_flag = 1;
    }
    
    
    if(firstRun == 0)
    {
        firstRun =1;
        write_pdata_xram("^Event", (const char *)&flow_flux_flag, FluxOverTopAddr, 1);
        return ret;
    }


    if (read_pdata_xram("^Event", &last_flag, FluxOverTopAddr, 1) < 0)
    {
        EVENT_FMT_DEBUG("evt_check_3110 read_pdata_xram err\n");
        return ret;
    }
    else if (last_flag == 0xff)
    {
        last_flag = 0;
    }
     
    EVENT_FMT_TRACE("evt_check_3110 flag=%d last flag=%d\n", flow_flux_flag, last_flag);

    //记录流量超标标志
    if (last_flag != flow_flux_flag)
    {
        write_pdata_xram("^Event", (const char *)&flow_flux_flag, FluxOverTopAddr, 1);
#if STD_698PRO_BETA
        //事件发生源NULL
        eventSource.choice = DT_NULL;

        //事件关联数据
        
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X22004202;
        eventData.cols.oad[1].value = 0X31100601;

        eventData.idLen[0] = sizeof(uint32);
        eventData.idLen[1] = sizeof(uint32);
        memcpy(eventData.buf + eventData.len, &event_3110.recvdataFlowrate.nValue2, eventData.idLen[0]);
        eventData.len += eventData.idLen[0];
        memcpy(eventData.buf + eventData.len, &event_3110.eventParam.nValue, eventData.idLen[1]);
        eventData.len += eventData.idLen[1];

        if (flow_flux_flag == 1)
        {
            if (last_flag != 1)
            {

                //写数据库
                ret = WriteEventToDB(clientid, 0, &event_3110.eventCommInfo, eventSource, eventData, 1);
                if (ret < ERR_OK)
                {
                    EVENT_FMT_DEBUG("evt_check_3110记录失败返回%d\n", ret);
                }
                else
                    EVENT_FMT_LOGCC("evt_check_3110记录成功, storagenum=%u !\n", event_3110.eventCommInfo.storagenum);

            }
        }
        else
        {
            if (last_flag == 1)
            {

                //写数据库
                ret = WriteEventToDB(clientid, 0, &event_3110.eventCommInfo, eventSource, eventData, 2);
                if (ret < ERR_OK)
                {
                    EVENT_FMT_DEBUG("evt_check_3110恢复事件记录失败返回%d\n", ret);
                }
                else
                    EVENT_FMT_LOGCC("evt_check_3110恢复事件记录成功, storagenum=%u !\n", event_3110.eventCommInfo.storagenum);
            }

        }
#endif
        
    }
    return ret;
}
#endif


/*********************************************************************
所属功能单元：
功能描述：终端对时事件
输入参数：void
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_3114(int fd, OOP_DATETIME_S_T startTm, OOP_DATETIME_S_T endTm)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));


    EVENT_FMT_TRACE("CheckERC_3114 starts\n");
    
    if (event_3114.eventCommInfo.checkflag)                                 //需要记录
    {    
        //事件发生源NULL
        eventSource.choice = DT_NULL;

        //事件关联数据
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X40002200;
        eventData.cols.oad[1].value = 0X40008200;
        eventData.idLen[0] = sizeof(OOP_DATETIME_S_T);
        eventData.idLen[1] = sizeof(OOP_DATETIME_S_T);
        eventData.len = eventData.idLen[0] + eventData.idLen[1];
        memcpy(eventData.buf, &startTm, eventData.idLen[0]);
        memcpy(eventData.buf+eventData.idLen[0], &endTm, eventData.idLen[1]);

        //写数据库
        ret = WriteEventToDB(fd, 0, &event_3114.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3114 error ret%d\n", ret);
            return;
        }

        EVENT_FMT_LOGCC("CheckERC_3114 success, storagenum=%u !\n", event_3114.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_LOGCC("CheckERC_3114 checkflag 0\n");

    return;
}
/*********************************************************************
所属功能单元：
功能描述：终端对电能表校时事件
输入参数：
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_311B(int fd,uint16 infonum,OOP_TSA_T Addr, OOP_DATETIME_S_T preTime, char offset)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));


    EVENT_FMT_DEBUG("CheckERC_311B starts\n");
    
    if (event_311B.eventCommInfo.checkflag)                                 //需要记录
    {    
        //事件发生源tsa
        eventSource.choice = DT_TSA;
        eventSource.add = Addr;
        //事件关联数据
        eventData.oadNum = 2;
        eventData.cols.nNum = 2;
        eventData.cols.oad[0].value = 0X330D0206;
        eventData.cols.oad[1].value = 0X330D0207;
        eventData.idLen[0] = sizeof(OOP_DATETIME_S_T);
        eventData.idLen[1] = sizeof(char);
        eventData.len = eventData.idLen[0] + eventData.idLen[1];
        memcpy(eventData.buf, &preTime, eventData.idLen[0]);
        memcpy(eventData.buf+eventData.idLen[0], &offset, eventData.idLen[1]);
        event_311B.preTime = preTime;
        event_311B.offset = offset;
        //写数据库
        ret = WriteEventToDB(fd, infonum, &event_311B.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_311B 终端对电表‘校时事件记录失败返回%d\n", ret);
            return;
        }

        EVENT_FMT_DEBUG("CheckERC_311B 终端对电表校时事件 记录成功, storagenum=%u !\n", event_311B.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_DEBUG("CheckERC_311B checkflag 0\n");

    return;
}

/*********************************************************************
所属功能单元：
功能描述：拒绝从节点入网记录事件
输入参数：
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_311F(int fd,OOP_REFUSESLAVE_T info)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));


    EVENT_FMT_DEBUG("CheckERC_311F starts\n");
    
    if (event_311F.eventCommInfo.checkflag)                                 //需要记录
    {    
        //事件发生源tsa
        eventSource.choice = DT_NULL;

        //事件关联数据
        eventData.oadNum = 0;
//        eventData.cols.nNum = 1;
//        eventData.cols.oad[0].value = 0X332F0206;
//
//        eventData.idLen[0] = sizeof(OOP_DATETIME_S_T);
//     
//        eventData.len = eventData.idLen[0] + eventData.idLen[1];
//        memcpy(eventData.buf, &preTime, eventData.idLen[0]);
//        memcpy(eventData.buf+eventData.idLen[0], &offset, eventData.idLen[1]);
        event_311F.refuseslave = info;
 
        //写数据库
        ret = WriteEventToDB(fd, 0, &event_311F.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_311F 拒绝从节点入网记录事件记录失败返回%d\n", ret);
            return;
        }

        EVENT_FMT_DEBUG("CheckERC_311F 拒绝从节点入网记录事件 记录成功, storagenum=%u !\n", event_311F.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_DEBUG("CheckERC_311F checkflag 0\n");

    return;
}

/*********************************************************************
所属功能单元：
功能描述：拓扑事件
输入参数：
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
#if AREA_ZHEJIANG
void CheckERC_3E03(int fd,Oop_Identify_Result info,OOP_TSA_T tsa,uint16 pn)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));


    EVENT_FMT_DEBUG("CheckERC_3E03 starts\n");
    
    if (event_3E03.eventCommInfo.checkflag)                                 //需要记录
    {    
        //事件发生源tsa
        eventSource.choice = DT_TSA;
        if(0 == pn)//终端事件
        {
            EVENT_FMT_DEBUG("if(0 == pn) \n");
            eventSource.choice = DT_NULL;
        }
        else
        {
            app_get_meter_tsa_list_by_pn(pn,&eventSource.add);
        }
        
        //事件关联数据
        eventData.oadNum = 0;
        
        event_3E03.IdentifyInformations = info;
        EVENT_FMT_DEBUG("Identifynum=%d, signalphase=%d \n", 
            info.Identifynum, info.IdentifyInformation[0].signalphase);
 
        //写数据库
        ret = WriteEventToDB(fd, pn, &event_3E03.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3E03 拓扑识别记录事件记录失败返回%d\n", ret);
            return;
        }

        EVENT_FMT_DEBUG("CheckERC_3E03 拓扑识别记录事件 记录成功 ret %d, storagenum=%u !\n", ret,event_3E03.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_DEBUG("CheckERC_3E03 checkflag 0\n");

    return;
}
#endif
/*********************************************************************
所属功能单元：
功能描述：跨台区电能表事件
输入参数：
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_3112(int fd,OOP_ACROSSAREA_RES_T info)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;

    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));


    EVENT_FMT_DEBUG("CheckERC_3112 starts\n");

    if (event_3112.eventCommInfo.checkflag)                                 //需要记录
    {    
        //事件发生源tsa
        eventSource.choice = DT_NULL;

        //事件关联数据
        eventData.oadNum = 0;

        event_3112.acrossarea = info;

        //写数据库
        ret = WriteEventToDB(fd, 0, &event_3112.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3112 跨台区电能表事件记录失败返回%d\n", ret);
            return;
        }

        EVENT_FMT_DEBUG("CheckERC_3112 跨台区电能表事件 记录成功, storagenum=%u !\n", event_3112.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_DEBUG("CheckERC_3112 checkflag 0\n");

    return;


}
/*********************************************************************
所属功能单元：
功能描述：模块变更事件
输入参数：
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_3030(int fd,OOP_OAD_U sourceoad,OOP_CHILDNODE_T pre,OOP_CHILDNODE_T now)
{
     OOP_EVTSOURCE_T eventSource; //事件发生源
     OOP_EVTBUF_T    eventData;   //事件关联数据
     int ret = 0;
    
     memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
     memset(&eventData,0,sizeof(OOP_EVTBUF_T));
    
    
     EVENT_FMT_DEBUG("CheckERC_3030 starts\n");
    
     if (event_3030.eventCommInfo.checkflag)                                 //需要记录
     {    
         //事件发生源
         eventSource.choice = DT_OAD;
         eventSource.oad = sourceoad;
         //事件关联数据
         eventData.oadNum = 0;
    
         event_3030.prenode = pre;
         event_3030.nownode = now;
         //写数据库
         ret = WriteEventToDB(fd, 0, &event_3030.eventCommInfo, eventSource, eventData, 0);
         if (ret < ERR_OK)
         {
             EVENT_FMT_DEBUG("CheckERC_3030 模块变更事件记录失败返回%d\n", ret);
             return;
         }
    
         EVENT_FMT_DEBUG("CheckERC_3030 模块变更事件 记录成功, storagenum=%u !\n", event_3030.eventCommInfo.storagenum);
    
     }
     else
         EVENT_FMT_DEBUG("CheckERC_3030 checkflag 0\n");
    
     return;


}


/*********************************************************************
所属功能单元：
功能描述：终端编程事件
输入参数：
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_3118(int fd, uint8 flag,OOP_OAD_U oad)
{
    int              ret = 0;
    static uint32    oadNum= 0;
    static OOP_OAD_U oads[OOP_MAX_OADS];
    OOP_EVTSOURCE_T  eventSource; //事件发生源
    OOP_EVTBUF_T     eventData;   //事件关联数据
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

	if (event_3118.eventCommInfo.checkflag)                                 //需要记录
    {
	    if(flag == PS_ON)
	    {
	        if(oadNum < OOP_MAX_OADS)
	        {
	            oads[oadNum] = oad;
	            oadNum++;
	            return;
	        }
	        else
	        {
	            //达到最大记录数，写事件
	            flag = PS_OK; 
	        }  
	    }

	    if(flag == PS_OK)
	    {
	        EVENT_FMT_TRACE("CheckERC_3118 starts\n");  
            event_3118.nOADNum = oadNum;
            memcpy(event_3118.oad, oads, sizeof(OOP_OAD_U)*oadNum);
            
            //事件发生源NULL
            eventSource.choice = DT_NULL;

            //事件关联数据

            //写数据库
            ret = WriteEventToDB(fd, 0, &event_3118.eventCommInfo, eventSource, eventData, 0);
            if (ret < ERR_OK)
            {
                EVENT_FMT_DEBUG("CheckERC_3118 终端编程事件记录失败返回%d\n", ret);
            }
            else
            {
                EVENT_FMT_LOGCC("CheckERC_3118 终端编程事件 记录成功, storagenum=%u !\n", event_3118.eventCommInfo.storagenum);
            }
	        
	    }

	}
    else
        EVENT_FMT_LOGCC("CheckERC_3118 checkflag 0\n");

    oadNum = 0;
    memset(oads, 0, sizeof(OOP_OAD_U)*OOP_MAX_OADS);
    return;
}



#if DESC("终端相序异常事件",1)
/**********************************************************************
* @name      : evt_check_300F
* @brief     : 终端相序异常事件
* @param[in] ：int clientid    db客户端
               uint8 logicId   逻辑设备号
               uint16 mtr_id   信息点号
               OOP_METER_T oopMeter 电表档案
               uint8* databuf       事件判断数据
               uint16 datalen       事件判断数据长度
* @param[out]：
* @return    ：0成功/<0异常
* @Create    : 梅安宁
* @Date      ：2020-12-26
* @Update    :
**********************************************************************/
int evt_check_300F()
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    static   uint8 happenCnt = 0;                 // 发生持续时间
    static   uint8 restoreCnt = 0;                // 恢复持续时间
    uint32   VoltLimit = 0;                       // 电压限值(.XXXX V)
    uint8    IsHappen   = 0;                      // 是否满足发生条件，1:满足，0:不满足
    uint32   StdVoltValue = 2200;                 //额定电压 单位0.1V
    uint8    cb_flag      = 0;                    //事件发生标记

    EVENT_FMT_TRACE("CheckERC_300F check start!\n");
    
    if (event_300F.eventCommInfo.checkflag == 0)                                 //需要记录
    {
        EVENT_FMT_LOGCC("CheckERC_300F checkflag 0\n");
        return ret;
    }

    // 计算电压限值
    VoltLimit = StdVoltValue * 0.6;

    event_300F.valTemp = tmnVoltage;


    EVENT_FMT_TRACE("evt_check_300F 电压限值VoltLimit=%u A、B、C相电压:%u,%u,%u, metWord%04x!\n", VoltLimit, event_300F.valTemp.nValue[0], event_300F.valTemp.nValue[1],event_300F.valTemp.nValue[2], event_300F.meterRsw.metWord[6]);
    // 三相四线检查A、B、C相电压是否正常
    if ((event_300F.valTemp.nValue[0] > VoltLimit)
        && (event_300F.valTemp.nValue[1] > VoltLimit)
        && (event_300F.valTemp.nValue[2] > VoltLimit))
    {
        
        if((event_300F.meterRsw.metWord[6]&0x0001) == 1)
        {
            IsHappen = 1;
            restoreCnt = 0;
        }
        else
        {
            happenCnt = 0;
        }
    }
    else
    {
        //电压断（缺）相时，不判逆相序，即：任意相电压 < 断相门限，不判逆相序
        EVENT_FMT_TRACE("evt_check_300F 电压异常 电压限值VoltLimit=%u A、B、C相电压:%u,%u,%u!\n", VoltLimit, event_300F.valTemp.nValue[0], event_300F.valTemp.nValue[1],event_300F.valTemp.nValue[2]);
        return ret;
    }

    if(read_pdata_xram("^Event", (char *)&cb_flag, EventFlagPhaseAbRecAddr, 1)<0 || cb_flag == 0xff)
    {
        cb_flag = 0;
        write_pdata_xram("^Event", (const char *)&cb_flag, EventFlagPhaseAbRecAddr, 1);
        EVENT_FMT_DEBUG("evt_check_300F 终端相序异常事件 获取事件发生标记失败或标记无效\n");
        return ret;
    }

    EVENT_FMT_TRACE("evt_check_300F IsHappen=%u , cb_flag=%u!\n", IsHappen, cb_flag);
    EVENT_FMT_TRACE("evt_check_300F happenCnt=%u , restoreCnt=%u, event_300F.nDelay=%u!\n", happenCnt, restoreCnt, event_300F.nDelay);

    if(IsHappen == 1 && cb_flag == 0)
    {
        happenCnt ++;
        restoreCnt = 0;
    }
    else if(IsHappen != 1 && cb_flag == 1)
    {
        restoreCnt ++;
        happenCnt = 0;
    }    

    if(happenCnt > event_300F.nDelay) //发生
    {
        happenCnt = 0;
        cb_flag = 1;
        write_pdata_xram("^Event", (const char *)&cb_flag, EventFlagPhaseAbRecAddr, 1);
    }
    else if(restoreCnt > event_300F.nDelay) //恢复
    {
        restoreCnt = 0;
        cb_flag = 2;
        write_pdata_xram("^Event", (const char *)&cb_flag, EventFlagPhaseAbRecAddr, 1);
    }
    else
        return ret;
        
    //事件发生源NULL
    eventSource.choice = DT_NULL;

    //事件关联数据
    get_evt_data_300F(gDbCenterClient, cb_flag, &eventData);

    ret = WriteEventToDB(gDbCenterClient, 0, &event_300F.eventCommInfo, eventSource, eventData, cb_flag);
    if (ret < ERR_OK)
    {
        EVENT_FMT_DEBUG("evt_check_300F 终端相序异常事件%hu记录失败返回%d\n", cb_flag, ret);
    }
    else
        EVENT_FMT_LOGCC("evt_check_300F 终端相序异常事件%hu 记录成功, storagenum=%u !\n", cb_flag, event_300F.eventCommInfo.storagenum);


    return ret;
}
#endif

/*********************************************************************
所属功能单元：
功能描述：安全事件变更记录单元
输入参数：测量点号
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_3140(int fd, uint32 msg_id, uint8 *pmsg, uint16 msglen)
{
    int              ret = 0;
    OOP_EVTSOURCE_T  eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    EVENT_COMM_INFO  eventCommInfo;
    uint32 offset = 0;
    uint32 i = 0;

    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData, 0, sizeof(OOP_EVTBUF_T));
    memcpy(&eventCommInfo, &event_3140.eventCommInfo, sizeof(EVENT_COMM_INFO));
    memset(&event_3140, 0, sizeof(EVENT_3140_INFO));
    memcpy(&event_3140.eventCommInfo, &eventCommInfo, sizeof(EVENT_COMM_INFO));
    
    EVENT_FMT_TRACE("CheckERC_3140 starts\n");
    
    if (event_3140.eventCommInfo.checkflag)//需要记录
    {   
        //事件发生源NULL
        eventSource.choice = DT_NULL;

        switch(msg_id)
        {
            case MSG_APPAMIN_ETH_PORT_OPEN: //以太网远程端口开放监控
            {
                event_3140.flag.nValue[0] |= 0x01;
                event_3140.nEthPort = pmsg[offset++];

                if(event_3140.nEthPort > OOP_MAX_SECURITYCHGS)
                {
                    EVENT_FMT_DEBUG("以太网端口开放监控数量越限. nEthPort(%d) > OOP_MAX_SECURITYCHGS(%d)\n", event_3140.nEthPort, OOP_MAX_SECURITYCHGS);
                    return;
                }

                EVENT_FMT_DEBUG("以太网端口开放nNum[%d].\n", event_3140.nEthPort);
                
                for(i = 0; i < event_3140.nEthPort; i++)
                {
                    memcpy_rs(&event_3140.ethPort[i], sizeof(uint16), &pmsg[offset], sizeof(uint16));
                    offset += sizeof(uint16);

                    EVENT_FMT_DEBUG("开放端口[%d] = %d\n", i, event_3140.ethPort[i]);
                }
                
            }break;

            case MSG_APPAMIN_ETH_PORT_CONN: //以太网外联信息监控
            {
                event_3140.flag.nValue[0] |= 0x02;
                event_3140.nEthConnects = pmsg[offset++];

                if(event_3140.nEthConnects > OOP_MAX_SECURITYCHGS)
                {
                    EVENT_FMT_DEBUG("Ethernet external connects info get failed. nEthPort(%d) > OOP_MAX_SECURITYCHGS(%d)\n", event_3140.nEthPort, OOP_MAX_SECURITYCHGS);
                    return;
                }

                EVENT_FMT_DEBUG("以太网外联事件发生. nNum[%d]\n", event_3140.nEthConnects);
                
                for(i = 0; i < event_3140.nEthConnects; i++)
                {
                    apdu_octetstrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.ethConnects[i].ip, sizeof(OOP_OCTETVAR16_T));
                    
                    memcpy_rs(&event_3140.ethConnects[i].port, sizeof(uint16), &pmsg[offset], sizeof(uint16));
                    offset += sizeof(uint16);

                    
                    EVENT_FMT_DEBUG("外联IP：%d.%d.%d.%d 外联端口：%d\n", 
                        event_3140.ethConnects[i].ip.value[0],event_3140.ethConnects[i].ip.value[1],
                        event_3140.ethConnects[i].ip.value[2],event_3140.ethConnects[i].ip.value[3],
                        event_3140.ethConnects[i].port);
                }
            }break;

            case MSG_APPAMIN_LOCAL_MAINTAIN: //本地维护信息监控
            {
                #if SEC_MONITOR_NEW_22 == 1
                event_3140.flag.nValue[1] |= 0x08;
                event_3140.errSSH = pmsg[offset++]+1;
                EVENT_FMT_DEBUG("SSH服务异常事件发生 异常类型[%d].\n", event_3140.errSSH);
                #else
                if(pmsg[offset] == 0) //SSH服务异常
                {
                    event_3140.flag.nValue[0] |= 0x20;
                    event_3140.rs232Info.nValue |= 0x08; //D3: 1-SSH服务安全异常
                    EVENT_FMT_DEBUG("SSH service abnormal event happen.\n");
                }
                else if(pmsg[offset] == 1) //Root账户口令变更
                {
                    event_3140.flag.nValue[1] |= 0x04; //Root账户口令变更
                    EVENT_FMT_DEBUG("Root account password changed event happen.\n");
                }                
                offset++;
                #endif
                
            }break;                   
                
            case MSG_APPAMIN_KEY_PATH_CHG: //关键目录文件变更监控
            {
                event_3140.flag.nValue[0] |= 0x40;
                event_3140.nChgPath = pmsg[offset++];
            
                if(event_3140.nChgPath > OOP_MAX_SECURITYCHGS)
                {
                    EVENT_FMT_DEBUG("关键目录文件变更数量越限. nEthPort(%d) > OOP_MAX_SECURITYCHGS(%d)\n", event_3140.nEthPort, OOP_MAX_SECURITYCHGS);
                    return;
                }
                
                EVENT_FMT_DEBUG("关键目录文件变更事件发生.nNum[%d]\n", event_3140.nChgPath);
                
                for(i = 0; i < event_3140.nChgPath; i++)
                {
                    apdu_visiblestrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.chgPath[i], sizeof(OOP_VISIBLEVAR_T));
                    EVENT_FMT_DEBUG("变更文件 %s\n", event_3140.chgPath[i].value);
                }
            }break;

            case MSG_APPAMIN_ETH_LOGIN_CONSOLE: //以太网登录控制台监控
            {
                event_3140.flag.nValue[0] |= 0x80;
                if(pmsg[offset] == 0) //登录控制台
                {
                    event_3140.ethFlag.nValue |= 0x01; //D0: 1-以太网登录控制台成功
                    EVENT_FMT_DEBUG("以太网登录控制台.\n");
                }
                else if(pmsg[offset] == 1) //退出控制台
                {
                    event_3140.ethFlag.nValue |= 0x02; //D1: 1-以太网退出登录控制台
                    EVENT_FMT_DEBUG("以太网退出控制台.\n");
                }
                else if(pmsg[offset] == 2) //登录失败
                {
                    event_3140.ethFlag.nValue |= 0x04; //D2: 1-以太网登录控制台失败
                    EVENT_FMT_DEBUG("以太网登录失败.\n");
                }
                offset++;
            }break;    

           case MSG_APPAMIN_DANGER_OPREATE: //危险命令操作监控
           {
                event_3140.flag.nValue[1] |= 0x02;
                 if(pmsg[offset] == 0) //删除
                {
                    event_3140.errFlag.nValue |= 0x01; //D0: 1-删除
                    EVENT_FMT_DEBUG("危险命令 删除.\n");
                }
                else if(pmsg[offset] == 1) //重命名
                {
                    event_3140.errFlag.nValue |= 0x02; //D1: 1-重命名
                    EVENT_FMT_DEBUG("危险命令 重命名.\n");
                }    
                else if(pmsg[offset] == 2) //复制
                {
                    event_3140.errFlag.nValue |= 0x04; //D2: 1-复制
                    EVENT_FMT_DEBUG("危险命令 复制.\n");
                }   
                else if(pmsg[offset] == 3) //进程结束
                {
                    event_3140.errFlag.nValue |= 0x08; //D3: 1-进程结束
                    EVENT_FMT_DEBUG("危险命令 进程结束.\n");
                }

                offset++;
            }break;  
            
            case MSG_APPAMIN_UDISK_AUTHORIZE: //USB设备接入信息监控
            {
                event_3140.flag.nValue[0] |= 0x10;
                if(pmsg[offset] == 0) //USB端口未授权
                {
                    event_3140.usbInfo.nValue |= 0x01; //D0: 1-USB端口未授权
                }
                else if(pmsg[offset] == 1) //USB端口本地认证失败
                {
                    event_3140.usbInfo.nValue |= 0x02; //D1: 1-USB端口本地认证失败
                }
                else if(pmsg[offset] == 2) //USB端口未授权，并且本地认证失败
                {
                    event_3140.usbInfo.nValue |= 0x03;
                }                
                offset++;

                EVENT_FMT_DEBUG("USB设备非法接入.\n");
            }break;  

            case MSG_APPAMIN_PORT485_AUTHORIZE://485维护口接入信息监控
            {
                event_3140.flag.nValue[0] |= 0x20;
                event_3140.rs232Info.nValue |= 0x02; //D1: 1-维护RS485端口安全信息异常

                offset++;
                
                EVENT_FMT_DEBUG("RS485维护口非法接入.\n");
            }break;         

            case MSG_APPAMIN_PASSWARD_CHANGED://口令信息变更异常检测
            {
                event_3140.flag.nValue[1] |= 0x04; //口令信息变更异常检测
                EVENT_FMT_DEBUG("口令信息变更异常事件发生 flag[0x%x]\n", event_3140.flag.nValue[1]);
            }break;         
            
            case MSG_APPAMIN_UART_PORT_LOGIN://串口登录控制台检测
            {
                event_3140.flag.nValue[1] |= 0x01; //串口登录控制台检测
                EVENT_FMT_DEBUG("串口登录控制台异常事件发生 flag[0x%x]\n", event_3140.flag.nValue[1]);
                if (pmsg[offset] == 0) //登录控制台
                {
                    event_3140.rs232Flag.nValue |= 0x01; //D0: 1-串口登录控制台成功
                    EVENT_FMT_DEBUG("串口登录控制台事件发生 value[%d]\n", event_3140.rs232Flag.nValue);
                }
                else if (pmsg[offset] == 1) //退出控制台
                {
                    event_3140.rs232Flag.nValue |= 0x02; //D1: 1-串口退出登录控制台
                    EVENT_FMT_DEBUG("串口退出控制台事件发生 value[%d]\n", event_3140.rs232Flag.nValue);
                }
                else if (pmsg[offset] == 2) //登录失败
                {
                    event_3140.rs232Flag.nValue |= 0x04; //D2: 1-串口登录控制台失败
                    EVENT_FMT_DEBUG("串口登录控制台失败事件发生 value[%d]\n", event_3140.rs232Flag.nValue);
                }
                offset++;
            }break; 

            case MSG_APPAMIN_SESSION_ABNORMAL: //流量会话异常监控
            {
                event_3140.flag.nValue[1] |= 0x10;
                int num = pmsg[offset++];

                if (num > OOP_MAX_SECURITYCHGS)
                {
                    EVENT_FMT_DEBUG("流量会话异常监控数量越限. num(%d) > OOP_MAX_SECURITYCHGS(%d)\n", num, OOP_MAX_SECURITYCHGS);
                    return;
                }

                EVENT_FMT_DEBUG("流量会话异常监控事件发生 num[%d]\n", num);

                //698规约和消息不一致，取最后一个事件
                for (i = 0; i < num; i++)
                {
                    apdu_octetstrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errSession.ipRemote, sizeof(OOP_OCTETVAR16_T));
                    
                    memcpy_rs(&event_3140.errSession.portRemote, sizeof(uint16), &pmsg[offset], sizeof(uint16));
                    offset += sizeof(uint16);

                    apdu_octetstrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errSession.ipLocal, sizeof(OOP_OCTETVAR16_T));
                    
                    memcpy_rs(&event_3140.errSession.portLocal, sizeof(uint16), &pmsg[offset], sizeof(uint16));
                    offset += sizeof(uint16);

                    apdu_visiblestrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errSession.proto, sizeof(OOP_VISIBLEVAR_T));
                    
                    apdu_visiblestrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errSession.prog, sizeof(OOP_VISIBLEVAR_T));

                    event_3140.errSession.progType = pmsg[offset++];

                    EVENT_FMT_DEBUG("远程[%d] = %d.%d.%d.%d ,port = %d\n", i, 
                        event_3140.errSession.ipRemote.value[0],event_3140.errSession.ipRemote.value[1],
                        event_3140.errSession.ipRemote.value[2],event_3140.errSession.ipRemote.value[3],
                        event_3140.errSession.portRemote);

                    EVENT_FMT_DEBUG("本地[%d] = %d.%d.%d.%d ,port = %d\n", i, 
                        event_3140.errSession.ipLocal.value[0],event_3140.errSession.ipLocal.value[1],
                        event_3140.errSession.ipLocal.value[2],event_3140.errSession.ipLocal.value[3],
                        event_3140.errSession.portLocal);

                    EVENT_FMT_DEBUG("协议[%d] = %s\n", i, event_3140.errSession.proto.value);

                    EVENT_FMT_DEBUG("关联进程[%d] = %s\n", i, event_3140.errSession.prog.value);

                    EVENT_FMT_DEBUG("进程类型[%d] = %d\n", i, event_3140.errSession.progType);
                }
            }break;

            case MSG_APPAMIN_BANDWIDTH_ABNORMAL: //流量带宽异常监控
            {
                event_3140.flag.nValue[1] |= 0x20;
                int num = pmsg[offset++];

                if (num > OOP_MAX_SECURITYCHGS)
                {
                    EVENT_FMT_DEBUG("流量带宽异常监控数量越限. num(%d) > OOP_MAX_SECURITYCHGS(%d)\n", num, OOP_MAX_SECURITYCHGS);
                    return;
                }

                EVENT_FMT_DEBUG("流量带宽异常监控事件发生 num[%d]\n", num);

                //698规约和消息不一致，取最后一个事件
                for (i = 0; i < num; i++)
                {
                    apdu_octetstrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errStream.ipRemote, sizeof(OOP_OCTETVAR16_T));
                    
                    memcpy_rs(&event_3140.errStream.portRemote, sizeof(uint16), &pmsg[offset], sizeof(uint16));
                    offset += sizeof(uint16);

                    apdu_octetstrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errStream.ipLocal, sizeof(OOP_OCTETVAR16_T));
                    
                    memcpy_rs(&event_3140.errStream.portLocal, sizeof(uint16), &pmsg[offset], sizeof(uint16));
                    offset += sizeof(uint16);

                    apdu_visiblestrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errStream.proto, sizeof(OOP_VISIBLEVAR_T));
                    
                    apdu_visiblestrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errStream.prog, sizeof(OOP_VISIBLEVAR_T));

                    memcpy_rs(&event_3140.errStream.streamLimit, sizeof(uint32), &pmsg[offset], sizeof(uint32));
                    offset += sizeof(uint32);

                    memcpy_rs(&event_3140.errStream.streamCur, sizeof(uint32), &pmsg[offset], sizeof(uint32));
                    offset += sizeof(uint32);

                    EVENT_FMT_DEBUG("远程[%d] = %d.%d.%d.%d ,port = %d\n", i, 
                        event_3140.errStream.ipRemote.value[0],event_3140.errStream.ipRemote.value[1],
                        event_3140.errStream.ipRemote.value[2],event_3140.errStream.ipRemote.value[3],
                        event_3140.errStream.portRemote);

                    EVENT_FMT_DEBUG("本地[%d] = %d.%d.%d.%d ,port = %d\n", i, 
                        event_3140.errStream.ipLocal.value[0],event_3140.errStream.ipLocal.value[1],
                        event_3140.errStream.ipLocal.value[2],event_3140.errStream.ipLocal.value[3],
                        event_3140.errStream.portLocal);

                    EVENT_FMT_DEBUG("协议[%d] = %s\n", i, event_3140.errStream.proto.value);

                    EVENT_FMT_DEBUG("进程[%d] = %s\n", i, event_3140.errStream.prog.value);

                    EVENT_FMT_DEBUG("流量限值[%d] = %d\n", i, event_3140.errStream.streamLimit);

                    EVENT_FMT_DEBUG("当前流量[%d] = %d\n", i, event_3140.errStream.streamCur);
                }
            }break;

            case MSG_APPAMIN_PROCESS_ABNORMAL: //进程异常监控
            {
                event_3140.flag.nValue[1] |= 0x40;
                int num = pmsg[offset++];

                if (num > OOP_MAX_SECURITYCHGS)
                {
                    EVENT_FMT_DEBUG("进程异常监控数量越限. num(%d) > OOP_MAX_SECURITYCHGS(%d)\n", num, OOP_MAX_SECURITYCHGS);
                    return;
                }

                EVENT_FMT_DEBUG("进程异常监控事件发生 num[%d]\n", num);

                //698规约和消息不一致，取最后一个事件
                for (i = 0; i < num; i++)
                {
                    event_3140.errProg.type = pmsg[offset++];
                        
                    apdu_datetime_s_fromfmt698(pmsg, msglen, &offset, &event_3140.errProg.dtStart, sizeof(OOP_DATETIME_S_T));

                    apdu_visiblestrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errProg.prog, sizeof(OOP_VISIBLEVAR_T));
                    
                    apdu_visiblestrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errProg.progUser, sizeof(OOP_VISIBLEVAR_T));

                    apdu_visiblestrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errProg.progGroup, sizeof(OOP_VISIBLEVAR_T));

                    apdu_visiblestrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errProg.progCmd, sizeof(OOP_VISIBLEVAR_T));

                    EVENT_FMT_DEBUG("异常类型[%d] = %d\n", i, event_3140.errProg.type);

                    EVENT_FMT_DEBUG("启动时间[%d] = %d-%d-%d %d:%d:%d\n", i, 
                        event_3140.errProg.dtStart.year, event_3140.errProg.dtStart.month,
                        event_3140.errProg.dtStart.day, event_3140.errProg.dtStart.hour, 
                        event_3140.errProg.dtStart.minute, event_3140.errProg.dtStart.second);

                    EVENT_FMT_DEBUG("进程名[%d] = %s\n", i, event_3140.errProg.prog.value);

                    EVENT_FMT_DEBUG("进程User[%d] = %s\n", i, event_3140.errProg.progUser.value);

                    EVENT_FMT_DEBUG("进程Group[%d] = %s\n", i, event_3140.errProg.progGroup.value);

                    EVENT_FMT_DEBUG("进程Cmd[%d] = %s\n", i, event_3140.errProg.progCmd.value);
                }
            }break;

            case MSG_APPAMIN_LOGIN_ABNORMAL: //用户登录异常监控
            {
                event_3140.flag.nValue[1] |= 0x80;

                EVENT_FMT_DEBUG("用户登录异常监控事件发生\n");

                event_3140.errLogin.type = pmsg[offset++];

                apdu_visiblestrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errLogin.user, sizeof(OOP_VISIBLEVAR_T));

                apdu_visiblestrvar_fromfmt698(pmsg, msglen, &offset, &event_3140.errLogin.source, sizeof(OOP_VISIBLEVAR_T));

                EVENT_FMT_DEBUG("异常类型 = %d\n", event_3140.errLogin.type);

                EVENT_FMT_DEBUG("登录用户 = %s\n", event_3140.errLogin.user.value);

                EVENT_FMT_DEBUG("ip名称 = %s\n", event_3140.errLogin.source.value);
            }break;
        }

        //字节调换顺序 先临时在这里改
        memrev(event_3140.flag.nValue, 2);

        //写数据库
        ret = WriteEventToDB(fd, 0, &event_3140.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3140 安全事件变更记录失败返回, %d\n", ret);
        }
        else
        {
            EVENT_FMT_TRACE("CheckERC_3140 安全事件变更记录成功, storagenum=%u!\n", event_3140.eventCommInfo.storagenum);
        }
    }
    else
    {
        EVENT_FMT_TRACE("CheckERC_3140 checkflag 0\n");
    }

    return;
}

#if DESC("电能表数据变更监控",1)
/**********************************************************************
* @name      : evt_check_311C
* @brief     ：电能表数据变更事件
* @param[in] ：uint16 mtr_id   信息点号
               OOP_METER_T oopMeter 电表档案               
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-12-26
* @Update    :
**********************************************************************/
void evt_check_311C(mtrnum_t mtr_id, OOP_METER_T oopMeter)
{
    int    ret          = 0;
    uint8  i            = 0;
    int    csdDataLen   = 0;
    OOP_EVTSOURCE_T     eventSource; //事件发生源
    OOP_EVTBUF_T        eventData;   //事件关联数据
    OOP_RECORD_COL_T    recordCol;

    memset(&eventSource, 0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,   0,sizeof(OOP_EVTBUF_T));
    memset(&recordCol,   0, sizeof(OOP_RECORD_COL_T));

    EVENT_FMT_TRACE("CheckERC_311C check start!\n");
    
    if (event_311C.eventCommInfo.checkflag == 0)                                 //不需要记录
    {
        EVENT_FMT_LOGCC("CheckERC_311C checkflag 0\n");
        return;
    }

  
    if(GetCSDFromPlanByTaskID(event_311C.nTID, &recordCol) <0)
    {
        EVENT_FMT_DEBUG("CheckERC_311C 获取采集方案csd失败\n");
        return;
    }

    for(i=0; i<recordCol.num; i++)
    {
        //目前为了跑台体只处理oad模式的
        //ROAD模式因协议有不确定因素暂不处理
        EVENT_FMT_TRACE("CheckERC_311C[%hu] choice%d\n", mtr_id, recordCol.csd[i].choice);
        if(recordCol.csd[i].choice == 0)
        {
            csdDataLen = evt_check_csd_data(mtr_id, recordCol.csd[i].oad, event_311C.pre, event_311C.aft);
            //数据获取到了，下一步要把数据写入E_DATA
            EVENT_FMT_TRACE("CheckERC_311C[%hu] csdDataLen%d\n", mtr_id, csdDataLen);
            //写E_DATA之前要先把本条事件记录的3302后三个oad长度写进去
            if(csdDataLen > 0)
            {
                //事件发生源
                eventSource.choice = DT_TSA;
                eventSource.add= oopMeter.basic.tsa;

                //事件关联数据
                event_311C.di = recordCol.csd[i];
                event_311C.preLen = csdDataLen;
                event_311C.aftLen = csdDataLen;

                EVENT_BUF_TRACE(event_311C.pre, csdDataLen, "CheckERC_311C[%hu] oad%08x,pre value:\n", mtr_id, recordCol.csd[i].oad.value);
                EVENT_BUF_TRACE(event_311C.aft, csdDataLen, "CheckERC_311C[%hu] oad%08x,aft value:\n", mtr_id, recordCol.csd[i].oad.value);
                
                //写数据库
                ret = WriteEventToDB(gDbCenterClient, mtr_id-1, &event_311C.eventCommInfo, eventSource, eventData, 0);
                if (ret < ERR_OK)
                {
                    EVENT_FMT_DEBUG( "CheckERC_311C[%hu] 电能表数据变更监控记录失败返回%d\n", mtr_id, ret);
                    return;
                }
                EVENT_FMT_LOGCC( "CheckERC_311C[%hu] 电能表数据变更监控记录成功 storagenum=%u \n", mtr_id, event_311C.eventCommInfo.storagenum);

            }
        }

    }

    return;
}

#endif

/**********************************************************************
* @name      : CheckERC_3117
* @brief     : 输出回路接入状态变位事件记录
* @param[in] ：int fd                           //数据库操作句柄
               OOP_SWITCHOUT_T switchOut       //继电器输出状态
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
void CheckERC_3117(int fd, OOP_SWITCHOUT_EVENT_T switchOut)
{
    int ret = 0;
    OOP_EVTSOURCE_T     eventSource; //事件发生源
    OOP_EVTBUF_T        eventData;   //事件关联数据
    
    memset(&eventSource, 0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,   0,sizeof(OOP_EVTBUF_T));


    EVENT_FMT_TRACE("CheckERC_3117 starts\n");
    
    if (event_3117.eventCommInfo.checkflag)                                 //需要记录
    {
        //事件发生源
        
        //事件关联数据
        eventData.oadNum = 1;
        eventData.cols.nNum = 1;
        eventData.cols.oad[0].value = 0Xf2050200;
        eventData.idLen[0] = sizeof(OOP_SWITCHOUT_EVENT_T);
        eventData.len = eventData.idLen[0];
        memcpy(eventData.buf, &switchOut, eventData.idLen[0]);

        //写数据库
        ret = WriteEventToDB(fd, 0, &event_3117.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3117 输出回路接入状态变位事件记录%d\n", ret);
            return;
        }

        EVENT_FMT_TRACE("CheckERC_3117 输出回路接入状态变位事件记录 记录成功, storagenum=%u !\n", event_3117.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_LOGCC("CheckERC_3117 checkflag 0\n");

    return;
}

/**********************************************************************
* @name      : CheckERC_3115
* @brief     : 遥控跳闸事件记录
* @param[in] ：int fd                           //数据库操作句柄
               OOP_OAD_U SourceOAD             //数据发生源，继电器单元oda
               uint8 rCtrlType				   //遥控命令类型 0跳闸，1合闸，2直接合闸
               int64 llPower                   //跳闸时候的总加功率（总加组8个）
               int64 prePower                  //跳闸后2分钟的功率（总加组8个）
               uint8 isUpdate                  // 1 表示遥控跳闸事件发生；2、表示2分钟后更新总加功率
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
void CheckERC_3115(int fd, OOP_OAD_U SourceOAD, uint8 rCtrlType, int64 llPower[], int64 prePower[], uint8 isUpdate)
{
    int ret = 0;
    OOP_EVTSOURCE_T     eventSource; //事件发生源
    OOP_EVTBUF_T        eventData;   //事件关联数据
    
    memset(&eventSource, 0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,   0,sizeof(OOP_EVTBUF_T));


    EVENT_FMT_TRACE("CheckERC_3115 starts\n");
    
    if (event_3115.eventCommInfo.checkflag)                                 //需要记录
    {
        //事件发生源
        eventSource.choice = DT_OAD;
        eventSource.oad = SourceOAD;
        
        //事件关联数据
        memcpy(event_3115.prePower, prePower, sizeof(int64)*8);
        memcpy(event_3115.llPower, llPower, sizeof(int64)*8);
		event_3115.rCtrlType = rCtrlType;

		event_3115.turn = SourceOAD.nIndex;
		if (event_3115.turn < 1)		//容错
        {
            event_3115.turn = 1;
        }

        //写数据库
        ret = WriteEventToDB(fd, 0, &event_3115.eventCommInfo, eventSource, eventData, isUpdate);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3115 遥控跳闸事件记录失败返回%d\n", ret);
            return;
        }

        EVENT_FMT_TRACE("CheckERC_3115 遥控跳闸事件 记录成功, storagenum=%u !\n", event_3115.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_LOGCC("CheckERC_3115 checkflag 0\n");

    return;
}

/**********************************************************************
* @name      : CheckERC_3200
* @brief     : 功控跳闸事件记录
* @param[in] ：int fd                         数据库操作句柄
               OI    SourceOI                 数据发生源
               OI    objCtrl                  控制对象
               OOP_BITSTR8_T turnState        跳闸轮次
               int64 llSetting                功控定值
               int64 power2Min                事件发生后 2 分钟功率
               int64 power                    跳闸时候的总加功率
               uint8 isUpdate                 1 表示功控跳闸事件发生；2、表示2分钟后更新总加功率
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
void CheckERC_3200(int fd, OI SourceOI, OI objCtrl, OOP_BITSTR8_T turnState, int64 llSetting, int64 power2Min, int64 power, uint8 isUpdate)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
	
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    EVENT_FMT_TRACE("CheckERC_3115 starts\n");
    
    if (event_3200.eventCommInfo.checkflag)                                 //需要记录
    { 
        //事件关联数据
        event_3200.power2Min = power2Min;
        event_3200.llSetting = llSetting;
        event_3200.objCtrl = objCtrl;
        event_3200.turnState = turnState;
        event_3200.power = power;

        //事件发生源
        eventSource.choice = DT_OI;
        eventSource.oi = SourceOI;

		int i;
		for (i=0; i<8; i++)	
		{
			if(((turnState.nValue>>i)&0x01) == 1)
			{
				event_3200.turn = i+1;
				break;
			}
		}
		if (event_3200.turn < 1)		//容错
        {
            event_3200.turn = 1;
        }

        //写数据库
        ret = WriteEventToDB(fd, 0, &event_3200.eventCommInfo, eventSource, eventData, isUpdate);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3200 功控跳闸事件记录失败返回%d\n", ret);
            return;
        }

        EVENT_FMT_TRACE("CheckERC_3200 功控跳闸事件 记录成功, storagenum=%u !\n", event_3200.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_LOGCC("CheckERC_3200 checkflag 0\n");

    return;
}

/**********************************************************************
* @name      : CheckERC_3201
* @brief     : 电控跳闸事件记录
* @param[in] ：int fd                         数据库操作句柄
               OI    SourceOI                 数据发生源
               OI    objCtrl                  控制对象
               OOP_BITSTR8_T turnState        跳闸轮次
               int64 llSetting                电控定值
               int64 energy                   跳闸发生时总加组电能量
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
void CheckERC_3201(int fd, OI SourceOI, OI objCtrl, OOP_BITSTR8_T turnState, int64 llSetting, int64 energy)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    EVENT_FMT_TRACE("CheckERC_3201 starts\n");
    
    if (event_3201.eventCommInfo.checkflag)                                 //需要记录
    { 
        //事件关联数据
        event_3201.llSetting = llSetting;
        event_3201.objCtrl = objCtrl;
        event_3201.turnState = turnState;
        event_3201.energy = energy;

        //事件发生源
        eventSource.choice = DT_OI;
        eventSource.oi = SourceOI;

        //写数据库
        ret = WriteEventToDB(fd, 0, &event_3201.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3201 电控跳闸事件记录失败返回%d\n", ret);
            return;
        }

        EVENT_FMT_TRACE("CheckERC_3201 电控跳闸事件 记录成功, storagenum=%u !\n", event_3201.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_LOGCC("CheckERC_3201 checkflag 0\n");

    return;
}

/**********************************************************************
* @name      : CheckERC_3202
* @brief     : 购电参数设置事件记录
* @param[in] ：int fd                          数据库操作句柄
               OI SourceOI                    数据发生源
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
void CheckERC_3202(int fd, OI SourceOI, OOP_CFGUNIT_810C_T cfgunit)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    EVENT_FMT_TRACE("CheckERC_3202 starts\n");
    
    if (event_3202.eventCommInfo.checkflag)                                 //需要记录
    { 
        //事件关联数据
        eventData.oadNum = 1;
        eventData.cols.nNum = 1;
        eventData.cols.oad[0].value = 0X810C2200;
        eventData.idLen[0] = sizeof(OOP_CFGUNIT_810C_T);
        eventData.len = sizeof(OOP_CFGUNIT_810C_T);
        memcpy(eventData.buf, &cfgunit, eventData.len);

        //事件发生源
        eventSource.choice = DT_OI;
        eventSource.oi = SourceOI;

        //写数据库
        ret = WriteEventToDB(fd, 0, &event_3202.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3202 购电参数设置事件记录失败返回%d\n", ret);
            return;
        }

        EVENT_FMT_TRACE("CheckERC_3202 购电参数设置事件 记录成功, storagenum=%u !\n", event_3202.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_LOGCC("CheckERC_3202 checkflag 0\n");

    return;
}

/**********************************************************************
* @name      : CheckERC_3203
* @brief     : 电控告警事件记录
* @param[in] ：int fd                          数据库操作句柄
               OI    SourceOI                 数据发生源
               OI    objCtrl                  控制对象
               int64 llSetting                电控定值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
void CheckERC_3203(int fd, OI SourceOI, OI objCtrl, int64 llSetting)
{
    OOP_EVTSOURCE_T eventSource; //事件发生源
    OOP_EVTBUF_T    eventData;   //事件关联数据
    int ret = 0;
    
    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    EVENT_FMT_TRACE("CheckERC_3203 starts\n");
    
    if (event_3203.eventCommInfo.checkflag)                                 //需要记录
    { 
        //事件关联数据
        event_3203.llSetting = llSetting;
        event_3203.objCtrl = objCtrl;

        //事件发生源
        eventSource.choice = DT_OI;
        eventSource.oi = SourceOI;

        //写数据库
        ret = WriteEventToDB(fd, 0, &event_3203.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3203 电控告警事件记录失败返回%d\n", ret);
            return;
        }

        EVENT_FMT_TRACE("CheckERC_3203 电控告警事件 记录成功, storagenum=%u !\n", event_3203.eventCommInfo.storagenum);

    }
    else
        EVENT_FMT_LOGCC("CheckERC_3203 checkflag 0\n");

    return;
}

int creat_unmeterlog_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    uint8 i=0;
    uint8 index=0;
    OOP_UNMETERLOG_T unmeterlog;
    OOP_ACROSSAREA_RES_T acrossarer;
    WRITE_RECORD_T      recInData;
    READ_RECORD_T       startReadRecord;
    RECORD_UNIT_T       stepRecordOut;
    UPDATE_RECORD_T     updateRecordData;
    time_t              timep   = 0;
    uint32 storeNo=0;

    PRTL_FMT_DEBUG("creat_unmeterlog_event\n");
  
    
    int ret = 0;
    NOMAL_OAD_T nomal = {0};
    uint32 len = 0;

    MEMZERO(&acrossarer, sizeof(acrossarer));
    MEMZERO(&recInData, sizeof(recInData));
    MEMZERO(&startReadRecord, sizeof(startReadRecord));
    MEMZERO(&stepRecordOut, sizeof(stepRecordOut));
    MEMZERO(&updateRecordData, sizeof(updateRecordData));
    MEMZERO(&unmeterlog, sizeof(unmeterlog));

    nomal.infoNum = infoNum;
    nomal.logicId = 0;
    nomal.oad.value = 0x60020500;
    PRTL_FMT_DEBUG("产生台区区分告警,nomal.oad.value is %x\n",nomal.oad.value);
    ret = db_read_nomal(clientid, &nomal, sizeof(acrossarer), (uint8*)&acrossarer, &len);
    if((ret != 0 )||(len !=sizeof(acrossarer)))
    {
        RPT_FMT_TRACE("tmn_port_authorize read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(acrossarer));
        return ret;
    }

    startReadRecord.recordOAD.optional = 0;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.road.oadCols.oad[0].value= 0x31120200;
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = 0;
    startReadRecord.cType = COL_TM_STORE;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = time(NULL)+1;
    startReadRecord.sortType = 1;
    startReadRecord.MAC = MAC;

    ret=db_read_record(clientid,&startReadRecord,DATA_LEN_MAX,&stepRecordOut);
    PRTL_FMT_DEBUG("是否存在台区区分的记录,ret is %x\n",ret);
    if((ret !=0)||(stepRecordOut.record[0].outDatalen==0))
    {
        time(&timep); /*获取time_t类型的当前时间*/
        recInData.recordOAD.classtag = CLASS_DATA_EVENT;
        recInData.recordOAD.road.oadCols.nNum = 1;
        recInData.recordOAD.road.oadCols.oad[0].value = 0x31120200;
//        recInData.MAC = MAC;
        recInData.colStartTm = timep;
        recInData.colEndTm = timep;
        recInData.colStoreTm = 1;

        unmeterlog.nIndex=1;
        unmeterlog.tmStart= acrossarer.change;
        unmeterlog.tmEnd = acrossarer.change;
        unmeterlog.evtState.nNum = g_tnmPort.nNum;
        for(i=0; i<g_tnmPort.nNum; i++)
        {
            unmeterlog.evtState.state[i].oad = g_tnmPort.port[i];
            unmeterlog.evtState.state[i].state = 0;
        }
        unmeterlog.source.choice=DT_NULL;
        unmeterlog.nResNum=1;
        unmeterlog.Res[0]=acrossarer;
        recInData.inDatalen=sizeof(OOP_UNMETERLOG_T);

        memcpy(recInData.inDataBuf,(uint8*)&unmeterlog,sizeof(OOP_UNMETERLOG_T));
        
        ret = db_write_record(clientid, &recInData, &storeNo);
        PRTL_FMT_DEBUG("第一条台区区分的记录,ret is %x\n",ret);
        if (ret != ERR_OK)
        {
            return ret;
        }
    }
    else
    {
        memcpy((uint8*)&unmeterlog,stepRecordOut.record[0].outDataBuf,stepRecordOut.record[0].outDatalen);
        PRTL_FMT_DEBUG("第一条台区区分的记录,unmeterlog.nIndex is %x\n",unmeterlog.nIndex);
        PRTL_BUF_DEBUG(stepRecordOut.record[0].outDataBuf,stepRecordOut.record[0].outDatalen,"读出来的台区告警信息");
        if(unmeterlog.nResNum>=OOP_MAX_SMREPORT)
        {
            index=unmeterlog.nIndex;
            MEMZERO(&unmeterlog, sizeof(unmeterlog));
            time(&timep); /*获取time_t类型的当前时间*/
            recInData.recordOAD.classtag = CLASS_DATA_EVENT;
            recInData.recordOAD.road.oadCols.nNum = 1;
            recInData.recordOAD.road.oadCols.oad[0].value = 0x31120200;
//            recInData.MAC = MAC;
            recInData.colStartTm = timep;
            recInData.colEndTm = timep;
            recInData.colStoreTm = index+1;

            unmeterlog.nIndex=index+1;
            unmeterlog.tmStart= acrossarer.change;
            unmeterlog.tmEnd = acrossarer.change;
            unmeterlog.evtState.nNum = g_tnmPort.nNum;
            for(i=0; i<g_tnmPort.nNum; i++)
            {
                unmeterlog.evtState.state[i].oad = g_tnmPort.port[i];
                unmeterlog.evtState.state[i].state = 0;
            }
            unmeterlog.source.choice=DT_NULL;
            unmeterlog.nResNum=1;
            unmeterlog.Res[0]=acrossarer;

            recInData.inDatalen = sizeof(OOP_UNMETERLOG_T);
            memcpy(recInData.inDataBuf,(uint8*)&unmeterlog,sizeof(OOP_UNMETERLOG_T));

            PRTL_FMT_DEBUG("台区区分满了16条记录,ret is %x\n",ret);
            ret = db_write_record(clientid, &recInData, &storeNo);
            if (ret != ERR_OK)
            {
                return ret;
            }

        }
        else
        {
            updateRecordData.recordOAD.classtag = CLASS_DATA_EVENT;
            updateRecordData.recordOAD.optional = 0;
            updateRecordData.recordOAD.road.oadCols.nNum = 1;
            updateRecordData.recordOAD.road.oadCols.oad[0].value = 0x31120200;
//            updateRecordData.MAC = MAC;
            updateRecordData.filter.choice = 1;
            updateRecordData.filter.storeTm = unmeterlog.nIndex;


            unmeterlog.tmEnd = acrossarer.change;
            unmeterlog.nResNum++;
            unmeterlog.Res[unmeterlog.nResNum-1]=acrossarer;
            updateRecordData.inDatalen = sizeof(OOP_UNMETERLOG_T);
            memcpy(updateRecordData.inDataBuf,(uint8*)&unmeterlog,sizeof(OOP_UNMETERLOG_T));
            
            ret = db_update_record(clientid, &updateRecordData);
            
            PRTL_FMT_DEBUG("台区区分告警存在但未满满了16条记录,ret is %d,unmeterlog.nIndex is %d\n",ret,unmeterlog.nIndex);
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("台区区分告警记录条数据更新失败ret is %d\n",ret);
                return ret;
            }
            
        }

    }

    

    return 0;
}

int creat_smlog_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    uint8 i=0;
    uint8 index=0;
    OOP_SMLOG_T smlog;
    OOP_SEARCHMET_RES_T searchmet;
    WRITE_RECORD_T      recInData;
    READ_RECORD_T       startReadRecord;
    RECORD_UNIT_T       stepRecordOut;
    UPDATE_RECORD_T     updateRecordData;
    time_t              timep   = 0;
    uint32 storeNo=0;

    if(value !=0x60020200)
    {
        return -1;
    }
    
    
    int ret = 0;
    NOMAL_OAD_T nomal = {0};
    uint32 len = 0;

    MEMZERO(&searchmet, sizeof(searchmet));
    MEMZERO(&recInData, sizeof(recInData));
    MEMZERO(&startReadRecord, sizeof(startReadRecord));
    MEMZERO(&stepRecordOut, sizeof(stepRecordOut));
    MEMZERO(&updateRecordData, sizeof(updateRecordData));
    MEMZERO(&smlog, sizeof(smlog));

    nomal.logicId = logicId;
    nomal.infoNum = infoNum;
    nomal.oad.value = 0x60020200;

    ret = db_read_nomal(clientid, &nomal, sizeof(searchmet), (uint8*)&searchmet, &len);
    if(ret != 0 )
    {
        PRTL_FMT_DEBUG("tmn_port_authorize read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
            ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(searchmet));
        return ret;
    }

    startReadRecord.recordOAD.optional = 0;
    startReadRecord.recordOAD.classtag = CLASS_DATA_EVENT;
    startReadRecord.recordOAD.road.oadMain.value = 0x00000000;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.road.oadCols.oad[0].value= 0x31110200;
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = 0;
    startReadRecord.cType = COL_TM_STORE;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = time(NULL)+1;
    startReadRecord.sortType = 1;
    startReadRecord.MAC = MAC;

    ret=db_read_record(clientid,&startReadRecord,DATA_LEN_MAX,&stepRecordOut);

    if(ret !=0)
    {
        PRTL_FMT_DEBUG("读取结果 ret is %d\n",ret);
        time(&timep); /*获取time_t类型的当前时间*/
        recInData.recordOAD.classtag = CLASS_DATA_EVENT;
        recInData.recordOAD.road.oadCols.nNum = 1;
        recInData.recordOAD.road.oadCols.oad[0].value = 0x31110200;
//        recInData.MAC = MAC;
        recInData.colStartTm = timep;
        recInData.colEndTm = timep;
        recInData.colStoreTm = 1;

        smlog.nIndex=1;
        smlog.tmStart= searchmet.serchTime;
        smlog.tmEnd = searchmet.serchTime;
        smlog.evtState.nNum = g_tnmPort.nNum;
        for(i=0; i<g_tnmPort.nNum; i++)
        {
            smlog.evtState.state[i].oad = g_tnmPort.port[i];
            smlog.evtState.state[i].state = 0;
        }
        smlog.source.choice=DT_NULL;
        smlog.nResNum=1;
        smlog.Res[0]=searchmet;
        recInData.inDatalen = sizeof(OOP_SMLOG_T);
        PRTL_FMT_DEBUG("一条搜表记录的长度 is %d\n",recInData.inDatalen);
        memcpy(recInData.inDataBuf,(uint8*)&smlog,sizeof(OOP_SMLOG_T));
        
        ret = db_write_record(clientid, &recInData, &storeNo);
        PRTL_FMT_DEBUG("写第一条记录结果 ret is %d\n",ret);
        if (ret != ERR_OK)
        {
            
            return ret;
        }
    }
    else
    {
        memcpy((uint8*)&smlog,stepRecordOut.record[0].outDataBuf,stepRecordOut.record[0].outDatalen);
        if(smlog.nResNum>=OOP_MAX_SMREPORT)
        {
            index=smlog.nIndex;
            MEMZERO(&smlog, sizeof(smlog));
            time(&timep); /*获取time_t类型的当前时间*/
            recInData.recordOAD.classtag = CLASS_DATA_EVENT;
            recInData.recordOAD.road.oadCols.nNum = 1;
            recInData.recordOAD.road.oadCols.oad[0].value = 0x31120200;
//            recInData.MAC = MAC;
            recInData.colStartTm = timep;
            recInData.colEndTm = timep;
            recInData.colStoreTm = index+1;

            smlog.nIndex=index+1;
            smlog.tmStart= searchmet.serchTime;
            smlog.tmEnd = searchmet.serchTime;
            smlog.evtState.nNum = g_tnmPort.nNum;
            for(i=0; i<g_tnmPort.nNum; i++)
            {
                smlog.evtState.state[i].oad = g_tnmPort.port[i];
                smlog.evtState.state[i].state = 0;
            }
            smlog.source.choice=DT_NULL;
            smlog.nResNum=1;
            smlog.Res[0]=searchmet;
            recInData.inDatalen = sizeof(OOP_SMLOG_T);
            memcpy(recInData.inDataBuf,(uint8*)&smlog,sizeof(OOP_SMLOG_T));
            
            ret = db_write_record(clientid, &recInData, &storeNo);
            PRTL_FMT_DEBUG("已经搜到了16个表 ret is %d\n",ret);
            if (ret != ERR_OK)
            {
                return ret;
            }

        }
        else
        {
            updateRecordData.recordOAD.optional = 0;
            updateRecordData.recordOAD.road.oadCols.nNum = 1;
            updateRecordData.recordOAD.road.oadCols.oad[0].value = 0x31110200;
//            updateRecordData.MAC = MAC;
            updateRecordData.filter.choice = 1;
            updateRecordData.filter.storeTm = smlog.nIndex;


            smlog.tmEnd = searchmet.serchTime;
            smlog.nResNum++;
            smlog.Res[smlog.nResNum-1]=searchmet;
            recInData.inDatalen = sizeof(OOP_SMLOG_T);
            updateRecordData.inDatalen = sizeof(OOP_SMLOG_T);
            memcpy(updateRecordData.inDataBuf,(uint8*)&smlog,sizeof(OOP_SMLOG_T));
            ret = db_update_record(clientid, &updateRecordData);
            PRTL_FMT_DEBUG("还没搜到了16个表 更新搜表记录 ret is %d\n",ret);
            if (ret != ERR_OK)
            {
                return ret;
            }
            
        }

    }
    PRTL_FMT_DEBUG("写搜表告警\n");
    

    return 0;
}
//
///**
//*********************************************************************
//* @brief:       模块变更事件上报入口(通知方式上报)
//* @detail:      两种方法：1-通知写队列，然后复用现有事件上报 2-更新通知中直接上报
//* @param[in]：   NA
//* @param[out]： NA 
//* @return：      void
//*********************************************************************
//*/
//int creat_route_module_change_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
//{
//    int ret = 0;
//    //RPT_MTREVT_INFO readData;
//    NOMAL_OAD_T normal;
//    CLEAR_DATA_T clearData;
//    ROUTEMODULE routemodule;
//    ROUTEMODULE preroutemodule;
////    OOP_CHILDNODE_T chipnode;
//    OOP_MODULELOG_T modulelog;
//    WRITE_RECORD_T      recInData;
//    char filename[20]={0};
//    uint32 len=0;
//    time_t              timep   = 0;
//    uint32 storeNo=0;
//    uint8 i=0;
//
//    return ret;
//    //MEMZERO(&readData, sizeof(readData));
//    MEMZERO(&normal, sizeof(normal));
//    MEMZERO(&clearData, sizeof(clearData));
//    MEMZERO(&routemodule, sizeof(routemodule));
//    MEMZERO(&preroutemodule, sizeof(preroutemodule));
//    MEMZERO(&modulelog, sizeof(modulelog));
//    MEMZERO(&recInData, sizeof(recInData));
//
//    PRTL_FMT_LOGCC("rpt_met_event_update logicId(%d) infoNum(%d) value(0x%08x) running.....\n", logicId, infoNum, value);
//
//    //读出数据中心更新的事件记录
//    if(value !=0xF2090200)
//    {
//        return -1;
//    }
//
//    normal.infoNum = infoNum;
//    normal.oad.value = value;
//
//    ret = db_read_nomal(clientid, &normal, sizeof(ROUTEMODULE), (uint8*)&routemodule, &len);
//    if((ret != 0)||(len !=sizeof(routemodule)))
//    {
//        RPT_FMT_DEBUG("read mtrevt failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
//            ret, normal.logicId, normal.infoNum, normal.oad.value);
//        return ret;
//    }
//
//   sprintf(filename,"%s",ROUTE_MODULE_CHANGE_FILE);
//
//   if(ERR_OK==read_pdata_xram(filename,(char*)&preroutemodule,infoNum*sizeof(preroutemodule),sizeof(preroutemodule)))
//   {
//     if(0 !=memcmp(&routemodule.name,&preroutemodule.name,sizeof(OOP_VISIBLEVAR256_T)))   
//     {
//           time(&timep); /*获取time_t类型的当前时间*/
//           memcpy((uint8*)&modulelog.curID,(uint8*)&routemodule.name,sizeof(OOP_VISIBLEVAR256_T));
//    
//           memcpy((uint8*)&modulelog.preID,(uint8*)&preroutemodule.name,sizeof(OOP_VISIBLEVAR256_T));
//           systm_to_ooptm(timep,&modulelog.tmStart);
//           //modulelog.tmStart=time(NULL);
//           modulelog.tmEnd = modulelog.tmStart;
//           modulelog.source.choice =3;
//           modulelog.source.oad.value=0xF2090200;
//           modulelog.evtState.nNum = g_tnmPort.nNum;
//           for(i=0; i<g_tnmPort.nNum; i++)
//           {
//               modulelog.evtState.state[i].oad = g_tnmPort.port[i];
//               modulelog.evtState.state[i].state = 0;
//           }
//           write_pdata_xram(filename,(char*)&routemodule,infoNum*sizeof(routemodule),sizeof(routemodule));
//           
//           
//            recInData.recordOAD.classtag = CLASS_DATA_EVENT;
//            recInData.recordOAD.road.oadCols.nNum = 1;
//            recInData.recordOAD.road.oadCols.oad[0].value = 0x30300200;
////            recInData.MAC = MAC;
//            recInData.colStartTm = timep;
//            recInData.colEndTm = timep;
//            recInData.colStoreTm = 1;
//            recInData.inDatalen=sizeof(modulelog);
//            memcpy(recInData.inDataBuf,(uint8*)&modulelog,sizeof(modulelog));
//            ret = db_write_record(clientid, &recInData, &storeNo);
//            if (ret != ERR_OK)
//            {
//                return ret;
//            }
//     }
//   }
//   else
//   {
//        write_pdata_xram(filename,(char*)&routemodule,infoNum*sizeof(routemodule),sizeof(routemodule));
//   }
//
//   
//} 

/**********************************************************************
* @brief     : 检测模块ID是否有效
* @param[in] ：mId                            //模块ID
               idLen                         //长度
               
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
int check_id(char *mId, uint32 idLen)
{
    int i= 0;

    if(mId == NULL || idLen == 0)
    {
        return -1;
    
}

    for(i=0; i<idLen; i++)
    {
        if(mId[i] != 0xff)
        {
            break;
        }
        
    
}
    
    if(i == idLen)
    {
        return -1;
    }
    
    for(i=0; i<idLen; i++)
    {
        if(mId[i] != 0)
        {
            break;
        }
    
}
    
    if(i == idLen)
    {
        return -1;
    }
    
    return 0;
}


/**********************************************************************
* @brief     : 从端口描述符中提取模块ID，芯片ID信息
* @param[in] ：prouteModule                    //端口描述符
               
* @param[out]：
                mId                            //模块ID
                Id                             //芯片ID
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2020-04-17
* @Update    :
**********************************************************************/
int get_id_info(char* prouteModule, char *mId, char* Id)
{
    char   *temp = NULL;
    char   *temp1 = NULL;
    uint8   len=0;

    if(prouteModule == NULL || mId == NULL || Id == NULL)
    {
        return -1;
    
}
    
    temp = strstr(prouteModule, "id=");
    if(NULL == temp)
    {
        return -1;
    }
    temp1 = temp + 3;
    temp = strstr(temp1, ";mmfrs");
    if(NULL == temp)
    {
        return -1;
    }

    memcpy(Id, temp1, MIN((uint32)(temp - temp1), 48));

    temp = strstr(prouteModule, "mid=");
    if(NULL == temp)
    {
        return -1;
    }
    temp1 = temp + 4;
    len=strlen(temp1);
    memcpy(mId, temp1, MIN(len, 32));
    
    return 0;
}
/**
*********************************************************************
* @brief:       模块变更事件上报入口(通知方式上报)
* @detail:      两种方法：1-通知写队列，然后复用现有事件上报 2-更新通知中直接上报
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int creat_route_module_change_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
     int ret = 0;
     NOMAL_OAD_T normal;
    
     OOP_CHILDNODE_T chipnode;
     OOP_CHILDNODE_T prechipnode;

     char mId[100] = {0};        //模块ID
     char Id[100] = {0};         //芯片ID
     char premId[100] = {0};     //上次模块ID
     char preId[100] = {0};
     
    
     char filename[200]={0};
     uint32 len=0;
     ROUTEMODULE routemodule;
     ROUTEMODULE preroutemodule;
     ROUTEMODULE routemoduletmp;
     DESCRIPTOR_T descriptor;

     MEMZERO(&normal, sizeof(normal));
     MEMZERO(&chipnode, sizeof(chipnode));
     MEMZERO(&prechipnode, sizeof(prechipnode));
     MEMZERO(&routemodule, sizeof(routemodule));
     MEMZERO(&preroutemodule, sizeof(preroutemodule));
     MEMZERO(&routemoduletmp, sizeof(routemoduletmp));
     MEMZERO(&descriptor, sizeof(descriptor));
    
     EVENT_FMT_DEBUG("rpt_met_event_update logicId(%d) infoNum(%d) value(0x%08x) running.....\n", logicId, infoNum, value);
    
     sprintf(filename, "%s", ROUTE_MODULE_CHANGE_FILE);
    
     //读出数据中心更新的事件记录
     if(value !=0xF2090200)
     {
         return -1;
     }

     if (infoNum == 0xffff)
     {
         return -1;
     }
    
     normal.infoNum = infoNum;
     normal.oad.value = value;
     OOP_OAD_U sourceoad={0xF2090200};
    
     ret = db_read_nomal(clientid, &normal, sizeof(routemodule), (uint8*)&routemodule, &len);
     if((ret != 0)||(len !=sizeof(routemodule)))
     {
         RPT_FMT_DEBUG("read mtrevt failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
             ret, normal.logicId, normal.infoNum, normal.oad.value);
         return ret;
     }

     memcpy(&routemoduletmp, &routemodule, sizeof(ROUTEMODULE));

     descriptor = UnpackDescriptor(0, routemoduletmp.name.value);
     if (event_data_is_null(descriptor.type, sizeof(descriptor.type)) ||
         event_data_is_null(descriptor.id, sizeof(descriptor.id)) ||
         event_data_is_null(descriptor.mid, sizeof(descriptor.mid)))
     {
         EVENT_BUF_DEBUG(descriptor.type, sizeof(descriptor.type), "read route_module failed type:");
         EVENT_BUF_DEBUG(descriptor.id, sizeof(descriptor.id), "read route_module failed id:");
         EVENT_BUF_DEBUG(descriptor.mid, sizeof(descriptor.type), "read route_module failed mid:");
         return -1;
     }
    
    if(ERR_OK==read_pdata_xram(filename,(char*)&preroutemodule,infoNum*sizeof(preroutemodule),sizeof(preroutemodule)))
    {

        RPT_FMT_DEBUG("read F2090200 ram file len > 0\n");
        
      //一开始从私有文件中读出来是全FF 直接过滤
      if(preroutemodule.name.nNum>256)
      {
        RPT_FMT_DEBUG("全FF\n");
         write_pdata_xram(filename,(char*)&routemodule,infoNum*sizeof(routemodule),sizeof(routemodule));
         return ret;
      }
      ret = get_id_info(preroutemodule.name.value, premId, preId);
      if(ret < 0)
      {
         RPT_FMT_DEBUG("get_id_info error preroutemodule %s \n", preroutemodule.name.value);
         return ret;
      }

      ret = get_id_info(routemodule.name.value, mId, Id);
      if(ret < 0)
      {
         RPT_FMT_DEBUG("get_id_info error routemodule %s \n", routemodule.name.value);
         return ret;
      }

      ret = check_id(mId, strlen(mId));
      if(ret < 0)
      {
         RPT_FMT_DEBUG("check_id error mId %s \n", mId);
         return ret;
      }
      RPT_FMT_DEBUG("开始比对\n");
      RPT_BUF_DEBUG(mId,32,"当前mid\n ");
      RPT_BUF_DEBUG(premId,32,"上次mid\n ");
      RPT_BUF_DEBUG(Id,48,"当前id\n ");
      RPT_BUF_DEBUG(preId,48,"上次id\n ");
      if(0 !=memcmp(&routemodule.name,&preroutemodule.name,sizeof(OOP_VISIBLEVAR256_T)) && 0 !=memcmp(mId,premId,32) && 0 !=memcmp(Id,preId,48))   
      {
            RPT_FMT_DEBUG("compare routemodule name, routemodule(%d), preroutemodule(%d)\n",routemodule.name, preroutemodule.name);
        
         prechipnode.id = preroutemodule.name;
         prechipnode.add.len =6;
         memcpy(prechipnode.add.add,MAC.value,6);


         chipnode.id = routemodule.name;
         chipnode.add.len =6;
         memcpy(chipnode.add.add,MAC.value,6);
         CheckERC_3030(clientid,sourceoad,prechipnode,chipnode);
         write_pdata_xram(filename,(char*)&routemodule,infoNum*sizeof(routemodule),sizeof(routemodule));
      }
    }
    else
    {
         write_pdata_xram(filename,(char*)&routemodule,infoNum*sizeof(routemodule),sizeof(routemodule));
    }
    return ret;

   
} 


/**
*********************************************************************
* @brief:       模块变更事件上报入口(通知方式上报)
* @detail:      两种方法：1-通知写队列，然后复用现有事件上报 2-更新通知中直接上报
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int creat_communicate_module_change_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
         int ret = 0;
         NOMAL_OAD_T normal;
        
         OOP_CHILDNODE_T chipnode;
         OOP_CHILDNODE_T prechipnode;
        
         char filename[200]={0};
         uint32 len=0;
         OOP_VISIBLEVAR_T  pre;
         OOP_VISIBLEVAR_T  now;
         MEMZERO(&normal, sizeof(normal));
         MEMZERO(&chipnode, sizeof(chipnode));
         MEMZERO(&prechipnode, sizeof(prechipnode));
        
        
         PRTL_FMT_LOGCC("rpt_met_event_update logicId(%d) infoNum(%d) value(0x%08x) running.....\n", logicId, infoNum, value);
        

        
         //读出数据中心更新的事件记录
         if(value !=0x45000C00)
         {
             return -1;
         }
        
         normal.infoNum = infoNum;
         normal.oad.value = value;
         OOP_OAD_U sourceoad={0x45000C00};
        
         ret = db_read_nomal(clientid, &normal, sizeof(now), (uint8*)&now, &len);
         if((ret != 0)||(len !=sizeof(now)))
         {
             RPT_FMT_DEBUG("read mtrevt failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
                 ret, normal.logicId, normal.infoNum, normal.oad.value);
             return ret;
         }
        
        sprintf(filename,"%s",COMMU_MODULE_CHANGE_FILE);
        
        if(ERR_OK==read_pdata_xram(filename,(char*)&pre,infoNum*sizeof(pre),sizeof(pre)))
        {
          //一开始从私有文件中读出来是全FF 直接过滤
          if(pre.nNum>254)
          {
             write_pdata_xram(filename,(char*)&now,infoNum*sizeof(now),sizeof(now));
             return ret;
          }
          if(0 !=memcmp(&now,&pre,sizeof(OOP_VISIBLEVAR_T)))   
          {
             prechipnode.id.nNum = pre.nNum;
             memcpy(prechipnode.id.value,pre.value,pre.nNum);
             prechipnode.add.len =6;
             memcpy(prechipnode.add.add,MAC.value,6);


             chipnode.id.nNum = now.nNum;
             memcpy(chipnode.id.value,now.value,now.nNum);
             chipnode.add.len =6;
             memcpy(chipnode.add.add,MAC.value,6);

             
             CheckERC_3030(clientid,sourceoad,prechipnode,chipnode);
             write_pdata_xram(filename,(char*)&now,infoNum*sizeof(now),sizeof(now));
          }
        }
        else
        {
             write_pdata_xram(filename,(char*)&now,infoNum*sizeof(now),sizeof(now));
        }
        return ret;

   
} 


/**
*********************************************************************
* @brief:       模块变更事件上报入口(通知方式上报)
* @detail:      两种方法：1-通知写队列，然后复用现有事件上报 2-更新通知中直接上报
* @param[in]：   NA
* @param[out]： NA 
* @return：      void
*********************************************************************
*/
int creat_chip_module_change_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int ret = 0;
    NOMAL_OAD_T normal;

    OOP_CHILDNODE_T chipnode;
    OOP_CHILDNODE_T prechipnode;
    OOP_CHILDNODE_T chipnodetmp;
    DESCRIPTOR_T    descriptor;

    #ifdef AREA_HENAN
    DESCRIPTOR_T    predescriptor;
    #endif

    char filename[200] = {0};
    uint32 len = 0;

    MEMZERO(&normal, sizeof(normal));
    MEMZERO(&chipnode, sizeof(chipnode));
    MEMZERO(&prechipnode, sizeof(prechipnode));
    MEMZERO(&chipnodetmp, sizeof(chipnodetmp));
    MEMZERO(&descriptor, sizeof(descriptor));

    #ifdef AREA_HENAN
    MEMZERO(&predescriptor, sizeof(predescriptor));
    #endif

    EVENT_FMT_DEBUG("rpt_met_event_update logicId(%d) infoNum(%d) value(0x%08x) running.....\n", logicId, infoNum, value);

    ret = appdb_get_childnode_list(clientid, logicId, infoNum, value);
    if (ret < 0)
    {
        EVENT_FMT_DEBUG("appdb_get_childnode_list err[%d]\n", ret);
        return -1;
    }

    //读出数据中心更新的事件记录
    if (value != 0xF2090500)
    {
        return -1;
    }

    sprintf(filename, "%s", NODE_MODULE_CHANGE_FILE);

    if (infoNum == 0xffff)
    {
        return -1;
    }

    normal.infoNum = infoNum;
    normal.oad.value = value;
    OOP_OAD_U sourceoad = {0xF2090500};

    ret = db_read_nomal(clientid, &normal, sizeof(OOP_CHILDNODE_T), (uint8*)&chipnode, &len);
    if ((ret != 0)||(len !=sizeof(chipnode)))
    {
        EVENT_FMT_DEBUG("read chip_module failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ret;
    }

    memcpy(&chipnodetmp, &chipnode, sizeof(OOP_CHILDNODE_T));

    descriptor = UnpackDescriptor(1, chipnodetmp.id.value);
#ifdef AREA_HENAN
    if (event_data_is_null(descriptor.type, sizeof(descriptor.type)) ||
        event_data_is_null(descriptor.id, sizeof(descriptor.id)) ||
        event_data_is_null(descriptor.mid, sizeof(descriptor.mid)))
#else
    if (event_data_is_null(descriptor.type, sizeof(descriptor.type)) ||
        event_data_is_null(descriptor.id, sizeof(descriptor.id)))
#endif
    {
        EVENT_BUF_DEBUG(descriptor.type, sizeof(descriptor.type), "read chip_module failed type:");
        EVENT_BUF_DEBUG(descriptor.id, sizeof(descriptor.id), "read chip_module failed id:");
        EVENT_BUF_DEBUG(descriptor.mid, sizeof(descriptor.type), "read chip_module failed mid:");
        return -1;
    }

    #ifdef AREA_HENAN
    if ((descriptor.mid[0] == 0x66) && 
        (descriptor.mid[1] == 0x66) &&
        (descriptor.mid[2] == 0x00) &&
        (descriptor.mid[3] == 0x00) &&
        (descriptor.mid[4] == 0x00))
    {
        EVENT_BUF_DEBUG(descriptor.mid, 5, "read chip_module failed mid:");
        return -1;
    }
    #endif

    if(ERR_OK==read_pdata_xram(filename,(char*)&prechipnode,infoNum*sizeof(prechipnode),sizeof(prechipnode)))
    {
        //一开始从私有文件中读出来是全FF 直接过滤
        if (prechipnode.id.nNum>256)
        {
            write_pdata_xram(filename,(char*)&chipnode,infoNum*sizeof(chipnode),sizeof(chipnode));
            EVENT_FMT_DEBUG("infoNum[%d]-prechipnode.id.nNum[>256]\n", infoNum);
            return ret;
        }

        //河南地区要求，芯片ID和模块ID同时变化才事件发生
        #ifdef AREA_HENAN
        memcpy(&chipnodetmp, &prechipnode, sizeof(OOP_CHILDNODE_T));
        predescriptor = UnpackDescriptor(1, chipnodetmp.id.value);
        if ((0 != memcmp(descriptor.id, predescriptor.id, sizeof(descriptor.id))) && (0 != memcmp(descriptor.mid, predescriptor.mid, sizeof(descriptor.mid))))
        #else
        if (0 != memcmp(&chipnode.id,&prechipnode.id,sizeof(OOP_VISIBLEVAR256_T)))   
        #endif
        {
            CheckERC_3030(clientid,sourceoad,prechipnode,chipnode);
            write_pdata_xram(filename,(char*)&chipnode,infoNum*sizeof(chipnode),sizeof(chipnode));
        } 

        #ifdef AREA_HENAN
        EVENT_BUF_DEBUG(descriptor.type, sizeof(descriptor.type),       "infoNum[%d]-curdescriptor chip_module type:", infoNum);
        EVENT_BUF_DEBUG(predescriptor.type, sizeof(predescriptor.type), "infoNum[%d]-predescriptor chip_module type:", infoNum);

        EVENT_BUF_DEBUG(descriptor.id, sizeof(descriptor.id),            "infoNum[%d]-curdescriptor chip_module id:", infoNum);
        EVENT_BUF_DEBUG(predescriptor.id, sizeof(predescriptor.id),      "infoNum[%d]-predescriptor chip_module id:", infoNum);

        EVENT_BUF_DEBUG(descriptor.mid, sizeof(descriptor.type),         "infoNum[%d]-curdescriptor chip_module mid:", infoNum);
        EVENT_BUF_DEBUG(predescriptor.mid, sizeof(predescriptor.type),   "infoNum[%d]-predescriptor chip_module mid:", infoNum);
        #endif
    }
    else
    {
        EVENT_FMT_DEBUG("infoNum[%d]-read_pdata_xram err\n", infoNum);
        write_pdata_xram(filename,(char*)&chipnode,infoNum*sizeof(chipnode),sizeof(chipnode));
    }
    return ret;
} 

int creat_refuseslave_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
   // uint8 i=0;

    //OOP_REFUSENODEACCESSLOG_T refuseslave;
    OOP_REFUSESLAVE_T   info;

//    WRITE_RECORD_T      recInData;
//    READ_RECORD_T       startReadRecord;
//    RECORD_UNIT_T       stepRecordOut;
//    UPDATE_RECORD_T     updateRecordData;
//    time_t              timep   = 0;
//    uint32 storeNo=0;

    if(value == 0xF2091500)
    {
        int ret = 0;
        NOMAL_OAD_T nomal = {0};
        uint32 len = 0;

//        MEMZERO(&recInData, sizeof(recInData));
//        MEMZERO(&startReadRecord, sizeof(startReadRecord));
//        MEMZERO(&stepRecordOut, sizeof(stepRecordOut));
//        MEMZERO(&updateRecordData, sizeof(updateRecordData));
//        MEMZERO(&refuseslave, sizeof(refuseslave));
        
        nomal.oad.value = 0xF2091500;
        nomal.logicId = logicId;
        nomal.infoNum = infoNum;
        ret = db_read_nomal(clientid, &nomal, sizeof(info), (uint8*)&info, &len);
        if(ret != 0 )
        {
            STAT_FMT_TRACE("tmn_port_authorize read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(info));
            return ret;
        }
        CheckERC_311F(clientid,info);
//        startReadRecord.recordOAD.optional = 0;
//        startReadRecord.recordOAD.road.oadCols.nNum = 1;
//        startReadRecord.recordOAD.road.oadCols.oad[0].value= 0x311F0200;
//        startReadRecord.recordOAD.logicId = 0;
//        startReadRecord.recordOAD.infoNum = 0;
//        startReadRecord.cType = COL_TM_STORE;
//        startReadRecord.cStart = 0;
//        startReadRecord.cEnd = time(NULL)+1;
//        startReadRecord.sortType = 1;
//        startReadRecord.MAC = MAC;
//
//        ret=db_read_record(clientid,&startReadRecord,DATA_LEN_MAX,&stepRecordOut);
//        if(ret !=0)
//        {
//            time(&timep); /*获取time_t类型的当前时间*/
//            recInData.recordOAD.classtag = CLASS_DATA_EVENT;
//            recInData.recordOAD.road.oadCols.nNum = 1;
//            recInData.recordOAD.road.oadCols.oad[0].value = 0x311F0200;
//            recInData.MAC = MAC;
//            recInData.colStartTm = timep;
//            recInData.colEndTm = timep;
//            recInData.colStoreTm = 1;
//
//            refuseslave.nIndex=1;
//            systm_to_ooptm(timep,&refuseslave.tmStart);
//         
//            refuseslave.tmEnd = refuseslave.tmStart;
//            refuseslave.evtState.nNum = g_tnmPort.nNum;
//            for(i=0; i<g_tnmPort.nNum; i++)
//            {
//                refuseslave.evtState.state[i].oad = g_tnmPort.port[i];
//                refuseslave.evtState.state[i].state = 0;
//            }
//            refuseslave.source.choice=DT_NULL;
//            refuseslave.nIndex=1;
//            refuseslave.info=info;
//
//            memcpy_s(recInData.inDataBuf, sizeof(OOP_REFUSENODEACCESSLOG_T), (uint8*)&refuseslave, sizeof(OOP_REFUSENODEACCESSLOG_T));
//            
//            ret = db_write_record(clientid, &recInData, &storeNo);
//            if (ret != ERR_OK)
//            {
//                return ret;
//            }
//        }
//        else
//        {
//            memcpy_s((uint8*)&refuseslave, stepRecordOut.record[0].outDatalen, stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
//            time(&timep); /*获取time_t类型的当前时间*/
//            recInData.recordOAD.classtag = CLASS_DATA_EVENT;
//            recInData.recordOAD.road.oadCols.nNum = 1;
//            recInData.recordOAD.road.oadCols.oad[0].value = 0x311F0200;
//            recInData.MAC = MAC;
//            recInData.colStartTm = timep;
//            recInData.colEndTm = timep;
//            recInData.colStoreTm = refuseslave.nIndex +1;
//
//            systm_to_ooptm(timep,&refuseslave.tmStart);
//            refuseslave.tmEnd = refuseslave.tmStart;
//            refuseslave.evtState.nNum = g_tnmPort.nNum;
//            for(i=0; i<g_tnmPort.nNum; i++)
//            {
//                refuseslave.evtState.state[i].oad = g_tnmPort.port[i];
//                refuseslave.evtState.state[i].state = 0;
//            }
//            refuseslave.source.choice=DT_NULL;
//            refuseslave.nIndex=refuseslave.nIndex+1;
//            refuseslave.info=info;
//
//            memcpy_s(recInData.inDataBuf, sizeof(OOP_REFUSENODEACCESSLOG_T), (uint8*)&refuseslave, sizeof(OOP_REFUSENODEACCESSLOG_T));
//            
//            ret = db_write_record(clientid, &recInData, &storeNo);
//            if (ret != ERR_OK)
//            {
//                return ret;
//            }
//
//        }

    }

    return 0;
}
#if AREA_ZHEJIANG
int creat_identify_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{

    Oop_Identify_Result IdentifyInformations;
    OOP_TSA_T tsa;

    if(value == 0x4E070200)
    {
        int ret = 0;
        NOMAL_OAD_T nomal = {0};
        uint32 len = 0;

        
        nomal.oad.value = 0x4E070200;
        nomal.logicId = logicId;
        nomal.infoNum = infoNum;
        ret = db_read_nomal(clientid, &nomal, sizeof(IdentifyInformations), (uint8*)&IdentifyInformations, &len);
        if(ret != 0 )
        {
            STAT_FMT_TRACE("tmn_port_authorize read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                ret, nomal.logicId, nomal.infoNum, nomal.oad.value, sizeof(IdentifyInformations));
            return ret;
        }

        EVENT_FMT_DEBUG("Identifynum=%d, signalphase=%d, logicId=%d, infoNum=%d \n", 
            IdentifyInformations.Identifynum, IdentifyInformations.IdentifyInformation[0].signalphase, logicId, infoNum);
        CheckERC_3E03(clientid,IdentifyInformations,tsa,infoNum);


    }

    return 0;
}
#endif
int creat_meter_adjust_tmie_event(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int ret = 0;

    NOMAL_OAD_T normal;
    //CLEAR_DATA_T clearData;
    OOP_CHK_TIMEINFO_T Chktimeinfo;
    //READ_RECORD_T       startReadRecord;
   // RECORD_UNIT_T       stepRecordOut;
   // OOP_P2PCLOCKLOG_T p2pclocklog;
   // WRITE_RECORD_T      recInData;
    OOP_TSA_T Addr;
    uint32 len=0;
    //time_t              timep   = 0;
    //uint32 storeNo=0;
    //uint8 i=0;

    if(value != 0xFFA00100)
    {
        return ret;
    }
    MEMZERO(&Addr,sizeof(OOP_TSA_T));
    MEMZERO(&normal, sizeof(normal));
    //MEMZERO(&clearData, sizeof(clearData));
    MEMZERO(&Chktimeinfo, sizeof(Chktimeinfo));

   // MEMZERO(&recInData, sizeof(recInData));

    normal.infoNum = infoNum;
    normal.oad.value = value;

    ret = db_read_nomal(clientid, &normal, sizeof(Chktimeinfo), (uint8*)&Chktimeinfo, &len);
    if((ret != 0)||(len !=sizeof(Chktimeinfo)))
    {
        RPT_FMT_DEBUG("read mtrevt failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ret;
    }
    Addr.len = Chktimeinfo.add.nNum-1;
    memcpy(Addr.add,Chktimeinfo.add.value, Chktimeinfo.add.nNum);
    char offset;
    if(Chktimeinfo.error>127)
        offset = 127;
    else
        offset = Chktimeinfo.error;
    CheckERC_311B(clientid,infoNum,Addr,Chktimeinfo.beforechkTime,offset);
//    startReadRecord.recordOAD.optional = 0;
//    startReadRecord.recordOAD.classtag = CLASS_DATA_EVENT;
//    startReadRecord.recordOAD.road.oadMain.value = 0x00000000;
//    startReadRecord.recordOAD.road.oadCols.nNum = 1;
//    startReadRecord.recordOAD.road.oadCols.oad[0].value= 0x311B0200;
//    startReadRecord.recordOAD.logicId = 0;
//    startReadRecord.recordOAD.infoNum = 0;
//    startReadRecord.cType = COL_TM_STORE;
//    startReadRecord.cStart = 0;
//    startReadRecord.cEnd = time(NULL)+1;
//    startReadRecord.sortType = 1;
//    startReadRecord.MAC = MAC;
//
//    ret=db_read_record(clientid,&startReadRecord,DATA_LEN_MAX,&stepRecordOut);
//
//    if(ret !=0)
//    {
//        PRTL_FMT_DEBUG("读取结果 ret is %d\n",ret);
//        time(&timep); /*获取time_t类型的当前时间*/
//        recInData.recordOAD.classtag = CLASS_DATA_EVENT;
//        recInData.recordOAD.road.oadCols.nNum = 1;
//        recInData.recordOAD.road.oadCols.oad[0].value = 0x311B0200;
//        recInData.MAC = MAC;
//        recInData.colStartTm = timep;
//        recInData.colEndTm = timep;
//        recInData.colStoreTm = 1;
//
//        p2pclocklog.nIndex=1;
//        p2pclocklog.tmStart= Chktimeinfo.chkTime;
//        p2pclocklog.tmEnd = Chktimeinfo.chkTime;
//        p2pclocklog.evtState.nNum = g_tnmPort.nNum;
//        for(i=0; i<g_tnmPort.nNum; i++)
//        {
//            p2pclocklog.evtState.state[i].oad = g_tnmPort.port[i];
//            p2pclocklog.evtState.state[i].state = 0;
//        }
//        p2pclocklog.source.choice=DT_TSA;
//        p2pclocklog.source.add.af = Chktimeinfo.add.nNum-1;
//        memcpy(p2pclocklog.source.add.add,Chktimeinfo.add.value,Chktimeinfo.add.nNum);
//        p2pclocklog.preTime = Chktimeinfo.beforechkTime;
//        p2pclocklog.offset = (char)Chktimeinfo.error;
//        recInData.inDatalen = sizeof(p2pclocklog);
//        PRTL_FMT_DEBUG("一条校时记录 is %d\n",recInData.inDatalen);
//        memcpy(recInData.inDataBuf,(uint8*)&p2pclocklog,sizeof(p2pclocklog));
//        
//        ret = db_write_record(clientid, &recInData, &storeNo);
//        PRTL_FMT_DEBUG("写第一条校时记录结果 ret is %d\n",ret);
//        if (ret != ERR_OK)
//        {
//            
//            return ret;
//        }
//    }
//    else
//    {
//        memcpy((uint8*)&p2pclocklog,stepRecordOut.record[0].outDataBuf,stepRecordOut.record[0].outDatalen);
//        
//        PRTL_FMT_DEBUG("读取结果 ret is %d\n",ret);
//        time(&timep); /*获取time_t类型的当前时间*/
//        recInData.recordOAD.classtag = CLASS_DATA_EVENT;
//        recInData.recordOAD.road.oadCols.nNum = 1;
//        recInData.recordOAD.road.oadCols.oad[0].value = 0x311B0200;
//        recInData.MAC = MAC;
//        recInData.colStartTm = timep;
//        recInData.colEndTm = timep;
//        recInData.colStoreTm = 1;
//
//        p2pclocklog.nIndex=p2pclocklog.nIndex+1;
//        p2pclocklog.tmStart= Chktimeinfo.chkTime;
//        p2pclocklog.tmEnd = Chktimeinfo.chkTime;
//        p2pclocklog.evtState.nNum = g_tnmPort.nNum;
//        for(i=0; i<g_tnmPort.nNum; i++)
//        {
//            p2pclocklog.evtState.state[i].oad = g_tnmPort.port[i];
//            p2pclocklog.evtState.state[i].state = 0;
//        }
//        p2pclocklog.source.choice=DT_TSA;
//        p2pclocklog.source.add.af = Chktimeinfo.add.nNum-1;
//        memcpy(p2pclocklog.source.add.add,Chktimeinfo.add.value,Chktimeinfo.add.nNum);
//        p2pclocklog.preTime = Chktimeinfo.beforechkTime;
//        p2pclocklog.offset = (char)Chktimeinfo.error;
//        recInData.inDatalen = sizeof(p2pclocklog);
//        PRTL_FMT_DEBUG("一条校时记录 is %d\n",recInData.inDatalen);
//        memcpy(recInData.inDataBuf,(uint8*)&p2pclocklog,sizeof(p2pclocklog));
//        
//        ret = db_write_record(clientid, &recInData, &storeNo);
//        PRTL_FMT_DEBUG("写校时记录结果 ret is %d\n",ret);
//        if (ret != ERR_OK)
//        {
//            
//            return ret;
//        }
//
//    }

   return ret;
}

int update_perceptionterminal_info(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int ret = 0;

    NOMAL_OAD_T normal;
    uint32 len=0;
    OOP_PERCEPTION_TERMINAL_INFO_T terminalinfo;
    uint8 num;

    if(value != 0xFF110E00)
    {
        return ret;
    }
 
    MEMZERO(&normal, sizeof(normal));

    normal.infoNum = infoNum;
    normal.oad.value = value;
    
    PRTL_FMT_DEBUG("read mtrevt failed. ret(%d), logicId(%u), infoNum(%u), oad(0x%08x)\n",
                       ret, normal.logicId, normal.infoNum, normal.oad.value);

    ret = db_read_nomal(clientid, &normal, sizeof(terminalinfo), (uint8*)&terminalinfo, &len);
    if((ret != 0)||(len !=sizeof(terminalinfo)))
    {
        PRTL_FMT_DEBUG("read mtrevt failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return ret;
    }
    perceptionterminal.num = infoNum;
    memcpy((uint8*)&perceptionterminal.terminalinfo[infoNum-1],(uint8*)&terminalinfo,sizeof(terminalinfo));
    num = terminalinfo.meterlist.num;
    perceptionterminal.totalpn = terminalinfo.meterlist.meterinfo[num-1].pn;
    PRTL_FMT_DEBUG("perceptionterminal.totalpn(%d), perceptionterminal.num %d\n",
                       perceptionterminal.totalpn, perceptionterminal.num);
   return ret;
}


