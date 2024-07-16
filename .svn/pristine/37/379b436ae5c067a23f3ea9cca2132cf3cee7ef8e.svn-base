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
#include "Event.h"
#include "EventFun.h"
#include "commfun.h"
#include "framecomm.h"
#include "msg_id.h"
#include "label_id.h"
#include "storage.h"
#include "oopVarUpdate.h"
#include "../app104/include/pw_all.h"


/*-******************************************************************-*/
/*-                             全局变量                             -*/
/*-******************************************************************-*/
#define EventReportCount  28    //上报的事件总量,每一个事件一个文件，需要遍历所有时间文件
extern int ReadMetPara(int mtr_id, StoreMetStruct *InMetPara);
extern float32 ratio_convert(int8 label, CONVERT_TYPE_E type);
extern FAULT_REPORT_CONFIG g_aFaultDataConfig[FAULT_MAX_NUM];
extern uint16 g_nConfigFaultNum;

//METER_HALT_STAT_STRUCT gMeterHalt_Info;//需要读取停电事件的结构信息

//int EventGLBpiid = 0;
//int EventGLBpiid_met = 0;

//uint32 DelaySaveRemoteEventNo[TURN_NUM] = { 0 };
//uint32 DelaySavePowerCtrlEventNo[TURN_NUM] = { 0 };
EVENT_3000_INFO event_3000;   //失压事件
EVENT_3001_INFO event_3001;   //欠压事件
EVENT_3002_INFO event_3002;   //过压事件
EVENT_3003_INFO event_3003;   //断相事件
EVENT_3005_INFO event_3005;   //过流事件
EVENT_300F_INFO event_300F;   //电压逆相事件
EVENT_301D_INFO event_301D;   //电压不平衡事件
EVENT_301E_INFO event_301E;   //电流不平衡事件
EVENT_3106_INFO event_3106;   //终端停上电事件
EVENT_3119_INFO event_3119;   //电流回路异常事件

OOP_OCTETVAR16_T    tmnAddr;                            //终端通信地址
//OOP_DWORD3V_T       tmnVoltage;                         //终端电压
OOP_OCTETVAR64_T    MAC;                                //校验码

//最新抄表数据
LatestMeterReadData g_LatestMeterReadData;

//普通型数据更新列表
EventUpdateDataInfo g_EventUpdateNormalDataList[] =
{/*     数据标识,   数据长度,     数据内容 */
    {0x30000800,    sizeof(uint8),                  &event_3000.eventCommInfo.reportflag},
    {0x30000900,    sizeof(uint8),                  &event_3000.eventCommInfo.checkflag},
    {0x30010800,    sizeof(uint8),                  &event_3001.eventCommInfo.reportflag},
    {0x30010900,    sizeof(uint8),                  &event_3001.eventCommInfo.checkflag},
    {0x30020800,    sizeof(uint8),                  &event_3002.eventCommInfo.reportflag},
    {0x30020900,    sizeof(uint8),                  &event_3002.eventCommInfo.checkflag},
    {0x30030800,    sizeof(uint8),                  &event_3003.eventCommInfo.reportflag},
    {0x30030900,    sizeof(uint8),                  &event_3003.eventCommInfo.checkflag},
    {0x30050800,    sizeof(uint8),                  &event_3005.eventCommInfo.reportflag},
    {0x30050900,    sizeof(uint8),                  &event_3005.eventCommInfo.checkflag},
    {0x300F0800,    sizeof(uint8),                  &event_300F.eventCommInfo.reportflag},
    {0x300F0900,    sizeof(uint8),                  &event_300F.eventCommInfo.checkflag},
    {0x300f0600,    sizeof(uint8),                  &event_300F.nDelay},
    {0x20140200,    sizeof(OOP_METWORD_T),          (uint8 *)&event_300F.meterRsw},
    {0x20030200,    sizeof(OOP_WORD4V_T),           (uint8*)g_LatestMeterReadData.oopWord4C},
    {0x20020200,    sizeof(OOP_WORD4V_T),           (uint8*)g_LatestMeterReadData.oopWord4V},
    {0x301D0800,    sizeof(uint8),                  &event_301D.eventCommInfo.reportflag},
    {0x301D0900,    sizeof(uint8),                  &event_301D.eventCommInfo.checkflag},
    {0x301E0800,    sizeof(uint8),                  &event_301E.eventCommInfo.reportflag},
    {0x301E0900,    sizeof(uint8),                  &event_301E.eventCommInfo.checkflag},
    {0x20000200,    sizeof(OOP_WORD3V_T),          (uint8 *)&g_LatestMeterReadData.valTemp},
    {0x20010200,    sizeof(OOP_INT3V_T),            (uint8 *)&g_LatestMeterReadData.curTemp},
    {0x31060600,    sizeof(OOP_POWERONCUT_T),       (uint8 *)&event_3106.eventParam},
    {0x31060800,    sizeof(uint8),                  &event_3106.eventCommInfo.reportflag},
    {0x31060900,    sizeof(uint8),                  &event_3106.eventCommInfo.checkflag},
    {0x31190800,    sizeof(uint8),                  &event_3119.eventCommInfo.reportflag},
    {0x31190900,    sizeof(uint8),                  &event_3119.eventCommInfo.checkflag},
};

uint16 g_EventUpdateNormalDataNum = sizeof(g_EventUpdateNormalDataList)/sizeof(EventUpdateDataInfo);

//记录型数据更新列表
EventUpdateDataInfo g_EventUpdateRecordDataList[] =
{/*     数据标识,   数据长度,     数据内容 */
    {0x40000200,    sizeof(OOP_DATETIME_S_T),       (uint8*)g_LatestMeterReadData.meterReadTime},
    {0x20030200,    sizeof(OOP_WORD4V_T),           (uint8*)g_LatestMeterReadData.oopWord4C},
    {0x20020200,    sizeof(OOP_WORD4V_T),           (uint8*)g_LatestMeterReadData.oopWord4V},
    {0x20010200,    sizeof(OOP_INT3V_T),            (uint8*)g_LatestMeterReadData.curTemp},
    {0x20000200,    sizeof(OOP_WORD3V_T),          (uint8*)g_LatestMeterReadData.valTemp},
    {0x01000200,    sizeof(OOP_ENERGY_T),           (uint8*)g_LatestMeterReadData.forwardActive},
    {0x03000200,    sizeof(OOP_ENERGYS_T),          (uint8*)g_LatestMeterReadData.forwardUnActive},
};

//国网2019年2月发布最新规范
static const FAULT_REPORT_RAMINFO fault_report_ramoff[FAULT_MAX_NUM] = //目前终端只支持交采事件，电表事件不支持
{
    {//终端停电
         FALT_START_NO, RTUSelfCheckAddr,	 0,8,8,M_ME_NC_1,\
		 {0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//终端自检
		 FALT_START_NO+1,RTUSelfCheckAddr,	 1,9,8,M_ME_NC_1,\
		 {0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//A相电压断相
		 FALT_START_NO+2,GBLostvltAddr,		 0,4,8,M_ME_NC_1,\
		 {0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//B相电压断相
		 FALT_START_NO+3,GBLostvltAddr,		 1,5,8,M_ME_NC_1,\
		 {0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//C相电压断相
		 FALT_START_NO+4,GBLostvltAddr,		 2,6,8,M_ME_NC_1,\
		 {0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//A相电压失压
		 FALT_START_NO+5,GBLowvltAddr,	 0,4,8,M_ME_NC_1,\
		 {0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//B相电压失压
		 FALT_START_NO+6,GBLowvltAddr,	 1,5,8,M_ME_NC_1,\
		 {0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//C相电压失压
		 FALT_START_NO+7,GBLowvltAddr,	 2,6,8,M_ME_NC_1,\
		 {0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//A相电压越上限
		 FALT_START_NO+8,GBVltUpAddr, 	 0,4,8,M_ME_NC_1,\
		 {0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}
	},

	{//B相电压越上限
		FALT_START_NO+9,GBVltUpAddr,	 1,5,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//C相电压越上限
		FALT_START_NO+10,GBVltUpAddr,	 2,6,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//A相电压越下限
		FALT_START_NO+11,GBVltDownAddr, 	0,4,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//B相电压越下限
		FALT_START_NO+12,GBVltDownAddr, 	1,5,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//C相电压越下限
		FALT_START_NO+13,GBVltDownAddr, 	2,6,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//A相电流越上限
		FALT_START_NO+14,GBCurrUpAddr,	0,4,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}
	},

	{//B相电流越上限
		FALT_START_NO+15,GBCurrUpAddr,1,5,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}
	},

	{//C相电流越上限
		FALT_START_NO+16,GBCurrUpAddr,2,6,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//A相电流越下限
		FALT_START_NO+17,GBCurrDownAddr,	0,4,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//B相电流越下限
		FALT_START_NO+18,GBCurrDownAddr,	1,5,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//C相电流越下限
		FALT_START_NO+19,GBCurrDownAddr,	2,6,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//电压不平衡
		FALT_START_NO+20,GBVltUnbAddr,0,4,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//电流不平衡
		FALT_START_NO+21,GBCurrUnbAddr, 	0,4,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//A相电流反极性
		FALT_START_NO+22,GBPhaseReveAddr,	0,4,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//B相电流反极性
		FALT_START_NO+23,GBPhaseReveAddr,	1,5,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//C相电流反极性
		FALT_START_NO+24,GBPhaseReveAddr,	2,6,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//相序异常(电压)
		FALT_START_NO+25,GBVltAbnormalAddr,  0,4,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	},

	{//相序异常(电流)
		FALT_START_NO+26,GBCurrAbnormalAddr, 0,4,8,M_ME_NC_1,\
		{0x4001,0x4002,0x4003,0x4004,0x4005,0x4006,0x4007,0x4008,0,0,0,0}\
	}
};

static const uint32 fault_report_ramsize = sizeof(fault_report_ramoff) / sizeof(fault_report_ramoff[0]);

uint16 g_EventUpdateRecordDataNum = sizeof(g_EventUpdateRecordDataList)/sizeof(EventUpdateDataInfo);

int8        bfgSendEvent = 0;                       //-事件上报标志，在非主动上报通道中使用-
uint8       bCheckParaTemp[128] = { 0 };            //-检测终端参数时，临时存放读取的参数-
mtrnum_t    MtrACID = 0;                            //-第一个交采测量点的测量点号-
#if WRITE_LOG_EN
uint8 bLogBuf[1000] = { 0 };
#endif

uint8 CurrentReportEventIndex;                     //当前上报事件索引
uint8 WtiteBackInfo[9];                            //回写上报状态信息
uint16 ResendBufferLenth = 0;//重新发送长度
uint8  ResendBuffer[1024];//重发缓冲区

uint8 ConfirmFlag_event_met = 0;
uint8 outOfTime_event_met;//超时时间
uint8 retryCunt_event_met;//重试次数
uint16 ResendBufferLenth_met = 0;//重新发送长度
uint8  ResendBuffer_met[1024];//重发缓冲区
FAULT_REPORT_SEND faultsenddata[FAULT_MAX_NUM] = {{0}};


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
    int i = 0;
    uint32 dwflag = 0;

    EVENT_FMT_TRACE("InitEventStatusFlag set 0!\n");

    for(i=0; i<MTR485_MAX_NUM; i++)
    {
        write_pdata_xram("^Event", (const char *)&dwflag, (EventStatusRecAddr+(i)*4), 4);
    }

    write_pdata_xram("^Event", (const char *)&dwflag, EventFlagRecAddr, 1);
    write_pdata_xram("^Event", (const char *)&dwflag, FluxOverTopAddr, 1);
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
    eventCommInfo->eventNum = 0;
    eventCommInfo->recordnum = 0;
    memset(&eventCommInfo->eventState, 0, sizeof(EVTSTATE));
    memset(&eventCommInfo->eventTime, 0, sizeof(EVTTIME));
}

/**********************************************************************
* @name      : GetInitEventInfo
* @brief     ：初始化事件
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-14
* @Update    :
**********************************************************************/
int GetInitEventInfo(int clientid,uint8 initType, uint8 logicId,uint16 classTag)
{
    EVENT_FMT_DEBUG("Init event initType=%u, logicId=%u, classTag=%u!\n", initType, logicId, classTag);

    //初始化事件参数
    if(initType == 1 || classTag == CLASS_DATA_EVENT || classTag == 0)
    {
        CleanEventStatusFlag();
        CleanEventCommInfo(&event_3000.eventCommInfo);
        CleanEventCommInfo(&event_3001.eventCommInfo);
        CleanEventCommInfo(&event_3002.eventCommInfo);
        CleanEventCommInfo(&event_3003.eventCommInfo);
        CleanEventCommInfo(&event_3106.eventCommInfo);
        CleanEventCommInfo(&event_3005.eventCommInfo);
        CleanEventCommInfo(&event_300F.eventCommInfo);
        CleanEventCommInfo(&event_301D.eventCommInfo);
        CleanEventCommInfo(&event_301E.eventCommInfo);
        CleanEventCommInfo(&event_3119.eventCommInfo);
    }

    return 0;
}

#if 0
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
        return -1;
    }

    memset(&oopTaskTemp, 0, sizeof(OOP_TASK_T));

    if(app_get_task_list(taskID, &oopTaskTemp) == 0)
    {
        *oopTI = oopTaskTemp.acqFreq;
        EVENT_FMT_TRACE("Get GetTaskOopTI.unit = %hu OK!\n", oopTI->unit);
        EVENT_FMT_TRACE("Get GetTaskOopTI.value = %hu OK!\n", oopTI->value);
    }

    return 0;
}
#endif

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
    int                 j           = 0;
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
    startReadRecord.cEnd = timep;
    startReadRecord.sortType = 1;
    startReadRecord.MAC.nNum = 6;

    EVENT_FMT_DEBUG("Initializing latest meter reading data. please wait......\n");
    for (i = 0; i < g_EventUpdateRecordDataNum; i++)
    {
        startReadRecord.recordOAD.road.oadCols.oad[0].value = g_EventUpdateRecordDataList[i].oadValue;

        for (j = 0; j < g_sMeterFileAll.MeterCount; j++)
        {
            mtr_id = g_sMeterFileAll.MeterFile[j].nIndex;
            oopMeter = g_sMeterFileAll.MeterFile[j];
            startReadRecord.recordOAD.infoNum = mtr_id;
            memcpy(startReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);

            ret = db_read_record(clientid, &startReadRecord, g_EventUpdateRecordDataList[i].dateLen, &stepRecordOut);
            if (ret != ERR_OK)
            {
                //如果没有读取到电表抄表标志，直接返回
                EVENT_FMT_DEBUG("db_read_record infoNum=%u oad=%08X error ret%d\n", mtr_id, g_EventUpdateRecordDataList[i].oadValue, ret);
                continue;
            }
            else
            {
                if(stepRecordOut.record[0].outDatalen == g_EventUpdateRecordDataList[i].dateLen)
                {
                    if(g_EventUpdateRecordDataList[i].oadValue == 0x40000200)
                    {
                        offset = sizeof(RECORD_UNIT_T)*(mtr_id);
                        memcpy(g_EventUpdateRecordDataList[i].pBuf + offset, &stepRecordOut, sizeof(RECORD_UNIT_T));
                    }
                    else
                    {
                        offset = g_EventUpdateRecordDataList[i].dateLen*(mtr_id);
                        memcpy(g_EventUpdateRecordDataList[i].pBuf + offset, stepRecordOut.record[0].outDataBuf, g_EventUpdateRecordDataList[i].dateLen);
                    }
                }
            }
        }
    }

    EVENT_FMT_DEBUG("Initializing latest meter reading data end.\n");

    return;
}

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

    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&startReadRecord,0,sizeof(READ_RECORD_T));
    memset(&stepRecordOut,0,sizeof(RECORD_UNIT_T));

    time(&timep); //获取从1970至今过了多少秒，存入time_t类型的timep
    startReadRecord.recordOAD.road.oadMain.value = 0;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;

    if(infoNum > MTR485_MAX_NUM)
    {
        EVENT_FMT_TRACE("只处理485数据, infoNum[%u]>%u!\n", infoNum, MTR485_MAX_NUM);
        return 0;
    }

    startReadRecord.recordOAD.logicId = logicId;
    startReadRecord.recordOAD.infoNum = infoNum;
    startReadRecord.cType = 0x00;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = timep;
    startReadRecord.sortType = 1;
    startReadRecord.MAC.nNum = 6;

    for (i = 0; i < g_sMeterFileAll.MeterCount; i++)
    {
        if (g_sMeterFileAll.MeterFile[i].nIndex == infoNum)
        {
            oopMeter = g_sMeterFileAll.MeterFile[i];
            ret = 1;
        }
    }

    if (ret)
    {
        memcpy(startReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);

        for(j = 0; j < oadColsNum; j++)
        {
            startReadRecord.recordOAD.road.oadCols.oad[0].value = oadCols[j];
            for (i = 0; i < g_EventUpdateRecordDataNum; i++)
            {
                if(g_EventUpdateRecordDataList[i].oadValue == oadCols[j])
                {
                    EVENT_FMT_TRACE("UpdateLatestMeterReadDataList, Update logicId=%u, infoNum=%u, OAD=%8X\n", logicId, infoNum, oadCols[j]);

                    ret = db_read_record(clientid, &startReadRecord, g_EventUpdateRecordDataList[i].dateLen, &stepRecordOut);
                    if (ret != ERR_OK)
                    {
                        //如果没有读取到电表抄表标志，直接返回
                        EVENT_FMT_DEBUG("db_read_record infoNum=%u oad=08X error ret%d\n", infoNum, g_EventUpdateRecordDataList[i].oadValue, ret);
                        return -1;
                    }

                    /* 使用互斥量进行加锁 */
                    pthread_mutex_lock(&g_LatestMeterReadData.mutex);

                    if(stepRecordOut.record[0].outDatalen == g_EventUpdateRecordDataList[i].dateLen)
                    {
                        if(oadCols[j] == 0x40000200)
                        {
                            offset = sizeof(RECORD_UNIT_T)*(infoNum - 1);
                            memcpy(g_EventUpdateRecordDataList[i].pBuf + offset, &stepRecordOut, sizeof(RECORD_UNIT_T));
                        }
                        else
                        {
                            offset = g_EventUpdateRecordDataList[i].dateLen*(infoNum - 1);
                            memcpy(g_EventUpdateRecordDataList[i].pBuf + offset, stepRecordOut.record[0].outDataBuf, g_EventUpdateRecordDataList[i].dateLen);
                        }
                    }
                    /* 使用互斥量进行解锁 */
                    pthread_mutex_unlock(&g_LatestMeterReadData.mutex);

                    return 0;
                }

            }
        }
    }

    return -1;
}

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

    memset(&nomalOAD, 0, sizeof(NOMAL_OAD_T));

    nomalOAD.logicId = logicId;
    nomalOAD.infoNum = infoNum;
    nomalOAD.oad.value = value;
    ret = db_read_nomal(clientid, &nomalOAD, 1024, pDataBuf, &readlen);
    if (ret != ERR_OK || readlen == 0)
    {
        EVENT_FMT_DEBUG("Updata logicId=%u, infoNum=%u, OAD=%8X  error ret=%d, len=%u\n", logicId, infoNum, value, ret, readlen);
        return ret;
    }

    EVENT_FMT_TRACE("Updata logicId=%u, infoNum=%u, OAD=%8X  OK !\n", logicId, infoNum, value);

    for(i=0; i<g_EventUpdateNormalDataNum; i++)
    {
        if(g_EventUpdateNormalDataList[i].oadValue == value && g_EventUpdateNormalDataList[i].dateLen == readlen)
        {
            memcpy(g_EventUpdateNormalDataList[i].pBuf, pDataBuf, readlen);

            return 0;
        }
    }

    return -1;
}

/**********************************************************************
* @name      : CompareTSA
* @brief     ：比较两个OOP_TSA_T 地址是否相同
* @param[in] ：OOP_TSA_T tsa1
               OOP_TSA_T tsa2
* @param[out]：
* @return    ：0相同/<0不相同
* @Create    : 梅安宁
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int CompareTSA(OOP_TSA_T tsa1, OOP_TSA_T tsa2)
{
    int i;
    if(tsa1.af != tsa2.af)
        return -1;

    for(i=0; i<tsa1.len; i++)
    {
        if(tsa1.add[i] != tsa2.add[i])
            return -1;
    }
    return 0;
}

/**********************************************************************
* @name      : OopTIToMIn
* @brief     ：执行时间间隔转换成分钟
* @param[in] ：OOP_TI_T oopTI          执行时间间隔
* @param[out]：
* @return    ：分钟数
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
uint32 OopTIToMIn(OOP_TI_T oopTI)
{
    switch(oopTI.unit)
    {
    case 0:
        return oopTI.value/60;
    case 1:
        return oopTI.value;
    case 2:
        return oopTI.value*60;
    case 3:
        return oopTI.value*60*24;
    case 4:
        return oopTI.value*60*24*30;
    case 5:
        return oopTI.value*60*24*30*365;
    default:
        return CB_INTERVAL;
    }
}


/**********************************************************************
* @name      : GetEventNumAndIndex
* @brief     ：获取事件的记录数和记录序号
* @param[in] ：RECORD_OAD_T    recordOAD     事件OAD
* @param[out]：uint16*         nEventNum     事件记录数
               uint32*        nIndex        记录序号
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int GetEventNumAndIndex(RECORD_OAD_T recordOAD, uint16* nEventNum, uint32* nIndex)
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
    startReadRecord.recordOAD = recordOAD;
    startReadRecord.cType = 0x02;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = timep;
    startReadRecord.sortType = 0x01;

    ret = db_read_record_start(g_sDataCenterClient, &startReadRecord, &handle, &recNum);
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_read_record_start, OAD=%8X error ret %d\n", recordOAD.road.oadMain.value, ret);
        return ret;

    }

    *nEventNum = (uint16)recNum;

    if(recNum == 0)
    {
        db_read_record_end(g_sDataCenterClient, handle);

        return ERR_OK;
    }

    stepReadRecord.MAC = MAC;

    ret = db_read_record_step(g_sDataCenterClient, handle, &stepReadRecord, &stepRecordOut);
    db_read_record_end(g_sDataCenterClient, handle);

    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("db_read_record_step,infoNum=%hu, OAD=%8X error ret %d\n", recordOAD.infoNum, recordOAD.road.oadMain.value, ret);
        return ret;

    }
    else
    {
        pTemp = (OOP_EVENT_T *)stepRecordOut.record[0].outDataBuf;
        *nIndex = pTemp->nIndex;
    }

    return ERR_OK;
}

#if 0
/**********************************************************************
* @name      : GetCollectFileConfig
* @brief     ：获取采集档案配置表
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int GetCollectFileConfig()
{
    NOMAL_OAD_T nomalOAD;
    int32 ret = -1;
    uint32 len = 0;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
    memset(oopMeterInfo, 0, sizeof(OOP_METER_INFO)*MTR485_MAX_NUM);

    nomalOAD.logicId = 0;
    nomalOAD.oad.value = 0x60000200;

//    MTR485Num = MTR485_MAX_NUM;
    int i;
    for(i=0; i<NO_VIR_MTR_MAX_NUM; i++)
    {
        nomalOAD.infoNum = i+1;
        ret = db_read_nomal(g_sDataCenterClient, &nomalOAD, sizeof(OOP_METER_T), (uint8 *)&oopMeterInfo[i].oopMeter, &len);
        if (ret != ERR_OK || len == 0)
        {
            EVENT_FMT_DEBUG("nomal_data_read OAD=%8X info=[%hu] error ret%d, len=%u\n", nomalOAD.oad.value, nomalOAD.infoNum, ret, len);
            continue;
        }
        else if(len == sizeof(OOP_METER_T))
        {
            EVENT_FMT_TRACE("Get OOP_METER_T %d \n", i+1);
//            EVENT_FMT_TRACE("oopMeterInfo[%d].oopMeter.port%08X,OI%04X Index%02X!\n", i+1, oopMeterInfo[i].oopMeter.basic.port.value
//                , oopMeterInfo[i].oopMeter.basic.port.nObjID, oopMeterInfo[i].oopMeter.basic.port.nIndex);

//            EVENT_FMT_TRACE("TSA[%hu] %02X %02X %02X %02X %02X %02X\n",  i+1
//                , oopMeterInfo[i].oopMeter.basic.tsa.add[0], oopMeterInfo[i].oopMeter.basic.tsa.add[1]
//                , oopMeterInfo[i].oopMeter.basic.tsa.add[2], oopMeterInfo[i].oopMeter.basic.tsa.add[3]
//                , oopMeterInfo[i].oopMeter.basic.tsa.add[4], oopMeterInfo[i].oopMeter.basic.tsa.add[5]);

            oopMeterInfo[i].enabled = 1;
        }
    }

    return ERR_OK;
}

/**********************************************************************
* @name      : GetTaskConfig
* @brief     ：获取任务配置表
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-17
* @Update    :
**********************************************************************/
int GetTaskConfig()
{
    NOMAL_OAD_T nomalOAD;
    int32 ret = -1;
    uint32 len = 0;
    int i = 0;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
    memset(oopTask, 0, sizeof(OOP_TASK_T)*TASK_MAX_NUM);

    //获取 事件有效标识
    nomalOAD.logicId = 0;
    nomalOAD.oad.value = 0x60120200;
    for(i=0; i<TASK_MAX_NUM; i++)
    {
        nomalOAD.infoNum = i+1;
        ret = db_read_nomal(g_sDataCenterClient, &nomalOAD, sizeof(OOP_TASK_T), (uint8 *)oopTask+i, &len);
        if (ret != ERR_OK)
        {
            EVENT_FMT_DEBUG("nomal_data_read OAD=%8X info=[%hu] error ret%d, len=%u\n", nomalOAD.oad.value, nomalOAD.infoNum, ret, len);
            continue;
        }
        else if(len == sizeof(OOP_TASK_T))
        {
            EVENT_FMT_TRACE("Get oopTask %hu OK\n", oopTask[i].id);
        }
    }
    return ERR_OK;
}
#endif

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
    NOMAL_OAD_T  nomalOAD;
    RECORD_OAD_T recordOAD;
    int        ret = -1;
    uint32       len = 0;
//    int          i = 0;

    if(eventCommInfo == NULL)
    {
        EVENT_FMT_DEBUG("IniEventCommInfo error, eventCommInfo is  null.\n");
        return -1;
    }

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
    memset(&recordOAD,0,sizeof(RECORD_OAD_T));

    //获取 事件有效标识
    nomalOAD.logicId = 0;
    nomalOAD.infoNum = 0;
    nomalOAD.oad.value = Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0900);
    ret = db_read_nomal(g_sDataCenterClient, &nomalOAD, sizeof(uint8), &eventCommInfo->checkflag, &len);
    if (ret != ERR_OK || len == 0)
    {
        EVENT_FMT_DEBUG("nomal_data_read OAD=%8X error, ret=%d, len=%u\n", nomalOAD.oad.value, ret, len);
        return ret;
    }
/*
    if(eventCommInfo->checkflag == 0)//事件无效
    {
        EVENT_FMT_DEBUG("OAD=[%8X] eventCommInfo->checkflag=0\n", nomalOAD.oad.value);
        return ERR_OK;
    }
*/
    //获取 事件上报标识
    len = 0;
    nomalOAD.logicId = 0;
    nomalOAD.infoNum = 0;
    nomalOAD.oad.value = Uint16To32(eventCommInfo->eventOAD.nObjID, 0X0800);
    ret = db_read_nomal(g_sDataCenterClient, &nomalOAD, sizeof(uint8), &eventCommInfo->reportflag, &len);
    if (ret != ERR_OK || len == 0)
    {
        EVENT_FMT_DEBUG("nomal_data_read OAD=%8X error, ret=%d, len=%u\n", nomalOAD.oad.value, ret, len);
        return ret;
    }

    // 事件记录序号
    recordOAD.road.oadMain.value = 0;
    recordOAD.road.oadCols.nNum = 1;
    recordOAD.road.oadCols.oad[0] = eventCommInfo->eventOAD;
    recordOAD.logicId = 0;
    recordOAD.infoNum = 0;
    ret = GetEventNumAndIndex(recordOAD, &eventCommInfo->eventNum, &eventCommInfo->recordnum);
    if(ret != ERR_OK )
    {
        EVENT_FMT_DEBUG("Get event[%8X] recordnum error return %d\n", eventCommInfo->eventOAD.value, ret);
        return ERR_NORMAL;
    }

    EVENT_FMT_TRACE("IniEventCommInfo,event[%8X] checkflag=%u ,recordnum=%u!\n", eventCommInfo->eventOAD.value, eventCommInfo->checkflag, eventCommInfo->recordnum);

    return ERR_OK;
}

/**********************************************************************
* @name      : GetMntAddr
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

    memcpy(MAC.value, tmnAddr.value, MAC.nNum);
    EVENT_FMT_TRACE("MAC=%02X %02X %02X %02X %02X %02X \n", MAC.value[0], MAC.value[1], MAC.value[2], MAC.value[3], MAC.value[4], MAC.value[5]);
    return ;
}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取终端初始化事件参数
输入参数：

输出参数：
  event_3000   终端初始化事件
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
********************************************************************/
int Init3000Para()
{
    int ret = -1;

    memset(&event_3000, 0, sizeof(EVENT_3000_INFO));

    OOP_RCSD_T eventRCSD={
        7, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x20002200},}},//事件发生时电压
                {0,{{0x20012200},}},//事件发生时电流
            }
    };

    event_3000.eventCommInfo.eventRCSD = eventRCSD;
    event_3000.eventCommInfo.eventOAD.nObjID = 0X3000;
    event_3000.eventCommInfo.eventOAD.value = 0X30000200;

    // 公共信息
    ret = IniEventCommInfo(&event_3000.eventCommInfo);
    if (ret < ERR_OK)
    {
        EVENT_FMT_DEBUG("OAD=[%8X] IniEventCommInfo error!\n", event_3000.eventCommInfo.eventOAD.value);
        return ret;
    }

    event_3000.initflag = 1;
    return ERR_OK;
}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取终端版本号变更事件参数
输入参数：

输出参数：
  event_3001   终端版本号变更事件全局变量
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
    从数据中心获取终端版本号变更事件参数，写入全局变量中。
********************************************************************/
int Init3001Para()
{
    int             ret     = -1;
    uint32          len     = 0;
    NOMAL_OAD_T     nomalOAD;
    OOP_VERSION_T   tVersion;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
    memset(&tVersion,0,sizeof(OOP_VERSION_T));
    memset(&event_3001, 0, sizeof(EVENT_3001_INFO));

    OOP_RCSD_T eventRCSD={
        6, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x20002200},}},//事件发生时电压
                {0,{{0x20012200},}},//事件发生时电流
            }
    };

    event_3001.eventCommInfo.eventRCSD = eventRCSD;

    event_3001.eventCommInfo.eventOAD.nObjID= 0X3001;
    event_3001.eventCommInfo.eventOAD.value= 0X30010200;

    // 公共信息
    ret = IniEventCommInfo(&event_3001.eventCommInfo);
    if (ret != ERR_OK )
    {
        EVENT_FMT_DEBUG("OAD=[%8X] IniEventCommInfo error!\n", event_3001.eventCommInfo.eventOAD.value);
//        return ret;

    }
/*
    //事件无效
    if(event_3001.eventCommInfo.checkflag == 0)
    {
        event_3001.initflag = 1;
        return ERR_OK;
    }
*/
    //TODO 读取电压
    nomalOAD.oad.value = 0x43000300;
    ret = db_read_nomal(g_sDataCenterClient, &nomalOAD, sizeof(OOP_VERSION_T), (uint8*)&tVersion, &len);
    if (ret != ERR_OK || len == 0)
    {
        EVENT_FMT_DEBUG("nomal_data_read OAD=%8X error ret=%d,len=%u\n", nomalOAD.oad.value, ret, len);
        return ret;
    }

    event_3001.initflag = 1;

    return 0;
}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取状态量变位事件参数
输入参数：

输出参数：
  event_3002   状态量变位事件
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
********************************************************************/
int Init3002Para()
{
    int ret = -1;

    memset(&event_3002, 0, sizeof(EVENT_3002_INFO));

    event_3002.eventCommInfo.eventOAD.nObjID = 0X3002;
    event_3002.eventCommInfo.eventOAD.value = 0X30020200;

    OOP_RCSD_T eventRCSD={
        6, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x20002200},}},//事件发生时电压
                {0,{{0x20012200},}},//事件发生时电流
            }
    };

    event_3002.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    ret = IniEventCommInfo(&event_3002.eventCommInfo);
    if (ret != ERR_OK)
    {
        EVENT_FMT_DEBUG("OAD=[%8X] IniEventCommInfo error!\n", event_3002.eventCommInfo.eventOAD.value);
//        return ret;
    }

    GetEventNomalParam(g_sDataCenterClient, 0, 0, 0xf2030200);
    GetEventNomalParam(g_sDataCenterClient, 0, 1, 0xf2030200);
    GetEventNomalParam(g_sDataCenterClient, 0, 0, 0xf2030400);

    event_3002.initflag = 1;
    return ERR_OK;
}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取电能表时间超差事件参数，写入全局变量中。
输入参数：
输出参数：
  event_3003   电能表时间超差事件全局变量
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
    从数据中心获取电能表时间超差事件参数，写入全局变量中。
********************************************************************/
int Init3003Para()
{
    NOMAL_OAD_T nomalOAD;
    int         ret     = -1;
    uint32      len     = 0;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
    memset(&event_3003, 0, sizeof(EVENT_3003_INFO));

    event_3003.eventCommInfo.eventOAD.nObjID= 0X3003;
    event_3003.eventCommInfo.eventOAD.value= 0X30030200;
    event_3003.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr3003;
    event_3003.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr3003;

    OOP_RCSD_T eventRCSD={
        7, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x20002200},}},//事件发生时电压
                {0,{{0x20012200},}},//事件发生时电流
            }
    };

    event_3003.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    ret = IniEventCommInfo(&event_3003.eventCommInfo);
    if (ret < ERR_OK )
    {
        EVENT_FMT_DEBUG("OAD=[%8X] IniEventCommInfo error!\n", event_3003.eventCommInfo.eventOAD.value);
//        return ret;
    }
/*
    //事件无效
    if(event_3003.eventCommInfo.checkflag == 0)
    {
        event_3003.initflag = 1;
        return ERR_OK;
    }
*/
    //TODO 获取 时钟超差门限，关联采集任务号
    nomalOAD.oad.value = 0x30030600;
    //ret = db_read_nomal(g_sDataCenterClient, &nomalOAD, sizeof(OOP_EVTCFG_3003_T), (uint8 *)&event_3003.checkPara, &len);
    if (ret != ERR_OK || len == 0)
    {
        EVENT_FMT_DEBUG("nomal_data_read OAD=%8X error ret=%d,len=%u\n", nomalOAD.oad.value, ret, len);
//        return ret;
    }

    //EVENT_FMT_TRACE("event_3003.checkPara.nValue=%04X \n", event_3003.checkPara.nValue);
    //EVENT_FMT_TRACE("event_3003.checkPara.nTID=%u \n", event_3003.checkPara.nTID);

    event_3003.initflag = 1;
    return ERR_OK;
}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取电能表时间超差事件参数，写入全局变量中。
输入参数：
输出参数：
  event_3003   电能表时间超差事件全局变量
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
    从数据中心获取电能表时间超差事件参数，写入全局变量中。
********************************************************************/
int Init3106Para()
{
    NOMAL_OAD_T nomalOAD;
    int         ret     = -1;
    int         i       = 0;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
    memset(&event_3106, 0, sizeof(EVENT_3106_INFO));

    event_3106.eventCommInfo.eventOAD.nObjID= 0X3106;
    event_3106.eventCommInfo.eventOAD.value= 0X31060200;

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
    ret = IniEventCommInfo(&event_3106.eventCommInfo);
    if (ret < ERR_OK )
    {

        EVENT_FMT_DEBUG("OAD=[%8X] IniEventCommInfo error!\n", event_3106.eventCommInfo.eventOAD.value);
//        return ret;

    }
/*
    //事件无效
    if(event_3106.eventCommInfo.checkflag == 0)
    {
        event_3106.initflag = 1;
        return ERR_OK;
    }
*/
    //获取 终端电压
    //获取 终端电压
    GetEventNomalParam(g_sDataCenterClient, 0, 0, 0x20000200);

    for(i=0; i<event_3106.tmnVoltage.nNum; i++)
    {
        if(event_3106.tmnVoltage.nValue[i] == 0XFFFF)
            event_3106.tmnVoltage.nValue[i] = 0;
    }

    //获取 事件配置参数
    GetEventNomalParam(g_sDataCenterClient, 0, 0, 0x31060600);

    event_3106.initflag = 1;

    return ERR_OK;
}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取设备故障事件参数
输入参数：

输出参数：
  event_3005   设备故障记录事件
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
********************************************************************/
int Init3005Para()
{
    int ret = -1;

    memset(&event_3005, 0, sizeof(EVENT_3005_INFO));

    event_3005.eventCommInfo.eventOAD.nObjID = 0X3005;
    event_3005.eventCommInfo.eventOAD.value = 0X30050200;

    OOP_RCSD_T eventRCSD={
        6, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x20012200},}},//事件发生时电流
            }
    };

    event_3005.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    ret = IniEventCommInfo(&event_3005.eventCommInfo);
    if (ret < ERR_OK)
    {
        EVENT_FMT_DEBUG("OAD=[%8X] IniEventCommInfo error!\n", event_3005.eventCommInfo.eventOAD.value);
//        return ret;
    }

    event_3005.initflag = 1;
    return ERR_OK;
}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取设备故障事件参数
输入参数：

输出参数：
  event_3005   设备故障记录事件
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
********************************************************************/
int Init300FPara()
{
    NOMAL_OAD_T nomalOAD;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
    memset(&event_300F, 0, sizeof(EVENT_300F_INFO));

    event_300F.eventCommInfo.eventOAD.nObjID= 0X300F;
    event_300F.eventCommInfo.eventOAD.value= 0X300F0200;
    event_300F.eventCommInfo.EventStoragNumRecAddr = EventStoragNumRecAddr300F;
    event_300F.eventCommInfo.EventStoragTmRecAddr = EventStoragTmRecAddr300F;

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

    //延时参数和状态字
    GetEventNomalParam(g_sDataCenterClient, 0, 0, 0x300f0600);
    GetEventNomalParam(g_sDataCenterClient, 0, 0, 0x20140200);

    event_300F.initflag = 1;

    return ERR_OK;
}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取设备故障事件参数
输入参数：

输出参数：
  event_3000   设备故障记录事件
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
********************************************************************/
int Init301EPara()
{
    int ret = -1;

    memset(&event_301E, 0, sizeof(EVENT_301D_INFO));

    event_301E.eventCommInfo.eventOAD.nObjID = 0X301E;
    event_301E.eventCommInfo.eventOAD.value = 0X301E0200;

    OOP_RCSD_T eventRCSD={
        6, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x20012200},}},//事件发生时电流
            }
    };

    event_301E.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    ret = IniEventCommInfo(&event_301E.eventCommInfo);
    if (ret < ERR_OK)
    {
        EVENT_FMT_DEBUG("OAD=[%8X] IniEventCommInfo error!\n", event_301E.eventCommInfo.eventOAD.value);
//        return ret;
    }

    event_301E.initflag = 1;
    return ERR_OK;

}

/*********************************************************************
所属功能单元：事件处理模块提供应用层主调度使用
功能描述：从数据中心获取设备故障事件参数
输入参数：

输出参数：
  event_3000   设备故障记录事件
函数返回值：
  0     成功
  <0    发生错误
内部处理流程：
********************************************************************/
int Init301DPara()
{
    int ret = -1;

    memset(&event_301E, 0, sizeof(EVENT_301E_INFO));

    event_301E.eventCommInfo.eventOAD.nObjID = 0X301E;
    event_301E.eventCommInfo.eventOAD.value = 0X301E0200;

    OOP_RCSD_T eventRCSD={
        6, 0,{
                {0,{{0X20220200},}},//事件记录序号
                {0,{{0X201E0200},}},//事件发生时间
                {0,{{0X20200200},}},//事件结束时间
                {0,{{0X20240200},}},//事件发生源
                {0,{{0X33000200},}},//通道上报状态

                {0,{{0x20002200},}},//事件发生时电压
            }
    };

    event_301E.eventCommInfo.eventRCSD = eventRCSD;

    // 公共信息
    ret = IniEventCommInfo(&event_301E.eventCommInfo);
    if (ret < ERR_OK)
    {
        EVENT_FMT_DEBUG("OAD=[%8X] IniEventCommInfo error!\n", event_301E.eventCommInfo.eventOAD.value);
//        return ret;
    }

    event_301E.initflag = 1;

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
    int ret = -1;

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
    ret = IniEventCommInfo(&event_3119.eventCommInfo);
    if (ret < ERR_OK )
    {
        EVENT_FMT_DEBUG("OAD=[%8X] IniEventCommInfo error!\n", event_3119.eventCommInfo.eventOAD.value);
        return ret;
    }

    event_3119.initflag = 1;
    return ERR_OK;
}


/*********************************************************************
所属功能单元：事件检测模块提供主调度使用
名称：事件检测参数初始化函数
功能描述：事件检测参数的初始化
输入参数：无
输出参数：无
函数返回值：无
内部处理流程： 事件检测参数的初始化，对全局变量和私有数据的初始化
********************************************************************/
void InitEventPara(void)
{
    NOMAL_OAD_T nomalOAD;
    int32 ret = -1;
    uint32 len = 0;

    memset(&nomalOAD, 0, sizeof(NOMAL_OAD_T));

    memset(&tmnAddr, 0, sizeof(OOP_OCTETVAR16_T));
    memset(&MAC, 0, sizeof(OOP_OCTETVAR64_T));

    if(g_sDataCenterClient <= 0)
    {
        EVENT_FMT_DEBUG("g_sDataCenterClient创建失败, 返回! \n");
        return ;
    }

    Init3000Para();
    Init3001Para();
    Init3002Para();
    Init3003Para();
    Init3005Para();
    Init300FPara();
    Init3106Para();
    Init3119Para();

    //终端通信地址
    nomalOAD.logicId = 0;
    nomalOAD.infoNum = 0;
    nomalOAD.oad.value = 0x40010200;
    ret = db_read_nomal(g_sDataCenterClient, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&tmnAddr, &len);
    if (ret != ERR_OK || len == 0)
    {
        EVENT_FMT_DEBUG("nomal_data_read OAD=%8X error ret=%d,len=%d\n", nomalOAD.oad.value, ret, len);
//        return ;
    }

    MAC.nNum = 6;
    memcpy(MAC.value, tmnAddr.value, MAC.nNum);
    EVENT_FMT_TRACE("MAC=%02X %02X %02X %02X %02X %02X \n", MAC.value[0], MAC.value[1], MAC.value[2], MAC.value[3], MAC.value[4], MAC.value[5]);

#if 0
    //允许终端主动上报标志,1允许,0不允许
    nomalOAD.logicId = 0;
    nomalOAD.infoNum = 0;
    nomalOAD.oad.value = 0x43000800;
    ret = db_read_nomal(g_sDataCenterClient, &nomalOAD, sizeof(uint8), &TnmEventReport, &len);
    if (ret != ERR_OK || len == 0)
    {
        EVENT_FMT_DEBUG("nomal_data_read OAD=%8X error ret=%d,len=%d\n", nomalOAD.oad.value, ret, len);
//        return ;
    }
#endif

    InitLatestMeterReadDataList(g_sDataCenterClient);

    return;
}

/**********************************************************************
* @name      : dbCenter_recv_msginfo
* @brief     ：数据中心通知消息解析成指定格式
* @param[in] ：uint8 *pBuf           接受到的报文
               uint16 bufLen         报文长度
* @param[out]：MSG_INFO_T **ppMsgOut 解析出来的消息，用完需要释放free
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2019-12-07
* @Update    :
**********************************************************************//*
int dbCenter_recv_msginfo( uint8 *pBuf, uint16 bufLen, MSG_INFO_T **ppMsgOut)
{
    uint8  *pTemp = pBuf;
    uint8   offset = 0;
    uint16  tempLen = 0;
    MSG_INFO_T tMsgTemp;
    MSG_INFO_T *ptNewMsg = NULL;

    if(NULL == pBuf || NULL == ppMsgOut || 0 == bufLen)
    {
        return -1;
    }

    memset(&tMsgTemp, 0, sizeof(MSG_INFO_T));

    //PRM
    tMsgTemp.MsgRPM = (*pTemp) & 0x01;

    //PRIORITY
    tMsgTemp.MsgPRIORITY = ((*pTemp)>>1) & 0x7f;

    // INDEX
    pTemp++;
    tMsgTemp.MsgIndex = *((uint16 *)pTemp);

    // LABEL
    pTemp += 2;
    tMsgTemp.MsgLabel = *((uint16 *)pTemp);

    // SOURCE
    pTemp += 2;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
        MEMPRINT("SOURCE too long %d\n", tempLen);
        return -1;
    }
    memcpy(tMsgTemp.SourAddr, pTemp, tempLen);
    tMsgTemp.SourLen = tempLen;

    // DESTINATION
    pTemp += tempLen + 1;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
        MEMPRINT("DESTINATION too long %d\n", tempLen);
        return -1;
    }
    memcpy(tMsgTemp.DestAddr, pTemp, tempLen);
    tMsgTemp.DestLen = tempLen;

    // TAG
    pTemp += tempLen + 1;
    tMsgTemp.IID = (uint16)((uint16)(*(pTemp) << 8) + (uint16)(*(pTemp + 1)));
    tMsgTemp.IOP = (uint16)((uint16)(*(pTemp + 2) << 8) + (uint16)(*(pTemp + 3)));

    // MsgLen
    pTemp += 4;
    tMsgTemp.MsgLen = get_len_offset(pTemp, &offset);
    pTemp += offset;

    // 拷贝
    ptNewMsg = (MSG_INFO_T *)malloc(sizeof(MSG_INFO_T) + tMsgTemp.MsgLen);
    if(NULL == ptNewMsg)
    {
        MEMPRINT("malloc failed,size %d\n", sizeof(MSG_INFO_T) + tMsgTemp.MsgLen);
        return -1;
    }

    memcpy(ptNewMsg, &tMsgTemp, sizeof(MSG_INFO_T));
    memcpy(ptNewMsg->MsgData, pTemp, tMsgTemp.MsgLen);

    *ppMsgOut = ptNewMsg;
    return 0;
}
*/
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
    CheckERC_3106();        //停电时间检测
    //CheckSelfERC();         //自检
}

/*******************************************************************************
* 所属单元：事件处理模块用电现场检测子模块
* 功能描述：测量点告警处理接口。
* 输入参数：mtr_id  测量点号
* 输出参数：无
* 返 回 值：无
* 内部流程：根据当前告警配置信息，调用告警处理函数，处理告警数据。
*******************************************************************************/
void MetExam(mtrnum_t mtr_id, OOP_METER_T *pOopMeter)
{
#if DEBUG_EN == 1
    EVENT_FMT_DEBUG("抄表成功,准备填充全局结构体\n");
#endif
    OOP_METER_T oopMeter = {0};

    if (mtr_id != 0)
    {
        oopMeter = *pOopMeter;
    }
    else
    {
        oopMeter.basic.stVolt = 2200;
        oopMeter.basic.stAmp = 1500; //1.5A*1000
        oopMeter.basic.pwrType = 1;
    }

    FillEventGlobal(mtr_id, &oopMeter);                         //填充全局结构体

/*
    if (ReadRamXData(Tmn_Power_Info_ID,(char *)&bfgPower) < 0)
    {
    #if DEBUG_EN == 1
        EVENT_FMT_DEBUG("读取终端停电标志失败\n");
    #endif
        return;                                                 //读取终端停电标志
    }
    if ((bfgPower == 0) || (bfgPower <- 1))
    {
    #if DEBUG_EN == 1
        EVENT_FMT_DEBUG("停电/标志未知返回\n");
    #endif
        return;                                                 //未知
    }
*/
    CheckERC_3119(mtr_id, &oopMeter);                            //电流回路异常检测
    CheckERC_3000(mtr_id, &oopMeter);                            //失压检测
    CheckERC_3001(mtr_id, &oopMeter);                            //欠压检测
    CheckERC_3002(mtr_id, &oopMeter);                            //过压检测
    CheckERC_3003(mtr_id, &oopMeter);                            //断相检测
    CheckERC_3005(mtr_id, &oopMeter);                            //过流检测
    CheckERC_300F(mtr_id, &oopMeter);                            //逆相检测
    CheckERC_301D(mtr_id, &oopMeter);                            //电压不平衡检测
    CheckERC_301E(mtr_id, &oopMeter);                            //电流不平衡检测
}

/*********************************************************************
所属功能单元：事件检测
名称：       失压检测
输入参数：mtr_id  测量点号，测量点参数
输出参数：无
函数返回值：无
内部处理流程：根据终端不同复位标志填写记录。
********************************************************************/
void  CheckERC_3000(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
    if (event_3000.eventCommInfo.checkflag != 1) {
        EVENT_FMT_DEBUG("未使能3000失压检测\n");
        return;
    }

    event_3000.oopWord3V[mtr_id] = g_LatestMeterReadData.valTemp[mtr_id];
    event_3000.oopWord3C[mtr_id] = g_LatestMeterReadData.curTemp[mtr_id];

    if (event_3000.oopWord3V[mtr_id].nNum < 1) {
        EVENT_FMT_DEBUG("电压无效(3000)\n");
        return;
    }

    EVENT_FMT_DEBUG("测量点%u失压恢复检测开始\n", mtr_id);
    VltBreakEvent(mtr_id, oopMeter, 4);
    EVENT_FMT_DEBUG("测量点%u失压检测开始\n", mtr_id);
    VltBreakEvent(mtr_id, oopMeter, 3);
}

/*********************************************************************
所属功能单元：事件检测
名称：       电压越下限检测
输入参数：mtr_id  测量点号，测量点参数
输出参数：无
函数返回值：无
内部处理流程：根据终端不同复位标志填写记录。
********************************************************************/
void  CheckERC_3001(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
//    static uint8 bfgfisrt = 0; //首次运行标志
    if (event_3001.eventCommInfo.checkflag != 1){
        EVENT_FMT_DEBUG("未使能3001电压越限检测\n");
        return;
    }

    event_3001.oopWord3V[mtr_id] = g_LatestMeterReadData.valTemp[mtr_id];
    if (event_3001.oopWord3V[mtr_id].nNum < 1) {
        EVENT_FMT_DEBUG("电压无效(3001)\n");
        return;
    }

    EVENT_FMT_DEBUG("测量点%u电压越下限恢复检测开始\n", mtr_id);
    VltSupassEvent(mtr_id, oopMeter, 4);
    EVENT_FMT_DEBUG("测量点%u电压越下限检测开始\n", mtr_id);
    VltSupassEvent(mtr_id, oopMeter, 3);
}

/*********************************************************************
所属功能单元：事件检测
名称：       电压越上限检测
输入参数：mtr_id  测量点号，测量点参数
输出参数：无
函数返回值：无
内部处理流程：根据终端不同复位标志填写记录。
********************************************************************/
void CheckERC_3002(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
    /*- 数据内容                数据格式    字节数
        ERC=4                   BIN         1
        长度Le                  BIN         1
        变位时间：分时日月年    数据格式15  5
        状态变位                BS8         1
        变位后状态              BS8         1-*/
    if (event_3002.eventCommInfo.checkflag != 1){
        EVENT_FMT_DEBUG("未使能3002电压越限检测\n");
        return;
    }

    event_3002.oopWord3V[mtr_id] = g_LatestMeterReadData.valTemp[mtr_id];
    if (event_3002.oopWord3V[mtr_id].nNum < 1) {
        EVENT_FMT_DEBUG("电压无效(3002)\n");
        return;
    }

    EVENT_FMT_DEBUG("测量点%u电压越上限恢复检测开始\n", mtr_id);
    VltSupassEvent(mtr_id, oopMeter, 2);
    EVENT_FMT_DEBUG("测量点%u电压越上限检测开始\n", mtr_id);
    VltSupassEvent(mtr_id, oopMeter, 1);
}

/*******************************************************************************
* 函数名称:     crc_calc
* 函数功能:     电表校验未计算
* 输入参数：    databuf
                datalen
* 输出参数：
* 内部处理：
* 返 回 值:     NULL
*******************************************************************************/
uint8 crc_calc(char *databuf, uint8 datalen)
{
    uint8    start, i;
    uint8    crc = 0;

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
void PrintBuf(uint8 *buf, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        DPRINT("%02x ", buf[i]);
    }

    if (len > 0)
    {
        DPRINT("\n");
    }
}

void ConverToOopTime(char *Srcuff, uint8 *desBuff)
{
    uint16 year = 0;
    if (!Srcuff || !desBuff)
    {
        return;
    }
    year = 2000 + commfun_BcdToHex(Srcuff[5]);
    desBuff[0] = year >> 8;
    desBuff[1] = year & 0xff;
    desBuff[2] = commfun_BcdToHex(Srcuff[4]);
    desBuff[3] = commfun_BcdToHex(Srcuff[3]);
    desBuff[4] = commfun_BcdToHex(Srcuff[2]);
    desBuff[5] = commfun_BcdToHex(Srcuff[1]);
    desBuff[6] = commfun_BcdToHex(Srcuff[0]);
    return;
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

    ret = db_read_nomal(g_sDataCenterClient, &nomalOAD, sizeof(OOP_RS485_T), (uint8 *)&Rs485, &len);
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
* 函数名称: SavneTmnHaltEvent
* 函数功能: 终端的停/上电事件记录
* 输入参数: MetSn             //测量点号 0表示终端
            TmnHalt_Attrtbute //0:停电记录   ,1表示上电记录
            PowerOffTime      //停电时间
            PowerOnTime       //上电时间
            Valid             //置"1"：事件有效，置"0"：事件无效；
            HaltProtery       //置"1"：台区或用户停电事件，置"0"：终端或电能表停电事件
            uint8 WriteFlag   //0表示新生成  ,1表示刷新上电记录
* 输出参数:
* 内部处理:
* 返 回 值: 无
*******************************************************************************/
int SaveHaltEvent(uint8 TmnHalt_Attrtbute, char *PowerOffTime, char *PowerOnTime, uint8 Valid, uint8 HaltProtery, uint8 WriteFlag)
{
/*
    char                HaltInfo[12 * 10];
    uint16              HaltCnt = 0;
    int                 i;
    ERC14_RECORD_STRUCT Erc14_Record;

    memset(&Erc14_Record, 0xFF, sizeof(Erc14_Record));

    Erc14_Record.ErcNo = 14;
    Erc14_Record.ErcRecLen = 11;
#if ZHEJIANG_JZQ_EN == 1      //浙江要求状态位标识一律填01
    Erc14_Record.ErcMetSn.D0 = 1;
    Erc14_Record.ErcMetSn.D1 = 0;
#else
    Erc14_Record.ErcMetSn.D0 = Valid;
    Erc14_Record.ErcMetSn.D1 = HaltProtery;
#endif
    Erc14_Record.ErcMetSn.D2_D7 = 0;

    DEPRINT(ERC_NO_14, "终端类 停/上电事件 \n");

    if ((ReadTmnXData(RT_DATA, EVENT_TMNHALT_CNT, (char *)&HaltCnt) < 0) || (HaltCnt == 0xffff))
    {
        HaltCnt = 0;
    }
    if (ReadTmnXData(RT_DATA, EVENT_TMNHALT_INFO, HaltInfo) < 0)
    {
        memset(&HaltInfo, 0xFF, sizeof(HaltInfo));
    }

    if (TmnHalt_Attrtbute == 0)//停电记录
    {
        DEPRINT(ERC_NO_14, "存储 停电 记录 \n");

        memcpy_r(Erc14_Record.TmnPowerOffTime, &PowerOffTime[1], 5);

#if SHANDONG_FUN == 1
        char bData[100];
        memset(bData, 0x00, sizeof(bData));
        if (ReadTmnXData(RT_DATA, TMN_POWER_ON_TIME, bData) < 0)
        {
            memset(bData, 0xee, sizeof(bData));
        }
        memcpy_r(Erc14_Record.TmnPowerOnTime, (char *)&bData[1], 5);
#else
        memset(Erc14_Record.TmnPowerOnTime, 0xEE, 5);
#endif

        if (ENSaveRec(ERC_NO_14) == 1)
        {


            RecordWrite(ERC_NO_14, (uint8 *)&Erc14_Record,
                ERC_NO_14_LEN, NEW_RECORD_TYPE | NONE_SEND_TYPE, 0);
        }

        //刷新停电记录
        i = 9;
        while (i)
        {
            memcpy_r(&HaltInfo[i * 12], &HaltInfo[(i - 1) * 12], 12);//事件的偏移
            i--;
        }
        memset(HaltInfo, 0xFF, 12);
        memcpy_r(&HaltInfo, PowerOffTime, 6);//刷新停电时间


    }
    else
    {
        HaltCnt++;
        memcpy_r(Erc14_Record.TmnPowerOffTime, &PowerOffTime[1], 5);
        memcpy_r(Erc14_Record.TmnPowerOnTime, &PowerOnTime[1], 5);
        if (Valid)
        {
            RecordWrite(ERC_NO_14, (uint8 *)&Erc14_Record,
                ERC_NO_14_LEN, NEW_RECORD_TYPE | NONE_SEND_TYPE, 0);
        }
        if (HaltInfo[4] == 0xFF || HaltInfo[8] != 0xFF)
        {
            memcpy_r(&HaltInfo[0], PowerOffTime, 6);
        }

        memcpy_r(&HaltInfo[6], PowerOnTime, 6);
        DEPRINT(ERC_NO_14, "存储上电记录 %X %X %X %X %X %X\n", HaltInfo[6], HaltInfo[7],
            HaltInfo[8], HaltInfo[9], HaltInfo[10], HaltInfo[11]);
    }

    WriteTmnXData(RT_DATA, EVENT_TMNHALT_CNT, (char *)&HaltCnt);
    WriteTmnXData(RT_DATA, EVENT_TMNHALT_INFO, HaltInfo);
*/
    return 0;
}

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
#define         COUNT_NUM         5        // 持续判断的次数
int CheckTmn_Halt(void)
{
    int   bTemp = 0;                                    // 上电; 0:未上电
    OOP_DATETIME_S_T TmnTime = { 0 };                   //终端时间
    static OOP_DATETIME_S_T lastPowerOffTime = { 0 };   //终端时间
    static int8 bfgPower = 1;                           //-电池状态标志-
    uint16 SetVol = 132;                                //停电电压限值
    uint16 ResetVol = 176;                              //上电电压限值
    //OOP_METER_PWRTYPE_E powerType = PWRTYPE_UNKNOWN;
    int fixVolt = 0;                                    // 额定电压
    static uint8 PowerOffCount = 0;
    static uint8 PowerOnCount = 0;
    static uint8 FirstRun = 0x55;

    OOP_EVTSOURCE_T eventSource;                        //事件发生源
    OOP_EVTBUF_T    eventData;                          //事件关联数据

    memset(&eventSource,0,sizeof(OOP_EVTSOURCE_T));
    memset(&eventData,0,sizeof(OOP_EVTBUF_T));

    GetCurrentOopDateTimeS(&TmnTime);                   //-取实时时间-

    // 接线方式及额定电压，
    //powerType = PWRTYPE_3PHASE4;
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
        event_3106.ishappen = 0x55;        //停电已经发生
        bfgPower = 0;
    }
    else
    {
        event_3106.ishappen = 0xaa;        //停电未发生
        bfgPower = 1;
        write_pdata_xram("^Event", (const char *)&event_3106.ishappen, TmnPowerOffAddr, TmnPowerOffLen);
    }

    EVENT_FMT_DEBUG("CheckERC_3106 检测终端停电时间 bfgPower = %d Volt[0] = %d Volt[1] = %d Volt[2] = %d SetVol = %d ResetVol = %d\n",
                    bfgPower, event_3106.tmnVoltage.nValue[0], event_3106.tmnVoltage.nValue[1], event_3106.tmnVoltage.nValue[2], SetVol, ResetVol);

    // 防错// 不会出现电池供电的情况下还有大于工作门限的电压
    if ((bfgPower < 0) && (event_3106.tmnVoltage.nValue[0] >= 900 || event_3106.tmnVoltage.nValue[1] >= 900 || event_3106.tmnVoltage.nValue[2] >= 900))
    {
        return 1;
    }

    if (event_3106.tmnVoltage.nValue[0] <= SetVol && event_3106.tmnVoltage.nValue[1] <= SetVol && event_3106.tmnVoltage.nValue[2] <= SetVol
            && (event_3106.tmnVoltage.nValue[0] >= 40 || event_3106.tmnVoltage.nValue[1] >= 40 || event_3106.tmnVoltage.nValue[2] >= 40))
    {
        //多次，防止电压抖动
        if (PowerOffCount < 3)
        {
            PowerOffCount++;
        }
        PowerOnCount = 0;
    }
    //不用电池供电，且当前电压大于上电限值，算上电
    else if ((event_3106.tmnVoltage.nValue[0] >= ResetVol || event_3106.tmnVoltage.nValue[1] >= ResetVol || event_3106.tmnVoltage.nValue[2] >= ResetVol)
            || (event_3106.tmnVoltage.nValue[0] < 40 && event_3106.tmnVoltage.nValue[1] < 40 && event_3106.tmnVoltage.nValue[2] < 40))

    {
        //多次，防止电压抖动
        if (PowerOnCount < COUNT_NUM)
        {
            PowerOnCount++;
        }
        PowerOffCount = 0;
    }

    //检测终端停电时间是否超过60S
    bTemp = PowerDownCheck();

    EVENT_FMT_TRACE("CheckERC_3106 检测终端停电时间bTemp=[%d]秒  PowerOffCount = %d PowerOnCount = %d flag = %02x\n",
        bTemp, PowerOffCount, PowerOnCount, event_3106.ishappen);

    //当前停电且未记录过停电
    if (PowerOffCount >= 3 && event_3106.ishappen == 0xaa)
    {
        SaveFileRecBfg(ERC_NO_14 | 0x80, 0x80, 0);//停电
        PowerOnCount = 0;
        //电池供电
        bfgPower = 0;
        //停电状态保存
        event_3106.ishappen = 0x55;
        write_pdata_xram("^Event", (const char *)&event_3106.ishappen, TmnPowerOffAddr, TmnPowerOffLen);
        EVENT_FMT_TRACE("CheckERC_3106 检测终端停电时间超过%dS产生停上电事件\n", POWER_STOP_TIME_LMT);
        write_pdata_xram("^Event", (char *)&TmnTime, ProgDownAddr, ProgDownRecLen);

#if 0
        //事件发生源
        eventSource.choice = DT_ENUM;
        eventSource.nValue = 0;             //停电

        //事件关联数据

        //当前判断为停电且未写过停电记录
        event_3106.eventFlag = 0x80;
        //写数据库
        ret = WriteEventToDB(0, &event_3106.eventCommInfo, eventSource, eventData, 0);
        if (ret < ERR_OK)
        {
            EVENT_FMT_DEBUG("CheckERC_3106 终端停电事件记录失败返回%d\n", ret);
            return ret;
        }
        EVENT_FMT_TRACE("CheckERC_3106 终端停电事件记录成功, storagenum=%u !\n", event_3106.eventCommInfo.storagenum[0]);
#endif
        return -1;
    }
    //当前上电且未记录过上电，或第一次上电且停电时间超过限值
    else if ((PowerOnCount >= COUNT_NUM && event_3106.ishappen == 0x55)
        || (bTemp == 1 && FirstRun == 0x55))
    {
        SaveFileRecBfg(ERC_NO_14 | 0x80, 0, 0);//上电
        PowerOffCount = 0;
        //正常供电
        bfgPower = 1;
        read_pdata_xram("^Event", (char *)&event_3106.tmnStarttime,ProgDownAddr, ProgDownRecLen);
        event_3106.tmnEndtime = TmnTime;
        write_pdata_xram("^Event", (char *)&TmnTime,ProgDownAddr, ProgDownRecLen);
        event_3106.ishappen = 0xaa;
        write_pdata_xram("^Event", (const char *)&event_3106.ishappen, TmnPowerOffAddr, TmnPowerOffLen);

        return 0;
    }
    else if (bfgPower > 0)
    {
        //停电事件只要精确到秒，所以私有外村只记录到分钟即可，否则每秒都要写入，很浪费Nand写入次数
        if (lastPowerOffTime.minute != TmnTime.minute || lastPowerOffTime.hour != TmnTime.hour || lastPowerOffTime.day != TmnTime.day ||
            lastPowerOffTime.month != TmnTime.month || lastPowerOffTime.year != TmnTime.year)
        {
            lastPowerOffTime = TmnTime;
            EVENT_FMT_DEBUG("终端停电时间%u-%u-%u %u:%u:%u \n", TmnTime.year, TmnTime.month, TmnTime.day, TmnTime.hour, TmnTime.minute, TmnTime.second);
            write_pdata_xram("^Event", (char *)&TmnTime, ProgDownAddr, ProgDownRecLen);
        }
    }
    //终端在上电状态的情况下实时刷新终端时间
    //-从数据字典读取终端停电标志,这个标志是在selfcheck检测中维护
    //-从数据字典读取终端停电标志,这个标志是在selfcheck检测中维护的-

    FirstRun = 0xAA;
    return 1;
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
#if 0
    OOP_METER_T oopMeter;

    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    // 有效性
    if (app_get_meter_list(Pn, &oopMeter) == 0)
    {
        DEPRINT(ERC_NO_14, " // 有效性无效\n");
        return -1;
    }

    // 测量点性质
    if ((oopMeter.basic.port.nObjID != OI_485) && (oopMeter.basic.port.nObjID  != OI_PLC)
//        && (oopMeterInfo[Pn].oopMeter.basic.userType != MTR_PLCMUI)
        )
    {
        DEPRINT(ERC_NO_14, " // 测量点性质不符\n");
        return -1;
    }

    // 规约类型
    if ((oopMeter.basic.protocol != (uint8)PROTO_07 && oopMeter.basic.protocol != (uint8)PROTO_698))
    {
        DEPRINT(ERC_NO_14, " // 规约类型不符\n");
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
    gMeterHalt_Info.MeterHaltInfo[realMeterHaltNum].Pn = Pn+1;
    gMeterHalt_Info.MeterHaltInfo[realMeterHaltNum].commnum = oopMeter.basic.port;
    gMeterHalt_Info.MeterHaltInfo[realMeterHaltNum].mode = oopMeter.basic.baud;
    gMeterHalt_Info.MeterHaltInfo[realMeterHaltNum].protocol = oopMeter.basic.protocol;
#endif

    return 1;
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
void CheckERC_3106(void)
{
    int TmnHaltRet = -1; //终端停上电返回
    if (event_3106.eventCommInfo.checkflag != 1) {
        EVENT_FMT_DEBUG("未使能3106停电检测\n");
        return;
    }

    //读取三相电压实时值
    event_3106.tmnVoltage = g_LatestMeterReadData.valTemp[0];

    if (event_3106.tmnVoltage.nNum < 1) {
        EVENT_FMT_DEBUG("电压无效(3106)\n");
        return;
    }

    //每秒中进行判断终端是否产生新的停上电事件 -1:停电事件, 0:上电事件, 1:一直供电
    TmnHaltRet = CheckTmn_Halt();
	if(TmnHaltRet < 0)
	{
		yx_calculate_sta_set(CALC_YX_POWER_FAILURE, 0, 1);
	}
	else
	{
		yx_calculate_sta_set(CALC_YX_POWER_FAILURE, 0, 0);
	}
}

/*********************************************************************
所属功能单元：事件检测
功能描述：电压断相检测
输入参数：测量点号
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_3003(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
    /*- 数据内容                                        数据格式    字节数
    ERC=12                                          BIN         1
    长度Le                                          BIN         1
    发生时间：分时日月年                            数据格式15  5
    D7：起/止标志   D6=0  D5~D0：mtr_id（测量点号） BIN         1-*/
    if (event_3003.eventCommInfo.checkflag != 1) {
        EVENT_FMT_DEBUG("未使能3003断相检测\n");
        return;
    }

    event_3003.oopWord3V[mtr_id] = g_LatestMeterReadData.valTemp[mtr_id];
    event_3003.oopWord3C[mtr_id] = g_LatestMeterReadData.curTemp[mtr_id];

    if (event_3003.oopWord3V[mtr_id].nNum < 1 && event_3003.oopWord3C[mtr_id].nNum < 1)
    {
        EVENT_FMT_DEBUG("测量点%u电压电流无效返回(3003)\n", mtr_id);
        return;
    }

    EVENT_FMT_DEBUG("测量点%u电压断相恢复检测开始\n", mtr_id);
    VltBreakEvent(mtr_id, oopMeter, 2);
    EVENT_FMT_DEBUG("测量点%u电压断相检测开始\n", mtr_id);
    VltBreakEvent(mtr_id, oopMeter, 1);
}

/*******************************************************************************
* 所属功能单元：事件检测
* 功能描述：电流越限检测
* 输入参数：测量点号，测量点参数
* 输出参数：无
* 返 回 值：无
* 内部流程：
*******************************************************************************/
void CheckERC_3005(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
    if (event_3005.eventCommInfo.checkflag != 1) {
        EVENT_FMT_DEBUG("未使能3005电流越限检测\n");
        return;
    }

    if (g_LatestMeterReadData.curTemp[mtr_id].nNum < 1)
    {
        EVENT_FMT_DEBUG("测量点%u无有效电流值(3005)\n", mtr_id);
        return;
    }

    event_3005.oopWord3C[mtr_id] = g_LatestMeterReadData.curTemp[mtr_id];

    EVENT_FMT_DEBUG("测量点%u电流越上限恢复检测开始\n", mtr_id);
    CurHighEvent(mtr_id, oopMeter, 2);
    EVENT_FMT_DEBUG("测量点%u电流越上限检测开始\n", mtr_id);
    CurHighEvent(mtr_id, oopMeter, 1);

    EVENT_FMT_DEBUG("测量点%u电流越上上限恢复检测开始\n", mtr_id);
    CurHighEvent(mtr_id, oopMeter, 4);
    EVENT_FMT_DEBUG("测量点%u电流越上上限检测开始\n", mtr_id);
    CurHighEvent(mtr_id, oopMeter, 3);
}

/*********************************************************************
所属功能单元：事件检测
功能描述：电压逆相检测
输入参数：测量点号，测量点参数
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_300F(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
    /*- 数据内容                                        数据格式    字节数
    ERC=10                                          BIN         1
    长度Le                                          BIN         1
    发生时间：分时日月年                            数据格式15  5
    D7：起/止标志   D6=0    D5~D0：pn（测量点号）   BIN         1
    异常标志                                        BS8         1
    发生时的Ua/Uab                                  数据格式07  2
    发生时的Ub                                      数据格式07  2
    发生时的Uc/Ucb                                  数据格式07  2
    发生时的Ia                                      数据格式06  2
    发生时的Ib                                      数据格式06  2
    发生时的Ic                                      数据格式06  2
    发生时电能表正向有功总电能示值                  数据格式14  5-*/
    if (event_300F.eventCommInfo.checkflag != 1) {
        EVENT_FMT_DEBUG("未使能300f相序异常检测\n");
        return;
    }

    if (g_LatestMeterReadData.valTemp[mtr_id].nNum < 3) {
        EVENT_FMT_DEBUG("测量点%u无有效的电压相角\n", mtr_id);
        return;
    }

    EVENT_FMT_DEBUG("测量点%u电压逆相检测开始\n", mtr_id);
    PhaseReverseEvent(mtr_id, oopMeter);
}

/*********************************************************************
所属功能单元：事件检测
功能描述：电压不平衡检测
输入参数：测量点号，测量点参数
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_301D(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
    if (event_301D.eventCommInfo.checkflag != 1)
    {
        EVENT_FMT_DEBUG("未使能301D电压不平衡检测\n");
        return;
    }

    EVENT_FMT_DEBUG("测量点%u电压不平衡检测开始\n", mtr_id);
    event_301D.oopWord3V[mtr_id] = g_LatestMeterReadData.valTemp[mtr_id];
    if (event_301D.oopWord3V[mtr_id].nNum < 3) {
        EVENT_FMT_DEBUG("电压无效(301D)\n");
        return;
    }

    VltCurImbalanceEvent(mtr_id, oopMeter, 1);
}

/*********************************************************************
所属功能单元：事件检测
功能描述：电流不平衡检测
输入参数：测量点号，测量点参数
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_301E(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
    if (event_301E.eventCommInfo.checkflag != 1)
    {
        EVENT_FMT_DEBUG("未使能301E电流不平衡检测\n");
        return;
    }

    EVENT_FMT_DEBUG("测量点%u电流不平衡检测开始\n", mtr_id);
    event_301E.oopWord3C[mtr_id] = g_LatestMeterReadData.curTemp[mtr_id];

    if (event_301E.oopWord3C[mtr_id].nNum < 3) {
        EVENT_FMT_DEBUG("电流无效(301E)\n");
        return;
    }

    VltCurImbalanceEvent(mtr_id, oopMeter, 2);
}

/*********************************************************************
所属功能单元：
功能描述：电流逆相检测
输入参数：测量点号
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void CheckERC_3119(mtrnum_t mtr_id, OOP_METER_T *oopMeter)
{
    /*- 数据内容                                    数据格式    字节数
    ERC=10                                          BIN         1
    长度Le                                          BIN         1
    发生时间：分时日月年                            数据格式15  5
    D7：起/止标志   D6=0    D5~D0：pn（测量点号）   BIN         1
    异常标志                                        BS8         1
    发生时的Ua/Uab                                  数据格式07  2
    发生时的Ub                                      数据格式07  2
    发生时的Uc/Ucb                                  数据格式07  2
    发生时的Ia                                      数据格式06  2
    发生时的Ib                                      数据格式06  2
    发生时的Ic                                      数据格式06  2
    发生时电能表正向有功总电能示值                  数据格式14  5-*/
    if (event_3119.eventCommInfo.checkflag != 1) {
        EVENT_FMT_DEBUG("未使能3119电流反极性检测\n");
        return;
    }

    event_3119.oopEnergys[mtr_id] = g_LatestMeterReadData.forwardActive[mtr_id];
    event_3119.oopWord4V[mtr_id] = g_LatestMeterReadData.oopWord4V[mtr_id];
    event_3119.oopWord4C[mtr_id] = g_LatestMeterReadData.oopWord4C[mtr_id];

//    CurrentShortCutEvent(9,mtr_id,2);   //-一次二次侧短路开路恢复检测-
//    EVENT_FMT_TRACE("CheckERC_3119 一次二次侧短路开路恢复检测 complete\n");
    if (event_3119.oopWord4V[mtr_id].nNum < 3)
    {
        EVENT_FMT_DEBUG("电压相角无效\n");
        return;
    }

    if (event_3119.oopWord4C[mtr_id].nNum < 3)
    {
        EVENT_FMT_DEBUG("电流相角无效\n");
        return;
    }

    CurReverseEvent(mtr_id, oopMeter,2);  //-反极性恢复检测-
    EVENT_FMT_TRACE("CheckERC_3119反极性恢复检测 complete\n");

//    CurrentShortCutEvent(9,mtr_id,1);   //-一次二次侧短路开路检测-
//    EVENT_FMT_TRACE("CheckERC_3119一次二次侧短路开路检测 complete\n");

    CurReverseEvent(mtr_id, oopMeter,1);  //-反极性发生检测-
    EVENT_FMT_TRACE("CheckERC_3119 反极性发生检测 complete\n\n\n");
}

// Ram自检(申请一片内存区域，能成功则置正常标志，反之异常)
static void chk_ram(void)
{
    int8 ram_info_id;
    void *p = NULL;
    p = malloc(1024);
    if (p == NULL) ram_info_id = -1;
    else {
        ram_info_id = 1;
        free(p);
        p = NULL;
    }

    //写数据字典Ram自检标志
    WriteRamXData(Tmn_Ram_Info_ID, (char *)&ram_info_id, 1);
}

// Flash自检(在YAFFS文件系统上创建临时文件,能成功创建则置正常标志,反之异常)
static int chk_flash(void)
{
    int8 flash_info_id;
    int fd;

    if( (fd = open("/mnt/tmpfile", O_WRONLY | O_CREAT | O_TRUNC)) < 0) //新建文件
        flash_info_id = -1;
    else {
        flash_info_id = 1;
        if (unlink("/mnt/tmpfile") < 0) {
            printf("unlink error\n");
            return -1;
        }
    }

    //写数据字典Flash自检标志
    WriteRamXData(Tmn_Flash_Info_ID, (char *)&flash_info_id, 1);

    return 0;
}

static int chk_eeprom(void)
{
    int8 eeprom_info_id;
    int ret1,ret2,ret;

    ret1 = access("/sys/bus/i2c/devices/2-0050/eeprom", F_OK);
    ret2 = access("/dev/i2c-0", F_OK);
    if (ret1 == 0 && ret2 == 0) {
            eeprom_info_id = 1;
    ret = 0;
    } else if (ret1 < 0 && ret2 < 0) {
        eeprom_info_id = 0;
        ret = -1;
    } else {
        eeprom_info_id = -1;
        ret = -1;
    }

    WriteRamXData(Tmn_Eeprom_Info_ID, (char *)&eeprom_info_id, 1);

    return ret;
}

// RTC自检(节点存在置正常标志否则置设备不正常标志)
static int chk_rtc_device(void)
{
    int8 rtc_info_id;
    int ret1,ret2,ret;

    // 查看devfs下的设备节点是否存在
    ret1 = access("/dev/rtc0", F_OK);
    // 查看sysfs下的设备节点是否存在
    ret2 = access("/sys/bus/platform/devices/rtc", F_OK);

    // 都存在则正常,都不存在则RTC设备不存在,否则异常
    if (ret1 == 0 && ret2 == 0) {
        rtc_info_id = 1;
        ret = 0;
    } else if (ret1 < 0 && ret2 < 0) {
        rtc_info_id = 0;
        ret = -1;
    } else {
        rtc_info_id = -1;
        ret = -1;
    }

    //写数据字典RTC自检标志
    WriteRamXData(Tmn_Rtc_Info_ID, (char *)&rtc_info_id, 1);

    return ret;
}

void CheckSelfERC(void)
{
    static uint8 bfgFirstRun = 1;
    static int8 bRamInfo = 0;  //-终端内存自检信息-
    static int8 bFlashInfo = 0;//-终端Flash自检信息-
    static int8 bEepromIfo = 0;//-终端EEPROM自检信息-
    static int8 bRtcInfo = 0;  //-终端RTC自检信息-
    int8  bTempData = -1;
    uint8 bRecTemp = 0;//-临时记录下面4种硬件故障类型，D0`D4分别表示ram、flash、eeprom、rtc-

    chk_ram();
    chk_flash();
    chk_eeprom();
    chk_rtc_device();

    if(bfgFirstRun)//-首次上电不作变化判别，只记录状态-
    {
        bfgFirstRun = 0;
        ReadRamXData(Tmn_Ram_Info_ID,    (char *)&bRamInfo, 1);
        ReadRamXData(Tmn_Flash_Info_ID,  (char *)&bFlashInfo, 1);
        ReadRamXData(Tmn_Eeprom_Info_ID, (char *)&bEepromIfo, 1);
        ReadRamXData(Tmn_Rtc_Info_ID,    (char *)&bRtcInfo, 1);
    }
    else
    {//-开始检测硬件标志是否有变化，如果有变化，填写Xram，记录变化状态-
        ReadRamXData(Tmn_Ram_Info_ID, (char *)&bTempData, 1);
        if((bRamInfo != bTempData)&&(bTempData < 0))
        {
            bRamInfo = bTempData;
            bRecTemp |= 0x01;
        }
        ReadRamXData(Tmn_Flash_Info_ID, (char *)&bTempData, 1);
        if((bFlashInfo != bTempData)&&(bTempData < 0))
        {
            bFlashInfo = bTempData;
            bRecTemp |= 0x02;
        }
        ReadRamXData(Tmn_Eeprom_Info_ID, (char *)&bTempData, 1);
        if((bEepromIfo != bTempData)&&(bTempData < 0))
        {
            bEepromIfo = bTempData;
            bRecTemp |= 0x04;
        }
        ReadRamXData(Tmn_Rtc_Info_ID, (char *)&bTempData, 1);
        if((bRtcInfo != bTempData)&&(bTempData < 0))
        {
            bRtcInfo = bTempData;
            bRecTemp |= 0x08;
        }

        if(bRecTemp)
        {
            SaveFileRecBfg(ERC_NO_14,0x80,0);//-记录在终端停电的xram定义中，分别使用-
        }
    }
}

/*********************************************************************
所属功能单元：
功能描述：清应用层外存数据
输入参数：无
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void ClearAppXData(void)
{
    uint8 bData[sizeof(RECODE_SAVE_T) * 256];

    memset(bData, 0, sizeof(RECODE_SAVE_T) * 256);
    write_pdata_xram("^Event", (const char *)bData, PowerDownAddr, PowerDownRecLen);
    write_pdata_xram("^Event", (const char *)bData, ProgDownAddr, ProgDownRecLen);
    write_pdata_xram("^Event", (const char *)bData, EventPhaseRecAddr, EventPhaseRecLen);
    write_pdata_xram("^Event", (const char *)bData, LostvltAddr, LostvltLen);
    write_pdata_xram("^Event", (const char *)bData, BreakphaseAddr, BreakphaseLen);
    write_pdata_xram("^Event", (const char *)bData, CurReverseRecAddr, CurReverseRecLen);
    write_pdata_xram("^Event", (const char *)bData, PhaseAbnormalAddr, PhaseAbnormalLen);
    write_pdata_xram("^Event", (const char *)bData, PhaseRvsLastTimesAddr, PhaseRvsLastTimesLen);
#if 1

#else
    write_pdata_xram("^Event", (char *)bData, MetBMagneticAbNormalAddr, MetBMagneticAbNormalLen);
#endif
    write_pdata_xram("^Event", (char *)bData, TimeSettingAddr, TimeSettingLen);


}

/*********************************************************************
所属功能单元：
功能描述：清应用层外存数据
输入参数：无
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void ClearAppYData(void)
{
    uint8 bData[(VIR_BEGIN) * 4 * sizeof(uint64)];

    memset(bData, 0, (VIR_BEGIN) * 4 * sizeof(uint64));
    write_pdata_xram("^Event", (const char *)bData, EventStatusRecAddr, EventStatusRecLen);
}
//<Start Common Problem>
/*********************************************************************
所属功能单元：
功能描述：清应用层外存数据
输入参数：无
输出参数：无
函数返回值：无
内部处理流程：
********************************************************************/
void ClearAppZData(void)
{
    uint8 bData[256];
    memset(bData, 0, sizeof(bData));
    write_pdata_xram("^Event", (const char *)bData, ProgDownAddr, ProgDownRecLen);
}

/*
*********************************************************************
功能:
    终端停、上电时间处理函数；
返回值:
    1:        终端上电；
    0:        终端未上电；
*********************************************************************
*/
int PowerDownCheck(void)
{
    int len = -1;
    OOP_DATETIME_S_T last_time = {0};    //-停电时间-
    time_t nowSecond, lastSecond;

    len = read_pdata_xram("^Event", (char *)&last_time, ProgDownAddr, ProgDownRecLen);
    if (len < 0)    //-读停电时间-
        return 0;

    if ((last_time.month == 0) || (last_time.month > 0x12) ||
        (last_time.day == 0) || (last_time.day > 0x31))
        return 0;

    OopDateTimeSToLinuxTime(&last_time, &lastSecond);

    //-比较60S,超过算上电-
    time(&nowSecond);

    return ((nowSecond - lastSecond) > POWER_STOP_TIME_LMT);
}

/*********************************************************************
所属功能单元：用电监测提供给规约解释
功能描述：当重新设置表之后，清空表外存历史数据给
                事件检测。
输入参数：无
输出参数：无
函数返回值：无
内部处理流程：清空设置过后测量点的外存历史数据
********************************************************************/
void ClearMtrHisD(mtrnum_t Mtrid)
{
    uint8 bData[4];
    uint8 bData1[32];

    memset(bData, 0, 4);
    memset(bData1, APP_NONECHAR, 32);

    if (Mtrid > MTR485_MAX_NUM)    //只记录前64个测量点的事件，清外存时也只需清前64个测量点的数据.
    {
        return;
    }

    write_pdata_xram("^Event", (char *)bData, (CurReverseRecAddr + (Mtrid * sizeof(uint8))), 1);
    write_pdata_xram("^Event", (char *)bData, (EventStatusRecAddr + (Mtrid * sizeof(uint32))), sizeof(uint32));
    write_pdata_xram("^Event", (char *)bData, (LostvltAddr + (Mtrid * sizeof(uint16))), sizeof(uint16));
    write_pdata_xram("^Event", (char *)bData, (BreakphaseAddr + (Mtrid * sizeof(uint32))), sizeof(uint32));
    write_pdata_xram("^Event", (char *)bData, (PhaseAbnormalAddr + (Mtrid * 4)), 4);
    write_pdata_xram("^Event", (char *)bData, (PhaseRvsLastTimesAddr + (Mtrid * 2)), 2);
}

int Get_Event_Confirm(void)
{
/*    uint8 temp = 0;
    //DPRINT("收到确认报文\n");
    read_pdata_xram("^Event", (char *)&temp, InSeqAddr, sizeof(uint8));
    if (temp)
    {
        temp = 0;
        write_pdata_xram("^Event", (char *)&temp, InSeqAddr, sizeof(uint8));
#if JIANGSU_JZQ_FUN == 1
        //表示收到事件的确认报文
        uint8 Estate = 0;
        ReadRamXData(Event_Report_Flag, (char*)&Estate);
        Estate |= 0x10;
        WriteRamXData(Event_Report_Flag, (char*)&Estate);
#endif
    }
*/    return 0;
}

/*
********************************************************************
所属功能单元：GB101规约处理模块
功能描述    ：遥测数据转换，将数据字典的数据转换为104规约的数据格式
输入参数    ：Objaddr-数据信息体地址

输出参数    ：*pdata-返回数据缓冲区

函数返回值    ：无
作者        ：丁俊杰
创建日期    ：2014年4月3日
修改者    ：
修改说明    ：
修改日期    ：
*******************************************************************/
//-这个函数要根据数据字典类型、存储倍率和上送倍率进行分类 2016-06-
void TransYaoChe(uint16 Objaddr,float *pData)
{
    int i =0;

    for(i=0;i<MAX_YC_NUM;i++)
    {
        if(g_aYcDataConfig[i].nObjAddr == Objaddr)
        {
            if(g_aYcDataConfig[i].nSignalType == M_ME_NA_1)
            {
                //归一化数值
                float32 fRatio = ratio_convert(g_aYcDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
                float32 fConvertValue = g_aYcDataSend[i].fNewValue*fRatio;
                float32 fNvaValue = fConvertValue/g_aYcDataConfig[i].fFullValue;  //实际值除以满码值
                int16   nValueNVA = 0;  //转换之后的归一化值

                //溢出检查
                if(fNvaValue > (1-pow(2, -15)))
                {
                    fNvaValue = 1-pow(2, -15);
                }
                else if(fNvaValue < (-1+pow(2, -15)))
                {
                    fNvaValue = -1+pow(2, -15);
                }

                if(fNvaValue >= 0)
                {
                    nValueNVA = (int16)(fNvaValue*pow(2, 15));
                }
                else
                {
                    nValueNVA = (int16)(fNvaValue*pow(2, 15)*(-1));
                    nValueNVA = (int16)(nValueNVA*(-1));
                }

                memcpy((void *)pData, (void *)&nValueNVA, sizeof(int16));

                break;
            }

            if(g_aYcDataConfig[i].nSignalType == M_ME_NB_1)
            {
                //标度化数值
                float32 fRatio = ratio_convert(g_aYcDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
                float32 fConvertValue = g_aYcDataSend[i].fNewValue*fRatio;
                int16 nValueSVA;  //转换之后的标度化值

                //溢出检查
                if(fConvertValue > 32767)
                {
                    nValueSVA = 32767;
                }
                else if(fConvertValue < (-32768))
                {
                    nValueSVA = (-32768);
                }
                else
                {
                    nValueSVA = (int16)fConvertValue;
                }

                memcpy((void *)pData, (void *)&nValueSVA, sizeof(int16));

                break;
            }

            if(g_aYcDataConfig[i].nSignalType == M_ME_NC_1)
            {
                //浮点型数值
                float32 fRatio = ratio_convert(g_aYcDataConfig[i].nReportRatio, E_REAL_TO_DISPATCH);
                float32 fConvertValue = g_aYcDataSend[i].fNewValue*fRatio;

                memcpy((void *)pData, (void *)&fConvertValue, sizeof(float32));

                break;
            }
        }
    }

    return ;
}

#if 0
void WriteTestData(mtrnum_t mtrid, uint32 oad, uint8* value, uint16 vlen)
{
    WRITE_RECORD_T recInData;
    time_t         timep = 0;

    time(&timep);

    recInData.recordOAD.classtag = CLASS_DATA_COLLEC;
    recInData.recordOAD.road.oadCols.nNum = 1;
    recInData.recordOAD.road.oadCols.oad[0] = oad;
    recInData.MAC = MAC;
    recInData.colStartTm = timep;
    recInData.colEndTm = timep;
    recInData.colStoreTm = eventCommInfo->recordnum +1;
    recInData.inDatalen = sizeof(OOP_EVENT_T);
    memcpy(recInData.inDataBuf, &oopEvent, sizeof(OOP_EVENT_T));

    //db_write_record(DB_CLIENT fd, WRITE_RECORD_T * ptRecord, uint32 * storeNo)
}
#endif

/**********************************************************************
* @name      : event_happen_proc
* @brief     ：事件发生检查
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 陈德昶
* @Date      ：2020-03-12
* @Update    :

**********************************************************************/
void event_init(void)
{
    int i = 0, ret = 0;

    if ((ret = reg_pdata_xram(ADV_APP_NAME, "^Event")) != 0)
    {
        EVENT_FMT_DEBUG("event_init reg error1 ret=%d\n", ret);
    }

    if ((ret = reg_pdata_xram(ADV_APP_NAME, "^EventS104")) != 0)
    {
        EVENT_FMT_DEBUG("event_init reg error2 ret=%d\n", ret);
    }

    //初始化抄表数据
    for (i = 0; i < 8; i++)
    {
        g_LatestMeterReadData.oopWord4C[i].nNum = 0;
        g_LatestMeterReadData.oopWord4V[i].nNum = 0;
        g_LatestMeterReadData.valTemp[i].nNum = 0;
        g_LatestMeterReadData.curTemp[i].nNum = 0;
        memset(g_LatestMeterReadData.oopWord4C[i].nValue, APP_NONECHAR, sizeof(g_LatestMeterReadData.oopWord4C[i].nValue));
        memset(g_LatestMeterReadData.oopWord4V[i].nValue, APP_NONECHAR, sizeof(g_LatestMeterReadData.oopWord4V[i].nValue));
        memset(g_LatestMeterReadData.valTemp[i].nValue  , APP_NONECHAR, sizeof(g_LatestMeterReadData.valTemp[i].nValue  ));
        memset(g_LatestMeterReadData.curTemp[i].nValue  , APP_NONECHAR, sizeof(g_LatestMeterReadData.curTemp[i].nValue  ));
    }

    InitEventPara();
}

/**********************************************************************
* @name      : event_happen_proc
* @brief     ：事件发生检查
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 陈德昶
* @Date      ：2020-03-12
* @Update    :

**********************************************************************/
void event_happen_proc(void)
{
    static uint8    bfgMtrEnd=0;                //cfg of mtr dealed,１ end,０ not end，just dealing
    //uint32          i = 0;
    static uint8    pscnt = 0;                    //过秒的次数
    static time_t   timep_sec;
    time_t          timep_cur;
    time(&timep_cur); //获取从1970至今过了多少秒，存入time_t类型的timep_cur

    if(pscnt%60 == 0)
    {
        //过分进行测量点事件的检测
        bfgMtrEnd = 0;                            //past a minute,begin a new trun
    }

    if (timep_sec != timep_cur)
    {
        //过秒进行终端事件的检测
        timep_sec = timep_cur;

        EVENT_FMT_TRACE("进入事件生成模块\n");
        RtuExam();                                //Tmn running examing
        EVENT_FMT_TRACE("终端事件的检测完毕\n");

        pscnt++;
    }

    if (!bfgMtrEnd)                                //not complete，examing in turn
    {
        EVENT_FMT_TRACE("过分进行测量点事件的检测\n");
        pscnt = 1;

        MetExam(0, NULL);

#if 0
        while (i < g_sMeterFileAll.MeterCount)
        {
            mtrnum_t mtr_id = (mtrnum_t)g_sMeterFileAll.MeterFile[i].nIndex;
            EVENT_FMT_TRACE("测量点%d有效,检测\n",mtr_id);
            MetExam(mtr_id, &g_sMeterFileAll.MeterFile[i]);                    //测量点告警事件处理
            i++;
        }
#endif
        bfgMtrEnd = 1;
        EVENT_FMT_TRACE("测量点事件的检测完毕\n");
    }
}

/**********************************************************************
* @name      : event_happen_proc_new
* @brief     ：事件发生检查
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 李银银
* @Date      ：2020-06-09
* @Update    :

**********************************************************************/
void event_happen_proc_new(void)
{
    static time_t   timep_sec;
    time_t          timep_cur;
    time(&timep_cur); //获取从1970至今过了多少秒，存入time_t类型的timep_cur

    if (timep_sec != timep_cur)
    {
        //过秒进行终端事件的检测
        timep_sec = timep_cur;

        EVENT_FMT_TRACE("进入终端事件生成模块\n");
        RtuExam();                                //Tmn running examing
        EVENT_FMT_TRACE("终端事件的检测完毕\n");

		EVENT_FMT_TRACE("进入交采事件生成模块\n");
        MetExam(0, NULL);                         //Tmn running examing
        EVENT_FMT_TRACE("交采事件的检测完毕\n");
    }
}


/**********************************************************************
* @name      : event_check_faults
* @brief     ：检查是否有事件记录,并且缓存事件记录
* @param[in] ：
* @param[out]：
* @return    ：0没有记录，1有记录
* @Create    : 陈德昶
* @Date      ：2020-03-12
* @Update    :

**********************************************************************/
int event_check_faults(void)
{
    uint8 i ,j ,index,bno,bsit,bfgchange= 0;
    uint32 id = 0;
    uint8  bdata[32] = {0};
    float  fdata = 0;
    uint16 ddata = 0;
    uint8  rtn = 0;
    uint16 dtemp = 0;
    struct timeval   c_time;
    OOP_DATETIME_S_T SystemTime;

    if (g_nConfigFaultNum < 1 || g_nConfigFaultNum > FAULT_MAX_NUM)
    {
        EVENT_FMT_DEBUG("event_check_faults, 配置数量%u错误\n", g_nConfigFaultNum);

        return 0;
    }

    for(i=0;i<g_nConfigFaultNum;i++)
    {
        if (g_aFaultDataConfig[i].nObjAddr < FALT_START_ADDR || g_aFaultDataConfig[i].nObjAddr >= FALT_START_ADDR+fault_report_ramsize)
        {
            EVENT_FMT_DEBUG("event_check_faults, 第%u配置错误, nObjAddr:%04X\n", i, g_aFaultDataConfig[i].nObjAddr);

            continue;
        }

        id   = g_aFaultDataConfig[i].nObjAddr - FALT_START_ADDR;
        ReadRamXData(fault_report_ramoff[id].data_code, (char *)bdata, 9);//读取遥信存放数据字典
        bno  =    fault_report_ramoff[id].state_bit/8;//变位信息
        bsit =    fault_report_ramoff[id].state_bit%8;
        bfgchange = (bdata[bno]>>bsit)&0x01;

        if(bfgchange)
        {
            rtn = 1;
            EVENT_FMT_DEBUG("event_check_faults, ID=%u MTR:%u\n", id, bno);

            bdata[bno] &= ~(0x01<<bsit);//清变位标志
            WriteRamXData(id, (char*)bdata, 9);

            //填写数据
            faultsenddata[i].bfgchange = 1;//变位标志
            faultsenddata[i].objAddr = g_aFaultDataConfig[i].nObjAddr;
            bno  =    fault_report_ramoff[id].data_bit/8;
            bsit =    fault_report_ramoff[id].data_bit%8;
            faultsenddata[i].bstate = (bdata[bno]>>bsit)&0x01;//遥信状态

            //变位发生时间填写
            GetCurrentOopDateTimeS(&SystemTime); //-取系统时钟-

            gettimeofday(&c_time, NULL);
            dtemp = ((uint16)SystemTime.second)*1000 + c_time.tv_usec/1000;
            faultsenddata[i].btime[0] =  dtemp&0xff;
            faultsenddata[i].btime[1] =  dtemp>>8;
            faultsenddata[i].btime[2] =  SystemTime.minute&0x3f;//高两位保留
            faultsenddata[i].btime[3] =  SystemTime.hour&0x1f;
            faultsenddata[i].btime[4] =  SystemTime.day&0x1f;

            if(SystemTime.week == 0)
                faultsenddata[i].btime[4] |= (7<<5);
            else
                faultsenddata[i].btime[4] |= (SystemTime.week<<5);

            faultsenddata[i].btime[5] =  SystemTime.month&0x0f;
            faultsenddata[i].btime[6] =  (SystemTime.year%100)&0x7f;

            faultsenddata[i].reportnum = fault_report_ramoff[id].reportnum; //遥测数据个数
            faultsenddata[i].datatype = g_aFaultDataConfig[i].nSignalType;  //遥测数据类型
            index = 0;

            for(j=0;j<g_aFaultDataConfig[i].nReportNum;j++)
            {
                faultsenddata[i].reportdata[index++] = g_aFaultDataConfig[i].aReportAddr[j]&0xff;
                faultsenddata[i].reportdata[index++] = (g_aFaultDataConfig[i].aReportAddr[j]>>8) & 0xff;
                faultsenddata[i].reportdata[index++] = 0; //以3地址长度保存

                TransYaoChe(g_aFaultDataConfig[i].aReportAddr[j],&fdata);
                if(g_aFaultDataConfig[i].nSignalType != M_ME_NC_1)
                {
                    ddata = (uint16)fdata;
                    memcpy((void *)&faultsenddata[i].reportdata[index],(void *)&ddata,2);
                    index += 2;
                }
                else
                {
                    memcpy((void *)&faultsenddata[i].reportdata[index],(void *)&fdata,4);
                    index += 4;
                }
            }
        }
    }

    return rtn;
}

/**********************************************************************
* @name      : event_get_faults
* @brief     ：读取故障事件记录
* @param[in] ：infolen:地址长度 bstartnum记录起始索引 pNum最大读取事件记录数
* @param[out]：pData 读取数据缓存 pNum事件读取事件记录数
* @return    ：读取的数据长度
* @Create    : 陈德昶
* @Date      ：2020-03-12
* @Update    :

**********************************************************************/
int event_get_faults(uint8 infolen,uint8 *pData,uint8 bstartnum,uint8 *pNum)
{
    uint8 i = 0;
    uint8 j = 0;
    uint8 k = 0;
    uint8 bstep = 0;
    uint8 startnum = 0;
    uint8 bgetnum = 0;
    uint8 len = 0;
    uint8 index = 0;

    startnum = bstartnum;
    bgetnum  = *pNum;
    *pNum    = 0;

    if (g_nConfigFaultNum < 1 || g_nConfigFaultNum > FAULT_MAX_NUM)
    {
        EVENT_FMT_DEBUG("event_check_faults, 配置数量%u错误\n", g_nConfigFaultNum);

        return 0;
    }

    if(startnum >= FAULT_MAX_NUM)
    {
        return 0;
    }

    for(i=startnum;i<FAULT_MAX_NUM;i++)
    {
        if(faultsenddata[i].bfgchange ==  1)
        {
            faultsenddata[i].bfgchange = 0;

            pData[index++]  =  faultsenddata[i].objAddr&0xff;//信息体地址
            pData[index++]  =  faultsenddata[i].objAddr>>8;
            if(infolen == 3)
            {
                pData[index++] = 0;
            }
            pData[index++] = faultsenddata[i].bstate;
            for(k=0;k<7;k++)//变化时间
            {
                pData[index++] = faultsenddata[i].btime[k];
            }
            pData[index++] = faultsenddata[i].reportnum;
            pData[index++] = faultsenddata[i].datatype;

            bstep = 0;

            for(k=0;k<faultsenddata[i].reportnum;k++)
            {
                pData[index++] = faultsenddata[i].reportdata[bstep++];
                pData[index++] = faultsenddata[i].reportdata[bstep++];

                if(infolen == 3)
                {
                    pData[index++] = 0;
                }

                bstep++;

                if(faultsenddata[i].datatype == M_ME_NC_1 )
                {
                    memcpy((void *)&pData[index],(void *)&faultsenddata[i].reportdata[bstep],4);
                    bstep += 4;
                    index += 4;
                }
                else
                {
                    memcpy((void *)&pData[index],(void *)&faultsenddata[i].reportdata[bstep],2);
                    index += 2;
                    bstep += 2;
                }
            }

            *pNum = j+1;

            if((j+1)>=bgetnum)
                break;

            j++;
        }
    }

    len = index;

    return len;
}

